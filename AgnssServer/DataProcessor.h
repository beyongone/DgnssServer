/*
 * DataProcessor.h
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#ifndef DATAPROCESSOR_H_
#define DATAPROCESSOR_H_

#include "AgnssType.h"
#include <string>
#include "TByteBuffer.h"


class DataProcessor
{
public:
	DataProcessor();
	virtual ~DataProcessor();

	virtual bool open(int deviceType, const char* device);

	virtual void close();

	virtual int getDeviceType();

	virtual std::string getDevice();

	virtual void input(AgnssFrameHeader& header, const void* data, size_t length) =0;

	//virtual bool getOutput(AgnssParam& param, AgnssAssistData& data) =0;
	virtual bool getOutput(AgnssParam& param, DgnssDatAll& data) = 0;

	//virtual bool getOutputDGNSS(DgnssDatAll& data) = 0;


protected:
	int		m_deviceType;
	std::string	m_device;

};


#endif /* DATAPROCESSOR_H_ */
