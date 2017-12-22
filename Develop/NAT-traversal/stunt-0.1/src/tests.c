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
#include "stunt.h"
#include "sockc.h"
#include "packets.h"
#include "print.h"
#include "tests.h"

#include <string.h>
#include <stddef.h>

#define MAX_PACKETS         64

const struct sockaddr_in blankaddr;
struct bindres bindings[NUM_TRY_PORTS][4][NUM_TRY_EACH];
struct bindres bindovrld[NUM_TRY_EACH][2];
struct captres bindhair;
int timeleft_binding = 5;

int binding_interval = 10000;
int ppred_interval = 60;
int ppred_duration = 3600;
int timeleft_ppred = 3600;

bool xor_map_mismatch = FALSE;
struct captres captures[CPT_TMTEST_LAST + 1] = {
    {CPT_FWTEST_FIRST}         ,
    {FWTEST_UNSOLICITED        , pktadd_unsolicited      , ""  , FWTEST_TIMEO , FWTEST_DELAY} ,
    {FWTEST_SYN_RST_SYN        , pktadd_synrstsyn        , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_SYN_TTL_SYN        , pktadd_synttlsyn        , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_SYN_UNREACH_SYN    , pktadd_synunreachsyn    , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_SYN_DROP_SYN       , pktadd_syndropsyn       , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_SYN_RST_SYNACK     , pktadd_synrstsynack     , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_SYN_TTL_SYNACK     , pktadd_synttlsynack     , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_SYN_UNREACH_SYNACK , pktadd_synunreachsynack , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_SYN_DROP_SYNACK    , pktadd_syndropsynack    , ""  , FWTEST_TIMEO , 0}            ,
    {FWTEST_ESTD_SYN_ADDRESS   , pktadd_estdsynaddress   , ""  , FWTEST_TIMEO , FWTEST_DELAY} ,
    {FWTEST_ESTD_SYN_ANY       , pktadd_estdsynaddrport  , ""  , FWTEST_TIMEO , FWTEST_DELAY} ,
    {CPT_FWTEST_LAST}          ,
    {CPT_TMTEST_FIRST}         ,
    {TMTEST_SYN                , pktadd_tmsyn            , ""} ,
    {TMTEST_ESTD               , pktadd_tmestd           , ""} ,
    {TMTEST_FIN                , pktadd_tmfin            , ""} ,
    {TMTEST_RST                , pktadd_tmrst            , ""} ,
    {CPT_TMTEST_LAST}
};
int timeleft_filter = FWTEST_TIMEO * (CPT_FWTEST_LAST - (CPT_FWTEST_FIRST + 1)) / 1000;

struct captres tmcaptures[MAX_TMTESTS];
struct timeres timers[MAX_TMTESTS];
int timeleft_timer;

intpair portpred = {0, 0};

void update_timeleft() {
    int s = 0;
    s += timeleft_binding;
    s += timeleft_filter;
    s += timeleft_timer;
    s += timeleft_ppred;
    timeleft = time(NULL) + s + 5;
}


void process_packet(u_char *usr, const struct pcap_pkthdr *hdr, const u_char *data) {
    struct captres *res = (struct captres *)usr;
//void process_packet(struct captres *res, const struct pcap_pkthdr *hdr, const uint8 *data) {
    struct attribute attr;
    struct ip *ip, *icmpip;
    struct tcp *tcp, *icmptcp;
    struct icmp *icmp;
    int chop = 0, plen = hdr->caplen;

    switch(pcap_datalink(res->handle)) {
        case DLT_EN10MB:
        case DLT_IEEE802:
            chop = 14;
            break;

        case DLT_RAW:
            chop = 0;
            break;

        case DLT_NULL:
        case DLT_LOOP:
            chop = 4;
            break;

        case DLT_SLIP:
        case DLT_LINUX_SLL:
            chop = 16;
            break;

        case DLT_PPP:
            chop = 4;
            break;

        default:
            return;
    }
    data = &data[chop];
    plen -= chop;
    if (plen < 0) return;
    attrinit_capturedpacket(&attr, MIN(LEN_PACKETDATA, plen));
    memcpy(attr.a_data.packet.p_data, data, MIN(LEN_PACKETDATA, plen));
    attr.a_data.packet.p_times = 1;
    attr.a_data.packet.p_time = (uint16)(time(NULL) - res->start);
    alignptrs(&ip, &tcp, &icmp, &icmpip, &icmptcp, attr.a_data.packet.p_data);

    if ((ip->ip_src == res->l_addr->sin_addr.S_addr &&
        (tcp != NULL && tcp->tcp_srcport == res->l_addr->sin_port)) ||
        (icmp != NULL && icmpip->ip_dst == res->l_addr->sin_addr.S_addr &&
         icmptcp != NULL && icmptcp->tcp_dstport  == res->l_addr->sin_port)) {
        attr.a_data.packet.p_type = CPT_TO_SERVER;
    } else {
        attr.a_data.packet.p_type = CPT_FROM_SERVER;
    }
    msgadd(&res->at_client, &attr);
}

THREAD_START handle_packets(void *arg) {
    struct captres *res = (struct captres *)arg;

    msgzero(&res->at_client);
    msginit_request(&res->at_client, TYPE_CAPTURE_RES);
    res->start = time(NULL);

    while (pcap_dispatch(res->handle, -1, process_packet, (char *)res) >= 0) {
        usleep(100000);
    }

    pthread_exit(0);
}

int start_capture(struct captres *res) {
    struct bpf_program  filter;
    char errbuf[PCAP_ERRBUF_SIZE];

    res->handle = pcap_open_live(devstr[0], 96, 0, 1000, errbuf);
    if (res->handle == NULL) {
        error(-1, "initializing pcap");
    }
    if (pcap_setnonblock(res->handle, 1, errbuf) == -1 ||
        pcap_compile(res->handle, &filter, res->pcap_filter, 0, 0) == -1 ||
        pcap_setfilter(res->handle, &filter) == -1 ||
        pthread_create(&res->tid, NULL, handle_packets, res) == PTHREAD_ERROR) {

        pcap_close(res->handle);
        res->handle = NULL;
        error(-1, "initializing pcap or starting handler thread");
    } else {
        return 0;
    }
}

int stop_capture(struct captres *res) {
    if (res->handle != NULL) {
        pcap_breakloop(res->handle);
        pthread_join(res->tid, NULL);
        pcap_close(res->handle);
        res->handle = NULL;
    }
    return 0;
}

int abovethresh(intpair pair, int thresh) {
    return pair.a > 0 && pair.b >= pair.a * thresh / 100;
}

int beqmap(struct bindres *a, struct bindres *b) {
    return a->m_port == b->m_port && a->m_addr == b->m_addr;
}

void storebinding(struct bindres *bres, struct message *msg,
                  struct sockaddr_in *laddr, struct sockaddr_in *raddr) {

    struct attribute *mapped;
    mapped = findattr(msg, ATTR_XOR_MAPPED_ADDRESS);

    if (mapped != NULL) {
        struct sockaddr_in maddr;
        sockaddrinit_addr((struct sockaddr *)&maddr, sizeof(maddr), mapped, msg);
        {
            struct bindres mapping = {
                laddr->sin_addr.S_addr, ntohs(laddr->sin_port),
                    maddr.sin_addr.S_addr, ntohs(maddr.sin_port),
                    raddr->sin_addr.S_addr, ntohs(raddr->sin_port)
            };
            *bres = mapping;
        }
    }
}

int filterinit(struct captres *res, int test, struct message *req, struct sockaddr_in *laddr) {
    struct sockaddr_in sin, sin2;

    res->l_addr = laddr;
    switch(test) {
        case TMTEST_SYN:
        case TMTEST_ESTD:
        case TMTEST_FIN:
        case TMTEST_RST:
        case FWTEST_UNSOLICITED:
        case FWTEST_SYN_RST_SYN:
        case FWTEST_SYN_TTL_SYN:
        case FWTEST_SYN_UNREACH_SYN:
        case FWTEST_SYN_DROP_SYN:
        case FWTEST_SYN_RST_SYNACK:
        case FWTEST_SYN_TTL_SYNACK:
        case FWTEST_SYN_UNREACH_SYNACK:
        case FWTEST_SYN_DROP_SYNACK:
            if (sockaddrinit_addr((struct sockaddr *)&sin, sizeof(sin),
                              findattr(req, ATTR_CHANGED_ADDRESS), req) == -1) {
                dbgtrace();
                return -1;
            }
            snprintf(res->pcap_filter, sizeof(res->pcap_filter),
                     "(host %d.%d.%d.%d and tcp and port %d) or"
                     "(icmp and icmp[20:4] == 0x%x and icmp[28:2] == 0x%x)",
                     NIPQUAD(sin.sin_addr.S_addr),
                     ntohs(sin.sin_port),
                     ntohl(laddr->sin_addr.S_addr),
                     ntohs(laddr->sin_port)
                     );
            break;

        case FWTEST_ESTD_SYN_ADDRESS:
        case FWTEST_ESTD_SYN_ANY:
            if (sockaddrinit_addr((struct sockaddr *)&sin, sizeof(sin),
                              findattr(req, ATTR_CHANGED_ADDRESS), req) == -1 ||
                sockaddrinit_addr((struct sockaddr *)&sin2, sizeof(sin2),
                              findnattr(req, ATTR_CHANGED_ADDRESS, 2), req) == -1) {
                dbgtrace();
                return -1;
            }
            snprintf(res->pcap_filter, sizeof(res->pcap_filter),
                     "(host %d.%d.%d.%d and tcp and port %d) or"
                     "(host %d.%d.%d.%d and tcp and port %d) or"
                     "(icmp and icmp[20:4] == 0x%x and icmp[28:2] == 0x%x)",
                     NIPQUAD(sin.sin_addr.S_addr),
                     ntohs(sin.sin_port),
                     NIPQUAD(sin2.sin_addr.S_addr),
                     ntohs(sin2.sin_port),
                     ntohl(laddr->sin_addr.S_addr),
                     ntohs(laddr->sin_port)
                     );
            break;
    }

    return 0;
}

int test_bindings() {
    struct message req, res;
    struct sockaddr_in raddr, laddr;
    struct attribute *changed;
    int i, j, k;
    unsigned short orig_port;
    unsigned short high = freeport(P_HIGH);
    unsigned short low = freeport(P_LOW);
    unsigned short dyn = freeport(P_DYN);
    unsigned short tryports[NUM_TRY_PORTS] = {
        0,                                          // one random port
        superuser ? low : 0, superuser ? low+1 : 0, // two consecutive low ports
        high, high+1,                               // two consecitive high ports
        dyn, dyn+1                                  // two consecutive dynamic ports
    };

    for(k = 0; k < NUM_TRY_PORTS; k++) {
        laddr.sin_family = AF_INET;
        laddr.sin_addr.S_addr = inet_addr(laddress[0]);
        laddr.sin_port = htons(tryports[k]);

        raddr.sin_family = AF_INET;
        raddr.sin_addr.S_addr = inet_addr(raddress);
        raddr.sin_port = orig_port = htons(port);

        msgzero(&req);

        for(j = 0; j < 4; j++) {
            for(i = 0; i < NUM_TRY_EACH; i++) {

                msginit_request(&req, TYPE_BINDING_REQ);

                hardclosesocket(&serv);
                usleep(binding_interval);
                if (query(serv = conn(&raddr, &laddr), &res, &req) == -1) {
                    closesocket(serv);
                    dbgtrace();
                    return -1;
                }

                storebinding(&bindings[k][j][i], &res, &laddr, &raddr);
            }

            changed = findattr(&res, ATTR_CHANGED_ADDRESS);
            if (changed == NULL) {
                closesocket(serv);
                dbgtrace();
                return -1;
            }

            switch(j) {
                case 2:     // new addr and new port
                case 0:     // next change port
                    raddr.sin_port = changed->a_data.address.a_port;
                    break;

                case 1:     // orig port and new addr
                    raddr.sin_port = orig_port;
                    if (changed->a_data.address.a_type != ADDR_IPv4) {
                        dbgtrace();
                        return -1;
                    }
                    raddr.sin_addr.S_addr = changed->a_data.address.a_addr.ipv4.word.s_addr;
                    break;
            }
        }
    }


    // Hairpinning
    {
        SOCKET conn1 = -1;
        struct sockaddr_in haddr;

        laddr = blankaddr;
        analyze_bindings();
        closesocket(serv);

        conn1 = boundsocket(&laddr);
        serv = conn(&raddr, &laddr);

        if (conn1 == -1 || serv == -1) {
            dbgtrace();
            return -1;
        }

        msginit_request(&req, TYPE_BINDING_REQ);
        if (query(serv, &res, &req) == -1 ||
            sockaddrinit_addr((struct sockaddr *)&haddr, sizeof(haddr),
                              findattr(&res, ATTR_XOR_MAPPED_ADDRESS), &res) == -1) {
            closesocket(serv);
            dbgtrace();
            return -1;
        }
        haddr.sin_port = htons(ntohs(haddr.sin_port) + nat.tcp.binding_delta);

        bindhair.l_addr = &laddr;
        snprintf(bindhair.pcap_filter, sizeof(bindhair.pcap_filter),
                 "tcp and port %d", ntohs(laddr.sin_port));
        start_capture(&bindhair);
        connect(conn1, (struct sockaddr *)&haddr, sizeof(haddr));
        sleep(1);
        stop_capture(&bindhair);

        dbgmsg(&bindhair.at_client);

        closesocket(conn1);
    }


    // Overload
    if (strlen(laddress[1]) != 0) {
        struct sockaddr_in laddr2;
        SOCKET serv2;
        laddr2.sin_family = AF_INET;
        laddr2.sin_addr.S_addr = inet_addr(laddress[1]);

        for(i=0; i<NUM_TRY_EACH; i++) {
            struct bindres one, two;

            closesocket(serv);
            laddr.sin_port = 0;
            msginit_request(&req, TYPE_BINDING_REQ);
            if (query(serv = conn(&raddr, &laddr), &res, &req) == -1) {
                closesocket(serv);
                dbgtrace();
                return -1;
            }

            storebinding(&one, &res, &laddr, &raddr);

            sockaddrinit_addr((struct sockaddr *)&raddr, sizeof(raddr),
                              findattr(&res, ATTR_CHANGED_ADDRESS), &res);

            laddr2.sin_port = laddr.sin_port;
            msginit_request(&req, TYPE_BINDING_REQ);
            if (query(serv2 = conn(&raddr, &laddr2), &res, &req) == -1) {
                closesocket(serv2);
                i--;
                continue;
            }
            closesocket(serv2);
            storebinding(&two, &res, &laddr2, &raddr);

            if (one.m_addr == two.m_addr && one.m_port == two.m_port) {
                // can overload
                closesocket(serv);
                laddr.sin_port = laddr2.sin_port;
                msginit_request(&req, TYPE_BINDING_REQ);
                if (query(serv = conn(&raddr, &laddr), &res, &req) == -1) {
                    bindovrld[i][0] = one;
                    bindovrld[i][0] = two;
                }
                closesocket(serv);
            }

        }


    }


    return 0;
}

int test_firewall() {
    struct sockaddr_in raddr, laddr, gaddr;
    int raddrlen = sizeof(raddr), laddrlen = sizeof(laddr);
    struct attribute attr;
    struct message req, res;
    int test;

    laddr.sin_family = AF_INET;
    laddr.sin_addr.S_addr = inet_addr(laddress[0]);
    laddr.sin_port = 0;

    raddr.sin_family = AF_INET;
    raddr.sin_addr.S_addr = inet_addr(raddress);
    raddr.sin_port = htons(port);

    if (serv == -1) {
        serv = conn(&raddr, &laddr);
        if (serv == -1) {
            dbgtrace();
            return -1;
        }
    } else {
        getsockname(serv, (struct sockaddr *)&laddr, &laddrlen);
        getpeername(serv, (struct sockaddr *)&raddr, &raddrlen);
    }


    msgzero(&req);
    msginit_request(&req, TYPE_BINDING_REQ);
    if (query(serv, &res, &req) == -1 ||
        sockaddrinit_addr((struct sockaddr *)&gaddr, sizeof(gaddr),
                          findattr(&res, ATTR_MAPPED_ADDRESS), &res) == -1) {
        dbgtrace();
        return -1;
    }

    if (xor_map_mismatch == FALSE) {
        struct sockaddr_in a, b;
        sockaddrinit_addr((struct sockaddr *)&a, sizeof(a), findattr(&res, ATTR_XOR_MAPPED_ADDRESS), &res);
        sockaddrinit_addr((struct sockaddr *)&b, sizeof(b), findattr(&res, ATTR_MAPPED_ADDRESS), &res);
        if (a.sin_family == b.sin_family) {
            if (a.sin_addr.S_addr != b.sin_addr.S_addr ||
                a.sin_port != b.sin_port) xor_map_mismatch = TRUE;
        }
    }


    for (test=CPT_FWTEST_FIRST + 1; test < CPT_FWTEST_LAST; test++) {
        int wait = 1000;
        struct sockaddr_in laddr = blankaddr;
        struct sockaddr_in raddr = blankaddr;
        SOCKET sock = boundsocket(&laddr);
        struct captres *captres = &captures[test];
        if (sock == -1) continue;

        reviveconn(&serv);

        switch(test) {

            case FWTEST_UNSOLICITED:
                laddr.sin_addr.S_addr = gaddr.sin_addr.S_addr;
                msginit_capreq(&req, captres);
                attrinit_response(&attr, (struct sockaddr *)&laddr);
                msgadd(&req, &attr);
                listen(sock, 5);
                if (query(serv, &res, &req) == -1 ||
                    filterinit(captres, test, &res, &laddr) == -1 ||
                    start_capture(captres) == -1 ||
                    acceptclose(sock, captres->timeoms) == -1 ||
                    queryres(serv, &captres->at_server, &req) == -1 ||
                    stop_capture(captres) == -1) {

                    stop_capture(captres);
                    closesocket(sock);
                    dbgtrace();
                    return -1;
                }
                break;

            case FWTEST_ESTD_SYN_ADDRESS:
            case FWTEST_ESTD_SYN_ANY:
                wait = 4000;
            case FWTEST_SYN_RST_SYN:
            case FWTEST_SYN_TTL_SYN:
            case FWTEST_SYN_UNREACH_SYN:
            case FWTEST_SYN_DROP_SYN:
            case FWTEST_SYN_RST_SYNACK:
            case FWTEST_SYN_TTL_SYNACK:
            case FWTEST_SYN_UNREACH_SYNACK:
            case FWTEST_SYN_DROP_SYNACK:
                msginit_capreq(&req, captres);
                if (query(serv, &res, &req) == -1 ||
                    sockaddrinit_addr((struct sockaddr *)&raddr, sizeof(raddr),
                                      findattr(&res, ATTR_CHANGED_ADDRESS), &res) == -1 ||
                    filterinit(captres, test, &res, &laddr) == -1 ||
                    start_capture(captres) == -1 ||
                    connectclose(&sock, &laddr, &raddr, captres->timeoms, wait) == -1 ||
                    queryres(serv, &captres->at_server, &req) == -1 ||
                    stop_capture(captres) == -1) {

                    stop_capture(captres);
                    closesocket(sock);
                    dbgtrace();
                    return -1;
                }
                break;
        }
        closesocket(sock);
        dbgmsg(&captres->at_client);
        timeleft_filter -= captres->timeoms / 1000;
        update_timeleft();
    }


    return 0;
}


int test_timers() {
    struct sockaddr_in raddr, laddr;
    int raddrlen = sizeof(raddr), laddrlen = sizeof(laddr);
    struct message req, res;
    int i;
    SOCKET sock;

    laddr.sin_family = AF_INET;
    laddr.sin_addr.S_addr = inet_addr(laddress[0]);
    laddr.sin_port = 0;

    raddr.sin_family = AF_INET;
    raddr.sin_addr.S_addr = inet_addr(raddress);
    raddr.sin_port = htons(port);

    if (serv == -1) {
        serv = conn(&raddr, &laddr);
        if (serv == -1) {
            dbgtrace();
            return -1;
        }
    } else {
        getsockname(serv, (struct sockaddr *)&laddr, &laddrlen);
        getpeername(serv, (struct sockaddr *)&raddr, &raddrlen);
    }


    for (i=0; i < MIN(elemof(timers),MAX_TMTESTS); i++) {
        if (timers[i].captres == NULL) break;
        laddr = raddr = blankaddr;
        sock = boundsocket(&laddr);
        if (sock == -1) continue;

        reviveconn(&serv);

        tmcaptures[i] = *timers[i].captres;
        tmcaptures[i].delayms = timers[i].delays * 1000;
        tmcaptures[i].timeoms = timers[i].timeos * 1000;
        msginit_capreq(&req, &tmcaptures[i]);
        if (query(serv, &res, &req) == -1 ||
            sockaddrinit_addr((struct sockaddr *)&raddr, sizeof(raddr),
                              findattr(&res, ATTR_CHANGED_ADDRESS), &res) == -1 ||
            filterinit(&tmcaptures[i], tmcaptures[i].test, &res, &laddr) == -1 ||
            start_capture(&tmcaptures[i]) == -1 ||
            connectwait(&sock, &laddr, &raddr, tmcaptures[i].timeoms, timers[i].close) == -1 ||
            queryres(serv, &tmcaptures[i].at_server, &req) == -1 ||
            stop_capture(&tmcaptures[i]) == -1) {
            closesocket(sock);
            dbgtrace();
            return -1;
        }
        closesocket(sock);
        dbgmsg(&tmcaptures[i].at_client);
        timeleft_timer -= timers[i].timeos;
        update_timeleft();
    }

    return 0;
}

int test_ppred() {
    struct sockaddr_in raddr, laddr, haddr, saddr;
    struct attribute attr, *attrs;
    struct message req, res;
    SOCKET conn1;
    struct ip *ip; struct tcp *tcp;
    int start = time(NULL);
    int n = 0;

    raddr.sin_family = AF_INET;
    raddr.sin_addr.S_addr = inet_addr(raddress);
    raddr.sin_port = htons(port);

    analyze_bindings();
    closesocket(serv);


    while (time(NULL) - start + ppred_interval < ppred_duration) {
        timeleft_ppred = ppred_duration - (time(NULL) - start);
        update_timeleft();
        if (n++ != 0) sleep(ppred_interval);


        laddr = blankaddr;
        conn1 = boundsocket(&laddr);
        serv = conn(&raddr, &laddr);

        if (conn1 == -1 || serv == -1) {
            closesocket(serv);
            closesocket(conn1);
            dbgtrace();
            return -1;
        }


        msgzero(&req);
        msginit_request(&req, TYPE_BINDING_REQ);
        if (query(serv, &res, &req) == -1 ||
            sockaddrinit_addr((struct sockaddr *)&haddr, sizeof(haddr),
                              findattr(&res, ATTR_XOR_MAPPED_ADDRESS), &res) == -1) {
            closesocket(serv);
            closesocket(conn1);
            dbgtrace();
            return -1;
        }
        haddr.sin_port = htons(ntohs(haddr.sin_port) + nat.tcp.binding_delta);

        msgzero(&req);
        msginit_request(&req, TYPE_CAPTURE_REQ);
        attrinit_pktreq(&attr, IPPROTO_NULL, 0, 0, PKTREQ_WAIT | PKTREQ_ALTADDR | PKTREQ_ALTPORT);
        msgadd(&req, &attr);
        attrinit_capturetimeouts(&attr, PPTEST_TIMEO, 0);
        msgadd(&req, &attr);
        if (query(serv, &res, &req) == -1 ||
            sockaddrinit_addr((struct sockaddr *)&saddr, sizeof(saddr),
                              findattr(&res, ATTR_CHANGED_ADDRESS), &res) == -1) {
            closesocket(serv);
            closesocket(conn1);
            dbgtrace();
            return -1;
        }


        connect(conn1, (struct sockaddr *)&saddr, sizeof(saddr));


        if (queryres(serv, &res, &req) == -1) {
            closesocket(serv);
            closesocket(conn1);
            dbgtrace();
            return -1;
        }
        closesocket(conn1);
        closesocket(serv);




        portpred.a++;
        iterattrinit(&attrs, &res);
        while (attrs != NULL) {
            if (attrs->a_type == ATTR_CAPTURED_PACKET) {
                if ((attrs->a_data.packet.p_type & CPTMASK_DIRN) == CPT_TO_SERVER) {
                    alignptrs(&ip, &tcp, NULL, NULL, NULL, attrs->a_data.packet.p_data);
                    if (tcp != NULL && tcp->tcp_flags == TCP_SYN) {
                        if (ip->ip_src == haddr.sin_addr.S_addr &&
                            tcp->tcp_srcport == haddr.sin_port) {
                            portpred.b++;
                            break;
                        }
                    }
                }
            }
            iterattr(&attrs, &res);
        }

    }

    return 0;
}

void analyze_bindings() {
    int i,j,k,s;

    intpair low = {0,0}, high = {0,0}, dynamic = {0,0};
    intpair portpres = {0,0};
    intpair sameipport = {0,0}, sameip = {0,0}, sameport = {0,0}, sameany = {0,0};
    bool stablemaps[NUM_TRY_PORTS][4];
    intpair deltas[] = {{0,-1},{0,-1},{0,-1},{0,-1}};
    int numdelta = 0;
    intpair evenodd = {0,0}, nexthigher = {0,0};
    intpair overload = {0,0};
    intpair hairpin = {0,0};
    struct attribute *attr;

    memset(nat.tcp.ip, 0, sizeof(nat.tcp.ip));
    memset(nat.tcp.local_ip, 0, sizeof(nat.tcp.local_ip));
    memset(stablemaps,0,sizeof(stablemaps));

    for (i=0; i<NUM_TRY_PORTS; i++) {
        for(s=0; s<4; s++) {
            bool mapping_changed = FALSE;

            for(j=0; j<NUM_TRY_EACH; j++) {
                struct bindres *b = &bindings[i][s][j];
                int k;

                for(k=0; k<MAX_TNAT_IP; k++) {
                    if (nat.tcp.ip[k] == b->m_addr) break;
                    if (nat.tcp.ip[k] == 0) break;
                }
                if (k < MAX_TNAT_IP && nat.tcp.ip[k] == 0) {
                    nat.tcp.ip[k] = b->m_addr;
                }
                for(k=0; k<MAX_TNAT_IP; k++) {
                    if (nat.tcp.local_ip[k] == b->s_addr) break;
                    if (nat.tcp.local_ip[k] == 0) break;
                }
                if (k < MAX_TNAT_IP && nat.tcp.local_ip[k] == 0) {
                    nat.tcp.local_ip[k] = b->s_addr;
                }

                // Port range preservation
                if (is_port(b->s_port, P_LOW)) {
                    low.a++;
                    if (is_port(b->m_port, P_LOW)) low.b++;
                }
                if (is_port(b->s_port, P_HIGH)) {
                    high.a++;
                    if (is_port(b->m_port, P_HIGH)) high.b++;
                }
                if (is_port(b->s_port, P_DYN)) {
                    dynamic.a++;
                    if (is_port(b->m_port, P_DYN)) dynamic.b++;
                }

                // Port preservation
                portpres.a++;
                if (b->m_port == b->s_port) portpres.b++;

                // Binding properties
                if (!beqmap(b, &bindings[i][s][0])) {
                    mapping_changed = TRUE;
                }

                // Delta
                if (i+j+s != 0) {
                    struct bindres *bo = &bindings[i][s][j-1];
                    int delta = b->m_port - bo->m_port;
                    int pos = -1;
                    if (delta != 0 || b->d_port != bo->d_port || b->d_addr != bo->d_addr) {
                        numdelta++;
                        for(k=0; k<elemof(deltas); k++) {
                            if (deltas[k].b == -1) break;
                            if (deltas[k].a == delta) {pos = k; break;}
                        }
                        if (pos != -1) {
                            deltas[pos].b++;
                        } else if (k<elemof(deltas)) {
                            deltas[k].a = delta;
                            deltas[k].b = 1;
                        }
                    }
                }

            }

            if (mapping_changed == FALSE) {

                // Even odd parity
                evenodd.a+=5;
                if (bindings[i][s][0].m_port % 2 == bindings[i][s][0].s_port % 2)
                    evenodd.b+=5;

                // Next higher parity
                if (i > 0 && stablemaps[i-1][s] && bindings[i][s][0].s_port - bindings[i-1][s][0].s_port == 1) {
                    nexthigher.a++;
                    if (bindings[i][s][0].m_port - bindings[i-1][s][0].m_port == 1 &&
                        bindings[i][s][0].m_addr == bindings[i-1][s][0].m_addr)
                        nexthigher.b++;
                }
            }

            // Repeat connection to same ip/port
            sameipport.a++;
            if (mapping_changed == FALSE) {
                sameipport.b++;
                stablemaps[i][s] = TRUE;
            }



            // Connections to other ip/port
            if (mapping_changed == FALSE) {
                for(k=0; k<s; k++) {
                    intpair *p;
                    if (stablemaps[i][k] == FALSE) continue;
                    p = (bindings[i][k][0].d_addr == bindings[i][s][0].d_addr) ?
                        ((bindings[i][k][0].d_port == bindings[i][s][0].d_port) ? NULL : &sameip) :
                        (bindings[i][k][0].d_port == bindings[i][s][0].d_port) ? &sameport : NULL;
                    sameany.a++;
                    if (p!=NULL) p->a++;
                    if(beqmap(&bindings[i][k][0],&bindings[i][s][0])) {
                        sameany.b++;
                        if (p!=NULL) p->b++;
                    }
                }
            }
        }
    }

    for(i=0; i<NUM_TRY_EACH; i++) {

        // overload
        overload.a++;
        if (bindovrld[i][0].s_addr != bindovrld[i][1].s_addr &&
            bindovrld[i][0].m_addr == bindovrld[i][1].m_addr &&
            bindovrld[i][0].m_port == bindovrld[i][1].m_port) {
            overload.b++;
        }
    }

    // hairpin
    iterattrinit(&attr, &bindhair.at_client);
    while (attr != NULL) {
            struct ip *ip; struct tcp *tcp;
            if (attr->a_type == ATTR_CAPTURED_PACKET) {

                alignptrs(&ip, &tcp, NULL, NULL, NULL, attr->a_data.packet.p_data);
                if (tcp != NULL) {
                    if (tcp->tcp_flags == (TCP_SYN | TCP_ACK)) {
                        hairpin.b++;
                    } else if (tcp->tcp_flags == TCP_SYN) {
                        hairpin.a++;
                    }
                }
            }
            iterattr(&attr, &bindhair.at_client);
    }



    // Port range preservation
    if (low.a == 0) nat.tcp.port_range_preserved |= PR_NOT_TESTED_LOW;
    else if (abovethresh(low, THRESH_PORTASSIGN)) nat.tcp.port_range_preserved |= PR_LOW;
    if (abovethresh(high, THRESH_PORTASSIGN)) nat.tcp.port_range_preserved |= PR_HIGH;
    if (abovethresh(dynamic, THRESH_PORTASSIGN)) nat.tcp.port_range_preserved |= PR_DYNAMIC;

    // Port preservation
    if (portpres.b >= portpres.a * THRESH_PORTASSIGN / 100) nat.tcp.port_preserving = TRUE;

    // Port overloading
    if (strlen(laddress[1]) != 0) {
        nat.tcp.port_overloading = overload.b > 0 ? TRUE : FALSE;
    } else {
        nat.tcp.port_overloading = MAYBE;
    }

    // Binding behaviour
    if (!abovethresh(sameipport,THRESH_PORTASSIGN))
        nat.tcp.nat_binding = NB_NEITHER;
    else if (abovethresh(sameany,THRESH_PORTASSIGN))
        nat.tcp.nat_binding = NB_INDEPENDENT;
    else if (abovethresh(sameip,THRESH_PORTASSIGN))
        nat.tcp.nat_binding = NB_ADDRESS;
    else if (abovethresh(sameport,THRESH_PORTASSIGN))
        nat.tcp.nat_binding = NB_PORT;
    else
        nat.tcp.nat_binding = NB_ADDRESS_PORT;

    // Delta
    for(k=0; k<elemof(deltas); k++) {
        if (deltas[k].b == -1) break;
        else {
            intpair p = {numdelta, deltas[k].b};
            if (abovethresh(p, THRESH_PORTASSIGN)) {
                nat.tcp.binding_delta = deltas[k].a;
                break;
            }
        }
    }

    // Port parity preserving
    if (abovethresh(evenodd, THRESH_PORTASSIGN)) nat.tcp.port_parity |= PY_EVENODD;
    if (abovethresh(nexthigher, THRESH_PORTASSIGN)) nat.tcp.port_parity |= PY_NEXTHIGHER;


    // Hairpinning
    if (hairpin.a == 0) nat.tcp.hairpin = MAYBE;
    else if (abovethresh(hairpin, THRESH_PORTASSIGN)) nat.tcp.hairpin = TRUE;
    else nat.tcp.hairpin = FALSE;

}

void analyze_mangles(struct captres *cpt) {
    struct attribute *attrs = NULL, *attrc = NULL, *attrc2 = NULL;
    struct ip *ip, *ip2, *icmpip; struct tcp *tcp, *tcp2, *icmptcp;
    struct icmp *icmp;

    iterattrinit(&attrs, &cpt->at_server);

    if (xor_map_mismatch == TRUE) nat.tcp.mangles |= MG_IP_PAYLOAD;

    // For each SYN seen by server
    while (attrs != NULL) {
        if (attrs->a_type == ATTR_CAPTURED_PACKET &&
            (attrs->a_data.packet.p_type & CPTMASK_DIRN) == CPT_TO_SERVER) {
            alignptrs(&ip, &tcp, NULL, NULL, NULL, attrs->a_data.packet.p_data);
            if (tcp != NULL && tcp->tcp_flags == TCP_SYN) {
                bool seen_same_seq = FALSE, seen_high_ttl = FALSE;

                // Ensure the SYN sent by the client
                // had the same seq and higher ttl
                iterattrinit(&attrc, &cpt->at_client);
                while (attrc != NULL) {
                    if (attrc->a_type == ATTR_CAPTURED_PACKET &&
                        (attrc->a_data.packet.p_type & CPTMASK_DIRN) == CPT_TO_SERVER) {

                        alignptrs(&ip2, &tcp2, NULL, NULL, NULL, attrc->a_data.packet.p_data);
                        if (tcp2 != NULL && tcp2->tcp_flags == TCP_SYN) {
                            if (tcp2->tcp_seq == tcp->tcp_seq) seen_same_seq = TRUE;
                            if (ip2->ip_ttl < ip->ip_ttl) seen_high_ttl = TRUE;

                        }
                    }
                    iterattr(&attrc, &cpt->at_client);
                }

                if (!seen_same_seq) nat.tcp.mangles |= MG_TCP_SEQ;
                if (seen_high_ttl) nat.tcp.mangles |= MG_IP_TTL;

            }
        }
        iterattr(&attrs, &cpt->at_server);
    }

    iterattrinit(&attrc, &cpt->at_client);

    // Check each ICMP is correct
    while (attrc != NULL) {
        if (attrc->a_type == ATTR_CAPTURED_PACKET &&
            (attrc->a_data.packet.p_type & CPTMASK_DIRN) == CPT_FROM_SERVER) {
            alignptrs(&ip, &tcp, &icmp, &icmpip, &icmptcp, attrc->a_data.packet.p_data);
            if (icmp != NULL && icmp->icmp_type != ICMP_TYPE_TTL && icmp->icmp_type != ICMP_TYPE_UNREACH) {
                bool seen_same_pkt = FALSE;

                // Ensure the echoed packet was seen
                iterattrinit(&attrc2, &cpt->at_client);
                while (attrc2 != NULL) {
                    if (attrc2->a_type == ATTR_CAPTURED_PACKET &&
                        (attrc2->a_data.packet.p_type & CPTMASK_DIRN) == CPT_TO_SERVER) {
                        unsigned int i;
                        for (i = 0; i < attrc2->a_len - LEN_PACKETHDR; i++) {
                            if (attrc2->a_data.packet.p_data[i] !=
                                icmp->icmp_payload[i]) break;
                        }
                        if (i == attrc2->a_len - LEN_PACKETHDR) {
                            seen_same_pkt = TRUE;
                            break;
                        }
                    }
                    iterattr(&attrc2, &cpt->at_client);
                    if (attrc2 == attrc) break;
                }

                if (!seen_same_pkt) nat.tcp.mangles |= MG_ICMP_PAYLOAD;
            }
        }
        iterattr(&attrc, &cpt->at_client);
    }
}

void analyze_filtering() {
    int test;

    nat.tcp.mangles = 0;

    for(test = CPT_FWTEST_FIRST + 1; test < CPT_FWTEST_LAST; test++) {
        struct captres *cpt = &captures[test];
        struct attribute *attrs = NULL, *attrc = NULL;
        int flag;
        bool seen = FALSE;
        bool *result = NULL;

        iterattrinit(&attrs, &cpt->at_server);
        iterattrinit(&attrc, &cpt->at_client);

        switch(test) {
            case FWTEST_UNSOLICITED        : result = &nat.tcp.fw_unsolicited        ; flag = TCP_SYN           ; break ;
            case FWTEST_SYN_RST_SYN        : result = &nat.tcp.fw_syn_rst_syn        ; flag = TCP_SYN           ; break ;
            case FWTEST_SYN_TTL_SYN        : result = &nat.tcp.fw_syn_ttl_syn        ; flag = TCP_SYN           ; break ;
            case FWTEST_SYN_UNREACH_SYN    : result = &nat.tcp.fw_syn_unreach_syn    ; flag = TCP_SYN           ; break ;
            case FWTEST_SYN_DROP_SYN       : result = &nat.tcp.fw_syn_drop_syn       ; flag = TCP_SYN           ; break ;
            case FWTEST_ESTD_SYN_ADDRESS   : result = &nat.tcp.fw_estd_syn_address   ; flag = TCP_SYN           ; break ;
            case FWTEST_ESTD_SYN_ANY       : result = &nat.tcp.fw_estd_syn_any       ; flag = TCP_SYN           ; break ;
            case FWTEST_SYN_RST_SYNACK     : result = &nat.tcp.fw_syn_rst_synack     ; flag = TCP_SYN | TCP_ACK ; break ;
            case FWTEST_SYN_TTL_SYNACK     : result = &nat.tcp.fw_syn_ttl_synack     ; flag = TCP_SYN | TCP_ACK ; break ;
            case FWTEST_SYN_UNREACH_SYNACK : result = &nat.tcp.fw_syn_unreach_synack ; flag = TCP_SYN | TCP_ACK ; break ;
            case FWTEST_SYN_DROP_SYNACK    : result = &nat.tcp.fw_syn_drop_synack    ; flag = TCP_SYN | TCP_ACK ; break ;
        }

        analyze_mangles(cpt);

        while (attrc != NULL) {
            struct ip *ip; struct tcp *tcp;
            if (attrc->a_type == ATTR_CAPTURED_PACKET &&
                (attrc->a_data.packet.p_type & CPTMASK_DIRN) == CPT_FROM_SERVER) {

                alignptrs(&ip, &tcp, NULL, NULL, NULL, attrc->a_data.packet.p_data);
                if (tcp != NULL && tcp->tcp_flags == flag) {
                    seen = TRUE;
                    break;
                }
            }
            iterattr(&attrc, &cpt->at_client);
        }

        if (seen == TRUE) {
            *result = FW_ACCEPT;
            continue;
        } else {
            uint32 pseq, seq, nseq;
            uint16 id;
            *result = FW_DROP;
            while (attrs != NULL) {
                struct ip *ip, *icmpip; struct tcp *tcp, *icmptcp; struct icmp *icmp;
                if (attrs->a_type != ATTR_CAPTURED_PACKET) {
                    iterattr(&attrs, &cpt->at_server);
                    continue;
                }
                alignptrs(&ip, &tcp, &icmp, &icmpip, &icmptcp, attrs->a_data.packet.p_data);
                if ((attrs->a_data.packet.p_type & CPTMASK_DIRN) == CPT_FROM_SERVER) {
                        if (tcp != NULL && tcp->tcp_flags == flag) {
                            seq = tcp->tcp_seq;
                            nextseq(&nseq, &pseq, ip, tcp);
                            id = ip->ip_ident;
                        }
                } else if ((attrs->a_data.packet.p_type & CPTMASK_DIRN) == CPT_TO_SERVER) {
                    if (tcp != NULL &&
                        ((tcp->tcp_flags == (TCP_RST | TCP_ACK) && tcp->tcp_ack == nseq) ||
                         (tcp->tcp_flags == TCP_RST && tcp->tcp_seq == pseq))) {
                            *result = FW_RST;
                            break;
                    } else if (icmp != NULL && icmptcp != NULL &&
                        icmpip->ip_ident == id && icmptcp->tcp_seq == seq &&
                        *result == FW_DROP) {
                            *result = FW_ICMP;
                    }
                }
                iterattr(&attrs, &cpt->at_server);
            }
        }
    }
}

void analyze_timers() {
    int i;

    nat.tcp.timer_syn.min = -1;
    nat.tcp.timer_estd.min = -1;
    nat.tcp.timer_fin.min = -1;
    nat.tcp.timer_rst.min = -1;
    nat.tcp.timer_syn.max = MAX_INT;
    nat.tcp.timer_estd.max = MAX_INT;
    nat.tcp.timer_fin.max = MAX_INT;
    nat.tcp.timer_rst.max = MAX_INT;

    for(i = 0; i < MAX_TMTESTS; i++) {
        struct captres *cpt = &tmcaptures[i];
        struct attribute *attrs = NULL, *attrc = NULL;
        int flag;
        int time = 0, min, max, diff;
        struct timer *result = NULL;
        struct ip *ip; struct tcp *tcp;

        switch(cpt->test) {
            case TMTEST_SYN  : result = &nat.tcp.timer_syn  ; flag = TCP_SYN | TCP_ACK ; break ;
            case TMTEST_ESTD : result = &nat.tcp.timer_estd ; flag = TCP_ACK           ; break ;
            case TMTEST_FIN  : result = &nat.tcp.timer_fin  ; flag = TCP_FIN | TCP_ACK ; break ;
            case TMTEST_RST  : result = &nat.tcp.timer_rst  ; flag = TCP_RST           ; break ;
            default: continue;
        }
        min = result->min;
        max = result->max;

        iterattrinit(&attrs, &cpt->at_server);
        iterattrinit(&attrc, &cpt->at_client);

        time = 0;
        while (attrc != NULL) {
            if (attrc->a_type == ATTR_CAPTURED_PACKET) {
                diff = MAX(0, attrc->a_data.packet.p_time - time - 1);
                if ((attrc->a_data.packet.p_type & CPTMASK_DIRN) == CPT_FROM_SERVER) {
                    alignptrs(&ip, &tcp, NULL, NULL, NULL, attrc->a_data.packet.p_data);
                    if (tcp != NULL && (tcp->tcp_flags & flag) == flag) {
                        min = MAX(min, diff);
                    }
                }
                time = attrc->a_data.packet.p_time;
            }
            iterattr(&attrc, &cpt->at_client);
        }

        time = 0;
        while (attrs != NULL) {
            if (attrs->a_type == ATTR_CAPTURED_PACKET) {
                diff = MAX(0, attrs->a_data.packet.p_time - time - 1);
                if ((attrs->a_data.packet.p_type & CPTMASK_DIRN) == CPT_FROM_SERVER) {
                    alignptrs(&ip, &tcp, NULL, NULL, NULL, attrs->a_data.packet.p_data);
                    if (tcp != NULL && (tcp->tcp_flags & flag) == flag && diff > min + 4) {
                        max = MIN(max, diff);
                    }
                }
                time = attrs->a_data.packet.p_time;
            }
            iterattr(&attrs, &cpt->at_server);
        }

        result->min = min;
        result->max = max;

    }
}


void analyze_ppred() {
    if (portpred.a == 0)
        nat.tcp.ppred_rate = -1;
    else
        nat.tcp.ppred_rate = portpred.b / (float) portpred.a;
}
