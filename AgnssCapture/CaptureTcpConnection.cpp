/*
 * CaptureTcpConnection.cpp
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#include "CaptureTcpConnection.h"
#include "CLog.h"
using namespace std;


CaptureTcpConnection::CaptureTcpConnection():
	m_deviceType()
{
}

CaptureTcpConnection::~CaptureTcpConnection()
{
}

void CaptureTcpConnection::onRead(struct bufferevent *bev)
{
    size_t len = evbuffer_get_length(bufferevent_get_input(bev));
    m_buffer.ensure(len);
    size_t length = bufferevent_read(bev, (char*)m_buffer.data(), len);
	CLog::debug("length_m_buffer_data:%d   length_m_buffer:%d\n", length, len);
    if (length > 0)
    {
    	handleRecv((char*)m_buffer.data(), length);
    }
}

void CaptureTcpConnection::handleRecv(char* buf, size_t length)
{
	CLog::debug("device %s recv data. length:%d\n", m_device.c_str(), length);
	comn::ByteBuffer result;
	if (processData(buf, length, result))
	{
		fireResult(result);
	}
}

bool CaptureTcpConnection::processData(char* buf, size_t length, comn::ByteBuffer& result)
{
	result.write(buf, length);

	return true;
}

void CaptureTcpConnection::fireResult(comn::ByteBuffer& result)
{
	if (m_sink)
	{
		m_sink->writeData(m_device, m_deviceType, (char*)result.data(), result.length());
	}
}

void CaptureTcpConnection::setCaptureSink(CaptureSinkPtr sink)
{
	m_sink = sink;
}

void CaptureTcpConnection::setDevice(const std::string& device)
{
	m_device = device;
}

std::string CaptureTcpConnection::getDevice() const
{
	return m_device;
}

void CaptureTcpConnection::setDeviceType(int deviceType)
{
	m_deviceType = deviceType;
}

int CaptureTcpConnection::getDeviceType()
{
	return m_deviceType;
}

int CaptureTcpConnection::onOpen()
{
	if (m_sink)
	{
		std::string info = getPeerName();
		m_sink->writeStatus(m_device, m_deviceType, AGNSS_STATUS_ONLINE, info);
	}

	return 0;
}

void CaptureTcpConnection::onClose()
{
	if (m_sink)
	{
		std::string info = getPeerName();
		m_sink->writeStatus(m_device, m_deviceType, AGNSS_STATUS_OFFLINE, info);
	}

}
