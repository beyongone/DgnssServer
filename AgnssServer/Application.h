/*
 * Application.h
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <stdio.h>
#include <iostream>
#include <signal.h>

#include "AppConfig.h"
#include "SharedPtr.h"

#include <zmqpp/zmqpp.hpp>
#include "AgnssType.h"
#include "TByteBuffer.h"
#include "DataProcessorMap.h"
#include "TimerQueue.h"
#include "CTimerQueue.h"


class Application : public comn::TimerHandler
{
public:
	Application();
	virtual ~Application();

	AppConfig& config();

	void setupSignal();

	void setupLog();

	bool initialize();

	void uninitialize();

	std::string getConfigPath();

	void exit();

	void run();


	bool handleOutput(const std::string& device, DataProcessorPtr processor);

protected:
	virtual bool onTimer(const comn::TimerID& id, uint32_t& interval);

protected:
	bool setupCaptureSink();
	bool setupPublish();

	void handleCaptureMessage(zmqpp::message& msg, AgnssFrameHeader& header, const unsigned char *data, size_t length);

	void handleStatus(AgnssFrameHeader& header, const std::string& info);

	bool handleCaptureData(AgnssFrameHeader& header, const void* data, size_t length);

	//void publish(DataProcessorPtr& processor, AgnssParam& param, AgnssAssistData& data);
	void publish(DataProcessorPtr& processor, AgnssParam& param, DgnssDatAll& data);

	void publishdgnss(DataProcessorPtr& processor, DgnssDatAll& data);

	DataProcessorPtr createDataProcessor(int deviceType, const char* device);

protected:
	zmqpp::context	m_context;
	zmqpp::socket	m_socket;
	bool	m_canExit;

	zmqpp::socket	m_pubSocket;

	DataProcessorMap	m_processorMap;
	comn::CTimerQueue	m_timerQueue;
	comn::TimerID	m_timerId;

};

#endif /* APPLICATION_H_ */
