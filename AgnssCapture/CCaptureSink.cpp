/*
 * CCaptureSink.cpp
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#include "CCaptureSink.h"
#include <time.h>
#include "TSafeStr.h"
#include "CLog.h"
#include "TEvent.h"


CCaptureSink::CCaptureSink():
	m_context(),
	m_socket(m_context, zmqpp::socket_type::push),
	m_canExit()
{
}

CCaptureSink::~CCaptureSink()
{
}

void CCaptureSink::writeStatus(const std::string& device, int deviceType, int status, const std::string& info)
{
	CLog::debug("device(%s) type:%d, status:%d, info:%s\n", device.c_str(), deviceType, status, info.c_str());

	zmqpp::message message;

	AgnssFrameHeader header;
	memset(&header, 0, sizeof(header));
	header.tag = AGNSS_TAG_STATUS;
	header.subtype = status;
	header.time = time(NULL);
	header.length = 0;
	header.deviceType = deviceType;
	comn::copyStr(header.device, device);

	message.push_back(&header, sizeof(header));
	message.push_back(info);

	m_socket.send(message);
}

void CCaptureSink::writeData(const std::string& device, int deviceType, const char* data, size_t length)
{
	zmqpp::message message;

	AgnssFrameHeader header;
	memset(&header, 0, sizeof(header));
	header.tag = AGNSS_TAG_DATA;
	header.time = time(NULL);
	header.length = length;
	header.deviceType = deviceType;
	comn::copyStr(header.device, device);

	message.push_back(&header, sizeof(header));
	message.push_back(data, length);

	m_socket.send(message);
}

bool CCaptureSink::open(const std::string& url)
{
	m_socket.connect(url);

	return true;
}

void CCaptureSink::close()
{
	m_socket.close();
}

bool CCaptureSink::isOpen()
{
	return (bool)m_socket;
}


void CCaptureSink::cancel()
{
	m_canExit = true;

	try
	{
		m_socket.close();
	}
	catch (std::exception& ex)
	{
		printf("%s\n", ex.what());
	}
}
