/*-----------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "up_sys_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <net/if.h>


#include "up_types.h"
#include "up_sys_log.h"
#include "up_sys_sock.h"



/*-----------------------------------------------------------------------------
 * Marcos, Types, and Structures
 *----------------------------------------------------------------------------*/
#define SOCKET_MAX_SESSION_NUMBER  10
/*-----------------------------------------------------------------------------
 * Types, and Structures
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * static variables & functions
 *----------------------------------------------------------------------------*/
static int m_upsock_sendBuf_size =4096;
static int m_upsock_recvBuf_size = 0;

//#define UBLOG_DEBUG_UP_SYS_SOCK
#ifndef UBLOG_DEBUG_UP_SYS_SOCK
#undef  UPLOG
#define UPLOG(function,parameters)
#undef  UPLOG_ERROR
#define UPLOG_ERROR(function,parameters)
#endif

/*-----------------------------------------------------------------------------
 * External Function Prototypes
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Local Function Prototypes
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Global functions
 *----------------------------------------------------------------------------*/
UPSint32 up_sock_getMaxSessions(void)
{
	return SOCKET_MAX_SESSION_NUMBER;
}

//providing the socket is Async-ed
UPResult up_sock_bind(UPSocketHandle socket, int port)
{

	return UPSuccess;
}

//providing the socket is Async-ed
UPSocketHandle up_sock_accept(UPSocketHandle socket)
{

	return UP_NULL;
}

static const char *net_inet_ntop(struct sockaddr_storage *src, char *dst, size_t size)
{
	
	return UP_NULL;
}

//UPSint32 up_sock_async_accept(UPSocketHandle socket, char** peerip)
//{
//	int ret=0;
//
//	return ret;
//}

/** \brief try to open a tcp socket */
UPSocketHandle up_sock_open(void)
{
	int socket_id;
	int len;
	UPLOG(up_sock_open,("Enter"))

		socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(socket_id < 0)
	{
		return (UPSocketHandle)-1;
	}

	//set the recv and send buffer size
	len = 16 * 1024;
	setsockopt((int)socket_id, SOL_SOCKET, SO_SNDBUF, (const char*)&len, sizeof(len));
	setsockopt((int)socket_id, SOL_SOCKET, SO_RCVBUF, (const char*)&len, sizeof(len));
	return (UPSocketHandle)socket_id;
}

UPSint32 up_sock_connect(UPSocketHandle socket, const UPSint8* address, UPSint32 addressLength, UPSint32 port, UPSint32 timout, UPBool DNSNeeded)
{
	int ret;
	unsigned long flags;
	struct sockaddr_in him;
	struct hostent* hp = NULL;
	fd_set readfds, *writefds, *exceptfds;
	struct timeval timeout;
	int len = sizeof(struct sockaddr_in);

	/* 
	   if start connect sucessfully, return UP_SOCKET_SUCCESS, else return UP_SOCKET_ERROR_OTHER
	   notice that at this time the connect operation could control duration 
	 */
	UPLOG(up_sock_connect,("Enter"))

		if(address == NULL || addressLength <= 0)
		{
			return UP_SOCKET_ERROR_OTHER;
		}

	//set the connect timeout mechansim
	/* set the socket as non block */
	flags = 1;
	ioctl((int)socket, FIONBIO, &flags);
	memset((void*)&him, 0, sizeof(him));

	him.sin_family = AF_INET;
	him.sin_port   = htons((short)port);

	if(DNSNeeded)
	{
		hp = gethostbyname((const char*)address);
		if(hp == NULL)
		{
			ret = UP_SOCKET_ERROR_URLRESOL;
			goto end_of_connect;
		}
	}

	if(hp != NULL)
	{
		/* server address is presented by IPv4 address*/
		//him.sin_addr.s_addr = inet_addr((const char*)address);
		memcpy((void*)&(him.sin_addr), hp->h_addr, hp->h_length);
		him.sin_family = hp->h_addrtype;
	}
	else
	{
		/* error happened */
		him.sin_addr.s_addr = inet_addr((const char*)address);
	}

	ret = connect((int)socket, (struct sockaddr *)&him, len);

	if(ret == 0)
	{
		//the connection has been created
		ret = UP_SOCKET_SUCCESS;
		goto end_of_connect;
	}

	if(ret == -1 && errno == EINPROGRESS)
	{
		//should check the read and write events
		FD_ZERO(&readfds);
		FD_SET ((int)socket, &readfds);

		/*
		   FD_ZERO(&writefds);
		   FD_SET ((int)socket, &writefds);

		   FD_ZERO(&exceptfds);
		   FD_SET ((int)socket, &exceptfds);
		 */

		writefds  = &readfds;
		exceptfds = &readfds;

		//select to set the timeout
		timeout.tv_sec  = timout / 1000;
		timeout.tv_usec = (timout % 1000) * 1000;

		ret = select((int)socket + 1, &readfds, writefds, exceptfds, &timeout);
		UPLOG(up_sock_connect, ("RET = %d, error = %d", ret, errno))
			if(ret <= 0 || ret != 1)
			{
				ret= UP_SOCKET_ERROR_OTHER;
			}
			else
			{
				ret = UP_SOCKET_SUCCESS;
			}
	}
	else
	{
		ret = UP_SOCKET_ERROR_OTHER;
	}

end_of_connect:
	flags = 0;
	ioctl((int)socket, FIONBIO, &flags);
	return ret;
}

UPSint32 up_sock_send(UPSocketHandle socket, const UPSint8* data, UPSint32 dataLength)
{
	/*
	   if a WSASend request suceess, return UP_SOCKET_SUCCESS, else return UP_SOCKET_ERROR_OTHER
	   notice that this step is only to startup the overlap send operation, the data may not actually being sent.
	 */
	int ret = 0;
	ret = send((int)socket, (char*)data, dataLength, 0);
	if(ret <= 0)
	{
		if(errno == EBADF || errno == ECONNRESET || errno == EPIPE)
		{
			ret = -1;
		}
		ret = -1;
	}
	else
	{
		//ret = 0;
	}
	return ret;
}

UPSint32 up_sock_receive(UPSocketHandle socket, UPSint8* data, UPSint32 dataLength)
{
	return recv((int)socket, (char*)data, dataLength, 0);
}

/** \brief for recv, it detects how many data in the socket recv buffer,
 *        for send, it detects how many data could put int the socket send buffer
 */
UPSint32 up_sock_available(UPSocketHandle socket, UPSint type, UPSint timeout)
{
	int ret = 0, readAble = 0;
	//check the socket is writeable or not
	fd_set tbl;
	struct timeval t;
	int fd = (int)socket;

	UPLOG(up_sock_available,("Enter"))

		//check the availablity
		if((fd < 0) || (fd >= FD_SETSIZE))
		{
			return UP_SOCKET_ERROR_OTHER;
		}

	t.tv_sec  = timeout / 1000;
	t.tv_usec = (timeout % 1000) * 1000;

	FD_ZERO(&tbl);
	FD_SET(fd, &tbl);

	switch(type)
	{
		case UP_SOCKET_OPERATION_TYPE_SEND:
			ret = select((int)socket + 1, NULL, &tbl, NULL, &t);
			break;
		case UP_SOCKET_OPERATION_TYPE_RECEIVE:
			ret = select((int)socket + 1, &tbl, NULL, NULL, &t);
			break;
		default:
			ret = select((int)socket + 1, &tbl, &tbl, NULL, &t);
			break;
	}
	if(ret == -1)
	{
		return UP_SOCKET_ERROR_OTHER;
	}
	else if(ret == 0)
	{
		UPLOG(up_sock_available,("Time out when waiting"))
			return UP_SOCKET_ERROR_TIMEOUT;
	}
	else
	{
		if(FD_ISSET(fd, &tbl) == 0)
		{
			UPLOG(up_sock_available,("No data on this socket"))
				return UP_SOCKET_ERROR_TIMEOUT;
		}
	}

	//get the available data
	switch(type)
	{
		case UP_SOCKET_OPERATION_TYPE_SEND:
			ret = 0;
			//return the available buffer for send
			return (up_sock_getoption(socket, UP_SOCKET_OPTION_SNDBUF) - ret);

		case UP_SOCKET_OPERATION_TYPE_RECEIVE:
			if(ioctl((int)socket, FIONREAD, (u_long*)&readAble) < 0)
			{
				ret = UP_SOCKET_ERROR_OTHER;
			}

			if(readAble == 0 && ret > 0)
			{
				//should due to the connection is dropped
				readAble = UP_SOCKET_ERROR_DROPLINE;
			}

			return readAble;

		default:
			UPLOG(up_sock_available,("Type(%d) is invalid", type))
				return UP_SOCKET_ERROR_OTHER;
	}
}

UPSint32 up_sock_setoption(UPSocketHandle socket, UPSint32 option, UPSint32 value)
{
	UPLOG(up_sock_setoption,("Enter"))
#if 1
		switch(option)
		{
			case UP_SOCKET_OPTION_KEEPALIVE:
				//get the send buffer size
				if(0 != setsockopt((int)socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&value, sizeof(value)))
				{
					return UP_SOCKET_ERROR_OTHER;
				}
				return UP_SOCKET_SUCCESS;
			case UP_SOCKET_OPTION_NODELAY:
				//get the send buffer size
				/*
				   if(0 != setsockopt((int)socket, SOL_SOCKET, TCP_NODELAY, (const char*)&value, sizeof(value)))
				   {
				   return UP_SOCKET_ERROR_OTHER;
				   }
				 */
				return UP_SOCKET_SUCCESS;
			case UP_SOCKET_OPTION_REUSEADDR:
				//get the send buffer size
				if(0 != setsockopt((int)socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&value, sizeof(value)))
				{
					return UP_SOCKET_ERROR_OTHER;
				}
				return UP_SOCKET_SUCCESS;
		}
#endif
	return UP_SOCKET_ERROR_OTHER;
}

UPSint32 up_sock_getoption(UPSocketHandle socket, UPSint32 option)
{
	int ret, size;
	UPLOG(up_sock_getoption,("Enter"))

		size = sizeof(ret);
	switch(option)
	{
		case UP_SOCKET_OPTION_SNDBUF:
			//get the send buffer size
			if(m_upsock_sendBuf_size <= 0)
			{
				getopt((int)socket, SOL_SOCKET, SO_SNDBUF, (char*)&ret, &size);
				m_upsock_sendBuf_size = ret;
			}
			else
			{
				ret = m_upsock_sendBuf_size;
			}
			return ret;

		case UP_SOCKET_OPTION_RCVBUF:
			//get the recv buffer size
			if(m_upsock_recvBuf_size <= 0)
			{
				getopt((int)socket, SOL_SOCKET, SO_RCVBUF, (char*)&ret, &size);
				m_upsock_recvBuf_size = ret;
			}
			else
			{
				ret = m_upsock_recvBuf_size;
			}
			return ret;
	}
	return UP_SOCKET_ERROR_OTHER;
}

UPSint32 up_sock_shutdown(UPSocketHandle socket, UPSint32 how)
{
	int ret, howto;
	UPLOG(up_sock_shutdown,("Enter"))

		if(socket == UP_INVALID_HANDLE) return UP_SOCKET_ERROR_OTHER;

	switch(how)
	{
		case UP_SOCKET_SHUTDOWN_RECEIVE:
			howto = SHUT_RD;
			break;
		case UP_SOCKET_SHUTDOWN_SEND:
			howto = SHUT_WR;
			break;
		case UP_SOCKET_SHUTDOWN_BOTH:
			howto = SHUT_RDWR;
			break;
		default:
			return UP_SOCKET_ERROR_OTHER;
	}

	ret = shutdown((int)socket, howto);

	if(ret < 0)
	{
		ret = UP_SOCKET_ERROR_OTHER;
	}
	else
	{
		ret = UP_SOCKET_SUCCESS;
	}

	return ret;
}

UPSint32 up_sock_close(UPSocketHandle socket)
{
	UPLOG(up_sock_close,("Enter"))

		if(socket == UP_INVALID_HANDLE) return UP_SOCKET_ERROR_OTHER;

	if(socket && close((int)socket) < 0)
	{
		return UP_SOCKET_ERROR_OTHER;
	}
	return UP_SOCKET_SUCCESS;
}
