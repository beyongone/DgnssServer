/*
 * CaptureTcpServer.cpp
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#include "CaptureTcpServer.h"
#include "CaptureTcpConnection.h"



CaptureTcpServer::CaptureTcpServer():
	m_deviceType()
{
}

CaptureTcpServer::~CaptureTcpServer()
{
}

TcpConnectionPtr CaptureTcpServer::create(evutil_socket_t fd)
{
	CaptureTcpConnection* conn = new CaptureTcpConnection();

	conn->setCaptureSink(m_sink);
	conn->setDevice(m_device);

    return TcpConnectionPtr(conn);
}

void CaptureTcpServer::onClose(TcpConnectionPtr& conn)
{
	TcpServer::onClose(conn);
}

void CaptureTcpServer::setCaptureSink(CaptureSinkPtr sink)
{
	m_sink = sink;
}

void CaptureTcpServer::setDevice(const std::string& device, int deviceType)
{
	m_device = device;
	m_deviceType = deviceType;
}
