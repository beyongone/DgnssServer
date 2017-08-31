/*
 * DataProcessor.cpp
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#include "DataProcessor.h"

DataProcessor::DataProcessor():
	m_deviceType(),
	m_device()
{
}

DataProcessor::~DataProcessor()
{
}

bool DataProcessor::open(int deviceType, const char* device)
{
	m_deviceType = deviceType;
	m_device = device;

	return true;
}

void DataProcessor::close()
{
	m_deviceType = 0;
	m_device.clear();
}

int DataProcessor::getDeviceType()
{
	return m_deviceType;
}

std::string DataProcessor::getDevice()
{
	return m_device;
}
