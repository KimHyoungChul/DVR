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

#ifndef __SOCKC_H
#define __SOCKC_H

#include "stunt.h"
#include "client.h"

SOCKET boundsocket(struct sockaddr_in *addr);
int acceptclose(SOCKET sock, int timeoutms);
int connectwait(SOCKET *sock, struct sockaddr_in *laddr, struct sockaddr_in *raddr, int timeoutms, bool closefast);
int connectclose(SOCKET *sock, struct sockaddr_in *laddr, struct sockaddr_in *raddr, int timeoutms, int waitms);
int queryres(SOCKET serv, struct message *res, struct message *req);
int query(SOCKET serv, struct message *res, struct message *req);
SOCKET conn(struct sockaddr_in *raddr, struct sockaddr_in *laddr);
void hardclosesocket(SOCKET *s);
void reviveconn(SOCKET *serv);
int is_port(unsigned short t, port_type p);
unsigned short freeport(port_type p);

#endif
