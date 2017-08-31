/*
 * TcpConnection.h
 *
 *  Created on: 2015年11月23日
 *      Author: terry
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include "BasicType.h"
#include "SharedPtr.h"
#include <string>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/util.h>


class TcpServer;

class TcpConnection
{
public:
	TcpConnection();
	virtual ~TcpConnection();

	int open(evutil_socket_t fd);

	void close();

	bool isOpen();

	evutil_socket_t getSocket();

	std::string getPeerName();

	static std::string getPeerName(evutil_socket_t fd);

protected:
	virtual int onOpen();

	virtual void onClose();

	virtual void onWrite(struct bufferevent *bev);

	virtual void onRead(struct bufferevent *bev);

	virtual void onEvent(struct bufferevent *bev, short events);


protected:
	bufferevent* getBufferEvent();

    TcpServer* getServer();

private:

	friend class TcpServer;
	void setContext(evutil_socket_t fd, TcpServer* server, struct bufferevent* bev);

	void internalClose();

private:
	evutil_socket_t	m_fd;
	TcpServer*	m_server;
	struct bufferevent* m_bev;

};


typedef std::shared_ptr< TcpConnection >	TcpConnectionPtr;


#endif /* TCPCONNECTION_H_ */
