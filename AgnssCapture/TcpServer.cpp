/*
 * TcpServer.cpp
 *
 *  Created on: 2015年11月23日
 *      Author: terry
 */

#include "TcpServer.h"
#include <event2/thread.h>
#include <event2/listener.h>
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif //

#include "Socket.h"
#include "CLog.h"


void TcpServer::Startup()
{
#ifdef WIN32
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif //

	//CLog::setLogger(CLog::DEBUGWINDOW);
}

void TcpServer::Cleanup()
{

}


void TcpServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	TcpServer* server = (TcpServer*)user_data;
	server->listenerCallback(listener, fd, sa, socklen);
}


void TcpServer::conn_writecb(struct bufferevent *bev, void *user_data)
{
	TcpConnection* conn = (TcpConnection*)user_data;
	conn->onWrite(bev);
}

void TcpServer::conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	TcpConnection* conn = (TcpConnection*)user_data;
	conn->onEvent(bev, events);
}

void TcpServer::conn_readcb(struct bufferevent *bev, void *user_data)
{
	TcpConnection* conn = (TcpConnection*)user_data;
	conn->onRead(bev);
}



TcpServer::TcpServer():
	m_base(),
	m_listener(),
    m_port()
{
	m_base = event_base_new();
}

TcpServer::~TcpServer()
{
	event_base_free(m_base);
}


bool TcpServer::start(const char* ip, int port)
{
	if (isRunning())
	{
		stop();
	}

    m_port = port;

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	if ((ip != NULL) && strlen(ip) != 0)
	{
		sin.sin_addr.s_addr = inet_addr(ip);
	}

	m_listener = evconnlistener_new_bind(m_base, listener_cb, (void *)this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));
	if (!m_listener)
	{
		return false;
	}

	comn::Thread::start();

	return true;
}

void TcpServer::stop()
{
    if (isRunning())
    {
	    comn::Thread::stop();
    }

	if (m_listener)
	{
		evconnlistener_free(m_listener);
		m_listener = NULL;
	}
}

bool TcpServer::isStarted()
{
	return (m_listener != NULL);
}

TcpConnectionPtr TcpServer::find(evutil_socket_t fd)
{
	TcpConnectionPtr conn;
	m_connMap.find(fd, conn);
	return conn;
}

TcpConnectionPtr TcpServer::createConnection(evutil_socket_t fd, struct bufferevent* bev)
{
	TcpConnectionPtr conn = create(fd);
	if (conn)
	{
		conn->setContext(fd, this, bev);
	}
	return conn;
}

bool TcpServer::close(evutil_socket_t fd)
{
	bool found = false;
	TcpConnectionPtr conn;
    found = m_connMap.remove(fd, conn);
	if (conn)
	{
        onClose(conn);

		conn->close();
		found = true;
	}
	return found;
}

int TcpServer::run()
{
	while (!m_canExit)
	{
		event_base_dispatch(m_base);
	}
	return 0;
}



void TcpServer::doStop()
{
    comn::Socket sock;
    sock.open(SOCK_STREAM);
    sock.connect("127.0.0.1", m_port);
    sock.close();

	event_base_loopexit(m_base, NULL);
}

void TcpServer::closeAll()
{
	evutil_socket_t fd = 0;
	TcpConnectionPtr conn;
	while (m_connMap.pop_front(fd, conn))
	{
        onClose(conn);

		conn->close();
	}
}

void TcpServer::listenerCallback(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen)
{
    if (m_canExit)
    {
		event_base_loopbreak(m_base);
        return;
    }

	struct bufferevent *bev;

	bev = bufferevent_socket_new(m_base, fd, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(m_base);
		return;
	}

	TcpConnectionPtr conn = createConnection(fd, bev);
	if (!conn)
	{
		bufferevent_free(bev);
		return;
	}

	int rc = conn->open(fd);
	if (rc != 0)
	{
		bufferevent_free(bev);
		return;
	}

	m_connMap.set(fd, conn);

    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, conn.get());

	//bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

}

void TcpServer::onClose(TcpConnectionPtr& conn)
{
    //
}
