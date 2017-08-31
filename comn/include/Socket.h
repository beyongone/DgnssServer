/*
 * Socket.h
 *
 *  Created on: 2012-1-10
 *      Author: terry
 */

#ifndef SOCKET_H_
#define SOCKET_H_

//#include "BasicType.h"
#include <string>

#ifdef WIN32
#include <Winsock2.h>
typedef SOCKET  socket_t;

#else
    #include <netinet/in.h>
	#include <sys/types.h>
    #include <sys/socket.h>
	typedef int socket_t;
    #define INVALID_SOCKET  -1
#endif //WIN32

///////////////////////////////////////////////////////////////////

namespace comn
{


/// socket address
class  SockAddr
{
public:
    struct sockaddr_in  m_sockaddr;
    unsigned int        m_socklen;

    SockAddr(const SockAddr& addr);

    SockAddr& operator = (const SockAddr& addr);

    bool operator == (const SockAddr& addr);

    SockAddr( const char * pHost = 0, int port = 0 );
    SockAddr( const std::string& host, int port = 0 );

    void set( const char * pHost = 0, int port = 0 );

    void set( const std::string& host, int port = 0 );

    /**
     * set address
     * @param addr can be host byte order and special addresses(INADDR_ANY, INADDR_BROADCAST ...)
     * @param port
     */
    void set( unsigned int addr, int port );

    void set( int addr, int port );

    void setPort( int port );

    unsigned int   getAddr() const;

    int getPort() const;

    const char * getIPAddr() const;

    size_t toString(char* buffer, size_t length);


    /**
     * get host IP address, return "127.0.0.1" on Linux
     */
    static const char* getHostIP();

    /**
     * get local host IP address. return eth0 on Linux
     */
    static const char* getLocalIP();

    /**
     * lookup address, return -1 on failed
     */
    static int getAddr(const char *hostname, struct in_addr *addr);

    /**
     * lookup address, return -1 on failed
     */
    static int lookup(const char *hostname, struct sockaddr_in *sin);

    static bool isDotstring(const std::string& host);

    /**
     * translate address to dot string
     */
    static const char*  host2DotString(const std::string& host);

    static const char* ntoa(unsigned long addr);

};




class  Socket
{
public:
    static bool startup();
    static void cleanup();

    static int getLastError();
    static void clearLastError();
    static std::string getLastErrorMessage(int err);

    static bool isInterrupt();
    static bool isWouldBlock(int err);

public:
    Socket();
    ~Socket();

    explicit Socket(socket_t sock);

    /// attach socket fd
    void attach(socket_t sock);

    void attach(const Socket& sock);

    /// detach and return socket fd
    socket_t detach();

    socket_t getHandle() const
    {
        return m_socket;
    }

    /// return true if the socket is valid
    bool isOpen() const
    {
        return (INVALID_SOCKET != m_socket);
    }


    /**
     * open socket.
     * @param type SOCK_STREAM for TCP and SOCK_DGRAM for UDP, TCP is default
     */
    bool open(int type = SOCK_STREAM);

    void close();

    /**
     * bind address.
     * @return on success, 0 is returned
     */
    int bind(const SockAddr& addr);

    /**
     * connect to server.
     * @return 0 on success
     */
    int connect(const SockAddr& addr);
    int connect(const char * hostName, int port);

    /**
     * accept connection.
     * @param addr is address of remote peer
     * @return socket
     */
    Socket accept(SockAddr& addr);

    /**
     * listen.
     * @return 0 on success
     */
    int listen(int backlog = 1024);

    /**
     * receive data.
     * @param buf
     * @param size
     * @param flag
     * @ return length of received data
     */
    int receive(char * buf, int size, int flag = 0);

    /**
     * send data.
     * @param buf
     * @param size
     * @param flag
     * @return length of sent data
     */
    int send(const char * buf, int size, int flag = 0);

    int receiveFrom(char * buf, int size, int flag, SockAddr& addr);
    int sendTo(const char * buf, int size, int flag, const SockAddr& addr);

    /// shut down
    int shutDown(int flag);

    /// enable broadcast
    bool setBroadcast(bool enable);

    bool setNoLinger(bool enable);

    /**
     * keep alive
     * @param enable
     * @param idle  in second
     * @param interval
     * @param count
     */
    bool setKeepAlive(bool enable, int idle, int interval, int count);

    int getSendBufferSize();
    int getRecvBufferSize();
    bool setSendBufferSize(int size);
    bool setRecvBufferSize(int size);

    /// get address of remote peer
    SockAddr getPeerName() const;

    /// get local address
    SockAddr getSockName() const;

    /// set none block
    bool setNonblock(bool enable);

    /// set reuse
    bool setReuse();

    /**
     * set timeout.
     * @param milliseconds
     */
    bool setSendTimeout(int milliseconds);

    bool setRecvTimeout(int milliseconds);

    /**
     * check readable.
     * @param millsec timeout in millisecond, if millsec < 0, socket will be blocked until it is readable
     * @return true if the socket is readable
     */
    bool checkReadable(long millsec);

    /**
     * check readable and closed. return 1 on readable , 0 on timeout -1 on closed
     */
    int checkReadAndClose(long millsec);

    /**
     * check writable
     * @param millsec timeout in millisecond, if millsec < 0, socket will be blocked until it is writable
     * @return true if the socket is writable
     */
    bool checkWritable(long millsec);

    /**
     * send data block.
     * send all of the data except error occurs. used for TCP
     * @return length of sent data
     */
    int sendBlock(const char * buf, int len);

    /**
     * receive data block
     * receive all of the data, except error occurs. used for TCP
     * @return length of received data
     */
    int receiveBlock(char* buf, int len);

    /**
     * check write and exception
     * @param millsec timeout in millisecond, if millsec < 0, socket will be blocked until it is writable
     * @return 0 on timeout, 1 on writable, -1 on exception
     */
    int checkWriteAndException(long millsec);

    /**
     * check write and exception
     * @param millsec timeout in millisecond, if millsec < 0, socket will be blocked until it is writable
     * @return 0 on timeout, 1 on writable, -1 on exception
     */
    int checkConnect(long millsec);

protected:
    socket_t     m_socket;
};




class SocketContext
{
public:
    SocketContext()
    {
        Socket::startup();
    }

    ~SocketContext()
    {
        Socket::cleanup();
    }

private:
    // disable copy and assign
    SocketContext(const SocketContext& other);
    SocketContext& operator = (const SocketContext& other);

};








}

#endif /* SOCKET_H_ */
