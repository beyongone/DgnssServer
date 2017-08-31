/*
 * Socket.cpp
 *
 *  Created on: 2012-1-10
 *      Author: terry
 */

#include "Socket.h"

#include <assert.h>
#include <sstream>
#include <cstdio>
#include <errno.h>


#ifdef WIN32

    typedef int socklen_t;

#ifdef __MINGW32__
    struct tcp_keepalive {
    u_long  onoff;
    u_long  keepalivetime;
    u_long  keepaliveinterval;
    };
    #define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)
#else
    #include <mstcpip.h>
#endif //


#else
    #include <signal.h>

    #include <unistd.h>
    #include <stdlib.h>
    #include <string.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <netinet/tcp.h>

    #ifndef MAX_PATH
    #define MAX_PATH    256
    #endif //MAX_PATH
#endif // WIN32



namespace comn
{

#ifdef WIN32
    enum
    {
        ERR_INTR = WSAEINTR,
        ERR_WOULDBLOCK = WSAEWOULDBLOCK
    };
#else
    #define SOCKET_ERROR    (-1)

    enum
    {
        ERR_INTR = EINTR,
        ERR_WOULDBLOCK = EWOULDBLOCK
    };

#endif //WIN32




SockAddr::SockAddr(const SockAddr& addr):
    m_sockaddr(addr.m_sockaddr),
    m_socklen(addr.m_socklen)
{

}

SockAddr& SockAddr::operator = (const SockAddr& addr)
{
    m_sockaddr = addr.m_sockaddr;
    m_socklen = addr.m_socklen;
    return (*this);
}

bool SockAddr::operator == (const SockAddr& addr)
{
    if (m_socklen != addr.m_socklen)
    {
        return false;
    }
    int ret = memcmp(&m_sockaddr, &addr.m_sockaddr, m_socklen);
    return (ret == 0);
}

SockAddr::SockAddr(const std::string& host, int port):
        m_sockaddr(),
        m_socklen(sizeof(m_sockaddr))
{
    set(host.c_str(), port);
}

SockAddr::SockAddr(const char * pHost, int port):
        m_sockaddr(),
        m_socklen(sizeof(m_sockaddr))
{
    set(pHost, port);
}

void SockAddr::set(const char * pHost, int port)
{
    std::string dotstring;
    if (!pHost)
    {
        pHost = "0.0.0.0";
    }
    else if (strlen(pHost) == 0)
    {
        pHost = "0.0.0.0";
    }
    else if (!SockAddr::isDotstring(pHost))
    {
        dotstring = SockAddr::host2DotString(pHost);
        pHost = dotstring.c_str();
    }

    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_addr.s_addr = inet_addr(pHost);
    m_sockaddr.sin_port = htons(port);
    m_socklen = (unsigned int) sizeof(m_sockaddr);
}

void SockAddr::set(const std::string& host, int port)
{
    set(host.c_str(), port);
}

void SockAddr::set(unsigned int addr, int port)
{
    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_addr.s_addr = htonl(addr);
    m_sockaddr.sin_port = htons(port);
    m_socklen = (unsigned int) sizeof(m_sockaddr);
}

void SockAddr::set(int addr, int port)
{
    set((unsigned int)addr, port);
}

void SockAddr::setPort(int port)
{
    m_sockaddr.sin_port = htons(port);
}

const char * SockAddr::getIPAddr() const
{
    return inet_ntoa(m_sockaddr.sin_addr);
}

unsigned int SockAddr::getAddr() const
{
    return ntohl(m_sockaddr.sin_addr.s_addr);
}

int SockAddr::getPort() const
{
    return ntohs(m_sockaddr.sin_port);
}

size_t SockAddr::toString(char* buffer, size_t length)
{
    #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    return sprintf_s(buffer, length, "%s:%d", getIPAddr(), getPort());
    #else
    return sprintf(buffer, "%s:%d", getIPAddr(), getPort());
    #endif //
}

const char* SockAddr::getHostIP()
{
    char buff[MAX_PATH];
    int ret = ::gethostname(buff, MAX_PATH);
    if (0 == ret)
    {
        hostent *host = ::gethostbyname(buff);
        if (host != NULL)
        {
            return inet_ntoa(*(in_addr*) host->h_addr_list[0]);
        }
    }
    return "127.0.0.1";
}

const char* SockAddr::getLocalIP()
{
#ifdef WIN32
    return getHostIP();
#else
    int ret = 0;
    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, "eth0");
    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0)
    {
        strcpy(ifr.ifr_name, "eth1");
        ret = ioctl(inet_sock, SIOCGIFADDR, &ifr);
    }
    ::close(inet_sock);

    if (0 == ret)
    {
        struct in_addr& addr = ((struct sockaddr_in*)&(ifr.ifr_ifru.ifru_addr))->sin_addr;
        return inet_ntoa(addr);
    }
    else
    {
        return "127.0.0.1";
    }

#endif //WIN32
}

int SockAddr::getAddr(const char *hostname, struct in_addr *addr)
{
    int ret = 0;
    struct hostent *h;

    h = ::gethostbyname(hostname);
    if (h != 0)
    {
        assert(h->h_addrtype == AF_INET);
        *addr = *(struct in_addr*) (h->h_addr);
    }
    else
    {
        ret = SOCKET_ERROR;
    }
    return ret;
}

int SockAddr::lookup(const char *hostname, struct sockaddr_in *sin)
{
    int ret = getAddr(hostname, &sin->sin_addr);
    sin->sin_family = AF_INET;
    return ret;
}

bool SockAddr::isDotstring(const std::string& host)
{
    bool ok = true;
    for (size_t i = 0; i < host.size(); ++i)
    {
        if (host[i] != '.')
        {
            if (host[i] < '0' || host[i] > '9')
            {
                ok = false;
                break;
            }
        }
    }
    return ok;
}

const char* SockAddr::host2DotString(const std::string& host)
{
    if (isDotstring(host))
    {
        return host.c_str();
    }
    else
    {
        sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        lookup(host.c_str(), &sin);

        struct
        {
            unsigned char s_b1, s_b2, s_b3, s_b4;
        } tmp;
        memcpy(&tmp, &sin.sin_addr, sizeof(tmp));

        return inet_ntoa(sin.sin_addr);
    }
}

const char* SockAddr::ntoa(unsigned long addr)
{
    struct in_addr inAddr;
    inAddr.s_addr = addr;
    return inet_ntoa(inAddr);
}


///////////////////////////////////////////////////////////////////
bool Socket::startup()
{
#ifdef WIN32
    WORD versionWanted = MAKEWORD(2,2);
    WSADATA wsaData;
    return (WSAStartup(versionWanted, &wsaData) != 0);
#else //assume linux/unix
    // SIGPIPE is generated when a remote socket is closed
    void (*handler)(int);
    handler = signal(SIGPIPE, SIG_IGN);
    if(handler != SIG_DFL)
    {
        signal(SIGPIPE, handler);
    }
    return true;
#endif
}

void Socket::cleanup()
{
#ifdef WIN32
    if (WSACleanup() == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAEINPROGRESS)
        {
            WSACancelBlockingCall();
            WSACleanup();
        }
    }
#else //assume linux/unix
    // Restore the SIGPIPE handler
    void (*handler)(int);
    handler = signal(SIGPIPE, SIG_DFL);
    if(handler != SIG_IGN)
    {
        signal(SIGPIPE, handler);
    }
#endif
}

int Socket::getLastError()
{
#ifdef WIN32
    return ::WSAGetLastError();
#else
    return errno;
#endif
}

void Socket::clearLastError()
{
#if defined(WIN32) || defined(__CYGWIN__)
    WSASetLastError(0);
#else
    errno = 0;
#endif
}

std::string Socket::getLastErrorMessage(int err)
{
#ifdef WIN32
    char* lpMsgBuf = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL);
    std::string msg(lpMsgBuf);
    LocalFree(lpMsgBuf);

    return msg;
#else
    return std::string();
#endif //WIN32
}


bool Socket::isInterrupt()
{
    return (ERR_INTR == getLastError());
}

bool Socket::isWouldBlock(int err)
{

#ifdef WIN32
    return (WSAEWOULDBLOCK == err);
#else
    return (EINPROGRESS == err || EWOULDBLOCK == err);
#endif//WIN32
}


///////////////////////////////////////////////////////////////////
Socket::Socket()
{
    m_socket = INVALID_SOCKET;
}

Socket::~Socket()
{

}

Socket::Socket(socket_t sock)
{
    m_socket = sock;
}

void Socket::attach(const Socket& sock)
{
    m_socket = sock.m_socket;
}

void Socket::attach(socket_t sock)
{
    m_socket = sock;
}

socket_t Socket::detach()
{
    socket_t theSocket = m_socket;
    m_socket = INVALID_SOCKET;
    return theSocket;
}

bool Socket::open(int type)
{
    assert(!isOpen());

    m_socket = ::socket(AF_INET, type, 0);
    return (INVALID_SOCKET != m_socket);
}

void Socket::close()
{
    if (isOpen())
    {
        ::shutdown(m_socket, 0x02);
#if defined(WIN32) && !defined(__CYGWIN32__)

        ::closesocket(m_socket);
#else
        ::close(m_socket);
#endif
        m_socket = INVALID_SOCKET;
    }
}

int Socket::bind(const SockAddr& addr)
{
    return ::bind(m_socket, (sockaddr*) &addr.m_sockaddr, addr.m_socklen);
}

int Socket::connect(const SockAddr& addr)
{
    return ::connect(m_socket, (sockaddr*) &addr.m_sockaddr, addr.m_socklen);
}

int Socket::connect(const char * hostName, int port)
{
    SockAddr addr(hostName, port);
    return connect(addr);
}

Socket Socket::accept(SockAddr& addr)
{
    socklen_t len = sizeof(addr);
    socket_t sock = ::accept(m_socket, (sockaddr*) &addr.m_sockaddr, &len);
    return Socket(sock);
}

int Socket::listen(int backlog)
{
    return ::listen(m_socket, backlog);
}

int Socket::receive(char * buf, int size, int flag)
{
    return ::recv(m_socket, buf, size, flag);
}

int Socket::send(const char * buf, int size, int flag)
{
    return ::send(m_socket, buf, size, flag);
}

int Socket::receiveFrom(char* buf, int size, int flag, SockAddr& addr)
{
    socklen_t len = sizeof(addr);
    return ::recvfrom(m_socket, buf, size, flag, (sockaddr*) &addr.m_sockaddr,
            &len);
}

int Socket::sendTo(const char* buf, int size, int flag, const SockAddr& addr)
{
    return ::sendto(m_socket, buf, size, flag, (sockaddr*) &addr.m_sockaddr,
            (int) addr.m_socklen);
}

int Socket::shutDown(int flag)
{
    return ::shutdown(m_socket, flag);
}

bool Socket::setBroadcast(bool enable)
{
    int val = enable ? 1 : 0;
    int ret = ::setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char *) &val,
            sizeof(val));
    return (0 == ret);
}

bool Socket::setNoLinger(bool enable)
{
    struct linger l;
    l.l_onoff = enable ? 1 : 0;
    l.l_linger = 0;
    int ret = ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char *) &l,
            sizeof(l));
    return (0 == ret);
}

bool Socket::setKeepAlive(bool enable, int idle, int interval, int count)
{
    int val = enable ? 1 : 0;
    int ret = ::setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char *) &val,
            sizeof(val));
#ifdef WIN32
    tcp_keepalive keepalive;
    keepalive.onoff = enable ? 1 : 0;
    keepalive.keepalivetime = idle * 1000;
    keepalive.keepaliveinterval = interval * 1000;

    DWORD dwReturn = 0;
    ret = ::WSAIoctl(m_socket, SIO_KEEPALIVE_VALS, &keepalive, sizeof(keepalive),
            NULL, 0, &dwReturn, NULL, NULL);

#else
    ::setsockopt(m_socket, SOL_TCP, TCP_KEEPIDLE, (void*)&idle,
            sizeof(idle));
    ::setsockopt(m_socket, SOL_TCP, TCP_KEEPINTVL, (void*)&interval,
            sizeof(interval));
    ::setsockopt(m_socket, SOL_TCP, TCP_KEEPCNT, (void*)&count,
            sizeof(count));
#endif //WIN32
    return (0 == ret);
}

int Socket::getSendBufferSize()
{
    int len = 0;
    socklen_t sizeInt = (int) sizeof(len);
    int ret = ::getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char *) &len,
            &sizeInt);
    if (0 != ret)
    {
        len = -1;
    }
    return len;
}

int Socket::getRecvBufferSize()
{
    int len = 0;
    socklen_t sizeInt = sizeof(len);
    int ret = ::getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char *) &len,
            &sizeInt);
    if (0 != ret)
    {
        len = -1;
    }
    return len;
}

bool Socket::setSendBufferSize(int size)
{
    int ret = ::setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char *) &size,
            sizeof(size));
    return (0 == ret);
}

bool Socket::setRecvBufferSize(int size)
{
    int ret = ::setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char *) &size,
            sizeof(size));
    return (0 == ret);
}

SockAddr Socket::getPeerName() const
{
    SockAddr addr;
    socklen_t len = sizeof(addr);
    ::getpeername(m_socket, (sockaddr*) &addr.m_sockaddr, &len);
    return addr;
}

SockAddr Socket::getSockName() const
{
    SockAddr addr;
    socklen_t len = sizeof(addr);
    ::getsockname(m_socket, (sockaddr*) &addr.m_sockaddr, &len);
    return addr;
}

bool Socket::setNonblock(bool enable)
{
    int ret = 0;
#ifdef WIN32
    unsigned long fl = enable ? 1 : 0;
    ret = ioctlsocket(m_socket, FIONBIO, &fl);
#else
    int fl = fcntl(m_socket, F_GETFL);
    if (fl >= 0)
    {
        ret = fcntl(m_socket, F_SETFL, fl | O_NONBLOCK);
    }
#endif
    return (ret >= 0);
}

bool Socket::setReuse()
{
    int ret = 0;

	const int one = 1;
#ifdef SO_REUSEADDR
    ret = ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &one,
            sizeof(one));
#endif

#ifdef SO_REUSEPORT
    ret = ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT,
            (const void*)&one, sizeof(one));

#endif
    return (ret >= 0);
}

bool Socket::setSendTimeout(int milliseconds)
{
#ifdef WIN32
    int ret = setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO,
            (const char*) &milliseconds, sizeof(int));
#else
    timeval tv = {   milliseconds / 1000, (milliseconds % 1000) * 1000};
    int ret = setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO,
            (const char*)&tv, sizeof(tv));
#endif //WIN32
    return (0 == ret);
}

bool Socket::setRecvTimeout(int milliseconds)
{
#ifdef WIN32
    int ret = setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,
            (const char*) &milliseconds, sizeof(int));
#else
    timeval tv = {   milliseconds / 1000, (milliseconds % 1000) * 1000};
    int ret = setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,
            (const char*)&tv, sizeof(tv));
#endif //WIN32
    return (0 == ret);
}

bool Socket::checkReadable(long millsec)
{
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(m_socket, &rset);
    timeval* ptv = NULL;
    timeval tv = { millsec / 1000, (millsec % 1000) * 1000 };
    ptv = (millsec >= 0) ? (&tv) : NULL;

    int ret = select((int) m_socket + 1, &rset, NULL, NULL, ptv);
    return (1 == ret);
}

int Socket::checkReadAndClose(long millsec)
{
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(m_socket, &rset);
    timeval* ptv = NULL;
    timeval tv = { millsec / 1000, (millsec % 1000) * 1000 };
    ptv = (millsec >= 0) ? (&tv) : NULL;

    int ret = select((int) m_socket + 1, &rset, NULL, NULL, ptv);
    if (ret == 1)
    {
        char buffer[8];
        int bytes = ::recv(m_socket, buffer, 8, MSG_PEEK);
        if (bytes <= 0)
        {
            ret = -1;
        }
    }
    return ret;
}


bool Socket::checkWritable(long millsec)
{
    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(m_socket, &wset);
    timeval* ptv = NULL;
    timeval tv = { millsec / 1000, (millsec % 1000) * 1000 };
    ptv = (millsec >= 0) ? (&tv) : NULL;

    int ret = select((int) m_socket + 1, NULL, &wset, NULL, ptv);
    return (1 == ret);
}

int Socket::sendBlock(const char * buf, int len)
{
    int length = len;
    int bytes = 0;
    do
    {
        bytes = send(buf, length, 0);
        if (bytes > 0)
        {
            length -= bytes;
            buf += bytes;
        }
        else if (SOCKET_ERROR == bytes)
        {
            if (ERR_INTR == Socket::getLastError())
            {
                continue;
            }
        }
    } while (bytes > 0 && length > 0);
    return (len - length);
}

int Socket::receiveBlock(char* buf, int len)
{
    int length = len;
    int bytes = 0;
    do
    {
        bytes = receive(buf, length);
        if (bytes > 0)
        {
            length -= bytes;
            buf += bytes;
        }
        else if (SOCKET_ERROR == bytes)
        {
            if (ERR_INTR == Socket::getLastError())
            {
                continue;
            }
        }
    } while (bytes > 0 && length > 0);
    return (len - length);
}

int Socket::checkWriteAndException(long millsec)
{
    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(m_socket, &wset);

    fd_set setEx;
    FD_ZERO(&setEx);
    FD_SET(m_socket, &setEx);

    timeval* ptv = NULL;
    timeval tv = { millsec / 1000, (millsec % 1000) * 1000 };
    ptv = (millsec >= 0) ? (&tv) : NULL;

    int ret = select((int) m_socket + 1, NULL, &wset, &setEx, ptv);
    if (ret > 0)
    {
        if (FD_ISSET(m_socket, &setEx))
        {
            ret = -1;
        }
    }
    return ret;
}

int Socket::checkConnect(long millsec)
{
#ifdef WIN32
    return checkWriteAndException(millsec);
#endif //WIN32
    fd_set rset;
    fd_set wset;
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_SET(m_socket, &rset);
    FD_SET(m_socket, &wset);

    timeval* ptv = NULL;
    timeval tv = { millsec / 1000, (millsec % 1000) * 1000 };
    ptv = (millsec >= 0) ? (&tv) : NULL;

    int ret = select((int) m_socket + 1, NULL, &wset, NULL, ptv);
    if (ret > 0)
    {
        int error = 0;
        socklen_t len = sizeof(error);
        getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char*) &error, &len);
        ret = (error == 0) ? 1 : -1;
    }
    return ret;
}

}
