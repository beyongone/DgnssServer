/*
 * CaptureTcpServer.h
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#ifndef CAPTURETCPSERVER_H_
#define CAPTURETCPSERVER_H_

#include "TcpServer.h"
#include "CaptureTcpConnection.h"


class CaptureTcpServer: public TcpServer
{
public:
	CaptureTcpServer();
	virtual ~CaptureTcpServer();

	void setCaptureSink(CaptureSinkPtr sink);

	void setDevice(const std::string& device, int deviceType);

protected:
	virtual TcpConnectionPtr create(evutil_socket_t fd);

	virtual void onClose(TcpConnectionPtr& conn);

protected:
	CaptureSinkPtr	m_sink;
	std::string		m_device;
	int m_deviceType;

};

#endif /* CAPTURETCPSERVER_H_ */
