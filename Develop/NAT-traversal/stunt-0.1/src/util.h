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

#ifndef __UTIL_H
#define __UTIL_H

#define MAXLEN_ATTR   (128 - 4)
#define MAXLEN_VALUES (8192 - 20)
#define MAXLEN_IFACE   IFNAMSIZ
#define MAX_DEVICES   16

#define MAJOR         0
#define MINOR         1

#include "stunt.h"
#include <sys/types.h>

#include "compat.h"

extern char usable_devices[MAX_DEVICES][MAXLEN_IFACE];

struct ip {
    uint8  ip_hlen    :4, ip_ver:4;      // LITTLE ENDIAN
    uint8  ip_tos;
    uint16 ip_totlen;
    uint16 ip_ident;
    uint16 ip_flag    :3, ip_fragoff:13;
    uint8  ip_ttl;
    uint8  ip_proto;
    uint16 ip_check;
    uint32 ip_src;
    uint32 ip_dst;
};

struct icmp {
    uint8  icmp_type;
    uint8  icmp_code;
    uint16 icmp_check;
    uint32 pad1;
    char   icmp_payload[0];
};

#define ICMP_TYPE_TTL          11
#define ICMP_TYPE_UNREACH      3
#define ICMP_CODE_HOST_UNREACH 1

struct udp {
    uint16 udp_srcport;
    uint16 udp_dstport;
    uint16 udp_length;
    uint16 udp_check;
};

struct tcp_opt_timestamp {
    uint8  t_kind;       // 8
    uint8  t_len;        // 10
    uint32 t_val;
    uint32 t_echo;
    union {
        struct {
            uint8 tcp_data[0];
        } none;
    } next;
};

struct tcp_opt_stunt {
    uint8  s_kind;
    uint8  s_len;        // 14
    uint8  s_magic[6];   // STUNT:
    uint32 s_ip;
    uint16 s_port;

    union {
        struct tcp_opt_timestamp ts;
        struct {
            uint8 tcp_data[0];
        } none;
    } next;
};

struct tcp {
    uint16 tcp_srcport;
    uint16 tcp_dstport;
    uint32 tcp_seq;
    uint32 tcp_ack;
    uint8 pad1:4,tcp_hlen:4; // LITTLE ENDIAN
    uint8 tcp_flags:6, pad2:2; // LITTLE ENDIAN
    uint16 tcp_win;
    uint16 tcp_check;
    uint16 tcp_urg;
    union {
        struct tcp_opt_stunt stunt;
        struct {
            uint8 tcp_data[0];
        } none;
        uint8 raw[0];
    } tcp_opt;
};

#define TCP_FIN              0x01
#define TCP_SYN              0x02
#define TCP_ACK              0x10
#define TCP_RST              0x04
#define TCP_OPT_EOL          0
#define TCP_OPT_NOP          1
#define TCP_OPT_STUNT        253
#define TCP_OPTLEN_STUNT     14
#define TCP_OPT_TIMESTAMP    8
#define TCP_OPTLEN_TIMESTAMP 10

struct tcp_pseudo {
    uint32 pseudo_src;
    uint32 pseudo_dst;
    uint8 pad1;
    uint8 pseudo_proto;
    uint16 pseudo_length;
};

struct intpair {
    int a;
    int b;
};
typedef struct intpair intpair;

#define PKTTIME_UNINIT (0xFFFF)


#define NIPQUAD(addr) ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]

void msgzero(struct message *msg);
void attrinit_error(struct attribute *attr, int err, char *msg);
void msginit_request(struct message *res, int type);
void msginit_response(struct message *res, struct message *req);
void msginit_error(struct message *res, struct message *req);
void msgreset(struct message *res);
struct attribute *msgadd(struct message *msg, struct attribute *attr);
int sockaddrinit_addr(struct sockaddr *saddr, int saddrlen, struct attribute *attr, struct message *msg);
int attrinit_unknown(struct attribute *attr, struct message *msg);
void attrinit_server(struct attribute *attr, const char *str);
void attrinit_addr(struct attribute *attr, struct sockaddr *addr, int type);
void attrinit_mapped(struct attribute *attr, struct sockaddr *addr);
void attrinit_changed(struct attribute *attr, struct sockaddr *addr);
void attrinit_source(struct attribute *attr, struct sockaddr *addr);
void attrinit_response(struct attribute *attr, struct sockaddr *addr);
void attrinit_xormapped(struct attribute *attr, struct sockaddr *addr, struct message *res);
void attrinit_changerequest(struct attribute *attr, int req);
void attrinit_capturetimeouts(struct attribute *attr, unsigned int timeoms, unsigned int delayms);
void attrinit_capturedpacket(struct attribute *attr, int len);
void attrinit_pktreq(struct attribute *attr, int proto, int type, int code, int flags);
int pktinit_syn(char *buf, unsigned int  buflen, struct sockaddr_in *dst, struct sockaddr_in *src, struct sockaddr_in *caddr, int seq);
int pktinit_synack(char *buf, unsigned int  buflen, char *rcv, struct sockaddr_in *caddr, int seq);
int pktinit_ack(char *buf, unsigned int  buflen, char *rcv, struct sockaddr_in *caddr);
int pktinit_finack(char *buf, unsigned int  buflen, char *rcv, struct sockaddr_in *caddr);
int pktinit_rstack(char *buf, unsigned int  buflen, char *rcv, struct sockaddr_in *caddr);
int pktinit_icmp(char *buf, unsigned int  buflen, char *rcv, struct sockaddr_in *caddr, int type, int code);
int pktfix_mkkeepalive(char *buf);
int writemsg(SOCKET sock, struct message *msg);
int readmsg(SOCKET sock, struct message *msg);
struct attribute *findattr(struct message *msg, int type);
struct attribute *findnattr(struct message *msg, int type, int idx);
int hasattr_all(struct message *msg, unsigned short *type, int typenum);
int getifaddr(char *ip, int size, char **iface);
pcap_if_t *getdevices();
void alignptrs(struct ip **ip, struct tcp **tcp, struct icmp **icmp, struct ip **icmpip, struct tcp **icmptcp, char *pkt);
void nextseq(uint32 *seq_self, uint32 *seq_peer, struct ip *ip, struct tcp *tcp);
void iterattrinit(struct attribute **attr, struct message *msg);
void iterattr(struct attribute **attr, struct message *msg);
void populate_devices();

int MAX(int a, int b);
int MIN(int a, int b);
void debug_message(struct message *msg);

#define elemof(x)       (sizeof(x)/sizeof(x[0]))

#define dbgtrace()     _dbgtrace(dbgout, "%s:%d:%s: tracepoint\n", __FILE__, __LINE__, __FUNCTION__)
extern int (*_dbgtrace)(FILE *out, const char *c, ...);
extern FILE *dbgout;


#endif
