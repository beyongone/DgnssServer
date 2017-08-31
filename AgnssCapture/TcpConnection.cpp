/*
 * TcpConnection.cpp
 *
 *  Created on: 2015年11月23日
 *      Author: terry
 */

#include "TcpConnection.h"
#include "TcpServer.h"
#include "Socket.h"
#include "TStringUtil.h"


TcpConnection::TcpConnection():
	m_fd(),
	m_server(),
	m_bev()
{
}

TcpConnection::~TcpConnection()
{

}

int TcpConnection::open(evutil_socket_t fd)
{
	return onOpen();
}

void TcpConnection::close()
{
	onClose();

	internalClose();
}

bool TcpConnection::isOpen()
{
	return (m_fd  != -1);
}

evutil_socket_t TcpConnection::getSocket()
{
	return m_fd;
}

std::string TcpConnection::getPeerName()
{
	return getPeerName(m_fd);
}

std::string TcpConnection::getPeerName(evutil_socket_t fd)
{
	comn::SockAddr addr;
	socklen_t len = sizeof(addr);
	getpeername(fd, (struct sockaddr*)&addr, &len);

	return comn::StringUtil::format("%s:%d", addr.getIPAddr(), addr.getPort());
}

void TcpConnection::setContext(evutil_socket_t fd, TcpServer* server, struct bufferevent* bev)
{
	m_fd = fd;
	m_server = server;
	m_bev = bev;
}

void TcpConnection::internalClose()
{
	if (!m_server)
	{
		return;
	}

	if (m_bev)
	{
        bufferevent_disable(m_bev, EV_WRITE | EV_READ);
		bufferevent_free(m_bev);
		m_bev = NULL;
	}

	if (m_server)
	{
        TcpServer* svr = m_server;
        m_server = NULL;
        m_fd = -1;

		svr->close(m_fd);
	}
}

int TcpConnection::onOpen()
{
	return 0;
}

void TcpConnection::onClose()
{
	// pass
}

void TcpConnection::onWrite(struct bufferevent *bev)
{
}

void TcpConnection::onRead(struct bufferevent *bev)
{
	
}

void TcpConnection::onEvent(struct bufferevent *bev, short events)
{
	if (events & BEV_EVENT_EOF)
	{
		close();
	}
    else if (events & BEV_EVENT_ERROR)
    {
        close();
    }

}

bufferevent* TcpConnection::getBufferEvent()
{
	return m_bev;
}
 
TcpServer* TcpConnection::getServer()
{
    return m_server;
}
