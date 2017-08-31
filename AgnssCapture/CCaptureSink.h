/*
 * CCaptureSink.h
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#ifndef CCAPTURESINK_H_
#define CCAPTURESINK_H_

#include "CaptureSink.h"
#include <zmqpp/zmqpp.hpp>

class CCaptureSink: public CaptureSink
{
public:
	CCaptureSink();
	virtual ~CCaptureSink();

	virtual bool open(const std::string& url);

	virtual void close();

	virtual bool isOpen();

	virtual void writeStatus(const std::string& device, int deviceType, int status, const std::string& info);

	virtual void writeData(const std::string& device, int deviceType, const char* data, size_t length);

	void cancel();

protected:
	zmqpp::context	m_context;
	zmqpp::socket	m_socket;
	bool	m_canExit;


};

#endif /* CCAPTURESINK_H_ */
