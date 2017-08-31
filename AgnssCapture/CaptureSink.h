/*
 * CaptureSink.h
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#ifndef CAPTURESINK_H_
#define CAPTURESINK_H_

#include "AgnssType.h"
#include <string>


class CaptureSink
{
public:
	virtual ~CaptureSink() {}

	virtual bool open(const std::string& url) =0;

	virtual void close() =0;

	virtual bool isOpen() =0;

	virtual void writeStatus(const std::string& device, int deviceType, int status, const std::string& info) =0;

	virtual void writeData(const std::string& device, int deviceType, const char* data, size_t length) =0;

};

#endif /* CAPTURESINK_H_ */
