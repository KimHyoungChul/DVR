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

#include "util.h"
#include "client.h"
#include "print.h"
#include "sockc.h"

#include <sys/types.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>

SOCKET boundsocket(struct sockaddr_in *addr) {
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int addrlen = sizeof(*addr);
    static const int one = 1;

    if (addr->sin_family != AF_INET) {
        addr->sin_family = AF_INET;
        addr->sin_addr.S_addr = inet_addr(laddress[0]);
        addr->sin_port = 0;
    }
    if (sock == -1 ||
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)) == -1 ||
        bind(sock, (struct sockaddr *)addr, sizeof(*addr)) == -1 ||
        getsockname(sock, (struct sockaddr *)addr, &addrlen) == -1) {

        closesocket(sock);
        error(-1, "creating and binding socket");

    }
    return sock;

}

int acceptclose(SOCKET sock, int timeoutms) {
    struct timeval t;
    fd_set rfds;
    int start = time(NULL);
    int timeout = timeoutms / 1000;

    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);
    t.tv_usec = 0;

    while(1) {
        fd_set _rfds = rfds;

        t.tv_sec = (start + timeout) - time(NULL);
        if (t.tv_sec < 0) {
            break;
        }

        if (select((int)sock + 1, &_rfds, NULL, NULL, &t) == 0) break;

        if (FD_ISSET(sock, &_rfds)) {
            SOCKET c = accept(sock, NULL, 0);
            closesocket(c);
        }
    }
    return 0;
}

int connectwait(SOCKET *sock, struct sockaddr_in *laddr, struct sockaddr_in *raddr, int timeoutms, bool closefast) {
    int start = time(NULL);
    int timeout = timeoutms / 1000;
    int remain;
    connect(*sock, (struct sockaddr *)raddr, sizeof(*raddr));
    if (closefast == TRUE) closesocket(*sock);
    remain = start + timeout - time(NULL);
    if (remain >  0) sleep(remain);
    closesocket(*sock);
    return 0;
}

int connectclose(SOCKET *sock, struct sockaddr_in *laddr, struct sockaddr_in *raddr, int timeoutms, int waitms) {
    int start = time(NULL);
    int r, i = 0;
    int attime[] = {0, 3, 9, 21};
    int timeout = timeoutms / 1000;

    do {
        if (*sock == -1)
            *sock = boundsocket(laddr);
        r = start + attime[i++] - time(NULL);
        if (r > 0) {
            usleep(r * 1000000);
        }

        r = connect(*sock, (struct sockaddr *)raddr, sizeof(*raddr));

        usleep(waitms * 1000);
        closesocket(*sock);
    } while (time(NULL) < start + timeout && i < 4);

    return 0;
}

int queryres(SOCKET serv, struct message *res, struct message *req) {
    if (serv == -1) error(-1, "querying server");
    while (1) {
        if (readmsg(serv, res) == -1) {
            closesocket(serv);
            dbgtrace();
            return -1;
        }
        dbgmsg(res);
        if (res->m_type == TYPE_NOP) continue;
        switch(req->m_type) {
            case TYPE_BINDING_REQ:
            if (res->m_type != TYPE_BINDING_RES) error(-1, "querying server");
            break;

            case TYPE_CAPTURE_REQ:
            if (res->m_type != TYPE_CAPTURE_RES &&
                res->m_type != TYPE_CAPTURE_ACK)
                error(-1, "querying server");
            break;
        }
        break;
    }


    return 0;
}

int query(SOCKET serv, struct message *res, struct message *req) {
    if (serv == -1) return -1;
    if (findattr(req, ATTR_SOURCE_ADDRESS) == NULL) {
        struct sockaddr_in laddr;
        int laddrlen = sizeof(laddr);
        struct attribute attr;

        getsockname(serv, (struct sockaddr *)&laddr, &laddrlen);
        attrinit_source(&attr, (struct sockaddr *)&laddr);
        msgadd(req, &attr);
    }
    dbgmsg(req);
    if (writemsg(serv, req) == -1 ||
        queryres(serv, res, req) == -1) {
        closesocket(serv);
        dbgtrace();
        return -1;
    }

    return 0;
}

void reviveconn(SOCKET *sock) {
    struct timeval tm = {0, 1000};
    int r;
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(*sock, &rfds);

    r = select(((int)*sock) + 1, &rfds, NULL, NULL, &tm);

    if (r != 0) {
        struct sockaddr_in laddr = {0}, raddr;
        int raddrlen = sizeof(raddr);

        getpeername(*sock, (struct sockaddr *)&raddr, &raddrlen);

        closesocket(*sock);

        *sock = conn(&raddr,&laddr);
    }
}

void hardclosesocket(SOCKET *s) {
    struct linger linger = {SO_DO_LINGER, 0};
    setsockopt(*s, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));
    closesocket(*s);
}

SOCKET conn(struct sockaddr_in *raddr, struct sockaddr_in *laddr) {
    SOCKET serv;
    int r;
    int one = 1;
    int t = 0;
    int laddrlen = sizeof(*laddr);
    int stime;

    serv = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv == -1) {
        error(-1, "creating socket");
    }

    if (laddr != NULL &&
        (setsockopt(serv, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)) == -1 ||
         setsockopt(serv, SOL_SOCKET, SO_KEEPALIVE, (char *)&one, sizeof(one)) == -1 ||
         bind(serv, (struct sockaddr *)laddr, laddrlen) == -1 ||
         getsockname(serv, (struct sockaddr *)laddr, &laddrlen) == -1)) {

        closesocket(serv);
        error(-1, "binding socket");
    }

    stime = 10000;
    do {
        usleep(stime);
        stime *= 2;
    } while ((r = connect(serv, (struct sockaddr *)raddr, sizeof(*raddr))) == -1 &&
           errnum == EADDRNOTAVAIL && t++ < 12);

    if (r == -1) {
        closesocket(serv);
        error(-1, "connecting to server");
    }

    return serv;

}

int is_port(unsigned short t, port_type p) {
    switch(p) {
        case P_HIGH: return t >= 1024;
        case P_LOW: return t <= 1023;
        case P_DYN: return t >= 49152;
        default:    return 0;
    }
}

unsigned short freeport(port_type p) {
    int low, high;
    struct sockaddr_in addr;
    SOCKET s;
    int one = 1;

    if (p == P_LOW && !superuser) return 0;

    addr.sin_family = AF_INET;
    addr.sin_addr.S_addr = INADDR_ANY;

    while (1) {
        int port;
        switch(p) {
            case P_HIGH: low = 1024, high = 49151; break;
            case P_LOW: low = 1, high = 1023; break;
            case P_DYN: low = 49152, high = 65535; break;
        }
        port = (int)((((float)random())/RAND_MAX) * (high - 1 - low)) + low;

        addr.sin_port = htons((unsigned short) port);
        if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 ||
            setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)) == -1 ||
            bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
                closesocket(s);
                continue;
        }
        closesocket(s);

        addr.sin_port = htons((unsigned short) port + 1);
        if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 ||
            setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)) == -1 ||
            bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
                closesocket(s);
                continue;
        }
        closesocket(s);

        return port;

    }
}


