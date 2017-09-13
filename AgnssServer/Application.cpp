/*
 * Application.cpp
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#include "Application.h"
#include "CLog.h"

#include "Socket.h"
#include "TStringUtil.h"
#include "Path.h"
#include "BasicMacro.h"

#include "AssistDataProcessor.h"
#include "TSafeStr.h"
#include "json/json.h"


#ifdef WIN32
	const char* LOG_FILE     = "DgnssServer.log";
#else
	const char* LOG_FILE     = "/var/log/DgnssServer.log";
#endif //

#define UINT_MAX      0xffffffff    // maximum unsigned int value
static Application* s_application = NULL;

static char* encode(char *out, int out_size, const unsigned char *in, int in_size);


static void SigInt_Handler(int n_signal)
{
	if (s_application)
	{
		s_application->exit();
	}

    exit(1);
}

static void SigBreak_Handler(int n_signal)
{
	if (s_application)
	{
		s_application->exit();
	}

    exit(2);
}



/*****************************************************************************
* b64_encode: Stolen from VLC's http.c.
* Simplified by Michael.
* Fixed edge cases and made it work from data (vs. strings) by Ryan.
*****************************************************************************/

char* encode(char *out, int out_size, const unsigned char *in, int in_size)
{
	static const char b64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char *ret, *dst;
	unsigned i_bits = 0;
	int i_shift = 0;
	int bytes_remaining = in_size;

	if ((size_t)in_size >= UINT_MAX / 4 ||
		out_size < (in_size + 2) / 3 * 4 + 1)
		return NULL;
	ret = dst = out;
	while (bytes_remaining) {
		i_bits = (i_bits << 8) + *in++;
		bytes_remaining--;
		i_shift += 8;

		do {
			*dst++ = b64[(i_bits << 6 >> i_shift) & 0x3f];
			i_shift -= 6;
		} while (i_shift > 6 || (bytes_remaining == 0 && i_shift > 0));
	}
	while ((dst - ret) & 3)
		*dst++ = '=';
	*dst = '\0';

	return ret;
}

///

Application::Application():
		m_context(),
		m_socket(m_context, zmqpp::socket_type::pull),
		m_canExit(),
		m_pubSocket(m_context, zmqpp::socket_type::pub)
{
	s_application = this;
}

Application::~Application()
{
	uninitialize();

	s_application = NULL;
}

AppConfig& Application::config()
{
	return AppConfig::instance();
}

void Application::setupSignal()
{
	signal(SIGINT, &SigInt_Handler);
	
	#ifdef WIN32
	signal(SIGBREAK, &SigBreak_Handler);
	#else
		
	#endif
}

void Application::setupLog()
{
    CLog::setLogger(CLog::COUT, CLog::kNone, 0);

    CLog::setLogger(CLog::DEBUGWINDOW, CLog::kNone, 0);
    CLog::setLogger(CLog::FILE);
    CLog::setFileParam(LOG_FILE, 10, 2);
}


std::string Application::getConfigPath()
{
#ifdef WIN32
	std::string workDir = comn::Path::getWorkDir();
	std::string pathList[] =
	{
		".",
		"../bin",
        "../AgnssServer",
		workDir
	};

#else
	std::string pathList[] =
	{
		".",
        "/usr/etc/AgnssServer",
		"/usr/etc",
		"/etc"
	};
#endif //

	return comn::Path::findFile(pathList, ARRAY_SIZE(pathList), "AgnssServer.ini");
}


bool Application::initialize()
{
	std::string configPath = getConfigPath();
	config().load(getConfigPath());

	setupLog();
	setupSignal();

	comn::Socket::startup();

	//
	if (!setupCaptureSink())
	{
		return false;
	}

	if (!setupPublish())
	{
		return false;
	}

	m_timerQueue.start();

	m_timerId = "output";
	m_timerQueue.addTimer(m_timerId, this, 1000 * 3);// 15s changed into 3000ms output

	return true;
}

void Application::uninitialize()
{

	m_timerQueue.cancel(m_timerId);
	m_timerQueue.stop();

	comn::Socket::cleanup();
}

void Application::exit()
{
	m_canExit = true;

	try
	{
		m_socket.close();
	}
	catch (...)
	{
		//
	}
}

void Application::run()
{
	zmqpp::message message;
	while (!m_canExit)
	{
		if (m_socket.receive(message))
		{
			size_t length = message.parts();
			CLog::debug("message. length:%d\n", length);
			
			if (length == 2)
			{
				AgnssFrameHeader header = *((AgnssFrameHeader*)message.raw_data(0));
				const unsigned char* data = (const unsigned char*)message.raw_data(1);
				size_t size = message.size(1);
				handleCaptureMessage(message, header, data, size);
			}/**/
		}
	}
}

bool Application::setupCaptureSink()
{
	std::string server = config().get("App.IP", "0.0.0.0");
	int port = config().getInt("App.SinkPort", 4007);

	std::string endpoint = comn::StringUtil::format("tcp://%s:%d", server.c_str(), port);
	try
	{
		CLog::info("bind capture sink %s\n", endpoint.c_str());
		m_socket.bind(endpoint);
	}
	catch(std::exception& ex)
	{
		CLog::warning("failed to bind capture sink %s. %s\n", endpoint.c_str(), ex.what());
		return false;
	}

	return true;
}

void Application::handleCaptureMessage(zmqpp::message& msg, AgnssFrameHeader& header, const unsigned char *data, size_t length)
{
	std::string text((const char*)data, length);
	CLog::debug("text:%s textlength:%d\n", text.c_str(), length);

	if (header.tag == AGNSS_TAG_STATUS)
	{
		handleStatus(header, text);
	}
	else if (header.tag == AGNSS_TAG_DATA)
	{
		handleCaptureData(header, data, length);
	}
	else
	{
		/// unknown
	}
}

bool Application::setupPublish()
{
	std::string server = config().get("App.IP", "0.0.0.0");
	int port = config().getInt("App.Port", 5002);

	std::string endpoint = comn::StringUtil::format("tcp://%s:%d", server.c_str(), port);
	try
	{
		CLog::info("bind publish %s\n", endpoint.c_str());
		m_pubSocket.bind(endpoint);
	}
	catch(std::exception& ex)
	{
		CLog::warning("failed to bind publish %s. %s\n", endpoint.c_str(), ex.what());
		return false;
	}

	return true;
}


static void writeBuffer(Json::Value& json, const char* name, char* data, int length)
{
	if (length > 0)
	{
		json[name] = std::string(data, length);
	}
	else
	{
		json[name] = std::string();
	}
}

void Application::publishdgnss(DataProcessorPtr& processor, DgnssDatAll& data)
{
	CLog::info("device(%s) publish. dgnss_bds_len:%d, dgnss_gps_len:%d\n",
		processor->getDevice().c_str(), data.dgnss_bdslen, data.dgnss_gpslen);

	zmqpp::message message;

	Json::Value json;

	writeBuffer(json, "dgnss_gps", data.dgnss_gps, data.dgnss_gpslen);
	writeBuffer(json, "dgnss_bds", data.dgnss_bds, data.dgnss_bdslen);

	Json::StyledWriter writer;
	std::string text = writer.write(json);

	message.push_back(text);

	m_pubSocket.send(message);
}


void Application::publish(DataProcessorPtr& processor, AgnssParam& param, DgnssDatAll& data)
{
	CLog::info("device(%s) publish. aideph_bds_len:%d, aideph_gps_len:%d\n",
		processor->getDevice().c_str(), data.dgnss_bdslen, data.dgnss_gpslen);

	AgnssFrameHeader header;
	memset(&header, 0, sizeof(header));
	header.deviceType = processor->getDeviceType();
	comn::copyStr(header.device, processor->getDevice());
	header.tag = AGNSS_TAG_DATA;
	header.time = time(NULL);

	header.length = sizeof(param);

	zmqpp::message message;

	Json::Value json;

	json["tag"] = header.tag;
	json["subtype"] = header.subtype;
	json["deviceType"] = header.deviceType;
	json["device"] = header.device;
	json["time"] = (Json::Int64)header.time;
	json["length"] = header.length;

	json["basetime"] = param.basetime;
	json["flag"] = param.flag;
	json["year"] = param.year;
	json["month"] = param.month;
	json["day"] = param.day;
	json["hour"] = param.hour;
	json["minute"] = param.minute;
	json["sec"] = param.sec;
	json["minsec"] = param.minsec;
	json["secacc"] = param.secacc;
	json["lat"] = param.lat;
	json["lon"] = param.lon;
	json["alt"] = param.alt;
	json["posacc"] = param.posacc;
	json["bdsmask_low"] = param.bdsmask_low;
	json["bdsmask_high"] = param.bdsmask_high;
	json["gpsmask"] = param.gpsmask;

	data.dgnss_gpsbdslenout = ((data.dgnss_gpsbdslen + 2) / 3 * 4 + 1);

	encode(data.dgnss_gpsbdsout, data.dgnss_gpsbdslenout, data.dgnss_gpsbds, data.dgnss_gpsbdslen);

	writeBuffer(json, "aideph_gpsbds", data.dgnss_gpsbdsout, data.dgnss_gpsbdslenout);
		
	Json::StyledWriter writer;
	std::string text = writer.write(json);

	message.push_back(text);

	m_pubSocket.send(message);
}

void Application::handleStatus(AgnssFrameHeader& header, const std::string& info)
{
	CLog::info("device(%s) type:%d status: %d, info:%s\n", header.device, header.deviceType, header.subtype, info.c_str());
}

bool Application::handleCaptureData(AgnssFrameHeader& header, const void* data, size_t length)
{
	DataProcessorPtr processor;
	if (!m_processorMap.find(header.device, processor))
	{
		processor = createDataProcessor(header.deviceType, header.device);
		if (processor)
		{
			m_processorMap.put(header.device, processor);
		}
		else
		{
			return false;
		}
	}

	processor->input(header, data, length);

	return true;
}

DataProcessorPtr Application::createDataProcessor(int deviceType, const char* device)
{
	DataProcessorPtr processor;
	//if (deviceType == AGNSS_DEVICE_ASSIST)
	{
		processor.reset(new AssistDataProcessor());
	}

	if (processor)
	{
		if (!processor->open(deviceType, device))
		{
			CLog::warning("failed to open data processor: %s/%d\n", device, deviceType);
			processor.reset();
		}
	}
	return processor;
}


struct ProcessorTimer : public std::binary_function<std::string, DataProcessorPtr, bool>
{
	ProcessorTimer(Application& app):
		m_app(app)
	{
	}

	bool operator () (const std::string& device, DataProcessorPtr processor)
	{
		return m_app.handleOutput(device, processor);
	}

	Application& m_app;
};

bool Application::onTimer(const comn::TimerID& id, uint32_t& interval)
{
	//CLog::info("handleOutput on timer. %s\n", CLog::getShortTime());

	ProcessorTimer func(*this);
	m_processorMap.forEach(func);

	if (0) 
	{
		/// test data
		DataProcessorPtr processor(new AssistDataProcessor());
		processor->open(1, "demo");

		AgnssParam param;
		memset(&param, 0, sizeof(param));
		param.year = 2017;

		AgnssAssistData data;
		DgnssDatAll dgnssdata;
		memset(&data, 0, sizeof(data));
		memset(&dgnssdata, 0, sizeof(dgnssdata));

		std::string dt = CLog::getShortTime();
		data.aideph_gps_len = dt.size();
		strncpy(data.aideph_gps, dt.c_str(), dt.size());

		data.aideph_bds_len = 8;
		strncpy(data.aideph_bds, CLog::getShortTime(), data.aideph_bds_len);
/*
std::string dgnsst = CLog::getShortTime();
dgnssdata.dgnss_gpslen = dgnsst.size();
strncpy(dgnssdata.dgnss_gps, dgnsst.c_str(), dgnsst.size());

data.aideph_bds_len = 8;
strncpy(data.aideph_bds, CLog::getShortTime(), data.aideph_bds_len);

*/
		
		publish(processor, param, dgnssdata);
	}

	return true;
}

bool Application::handleOutput(const std::string& device, DataProcessorPtr processor)
{
	if (!processor)
	{
		return false;
	}

	AgnssParam param;
//	AgnssAssistData data;
//	memset(&data, 0, sizeof(data));

	DgnssDatAll dgnssDat;
	memset(&dgnssDat, 0, sizeof(dgnssDat));

	if (!processor->getOutput(param, dgnssDat))
	{
		return false;
	}

	//publish(processor, param, dgnssDat);

	/*
	if (1 == dgnssDat.dgnss_flag) {
		publish(processor, param, dgnssDat);
		dgnssDat.dgnss_flag = 0;
	}

	
	if (!processor->getOutputDGNSS(dgnssDat))
	{
		return false;
	}
	*/
	if (1 == dgnssDat.dgnss_flag) {
		publish(processor, param, dgnssDat);
		dgnssDat.dgnss_flag = 0;
	}
		
	return true;
}
