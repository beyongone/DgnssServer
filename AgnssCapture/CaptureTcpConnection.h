/*
 * CaptureTcpConnection.h
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#ifndef CAPTURETCPCONNECTION_H_
#define CAPTURETCPCONNECTION_H_

#include "TcpConnection.h"
#include "TByteBuffer.h"

#include "CaptureSink.h"
#include <string>

typedef std::shared_ptr< CaptureSink > 	CaptureSinkPtr;


class CaptureTcpConnection: public TcpConnection
{
public:
	CaptureTcpConnection();
	virtual ~CaptureTcpConnection();

	void setCaptureSink(CaptureSinkPtr sink);

	void setDevice(const std::string& device);

	std::string getDevice() const;

	void setDeviceType(int deviceType);

	int getDeviceType();

protected:
	virtual void handleRecv(char* buf, size_t length);

	/**
	 * 处理数据
	 * @param buf		输入数据缓冲区
	 * @param length	数据长度
	 * @param result	输出结果
	 * @return	true 表示有输出结果
	 */
	virtual bool processData(char* buf, size_t length, comn::ByteBuffer& result);


protected:
	virtual void onRead(struct bufferevent *bev);

	void fireResult(comn::ByteBuffer& result);

	virtual int onOpen();

	virtual void onClose();

protected:
	comn::ByteBuffer	m_buffer;
	CaptureSinkPtr 	m_sink;
	std::string		m_device;
	int		m_deviceType;

};

#endif /* CAPTURETCPCONNECTION_H_ */
