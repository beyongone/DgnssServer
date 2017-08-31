/*
 * TcpServer.h
 *
 *  Created on: 2015年11月23日
 *      Author: terry
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_


#include "TcpConnection.h"
#include "TThread.h"
#include "TMap.h"



class TcpServer : public comn::Thread
{
public:
	static void Startup();
	static void Cleanup();

public:
	TcpServer();
	virtual ~TcpServer();

	bool start(const char* ip, int port);

	void stop();

	bool isStarted();

	TcpConnectionPtr find(evutil_socket_t fd);

	bool close(evutil_socket_t fd);

	void closeAll();

protected:
	virtual TcpConnectionPtr create(evutil_socket_t fd) =0;

    virtual void onClose(TcpConnectionPtr& conn);

protected:

    virtual int run();
    virtual void doStop();

protected:
	TcpConnectionPtr createConnection(evutil_socket_t fd, struct bufferevent* bev);

	void listenerCallback(struct evconnlistener *listener, evutil_socket_t fd,
	    struct sockaddr *sa, int socklen);

    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *sa, int socklen, void *user_data);
    static void conn_writecb(struct bufferevent *bev, void *user_data);
    static void conn_eventcb(struct bufferevent *bev, short events, void *user_data);
    static void conn_readcb(struct bufferevent *bev, void *user_data);

protected:
	struct event_base*	m_base;
	struct evconnlistener * m_listener;

    int     m_port;

	typedef comn::Map< evutil_socket_t, TcpConnectionPtr >	TcpConnectionMap;
	TcpConnectionMap m_connMap;

};

#endif /* TCPSERVER_H_ */
