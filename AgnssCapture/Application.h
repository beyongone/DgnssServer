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

#include "TcpServer.h"
#include "SharedPtr.h"
#include "CCaptureSink.h"


class Application
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



protected:
	bool setupTcpServer();

	void setupCaptureSink();

protected:
	std::shared_ptr<TcpServer>	m_tcpServer;
	std::shared_ptr<CCaptureSink> m_captureSink;

};

#endif /* APPLICATION_H_ */
