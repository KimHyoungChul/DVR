/**
 * stunt discovers TCP parameters of NATs
 * Copyright (C) 2005  saikat Guha
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/

#ifndef __COMPAT_H
#define __COMPAT_H




#ifdef LINUX

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#undef __USE_MISC
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pcap.h>
#include <pthread.h>

#define debug(s, ...)       printf(s "\n", ##__VA_ARGS__)
#define trace(s, ...)       printf("%s:%s:%d: " s "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define cp(s)       "[33m" s "[0m"
#define ca(s)       "[0m" s "[33m"
#define cc(s)       "[31m" s "[33m"
#define ce(s)       "[36m" s "[33m"
#define S_addr      s_addr

#define closesocket(s)     do { close(s); s = -1; } while(0)
#define errnum      errno
#define SO_DO_LINGER    1
#define SO_DONT_LINGER  0

#define THREAD_START    void *
#define PTHREAD_ERROR       -1

#define S_IREAD    S_IRUSR
#define S_IWRITE   S_IWUSR
#define S_IEXEC    S_IXUSR
#define O_BINARY   0


#define fwrite              write

typedef int SOCKET;

#endif

#ifdef WIN32

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include <conio.h>
#include "pcap.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "wpcap")
#pragma comment(lib, "Rpcrt4")

#define     debug           printf
#define     trace           printf

#define cp(s)       s
#define ca(s)       s
#define cc(s)       s
#define ce(s)       s
#undef s_addr
#define S_addr  S_un.S_addr

#define snprintf     _snprintf
#define random       rand
#define srandom      srand
#define read(s,b,l)       recv((s), (char *)(b), (l), 0)
#define write(s,b,l)       send((s), (char *)(b), (l), 0)
#define closesocket(s)     do { closesocket(s); s = -1; } while(0)

#define sleep(s)   _sleep((s)*1000)
#define usleep(s)  _sleep((s)/1000)

#define EADDRNOTAVAIL   WSAEADDRINUSE
#define SO_DO_LINGER    SO_LINGER
#define SO_DONT_LINGER    SO_DONTLINGER

#define pthread_t HANDLE
#define pthread_create(x, y, z, a) (*x = CreateThread(NULL, 0, z, a, 0, NULL))
#define pthread_exit(s)     ExitThread(s);
#define pthread_join(t,r)   WaitForSingleObject((t),INFINITE)
#define pthread_cancel(t)   TerminateThread((t), 0);
#define THREAD_START        DWORD WINAPI
#define PTHREAD_ERROR       NULL

#define fwrite              _write

#define errnum   (*_my_errno())
extern int my_errno;
int *_my_errno();

#define IFNAMSIZ            256

#endif





#endif

