/*
 * Application.cpp
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#include "Application.h"
#include "CLog.h"

#include "CaptureTcpServer.h"
#include "Socket.h"
#include "TStringUtil.h"
#include "Path.h"
#include "BasicMacro.h"


#ifdef WIN32
	const char* LOG_FILE     = "AgnssCapture.log";
#else
	const char* LOG_FILE     = "/var/log/AgnssCapture.log";
#endif //

static Application* s_application = NULL;


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


///

Application::Application()
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

bool Application::initialize()
{
	std::string configPath = getConfigPath();
	config().load(getConfigPath());

	setupLog();
	setupSignal();

	comn::Socket::startup();
	TcpServer::Startup();

	//
	CLog::info("configPath %s\n", configPath.c_str());
	setupCaptureSink();

	if (!setupTcpServer())
	{
		return false;
	}


	return true;
}

void Application::uninitialize()
{
	if (m_tcpServer)
	{
		m_tcpServer->stop();
	}

	if (m_captureSink)
	{
		m_captureSink->close();
	}

	TcpServer::Cleanup();
	comn::Socket::cleanup();
}

std::string Application::getConfigPath()
{
#ifdef WIN32
	std::string workDir = comn::Path::getWorkDir();
	std::string pathList[] =
	{
		".",
		"../bin",
        "../AgnssCapture",
		workDir
	};

#else
	std::string pathList[] =
	{
		".",
        "/usr/etc/AgnssCapture",
		"/usr/etc",
		"/etc"
	};
#endif //

	return comn::Path::findFile(pathList, ARRAY_SIZE(pathList), "AgnssCapture.ini");
}

void Application::exit()
{
	if (m_captureSink)
	{
		m_captureSink->cancel();
	}

	if (m_tcpServer)
	{
		m_tcpServer->stop();
	}
}

void Application::run()
{
	m_tcpServer->join(-1);
}

bool Application::setupTcpServer()
{
	std::shared_ptr< CaptureTcpServer > server(new CaptureTcpServer());

	std::string device = config().get("App.Device", "demo");
	int deviceType = config().getInt("App.DeviceType", 0);
	server->setDevice(device, deviceType);

	server->setCaptureSink(m_captureSink);

	std::string ip = config().get("App.IP", "0.0.0.0");
	int port = config().getInt("App.Port", 3001);

	CLog::info("tcp server bind %s:%d\n", ip.c_str(), port);

	if (!server->start(ip.c_str(), port))
	{
		CLog::error("failed to listen port %d\n", port);
		return false;
	}

	m_tcpServer = server;

	return true;
}

void Application::setupCaptureSink()
{
	std::string server = config().get("Server.IP", "127.0.0.1");
	int port = config().getInt("Server.Port", 4007);

	m_captureSink.reset(new CCaptureSink());

	std::string endpoint = comn::StringUtil::format("tcp://%s:%d", server.c_str(), port);

	CLog::info("try connecting server %s\n", endpoint.c_str());

	m_captureSink->open(endpoint);
}
