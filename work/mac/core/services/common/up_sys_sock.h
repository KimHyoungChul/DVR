#ifndef UP_SYS_SOCK_HEADER
#define UP_SYS_SOCK_HEADER

#ifdef __cplusplus
extern "C"{
#endif

#include "up_types.h"

/**
 *   Socket handle type
 */
typedef void* UPSocketHandle;

typedef struct __UPSocketCheckList
{
	int isSelected;
	UPSocketHandle sock;
	void*          p;
	struct __UPSocketCheckList* next;
}UPSocketCheckList;

/**
 * \brief Socket maximum recv data size in one step
 *
 */
#define UP_SOCKET_RECV_BUFFER_MAX_SIZE      1024000

/**
 * \brief Socket connect result notification event.
 *
 * This is used for notifying of the result of #up_sock_connect processing.
 */
#define UP_SOCKET_OPERATION_TYPE_CONNECT     1
/**
 * \brief Socket send result notification event.
 *
 * This is used for notifying of the result of #up_sock_send processing.
 */
#define UP_SOCKET_OPERATION_TYPE_SEND        2
/**
 * \brief Socket receive result notification event.
 *
 * This is used for notifying of the result of #up_sock_receive processing.
 */
#define UP_SOCKET_OPERATION_TYPE_RECEIVE     3
/**
 * \brief Socket shutdown result notification event.
 *
 * This is used for notifying of the result of #up_sock_shutdown processing.
 */
#define UP_SOCKET_OPERATION_TYPE_SHUTDOWN    4
/**
 * \brief Socket close/destroy result notification event.
 *
 * This is used for notifying of the result of #up_sock_close processing.
 */
#define UP_SOCKET_OPERATION_TYPE_CLOSE       5
/**
 * \brief Socket send/recv result notification event.
 *
 * This is used for notifying of the result of #up_sock_available processing.
 */
#define UP_SOCKET_OPERATION_TYPE_BOTH        6

/* Socket error codes */
#define UP_SOCKET_SUCCESS                                        0
#define UP_SOCKET_ERROR_TIMEOUT                                  -20000
#define UP_SOCKET_ERROR_OTHER                                    -30000
#define UP_SOCKET_ERROR_DROPLINE                                 -30001
#define UP_SOCKET_ERROR_URLRESOL								 -30002

/* Shutdown methods */
/** 
 * \brief Shutdown receiving direction only. 
 */
#define UP_SOCKET_SHUTDOWN_RECEIVE       1
/** 
 * \brief Shutdown sending direction only. 
 */
#define UP_SOCKET_SHUTDOWN_SEND          2
/** 
 * \brief Shutdown both sending and receiving directions. 
 */
#define UP_SOCKET_SHUTDOWN_BOTH          3

/* Socket options */
/**
 * \brief Socket option: Wether to use Nagle algorithm. 
 */
#define UP_SOCKET_OPTION_NODELAY           1
/** 
 * \brief Socket option: Wether to send KEEPALIVE signal. 
 */
#define UP_SOCKET_OPTION_KEEPALIVE       2
/** 
 * \brief Socket option: Linger time (wait time before disconnecting socket). 
 */
#define UP_SOCKET_OPTION_LINGER          3
/** 
 * \brief Socket option: Receive buffer size in bytes. 
 */
#define UP_SOCKET_OPTION_RCVBUF          4
/** 
 * \brief Socket option: Send buffer size in bytes. 
 */
#define UP_SOCKET_OPTION_SNDBUF          5

/**
 * \brief Socket option:socket reused
 */
#define UP_SOCKET_OPTION_REUSEADDR		 6

#define MAX_DIALUP_COUNT (6)

//kelvin
#define UP_SOCKET_PROCESS_SEND		1
#define UP_SOCKET_PROCESS_RECEIVE	2
/*-----------------------------------------------------------------------------
 * Global functions
 *----------------------------------------------------------------------------*/
/**
 * \brief get the maximum sessions allocated for ubiLive engine on the platform
 */
UPSint32 up_sock_getMaxSessions(void);

UPSocketHandle up_sock_open(void);
UPSint32 up_sock_connect(UPSocketHandle socket, const UPSint8* address, UPSint32 addressLength, UPSint32 port, UPSint32 timout, UPBool DNSNeeded);
UPSint32 up_sock_send(UPSocketHandle socket, const UPSint8* data, UPSint32 dataLength);
UPSint32 up_sock_receive(UPSocketHandle socket, UPSint8* data, UPSint32 dataLength);
//kelvin
UPResult up_sock_cancel(UPSocketHandle socket, UPSint type);
UPSint32 up_sock_available(UPSocketHandle socket, UPSint type, UPSint timeout);
UPSint32 up_sock_setoption(UPSocketHandle socket, UPSint32 option, UPSint32 value);
UPSint32 up_sock_getoption(UPSocketHandle socket, UPSint32 option);
UPSint32 up_sock_shutdown(UPSocketHandle socket, UPSint32 how);
UPSint32 up_sock_close(UPSocketHandle socket);
UPResult up_sock_bind(UPSocketHandle socket, int port);
UPSocketHandle up_sock_accept(UPSocketHandle socket);


/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* UP_SYS_SOCK_HEADER */
