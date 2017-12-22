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
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <error.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <dlfcn.h>
#include <signal.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <libipq.h>
#include "iptables.h"

#define MAX_READWAIT     60
#define MAX_PKTSIZE      68

#define MAX_KEEPALIVE    300

#define MAX_PORTS        65536
#define MAX_IP           3

#define DEFAULT_TIMEO    3000
#define DEFAULT_THROTTLE 1000


char addresses[3][21];
struct sockaddr *servaddr[sizeof(addresses)];
int ports[2] = {PORT_STUNTD, 0};
int sock_raw;
const int one = 1;
char usable_devices[MAX_DEVICES][MAXLEN_IFACE];

struct attribute attr_server, attr_changed_address;

struct callback {
    unsigned short     port;
    unsigned int       ip;
    int               *pipes;
};

struct callback *hash[MAX_PORTS][MAX_IP];
pthread_mutex_t glock = PTHREAD_MUTEX_INITIALIZER;

struct waitargs {
    struct attribute   *sndpkt;
    struct attribute   *csndpkt;
    struct attribute   *rcvpkt;
    struct attribute   *crcvpkt;
    int                *pipes;
    struct message     *res;
    int                 clnt;
    struct attribute   *ctimeo;
    int                 start;
    int                 active;
    struct timespec     waittill;
    int                 waitn;
    int                 waitonce;
    uint32             *seq_self;
    uint32             *seq_peer;
    int                *seen_rst;
    struct sockaddr_in *saddr;
};

void nodebug_message(struct message *msg);
int nodebug_trace(FILE *out, const char *c, ...);
void (*dbgmsg)(struct message *msg) = nodebug_message;
int (*_dbgtrace)(FILE *out, const char *c, ...) = nodebug_trace;
FILE *dbgout;

void nodebug_message(struct message *msg) {
}


int nodebug_trace(FILE *out, const char *c, ...) {
    return 0;
}
void lock() {
    pthread_mutex_lock(&glock);
}

void unlock() {
    pthread_mutex_unlock(&glock);
}

unsigned int serv2ip(int i) {
    struct sockaddr *serv = servaddr[i];
    struct sockaddr_in *serv_in = (struct sockaddr_in *) serv;
    if (serv == NULL) return 0;
    if (serv->sa_family == AF_INET) return serv_in->sin_addr.s_addr;
    return 0;
}

int ip2serv(unsigned int ip) {
    int i;
    for (i=0; i<sizeof(servaddr); i++) {
        struct sockaddr *serv = servaddr[i];
        struct sockaddr_in *serv_in = (struct sockaddr_in *) serv;
        if (serv == NULL) continue;
        if (serv->sa_family == AF_INET && serv_in->sin_addr.s_addr == ip) return i;
    }
    return -1;
}

void *handle_packets(void *args) {
    struct ipq_handle *handle = ipq_create_handle(0, PF_INET);
    uint8 buf[256];

    if (!handle) {
        ipq_perror("ipq_create_handle");
        pthread_exit(NULL);
    }

    if (ipq_set_mode(handle, IPQ_COPY_PACKET, MAX_PKTSIZE) == -1) {
        ipq_perror("ipq_set_mode");
        ipq_destroy_handle(handle);
        pthread_exit(NULL);
    }

    while (1) {

        if (ipq_read(handle, buf, sizeof(buf), 0) == -1) {
            ipq_perror("ipq_read");
            break;
        }

        switch (ipq_message_type(buf)) {
            case IPQM_PACKET:
                {
                    ipq_packet_msg_t* msg = ipq_get_packet(buf);
                    uint32 addr = 0;
                    uint16 port = 0;
                    int i;
                    struct ip *ip, *icmpip;
                    struct tcp *tcp, *icmptcp;
                    struct icmp *icmp;
                    struct callback *cb = NULL;

                    alignptrs(&ip, &tcp, &icmp, &icmpip, &icmptcp, msg->payload);


                    if (tcp != NULL) {
                        addr = ip->ip_dst;
                        port = tcp->tcp_dstport;
                    } else if (icmp != NULL && icmptcp != NULL) {
                        addr = icmpip->ip_src;
                        port = icmptcp->tcp_srcport;
                    }

                    lock();
                    for(i=0; i<sizeof(hash[0]); i++) {
                        if (hash[port][i] == NULL) break;
                        if (hash[port][i]->ip == addr) {
                            cb = hash[port][i];
                            break;
                        }
                    }
                    if (cb == NULL) {
                        ipq_set_verdict(handle, msg->packet_id, NF_ACCEPT, 0, NULL);
                    } else {
                        struct attribute packet;
                        ipq_set_verdict(handle, msg->packet_id, NF_DROP, 0, NULL);
                        attrinit_capturedpacket(&packet, msg->data_len);
                        memcpy(packet.a_data.packet.p_data, msg->payload, msg->data_len);
                        write(cb->pipes[1], &packet, sizeof(struct attribute));
                    }
                    unlock();


                }
                break;

            default:
                break;
        }

    }
    ipq_destroy_handle(handle);
    pthread_exit(NULL);

}

void sendpkt(struct waitargs *args) {
    static struct sockaddr_in sin = {AF_INET, 0, {0}};
    if (args->sndpkt == NULL) return;
    lock();

    sin.sin_addr.s_addr =
        ((struct ip *)args->sndpkt->a_data.packet.p_data)->ip_dst;

    if (sendto(sock_raw, args->sndpkt->a_data.packet.p_data,
           args->sndpkt->a_len - LEN_PACKETHDR, 0,
           (struct sockaddr *)&sin, sizeof(sin)) == -1) {

        perror("sento");

    }

    if (args->sndpkt->a_data.packet.p_time == PKTTIME_UNINIT) {
        args->sndpkt = msgadd(args->res, args->sndpkt);
        args->sndpkt->a_data.packet.p_time = time(NULL) - args->start;
    }
    args->sndpkt->a_data.packet.p_times++;
    args->sndpkt->a_data.packet.p_type = CPT_FROM_SERVER;

    unlock();
}

void waitreset(struct waitargs *args) {
    args->waittill.tv_sec = 0;
    args->waittill.tv_nsec = 0;
    args->waitn = 0;
    args->waitonce = 0;
}

int alt2idx(int pktreq_flags) {
    switch(pktreq_flags & (PKTREQ_ALTADDR | PKTREQ_ALTPORT)) {
        case 0: return 0;
        case PKTREQ_ALTPORT: return 1;
        case PKTREQ_ALTADDR: return 2;
        case PKTREQ_ALTADDR | PKTREQ_ALTPORT: return 3;
    }
    return 0;
}

int wait(struct waitargs *args) {
    struct attr_capturetimeouts *capt = &args->ctimeo->a_data.capture;
    struct timeval now;
    int final = (args->waittill.tv_nsec % 2 == 1);
    int idx = args->crcvpkt == NULL ? 0 : alt2idx(args->crcvpkt->a_data.packetreq.p_flags);

    while (1) {
        struct timeval tm;
        fd_set rfds;
        int keepalive = 0;
        int r;

        if (args->waittill.tv_sec == 0) {
            int s, ns, tms;

            int timeo = (args->start - time(NULL)) * 1000 + capt->c_timeoms;
            int waito = (1 << args->waitn) *
                (args->csndpkt != NULL && args->csndpkt->a_data.packetreq.p_flags & PKTREQ_DELAY ?
                 capt->c_delayms :
                 args->waitonce ? DEFAULT_THROTTLE : DEFAULT_TIMEO);

            if (timeo < waito) {
                tms = timeo;
                final = 1;
            } else {
                tms = waito;
                final = 0;
            }

            gettimeofday(&now, NULL);

            s = now.tv_sec + tms / 1000;
            ns = now.tv_usec * 1000 + (tms % 1000) * 1000000;
            s += ns / 1000000000;
            ns %= 1000000000;
            if (final) ns += 1;

            args->waittill.tv_sec = s;
            args->waittill.tv_nsec = ns;
        }

        tm.tv_sec = args->waittill.tv_sec - time(NULL);
        tm.tv_usec = 0;

        if (tm.tv_sec > MAX_KEEPALIVE) {
            tm.tv_sec = MAX_KEEPALIVE;
            keepalive = 1;
        }

        FD_ZERO(&rfds);
        FD_SET(args->pipes[0], &rfds);
        FD_SET(args->clnt, &rfds);

        if (args->waittill.tv_sec == 0 ||
            (r = select(MAX(args->clnt, args->pipes[0]) + 1, &rfds, NULL, NULL, &tm)) == 0) {

            if (keepalive) {
                int mt, ml, e = 0;
                mt = args->res->m_type;
                ml = args->res->m_len;
                args->res->m_type = TYPE_NOP;
                args->res->m_len = 0;
                e = writemsg(args->clnt, args->res);
                args->res->m_type = mt;
                args->res->m_len = ml;
                if (e == -1) {
                    args->active = 0;
                    return -1;
                }
                continue;
            }

            args->waittill.tv_sec = 0;

            if (final || args->waitonce) {
                if (final) args->active = 0;
                return -1;
            } else {
                sendpkt(args);
                args->waitn++;
                continue;
            }
        } else if (r < 0) {
            args->active = 0;
            return -1;
        } else if (FD_ISSET(args->pipes[0], &rfds)) {
            struct ip *ip;
            struct tcp *tcp;
            read(args->pipes[0], args->rcvpkt, sizeof(struct attribute));
            args->rcvpkt->a_data.packet.p_type = CPT_TO_SERVER;
            args->rcvpkt->a_data.packet.p_time = time(NULL) - args->start;
            args->rcvpkt->a_data.packet.p_times = 1;
            msgadd(args->res, args->rcvpkt);
            alignptrs(&ip, &tcp, NULL, NULL, NULL, args->rcvpkt->a_data.packet.p_data);

            if (tcp != NULL) {
                if (tcp->tcp_flags & TCP_ACK && tcp->tcp_ack == args->seq_self[idx] &&
                    !(args->csndpkt != NULL &&
                      args->csndpkt->a_data.packetreq.p_flags & PKTREQ_REXMITACKED)) {
                    args->sndpkt = NULL;
                }
                if (tcp->tcp_flags & TCP_RST && tcp->tcp_seq == args->seq_peer[idx]) {
                    args->seen_rst[idx] = 1;
                    return -2;
                }
            }

            if (ip->ip_dst != args->saddr[idx].sin_addr.s_addr) continue;
            if (args->crcvpkt == NULL) return 0;
            if (ip->ip_proto != args->crcvpkt->a_data.packetreq.p_proto) continue;
            if (tcp != NULL) {
                if (tcp->tcp_dstport == args->saddr[idx].sin_port &&
                    tcp->tcp_flags == args->crcvpkt->a_data.packetreq.p_type)
                    return 0;
                else
                    continue;
            }

            return 0;
        } else if (FD_ISSET(args->clnt, &rfds)) {
            // Not expecting talback from client
            args->active = 0;
            return -1;
        }
    }
}

void register_port(struct callback *cb) {
    int j;

    lock();
    for(j=0; j<sizeof(hash[0]); j++) {
        if (hash[cb->port][j] == NULL) {
            hash[cb->port][j] = cb;
            break;
        }
    }
    unlock();
}

void unregister_port(struct callback *cb) {
    int j;
    lock();
    for(j=0; j<sizeof(hash[0]); j++) {
        if (hash[cb->port][j] == cb) {
            for(;j<sizeof(hash[0])-1; j++) {
                hash[cb->port][j] = hash[cb->port][j+1];
            }
            hash[cb->port][j] = NULL;
            break;
        }
    }
    unlock();
}

int req2addrs(int *used, struct message *msg) {
    int idx, n = 0;
    struct attribute *attr;
    iterattrinit(&attr, msg);
    for(;attr != NULL; iterattr(&attr,msg)) {
        if (attr->a_type != ATTR_PACKET_REQUEST) continue;
        idx = alt2idx(attr->a_data.packetreq.p_flags);
        if (used[idx] == -1) used[idx] = n++;
    }
    return n;
}


int capture(struct message *res, int clnt, struct sockaddr *caddr,  struct message *req) {
    struct attribute    rcv, snd, *crcvpkt, *crespaddr;
    struct sockaddr_in  saddr[4] = {{0,0,{0}},{0,0,{0}},{0,0,{0}},{0,0,{0}}};
    int                 socks[4]     = {-1, -1, -1, -1};
    int                 addrused[4]  = {-1, -1, -1, -1};
    struct callback     callbacks[4];
    int                 i;
    struct sockaddr_in  dest = {0, 0, {0}};
    //pthread_cond_t      cond         = PTHREAD_COND_INITIALIZER;
    //pthread_mutex_t     mutex        = PTHREAD_MUTEX_INITIALIZER;
    uint32              seq_self[4]     = {-1, -1, -1, -1};
    uint32              seq_peer[4]     = {-1, -1, -1, -1};
    int                 seen_rst[4]     = {0, 0, 0, 0};
    int                 err          =  0;
    int                 itern        = 0;
    int                 rand         = random();
    struct ip           *ip;
    struct tcp          *tcp;
    int                 pipes[2] = {-1, -1};
    struct waitargs     waitargs     = {
        &snd, NULL,
        &rcv, NULL,
        pipes,
        res, clnt,
        NULL,
        0,
        1,
        {0, 0}, 0, 0,
        seq_self, seq_peer,
        seen_rst,
        saddr
    };
    int seq_syns[]   = {
        htonl(0xcafebabe), htonl(0x5ca1ab1e),
        htonl(0xca5cade5), htonl(0xdeadbeef),
        htonl(0xdecafbad), htonl(0x50f7ba11),
        htonl(0x0b57ac1e), htonl(0xabbaacdc),
        htonl(0xdeadf00d), htonl(0xabadcafe),
        htonl(0xcafeface), htonl(0x00c0ffee),
        htonl(0xbad5ca1e), htonl(0xbeefba11),
        htonl(0x1a1a1a1a)
    };

    if ((crespaddr = findattr(req, ATTR_RESPONSE_ADDRESS)) != NULL) {
        sockaddrinit_addr((struct sockaddr *)&dest, sizeof(dest), crespaddr, req);
    }

    if ((waitargs.ctimeo = findattr(req, ATTR_CAPTURE_TIMEOUTS)) == NULL) {
        err = -1;
    }

    if (req2addrs(addrused, req) == 0) {
        err = -1;
    }

    if (pipe(pipes) == -1) {
        err = -1;
    }

    // Bind addresses if used
    for(i = 0; i < 4; i++) {
        if (addrused[i] == -1) continue;
        socks[i] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socks[i] == -1) { err = -1; break; }
        saddr[i].sin_family = AF_INET;
        saddr[i].sin_addr.s_addr = i / 2 == 0 ? serv2ip(1) : serv2ip(2);
        saddr[i].sin_port = i / 2 == 0 ? 0 : saddr[i-2].sin_port;
        if (bind(socks[i], (struct sockaddr *)&saddr[i], sizeof(saddr[i])) == -1) { err = -1; break; }
        int salen = sizeof(saddr[i]);
        getsockname(socks[i], (struct sockaddr *)&saddr[i], &salen);
        callbacks[i].ip = saddr[i].sin_addr.s_addr;
        callbacks[i].port = saddr[i].sin_port;
        callbacks[i].pipes = pipes;
    }

    if (err == -1) {
        for(i=0; i <4; i++) {
            close(socks[i]);
        }
        close(pipes[0]);
        close(pipes[1]);
        return -1;
    }

    // Add addresses in order
    for(i=0; i<4; i++) {
        int j;
        for(j=0; j<4; j++) {
            if (addrused[j] == i) {
                attrinit_changed(&snd, (struct sockaddr *)&saddr[j]);
                msgadd(res, &snd);
                register_port(&callbacks[j]);
            }
        }
    }


    dbgmsg(res);
    writemsg(clnt, res);
    msgreset(res);

    waitargs.start = time(NULL);
    waitargs.active = 1;

    while (waitargs.active) {
        int seq_syn = seq_syns[(rand + ++itern) % elemof(seq_syns)];
        waitreset(&waitargs);
        seen_rst[0] = seen_rst[1] = seen_rst[2] = seen_rst[3] = 0;

        iterattrinit(&crcvpkt, req);
        for(;waitargs.active && crcvpkt != NULL; iterattr(&crcvpkt, req)) {
            int proto, type, code, flags, idx, sz;
            if (crcvpkt->a_type != ATTR_PACKET_REQUEST) continue;

            proto = crcvpkt->a_data.packetreq.p_proto;
            type = crcvpkt->a_data.packetreq.p_type;
            code = crcvpkt->a_data.packetreq.p_code;
            flags = crcvpkt->a_data.packetreq.p_flags;
            idx = alt2idx(flags);

            if (seen_rst[idx]) continue;


            if (flags & PKTREQ_SEND) {
                switch(proto) {
                    case IPPROTO_ICMP:
                        attrinit_capturedpacket(&snd,
                            pktinit_icmp(snd.a_data.packet.p_data, LEN_PACKETDATA, rcv.a_data.packet.p_data,
                                (struct sockaddr_in *)caddr, type, code));
                        break;

                    case IPPROTO_TCP:
                        switch(type) {
                            case TCP_SYN:
                                attrinit_capturedpacket(&snd,
                                    pktinit_syn(snd.a_data.packet.p_data, LEN_PACKETDATA, &dest,
                                        &saddr[idx], (struct sockaddr_in *)caddr, seq_syn));
                                break;

                            case TCP_SYN | TCP_ACK:
                                attrinit_capturedpacket(&snd,
                                    pktinit_synack(snd.a_data.packet.p_data, LEN_PACKETDATA, rcv.a_data.packet.p_data,
                                        (struct sockaddr_in *)caddr, seq_syn));
                                break;

                            case TCP_ACK:
                                sz = pktinit_ack(snd.a_data.packet.p_data, LEN_PACKETDATA, rcv.a_data.packet.p_data,
                                        (struct sockaddr_in *)caddr);
                                if (code & PKTREQ_KEEPALIVE) {
                                    sz = pktfix_mkkeepalive(snd.a_data.packet.p_data);
                                }
                                attrinit_capturedpacket(&snd, sz);
                                break;

                            case TCP_FIN | TCP_ACK:
                                attrinit_capturedpacket(&snd,
                                    pktinit_finack(snd.a_data.packet.p_data, LEN_PACKETDATA, rcv.a_data.packet.p_data,
                                        (struct sockaddr_in *)caddr));
                                break;

                            case TCP_RST | TCP_ACK:
                                attrinit_capturedpacket(&snd,
                                    pktinit_rstack(snd.a_data.packet.p_data, LEN_PACKETDATA, rcv.a_data.packet.p_data,
                                        (struct sockaddr_in *)caddr));
                                break;

                        }
                        break;
                }
                alignptrs(&ip, &tcp, NULL, NULL, NULL, snd.a_data.packet.p_data);
                if (tcp != NULL && !(type == TCP_ACK && code & PKTREQ_KEEPALIVE)) {
                    nextseq(&seq_self[idx], &seq_peer[idx], ip, tcp);
                }
                waitargs.sndpkt = &snd;
                waitargs.csndpkt = crcvpkt;
                if (!(flags & PKTREQ_DELAY)) sendpkt(&waitargs);
            } else {
                waitargs.crcvpkt = crcvpkt;
                wait(&waitargs);
                waitreset(&waitargs);
                if (dest.sin_family == 0) {
                    alignptrs(&ip, &tcp, NULL, NULL, NULL, rcv.a_data.packet.p_data);
                    if (tcp != NULL) {
                        dest.sin_family = AF_INET;
                        dest.sin_addr.s_addr = ip->ip_src;
                        dest.sin_port = tcp->tcp_srcport;;
                    }
                }
            }
        }

        if (waitargs.active) {
            waitreset(&waitargs);
            waitargs.sndpkt = NULL;
            waitargs.csndpkt = NULL;
            waitargs.crcvpkt = NULL;
            waitargs.waitn = (itern - 1);
            waitargs.waitonce = 1;
            while(1) {
                if (wait(&waitargs) == -1) break;
            }
        }

    }


    // Unregister and cleanup
    for(i=0; i<4; i++) {
        if (addrused[i] != -1) {
            unregister_port(&callbacks[i]);
            close(socks[i]);
        }
    }
    close(pipes[0]);
    close(pipes[1]);

    return 0;
}

void *handle_client(void *arg) {
    int clnt = (int) arg;
    struct message req;
    struct message res;
    struct attribute attr;
    struct timeval tval;
    struct sockaddr_in caddr, saddr;
    int caddrlen = sizeof(caddr), saddrlen = sizeof(saddr);

    getpeername(clnt, (struct sockaddr *)&caddr, &caddrlen);
    getsockname(clnt, (struct sockaddr *)&saddr, &saddrlen);

    tval.tv_sec = MAX_READWAIT;
    tval.tv_usec = 0;

    if(setsockopt(clnt, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)) == -1) {
        debug("setting SO_RCVTIMEO failed");
    }

    while(1) {
        msgzero(&req);
        msgzero(&res);

        if (readmsg(clnt, &req) == -1) {

            /*
            msginit_error(&res, &req);
            attrinit_error(&attr, 400, "Message not supported");
            msgadd(&res, &attr);
            writemsg(clnt, &res);
            */

            break;

        }

        dbgmsg(&req);

        msginit_response(&res, &req);
        msgadd(&res, &attr_server);

        if (attrinit_unknown(&attr, &req) == -1) {
            msginit_error(&res, &req);
            msgadd(&res, &attr);
            attrinit_error(&attr, 420, "Attributes not supported");
            msgadd(&res, &attr);
        } else if (req.m_type == TYPE_BINDING_REQ) {
            attrinit_mapped(&attr, (struct sockaddr *)&caddr);
            msgadd(&res, &attr);
            attrinit_xormapped(&attr, (struct sockaddr *)&caddr, &res);
            msgadd(&res, &attr);
            attrinit_source(&attr, (struct sockaddr *)&saddr);
            msgadd(&res, &attr);

            msgadd(&res, &attr_changed_address);
        } else if (req.m_type == TYPE_CAPTURE_REQ && servaddr[2] != NULL) {
            int err = -1;
            uint16 needed[] = {ATTR_PACKET_REQUEST, ATTR_CAPTURE_TIMEOUTS};
            if (hasattr_all(&req, needed, 2) == 0) {
                err = capture(&res, clnt, (struct sockaddr *)&caddr, &req);
            }

            if (err == -1) {
                msginit_error(&res, &req);
                attrinit_error(&attr, 400, "Required attributes not present");
                msgadd(&res, &attr);
            }
        } else {
            msginit_error(&res, &req);
            attrinit_error(&attr, 400, "Message not supported");
            msgadd(&res, &attr);
        }

        dbgmsg(&res);

        if (writemsg(clnt, &res) == -1) break;
        continue;
    }

    close(clnt);
    pthread_exit(NULL);
}

void display_help(char *argv0) {
    printf("Usage: \n");
    printf("        %s -i iface1 -i iface2 -i iface3\n", argv0);
    printf("        %s -i help\n", argv0);
    printf("Options:\n");
    printf("-i, --%-20s interface e.g. eth0, eth0:1\n", "interface iface");
    printf("-o, --%-20s e.g. \"Cornell STUNT server <admin@cornell.edu>\"\n", "operator string");
    printf("-v, --%-20s increase verbosity\n", "debug");
}

void display_devices() {
    pcap_if_t *device = getdevices();
    int i = 0;

    if (device == NULL) {
        fprintf(stderr, "Error opening device list. Need root privileges.");
        return;
    }

    fprintf(stderr, "Device: Name\n");
    do {
        pcap_addr_t *a = device->addresses;
        int sp = 0;
        sp = 0;
        do {
            if (a == NULL || device->flags & PCAP_IF_LOOPBACK) break;
            if (a->addr != NULL) {
                struct sockaddr *s = a->addr;
                if (s->sa_family != AF_INET) continue;
                if (sp == 0) {
                    fprintf(stderr, "%6d: ", i);
                    fprintf(stderr, "%s", device->name);
                    if (device->description != NULL) {
                        fprintf(stderr, "\n        %s", device->description);
                    }
                    fprintf(stderr, "\n        IP: ");
                    sp = 1;
                } else {
                    fprintf(stderr, ", ");
                }
                fprintf(stderr, "%s", inet_ntoa(((struct sockaddr_in *)s)->sin_addr));
            }
        } while ((a = a->next) != NULL);
        i++;
        if (sp) fprintf(stderr, "\n\n");
    } while ((device = device->next) != NULL);
}

int parse_args(int argc, char **argv) {
    int c;
    int a = 0;

    attrinit_server(&attr_server, "");

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"interface", 1, 0, 'i'},
            {"operator", 1, 0, 'o'},
            {"help", 1, 0, 'a'},
            {"debug", 0, 0, 'v'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "i:o:hv", long_options, &option_index);

        if (c == -1) break;

        switch(c) {
            case 'i':
                if (strcmp(optarg, "help") == 0) {
                    display_devices();
                    exit(1);
                } else if (a < sizeof(addresses) - 1 &&
                    getifaddr(addresses[a], sizeof(addresses[a]), &optarg) != -1) {

                    struct sockaddr_in *sin =
                        (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

                    sin->sin_family = AF_INET;
                    sin->sin_addr.s_addr = inet_addr(addresses[a]);
                    sin->sin_port = 0;

                    servaddr[a] = (struct sockaddr *)sin;

                    a++;

                } else {
                    fprintf(stderr, "Ignoring non-existant interface %s\n", optarg);
                }
                break;

            case 'o':
                {
                    char str[MAXLEN_ATTR];
                    snprintf(str, MAXLEN_ATTR, "STUNTD %d.%d, %s", MAJOR, MINOR, optarg);
                    attrinit_server(&attr_server, str);
                }
                break;

            case 'v':
                dbgmsg = debug_message;
                break;

            case '?':
            case 'h':
                display_help(argv[0]);
                exit(1);
        }
    }

    if (a < 3) {
        display_help(argv[0]);
        exit(1);
    }

    return 0;
}

struct iptables_target *ipt_queue = NULL;
void register_target(struct iptables_target *me) {
    ipt_queue = me;
}

void deinit_iptables() {
    iptc_handle_t handle;
    int i = 1;
    const struct ipt_entry *e, *ep;

    handle = iptc_init("filter");

    ep = NULL;
    e = iptc_first_rule("INPUT", &handle);
    while (e != NULL) {
        const char *t = iptc_get_target(e, &handle);
        if (t && strcmp(t, "QUEUE") == 0) {
            iptc_delete_num_entry("INPUT", i--, &handle);
            e = ep == NULL ? iptc_first_rule("INPUT", &handle) : ep;
            continue;
        }
        ep = e;
        e = iptc_next_rule(e, &handle);
        i++;
    }

    iptc_commit(&handle);
}

void cleanup(int i) {
    exit(1);
}

void init_iptables() {
    size_t size;
    struct iptables_target *target = NULL;
    iptc_handle_t handle;
    char buf[sizeof(struct ipt_entry) + sizeof(struct ipt_entry_target)];
    struct ipt_entry *fw = (struct ipt_entry *)buf;
    int i;

    memset(buf, 0, sizeof(buf));

    dlopen("libipt_standard.so", RTLD_NOW);
    dlopen("/lib/iptables/libipt_standard.so", RTLD_NOW);
    dlopen("/usr/lib/iptables/libipt_standard.so", RTLD_NOW);
    target = ipt_queue;
    handle = iptc_init("filter");

    if (target == NULL || handle == NULL) {
        fprintf(stderr, "stuntd: could not locate iptables or queue target\n");
        exit(1);
    }

    size = IPT_ALIGN(sizeof(struct ipt_entry_target)) + target->size;

    target->t = malloc(size);
    target->t->u.target_size = size;
    strcpy(target->t->u.user.name, "QUEUE");
    target->init(target->t, &fw->nfcache);

    fw->ip.dmsk.s_addr = ~0;
    fw->target_offset = sizeof(struct ipt_entry);
    fw->next_offset = fw->target_offset + target->t->u.target_size;

    memcpy(fw->elems, target->t, target->t->u.target_size);

    for(i=1; i<3; i++) {
        if (servaddr[i] == NULL) continue;
        fw->ip.dst.s_addr = ((struct sockaddr_in *)servaddr[i])->sin_addr.s_addr;
        iptc_insert_entry("INPUT", fw, 0, &handle);
    }

    iptc_commit(&handle);
    atexit(deinit_iptables);
}

int main(int argc, char **argv) {
    int                serv[4];
    struct sockaddr_in addr;
    fd_set             rfds;
    int                max     = -1;
    pthread_attr_t     attr;
    int                i;

    dbgout = stderr;

    if (getuid() != 0) {
        fprintf(stderr, "stuntd: Need root privileges\n");
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, cleanup);
    signal(SIGINT,  cleanup);
    signal(SIGQUIT, cleanup);
    signal(SIGABRT, cleanup);

    FD_ZERO(&rfds);
    addr.sin_family = AF_INET;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    if (parse_args(argc, argv) == -1) {
        display_help(argv[0]);
        exit(1);
    }

    init_iptables();

    for (i=0; i<4; i++) {

        serv[i] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (serv[i] == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        addr.sin_addr.s_addr = inet_addr(addresses[i/2]);
        addr.sin_port = htons(ports[i%2]);

        if (addr.sin_addr.s_addr == 0 ||
            addr.sin_addr.s_addr == -1 ||
            htonl(atoi(addresses[i / 2])) == addr.sin_addr.s_addr) {

            errnum = EADDRNOTAVAIL;
            perror("inet_addr");
            exit(EXIT_FAILURE);

        }

        if (bind(serv[i], (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        int addrlen = sizeof(addr);
        getsockname(serv[i], (struct sockaddr *)&addr, &addrlen);
        ports[i % 2] = ntohs(addr.sin_port);

        if (i == 3) {
            attrinit_changed(&attr_changed_address, (struct sockaddr *)&addr);
        }

        if (listen(serv[i], 5) == -1) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        FD_SET(serv[i], &rfds);
        max = max > serv[i] ? max : serv[i];

    }


    if ((sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(sock_raw, SOL_IP, IP_HDRINCL, (char *)&one, sizeof(one));

    {
        pthread_t tid;
        pthread_create(&tid, &attr, handle_packets, NULL);
    }

    while(1) {
        fd_set _rfds = rfds;
        int clnt;
        pthread_t tid;

        select(max + 1, &_rfds, NULL, NULL, NULL);

        for(i=0; i<4; i++) {
            if (FD_ISSET(serv[i], &_rfds)) {
                clnt = accept(serv[i], NULL, 0);
                if (clnt != -1) {
                    pthread_create(&tid, &attr, handle_client, (void *)clnt);
                }
            }
        }
    }


}
