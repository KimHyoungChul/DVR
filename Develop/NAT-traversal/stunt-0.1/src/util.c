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

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <time.h>

#include "compat.h"

int ipid = 0;

struct tcp_opt_stunt opt_stunt = {
    TCP_OPT_STUNT,
    TCP_OPTLEN_STUNT,
    "STUNT:",
    0,
    0,
    { {0,0,0,0} }
};

void msgzero(struct message *msg) {
    msg->m_len = 0;
    msg->m_type = 0;
    memset(msg, 0, sizeof(msg->m_type) + sizeof(msg->m_len) + sizeof(msg->m_xid));
}

void attrinit_error(struct attribute *attr, int err, char *msg) {
    attr->a_type = ATTR_ERROR_CODE;
    attr->a_len  = offsetof(struct attr_error, e_reason) + (uint16) strlen(msg) + 1;
    attr->a_data.error.e_class = err / 100;
    attr->a_data.error.e_number = err % 100;
    attr->a_data.error._pad = 0;
    strncpy(attr->a_data.error.e_reason, msg, MAXLEN_ATTR - LEN_ERRORHDR);
}

void msgreset(struct message *res) {
    res->m_len = 0;
}

void msginit_request(struct message *res, int type) {
    int i;

    msgzero(res);

    for(i = 0; i < sizeof(res->m_xid); i++) {
        res->m_xid[i] = random() % 0xFF;
    }

    res->m_type = type & 0xF00F;
}

void msginit_response(struct message *res, struct message *req) {
    memcpy(res->m_xid, req->m_xid, sizeof(res->m_xid));
    res->m_type = (req->m_type & 0xF00F) | 0x0100;
}

void msginit_error(struct message *res, struct message *req) {
    memcpy(res->m_xid, req->m_xid, sizeof(res->m_xid));
    res->m_type = (req->m_type & 0xF00F) | 0x0110;
}

struct attribute *msgadd(struct message *msg, struct attribute *attr) {
    int a_size = sizeof(attr->a_type) + sizeof(attr->a_len) + attr->a_len;
    struct attribute *dst = (struct attribute *)&msg->m_data[msg->m_len];
    if (msg->m_len + a_size > MAXLEN_VALUES) {
        return NULL;
    }
    memcpy(dst, attr, a_size);
    msg->m_len += a_size;
    return dst;
}

int attrinit_unknown(struct attribute *attr, struct message *msg) {
    unsigned int i = 0;
    unsigned int n = 0;
    unsigned int t = 0;

    attr->a_type = ATTR_UNKNOWN_ATTRIBUTES;
    attr->a_len  = 0;

    while (i < msg->m_len) {
        struct attribute *mattr = (struct attribute *)&msg->m_data[i];
        i += mattr->a_len + LEN_ATTRHDR;

        switch(mattr->a_type) {
            case ATTR_CHANGE_REQUEST:
            case ATTR_CAPTURE_TIMEOUTS:
            case ATTR_SOURCE_ADDRESS:
            case ATTR_RESPONSE_ADDRESS:
            case ATTR_PACKET_REQUEST:
                break;

            default:
                if (ntohs(mattr->a_type) > ATTR_OPTIONAL) break;
                if (sizeof(uint16) * 2 * (1 + (n / 2)) > MAXLEN_ATTR) break;
                attr->a_len = sizeof(uint16) * 2 * (1 + (n / 2));
                for(t = n; t < attr->a_len / sizeof(uint16); t++) {
                    attr->a_data.unknown.u_attr[t] = mattr->a_type;
                }
                n++;
        }
    }

    return n > 0 ? -1 : 0;
}

void attrinit_server(struct attribute *attr, const char *str) {
    attr->a_type = ATTR_SERVER;
    attr->a_len = MIN(4 * (1 + ((int)strlen(str) + 1) / 4), MAXLEN_ATTR);
    memset(attr->a_data.server.s_details, 0, attr->a_len);
    strncpy(attr->a_data.server.s_details, str, attr->a_len);
}

void xor(char *d, char *x, int c) {
    int i;
    for(i = 0; i < c; i++) {
        d[i] ^= x[i];
    }
}

int sockaddrinit_addr(struct sockaddr *saddr, int saddrlen, struct attribute *attr, struct message *msg) {
    struct sockaddr_in *sin = (struct sockaddr_in *)saddr;
    int err = 0;
    if (attr == NULL) {
        dbgtrace();
        return -1;
    }
    switch (attr->a_type) {
        case ATTR_XOR_MAPPED_ADDRESS:
            xor((char *)&attr->a_data.address.a_port, msg->m_xid,
                sizeof(attr->a_data.address.a_port));
        case ATTR_SOURCE_ADDRESS:
        case ATTR_RESPONSE_ADDRESS:
        case ATTR_CHANGED_ADDRESS:
        case ATTR_MAPPED_ADDRESS:
            switch(attr->a_data.address.a_type) {
                case ADDR_IPv4:
                    if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                        xor(attr->a_data.address.a_addr.ipv4.raw, msg->m_xid,
                            sizeof(attr->a_data.address.a_addr.ipv4.raw));
                    if (saddrlen >= sizeof(struct sockaddr_in)) {
                        sin->sin_addr.S_addr = attr->a_data.address.a_addr.ipv4.word.s_addr;
                        sin->sin_port = attr->a_data.address.a_port;
                        sin->sin_family = AF_INET;
                    } else {
                        err = -1;
                    }
                    if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                        xor(attr->a_data.address.a_addr.ipv4.raw, msg->m_xid,
                            sizeof(attr->a_data.address.a_addr.ipv4.raw));
                    break;

                case ADDR_IPv6:
                    err = -1;
                    break;
            }
            if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                xor((char *)&attr->a_data.address.a_port, msg->m_xid,
                    sizeof(attr->a_data.address.a_port));
            break;

        default:
            err = -1;

    }
    return err;
}

void attrinit_addr(struct attribute *attr, struct sockaddr *addr, int type) {
    switch(addr->sa_family) {
        case AF_INET:
            {
                struct sockaddr_in *sin = (struct sockaddr_in *)addr;
                attr->a_type = type;
                attr->a_len  = offsetof(struct attr_address, a_addr.ipv4) +
                    sizeof(union addr_ipv4);
                attr->a_data.address.a_type = ADDR_IPv4;
                attr->a_data.address.a_port = sin->sin_port;
                attr->a_data.address.a_addr.ipv4.word.s_addr = sin->sin_addr.S_addr;
            }
            break;

        default:
            attrinit_error(attr, 600, "Non IPv4 addresses not supported");
            break;
    }
}

void attrinit_mapped(struct attribute *attr, struct sockaddr *addr) {
    attrinit_addr(attr, addr, ATTR_MAPPED_ADDRESS);
}

void attrinit_response(struct attribute *attr, struct sockaddr *addr) {
    attrinit_addr(attr, addr, ATTR_RESPONSE_ADDRESS);
}

void attrinit_source(struct attribute *attr, struct sockaddr *addr) {
    attrinit_addr(attr, addr, ATTR_SOURCE_ADDRESS);
}

void attrinit_changed(struct attribute *attr, struct sockaddr *addr) {
    attrinit_addr(attr, addr, ATTR_CHANGED_ADDRESS);
}

void attrinit_xormapped(struct attribute *attr, struct sockaddr *addr, struct message *res) {
    attrinit_addr(attr, addr, ATTR_XOR_MAPPED_ADDRESS);
    if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS) {

        xor((char *)&attr->a_data.address.a_port, res->m_xid,
            sizeof(attr->a_data.address.a_port));

        if (attr->a_data.address.a_type == ADDR_IPv4) {
            xor(attr->a_data.address.a_addr.ipv4.raw, res->m_xid,
                sizeof(attr->a_data.address.a_addr.ipv4.raw));
        } else {
            xor(attr->a_data.address.a_addr.ipv6.raw, res->m_xid,
                sizeof(attr->a_data.address.a_addr.ipv6.raw));
        }
    }
}

void attrinit_changerequest(struct attribute *attr, int req) {
    attr->a_type = ATTR_CHANGE_REQUEST;
    attr->a_len  = sizeof(attr->a_data.change_request.c_change);
    attr->a_data.change_request.c_change = req;
}

void attrinit_capturetimeouts(struct attribute *attr, unsigned int timeoms,
                      unsigned int delayms) {

    attr->a_type = ATTR_CAPTURE_TIMEOUTS;
    attr->a_len  = sizeof(struct attr_capturetimeouts);
    attr->a_data.capture.c_timeoms = timeoms;
    attr->a_data.capture.c_delayms =delayms;
}

void attrinit_capturedpacket(struct attribute *attr, int len) {
    attr->a_type = ATTR_CAPTURED_PACKET;
    attr->a_len  = MIN(len + LEN_PACKETHDR, MAXLEN_ATTR);
    attr->a_data.packet.p_times = 0;
    attr->a_data.packet.p_time = PKTTIME_UNINIT;
    attr->a_data.packet.p_type = 0;
}

void attrinit_pktreq(struct attribute *attr, int proto, int type, int code, int flags) {
    attr->a_type = ATTR_PACKET_REQUEST;
    attr->a_len  = sizeof(struct attr_packetreq);
    attr->a_data.packetreq.p_proto = proto;
    attr->a_data.packetreq.p_type = type;
    attr->a_data.packetreq.p_code = code;
    attr->a_data.packetreq.p_flags = flags;
}

char *attr_aton(int a_type, char *buf, int bufsize) {
    snprintf(buf, bufsize, cc("%d"), a_type);
    return a_type == ATTR_SERVER ? "SERVER" :
        a_type == ATTR_ERROR_CODE ? "ERROR" :
        a_type == ATTR_UNKNOWN_ATTRIBUTES ? "UNKNOWN_ATTRIBUTES" :
        a_type == ATTR_CHANGED_ADDRESS ? "CHANGED_ADDRESS" :
        a_type == ATTR_SOURCE_ADDRESS ? "SOURCE_ADDRESS" :
        a_type == ATTR_RESPONSE_ADDRESS ? "RESPONSE_ADDRESS" :
        a_type == ATTR_XOR_MAPPED_ADDRESS ? "XOR_MAPPED_ADDRESS" :
        a_type == ATTR_MAPPED_ADDRESS ? "MAPPED_ADDRESS" :
        a_type == ATTR_REFLECTED_FROM ? "REFLECTED_FROM" :
        a_type == ATTR_XOR_MAPPED_ADDRESS ? "XOR_MAPPED_ADDRESS" :
        a_type == ATTR_CHANGE_REQUEST ? "CHANGE_REQUEST" :
        a_type == ATTR_CAPTURE_TIMEOUTS ? "CAPTURE_TIMEOUTS" :
        a_type == ATTR_CAPTURED_PACKET ? "CAPTURED_PACKET" :
        a_type == ATTR_PACKET_REQUEST ? "PACKET_REQUEST" :
        buf;
}

void debug_attribute(struct attribute *attr, struct message *msg) {
    char buf[16];
    fprintf(dbgout, cp("{" ca("type") "=" ce("%s") ", " ca("len") "=" cc("%d")),
           attr_aton(attr->a_type, buf, sizeof(buf)),
           attr->a_len);

    switch(attr->a_type) {
        case ATTR_SERVER:
            fprintf(dbgout, cp(", " ca("details") "=" cc("\"%s\"")), attr->a_data.server.s_details);
            break;

        case ATTR_ERROR_CODE:
            fprintf(dbgout, cp(", " ca("class") "=" cc("%d")), attr->a_data.error.e_class);
            fprintf(dbgout, cp(", " ca("number") "=" cc("%d")), attr->a_data.error.e_number);
            fprintf(dbgout, cp(", " ca("reason") "=" cc("\"%s\"")), attr->a_data.error.e_reason);
            break;

        case ATTR_PACKET_REQUEST:
            snprintf(buf, sizeof(buf), cc("%d"), attr->a_data.packetreq.p_proto);
            fprintf(dbgout, cp(", " ca("proto") "=" ce("%s")),
                       attr->a_data.packetreq.p_proto == IPPROTO_TCP ? "TCP" :
                       attr->a_data.packetreq.p_proto == IPPROTO_ICMP ? "ICMP" :
                       attr->a_data.packetreq.p_proto == IPPROTO_UDP ? "UDP" :
                       buf);
            if (attr->a_data.packetreq.p_proto == IPPROTO_TCP) {
                int type = attr->a_data.packetreq.p_type;
                snprintf(buf, sizeof(buf), cc("%x"), type);
                fprintf(dbgout, cp(", " ca("tcp_flags") "=[" ce("%s") "]"),
                       type == TCP_SYN ? "SYN" :
                       type == (TCP_SYN | TCP_ACK) ? "SYN" cp(", ") ce("ACK") :
                       type == TCP_ACK ? "ACK" :
                       type == (TCP_FIN | TCP_ACK) ? "FIN" cp(", ") ce("ACK") :
                       type == (TCP_RST | TCP_ACK) ? "RST" cp(", ") ce("ACK") :
                       buf
                       );
            } else {
                fprintf(dbgout, cp(", " ca("type") "=" cc("%x")), attr->a_data.packetreq.p_type);
            }
            fprintf(dbgout, cp(", " ca("code") "=" cc("%d")), attr->a_data.packetreq.p_code);
            {
                int flags = attr->a_data.packetreq.p_flags;
                fprintf(dbgout, cp(", " ca("flags") "=["));
                if (flags & PKTREQ_SEND) fprintf(dbgout, ce("SEND"));
                else fprintf(dbgout, ce("WAIT"));
                if (flags & PKTREQ_REXMITACKED) fprintf(dbgout, cp(", " ce("REXMIT_ACK'ed")));
                if (flags & PKTREQ_DELAY) fprintf(dbgout, cp(", " ce("DELAY")));
                if (flags & PKTREQ_ALTADDR) fprintf(dbgout, cp(", " ce("ALT_ADDR")));
                if (flags & PKTREQ_ALTPORT) fprintf(dbgout, cp(", " ce("ALT_PORT")));
                fprintf(dbgout, cp("]"));
            }
            break;

        case ATTR_CHANGE_REQUEST:
            fprintf(dbgout, cp(", " ca("change") "=" cc("0x%x")), (unsigned int) attr->a_data.change_request.c_change);
            break;

        case ATTR_CAPTURED_PACKET:
            {
                struct ip *ip, *icmpip;
                struct tcp *tcp, *icmptcp;
                struct icmp *icmp;
                alignptrs(&ip, &tcp, &icmp, &icmpip, &icmptcp,
                          attr->a_data.packet.p_data);

                fprintf(dbgout, cp(", " ca("type") "=" ce("%s")),
                       attr->a_data.packet.p_type & CPT_TO_SERVER ? "TO_SERVER" : "FROM_SERVER" );
                fprintf(dbgout, cp(", " ca("times") "=" cc("%d")), attr->a_data.packet.p_times);
                fprintf(dbgout, cp(", " ca("at") "=" cc("%ds")), attr->a_data.packet.p_time);
                fprintf(dbgout, cp(", " ca("data") "=["));
                fprintf(dbgout, cp(ca("ip_id") "=" cc("0x%04x")), ntohs(ip->ip_ident));
                fprintf(dbgout, cp(", " ca("ip_src") "=" cc("%d.%d.%d.%d")), NIPQUAD(ip->ip_src));
                fprintf(dbgout, cp(", " ca("ip_dst") "=" cc("%d.%d.%d.%d")), NIPQUAD(ip->ip_dst));
                fprintf(dbgout, cp(", " ca("ip_ttl") "=" cc("%d")), ip->ip_ttl);
                snprintf(buf, sizeof(buf), cc("%d"), ip->ip_proto);
                fprintf(dbgout, cp(", " ca("ip_proto") "=" ce("%s")),
                       ip->ip_proto == IPPROTO_TCP ? "TCP" :
                       ip->ip_proto == IPPROTO_ICMP ? "ICMP" :
                       ip->ip_proto == IPPROTO_UDP ? "UDP" :
                       buf);
                if (tcp != NULL) {
                    int flags = 0;
                    fprintf(dbgout, cp(", " ca("tcp_sport") "=" cc("%d")), ntohs(tcp->tcp_srcport));
                    fprintf(dbgout, cp(", " ca("tcp_dport") "=" cc("%d")), ntohs(tcp->tcp_dstport));
                    fprintf(dbgout, cp(", " ca("flags") "=["));
                    if (tcp->tcp_flags & TCP_SYN) { fprintf(dbgout, ce("SYN")); flags = 1; }
                    if (tcp->tcp_flags & TCP_FIN) { if (flags) fprintf(dbgout, cp(", ")); fprintf(dbgout, ce("FIN")); flags = 1; }
                    if (tcp->tcp_flags & TCP_RST) { if (flags) fprintf(dbgout, cp(", ")); fprintf(dbgout, ce("RST")); flags = 1; }
                    if (tcp->tcp_flags & TCP_ACK) { if (flags) fprintf(dbgout, cp(", ")); fprintf(dbgout, ce("ACK")); flags = 1; }
                    fprintf(dbgout, cp("]"));
                    fprintf(dbgout, cp(", " ca("seq") "=" cc("0x%08x")), ntohl(tcp->tcp_seq));
                    if (tcp->tcp_flags & TCP_ACK) {
                        fprintf(dbgout, cp(", " ca("ack") "=" cc("0x%08x")), ntohl(tcp->tcp_ack));
                    }
                }
                if (icmp != NULL) {
                    snprintf(buf, sizeof(buf), cc("%d"), icmp->icmp_type);
                    fprintf(dbgout, cp(", " ca("icmp_type") "=" ce("%s")),
                           icmp->icmp_type == ICMP_TYPE_TTL ? "TTL_EXCEEDED" :
                           icmp->icmp_type == ICMP_TYPE_UNREACH ? "UNREACHABLE" :
                           buf
                           );
                    if (icmp->icmp_type != ICMP_TYPE_TTL) {
                        snprintf(buf, sizeof(buf), cc("%d"), icmp->icmp_code);
                        fprintf(dbgout, cp(", " ca("icmp_code") "=" ce("%s")),
                               icmp->icmp_type == ICMP_TYPE_UNREACH && icmp->icmp_code == ICMP_CODE_HOST_UNREACH ? "HOST" :
                               buf
                               );
                    }
                    fprintf(dbgout, cp(", " ca("icmpip_id") "=" cc("0x%04x")), ntohs(icmpip->ip_ident));
                }
                fprintf(dbgout, cp("]"));
            }
            break;

        case ATTR_UNKNOWN_ATTRIBUTES:
            {
                unsigned int i;
                fprintf(dbgout, cp(", " ca("attributes") "=["));
                for(i = 0; i < attr->a_len / sizeof(attr->a_type); i++) {
                    if (i != 0) fprintf(dbgout, cp(", "));
                    fprintf(dbgout, ce("%s"), attr_aton(attr->a_data.unknown.u_attr[i], buf, sizeof(buf)));
                }
                fprintf(dbgout, cp("]"));
            }
            break;

        case ATTR_CAPTURE_TIMEOUTS:
            fprintf(dbgout, cp(", " ca("timeout") "=" cc("%ums") ", " ca("delay") "=" cc("%ums")),
                   (unsigned int)attr->a_data.capture.c_timeoms,
                   (unsigned int)attr->a_data.capture.c_delayms);
            break;


        case ATTR_XOR_MAPPED_ADDRESS:
            xor((char *)&attr->a_data.address.a_port, msg->m_xid,
                sizeof(attr->a_data.address.a_port));
        case ATTR_SOURCE_ADDRESS:
        case ATTR_RESPONSE_ADDRESS:
        case ATTR_CHANGED_ADDRESS:
        case ATTR_MAPPED_ADDRESS:
            switch(attr->a_data.address.a_type) {
                case ADDR_IPv4:
                    if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                        xor(attr->a_data.address.a_addr.ipv4.raw, msg->m_xid,
                            sizeof(attr->a_data.address.a_addr.ipv4.raw));
                    fprintf(dbgout, cp(", " ca("address") "=" cc("%d.%d.%d.%d") ", " ca("port") "=" cc("%d")),
                           attr->a_data.address.a_addr.ipv4.bytes.A,
                           attr->a_data.address.a_addr.ipv4.bytes.B,
                           attr->a_data.address.a_addr.ipv4.bytes.C,
                           attr->a_data.address.a_addr.ipv4.bytes.D,
                           ntohs(attr->a_data.address.a_port));
                    if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                        xor(attr->a_data.address.a_addr.ipv4.raw, msg->m_xid,
                            sizeof(attr->a_data.address.a_addr.ipv4.raw));
                    break;

                case ADDR_IPv6:
                    if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                        xor(attr->a_data.address.a_addr.ipv6.raw, msg->m_xid,
                            sizeof(attr->a_data.address.a_addr.ipv6.raw));
                    fprintf(dbgout, ", address=??:??:??:??, port=%d",
                           ntohs(attr->a_data.address.a_port));
                    if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                        xor(attr->a_data.address.a_addr.ipv6.raw, msg->m_xid,
                            sizeof(attr->a_data.address.a_addr.ipv6.raw));
                    break;
            }
            if (attr->a_type == ATTR_XOR_MAPPED_ADDRESS)
                xor((char *)&attr->a_data.address.a_port, msg->m_xid,
                    sizeof(attr->a_data.address.a_port));
            break;

    }


    fprintf(dbgout, cp("}"));
}

void debug_message(struct message *msg) {
    int i = 0;

    fprintf(dbgout, cp("{" ca("type") "=" ce("%s") ", " ca("len") "=" cc("%d") ", " ca("xid") "=" cc("%02x-%02x%02x-%02x")),
           msg->m_type == TYPE_NULL_REQ ? "NULL_REQ" :
           msg->m_type == TYPE_NULL_RES ? "NULL_RES" :
           msg->m_type == TYPE_NULL_ERR ? "NULL_ERR" :
           msg->m_type == TYPE_BINDING_REQ ? "BINDING_REQ" :
           msg->m_type == TYPE_BINDING_RES ? "BINDING_RES" :
           msg->m_type == TYPE_BINDING_ERR ? "BINDING_ERR" :
           msg->m_type == TYPE_CAPTURE_REQ ? "CAPTURE_REQ" :
           msg->m_type == TYPE_CAPTURE_ACK ? "CAPTURE_ACK" :
           msg->m_type == TYPE_CAPTURE_RES ? "CAPTURE_RES" :
           msg->m_type == TYPE_CAPTURE_ERR ? "CAPTURE_ERR" :
           "UNKNOWN",
           msg->m_len,
           *(msg->m_xid), *(msg->m_xid + 4), *(msg->m_xid + 8), *(msg->m_xid + 12)
           );

    fprintf(dbgout, cp(", " ca("attributes") "=["));
    while (i < msg->m_len) {
        struct attribute *mattr = (struct attribute *)&msg->m_data[i];
        if (i != 0) fprintf(dbgout, cp(","));
        fprintf(dbgout, cp("\n\t"));
        i += mattr->a_len + LEN_ATTRHDR;
        debug_attribute(mattr, msg);
        if (i >= msg->m_len) fprintf(dbgout, "\n");
    }
    fprintf(dbgout, cp("]"));

    fprintf(dbgout, cp("}\n"));
}

int htonm(struct message *msgn, struct message *msgh) {
    unsigned int i = 0, j;
    memcpy(msgn, msgh, sizeof(*msgn));
    msgn->m_type = htons(msgh->m_type);
    msgn->m_len = htons(msgh->m_len);

    while (i < msgh->m_len) {
        struct attribute *mattrh = (struct attribute *)&msgh->m_data[i];
        struct attribute *mattrn = (struct attribute *)&msgn->m_data[i];

        mattrn->a_type = htons(mattrh->a_type);
        mattrn->a_len = htons(mattrh->a_len);
        i += mattrh->a_len + LEN_ATTRHDR;

        switch (mattrh->a_type) {
            case ATTR_MAPPED_ADDRESS:
            case ATTR_SOURCE_ADDRESS:
            case ATTR_RESPONSE_ADDRESS:
            case ATTR_XOR_MAPPED_ADDRESS:
            case ATTR_CHANGED_ADDRESS:
                // ports and addresses always in network endian
                // no other multibyte fields
                break;

            case ATTR_SERVER:
            case ATTR_ERROR_CODE:
            case ATTR_PACKET_REQUEST:
                // no multibyte fields
                break;

            case ATTR_CAPTURED_PACKET:
                mattrn->a_data.packet.p_time =
                    htons(mattrh->a_data.packet.p_time);
                break;


            case ATTR_CHANGE_REQUEST:
                mattrn->a_data.change_request.c_change =
                    htonl(mattrh->a_data.change_request.c_change);
                break;

            case ATTR_UNKNOWN_ATTRIBUTES:
                for (j = 0; j < mattrh->a_len / sizeof(mattrh->a_type); j++) {
                    mattrn->a_data.unknown.u_attr[j] =
                        htons(mattrh->a_data.unknown.u_attr[j]);
                }
                break;

            case ATTR_CAPTURE_TIMEOUTS:
                 mattrn->a_data.capture.c_timeoms = htonl(mattrh->a_data.capture.c_timeoms);
                 mattrn->a_data.capture.c_delayms = htonl(mattrh->a_data.capture.c_delayms);
                 break;

            default:
                errno = EINVAL;
                perror("htonm");
                debug("Attribute %d not understood", mattrh->a_type);
                dbgtrace();
                return -1;
        }
    }
    return 0;
}

int ntohm(struct message *msgh, struct message *msgn) {
    unsigned int i = 0, j;
    memcpy(msgh, msgn, sizeof(*msgh));
    msgh->m_type = ntohs(msgn->m_type);
    msgh->m_len = ntohs(msgn->m_len);

    while (i < msgh->m_len) {
        struct attribute *mattrn = (struct attribute *)&msgn->m_data[i];
        struct attribute *mattrh = (struct attribute *)&msgh->m_data[i];

        mattrh->a_type = ntohs(mattrn->a_type);
        mattrh->a_len = ntohs(mattrn->a_len);

        i += mattrh->a_len + LEN_ATTRHDR;

        switch (mattrh->a_type) {
            case ATTR_MAPPED_ADDRESS:
            case ATTR_SOURCE_ADDRESS:
            case ATTR_RESPONSE_ADDRESS:
            case ATTR_XOR_MAPPED_ADDRESS:
            case ATTR_CHANGED_ADDRESS:
                // ports and addresses always in network endian
                // no other multibyte fields
                break;

            case ATTR_SERVER:
            case ATTR_ERROR_CODE:
            case ATTR_PACKET_REQUEST:
                // no multibyte fields
                break;

            case ATTR_CAPTURED_PACKET:
                mattrh->a_data.packet.p_time =
                    ntohs(mattrn->a_data.packet.p_time);
                break;

            case ATTR_CHANGE_REQUEST:
                mattrh->a_data.change_request.c_change =
                    ntohl(mattrn->a_data.change_request.c_change);
                break;

            case ATTR_UNKNOWN_ATTRIBUTES:
                for (j = 0; j < mattrh->a_len / sizeof(mattrh->a_type); j++) {
                    mattrh->a_data.unknown.u_attr[j] =
                        ntohs(mattrn->a_data.unknown.u_attr[j]);
                }
                break;

            case ATTR_CAPTURE_TIMEOUTS:
                 mattrh->a_data.capture.c_timeoms = ntohl(mattrn->a_data.capture.c_timeoms);
                 mattrh->a_data.capture.c_delayms = ntohl(mattrn->a_data.capture.c_delayms);
                 break;

            default:
                errno = EINVAL;
                perror("htonm");
                debug("Attribute %d not understood", mattrh->a_type);
                dbgtrace();
                return -1;
        }
    }
    return 0;
}

int readall(SOCKET sock, char *buf, int s) {
    int r = 0, t;

    while (r < s) {
        struct timeval tm = {0, 100000};
        fd_set fd;

        FD_ZERO(&fd);
        FD_SET(sock, &fd);
        t = select((int)sock+1, &fd, NULL, NULL, &tm);
        if (t == 1) {
            t = read(sock, &buf[r], s-r);
            if (t < 0) return t;
            r += t;
        } else if (t == 0) {
            return r;
        } else {
            return t;
        }
    }
    return r;
}

int readmsg(SOCKET sock, struct message *msg) {
    struct message msgn;
    if (read(sock, &msgn, LEN_MSGHDR) != LEN_MSGHDR ||
        readall(sock, msgn.m_data, ntohs(msgn.m_len)) != ntohs(msgn.m_len)) {
        dbgtrace();
        return -1;

    }
    if (ntohm(msg, &msgn) == -1) {
        dbgtrace();
        return -1;
    }
    return 0;
}

int writemsg(SOCKET sock, struct message *msgh) {
    struct message msg;
    if (htonm(&msg, msgh) == -1) {
        dbgtrace();
        return -1;
    }
    return (write(sock, &msg, LEN_MSGHDR + msgh->m_len) != LEN_MSGHDR + msgh->m_len) ? -1 : 0;
}

struct attribute *findnattr(struct message *msg, int type, int idx) {
    int i = 0;

    while (i < msg->m_len) {
        struct attribute *mattr = (struct attribute *)&msg->m_data[i];
        i += mattr->a_len + LEN_ATTRHDR;
        if (mattr->a_type == type) {
            idx--;
            if (idx == 0) return mattr;
        }
    }

    return NULL;
}

struct attribute *findattr(struct message *msg, int type) {
    return findnattr(msg, type, 1);
}

int hasattr_all(struct message *msg, unsigned short *type, int typenum) {
    int i;
    for(i=0; i<typenum; i++) {
        if (findattr(msg, type[i]) == NULL) {
            dbgtrace();
            return -1;
        }
    }
    return 0;
}

int MAX(int a, int b) {
    return a > b ? a : b;
}

int MIN(int a, int b) {
    return a < b ? a : b;
}

char *tcpfindopt(struct tcp *tcp, int opt) {
    int i = 0;
    while(i < 44) {
        if (tcp->tcp_opt.raw[i] == TCP_OPT_EOL) break;
        if (tcp->tcp_opt.raw[i] == opt) return &tcp->tcp_opt.raw[i];
        if (tcp->tcp_opt.raw[i] == TCP_OPT_NOP) {
            i++;
            continue;
        }
        i += tcp->tcp_opt.raw[i+1];
    }
    return NULL;
}

int tcphlen(struct tcp *tcp) {
    int s = offsetof(struct tcp, tcp_opt);
    int i = 0;
    while(i < 44) {
        if (tcp->tcp_opt.raw[i] == TCP_OPT_EOL) break;
        if (tcp->tcp_opt.raw[i] == TCP_OPT_NOP) {
            i++;
            s++;
            continue;
        }
        if (tcp->tcp_opt.raw[i+1] < 2) break;
        s += tcp->tcp_opt.raw[i+1];
        i += tcp->tcp_opt.raw[i+1];
    }
    return ((s + 3) / 4) * 4;
}

unsigned short in_cksum(unsigned short *addr, int len, int *oldans) {
    register int nleft  = len;
    register u_short *w = addr;
    register int sum    = *oldans;
    u_short answer      = 0;

    while (nleft > 1)  {
        sum   += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *(u_char *)(&answer) = *(u_char *)w ;
        sum += answer;
    }

    *oldans = sum;

    sum    = (sum >> 16) + (sum & 0xffff);
    sum   += (sum >> 16);
    answer = ~sum;
    return(answer);
}

void nextseq(uint32 *seq_self, uint32 *seq_peer, struct ip *ip, struct tcp *tcp) {
    if (seq_self != NULL) {
        *seq_self = htonl(ntohl(tcp->tcp_seq) +
            ntohs(ip->ip_totlen) - (ip->ip_hlen << 2) - (tcp->tcp_hlen << 2) +
            (tcp->tcp_flags & TCP_SYN ? 1 : 0) +
            (tcp->tcp_flags & TCP_FIN ? 1 : 0));
    }
    if (seq_peer != NULL) {
        *seq_peer = tcp->tcp_ack;
    }
}

int tcpcheck(struct ip *ip) {
    struct tcp *tcp = (struct tcp *)(((char *)ip) + (ip->ip_hlen << 2));
    struct tcp_pseudo pseudo;
    int sum = 0;

    tcp->tcp_check = 0;
    pseudo.pseudo_src = ip->ip_src;
    pseudo.pseudo_dst = ip->ip_dst;
    pseudo.pseudo_proto = ip->ip_proto;
    pseudo.pad1 = 0;
    pseudo.pseudo_length = htons(ntohs(ip->ip_totlen) - (ip->ip_hlen << 2));

    in_cksum((uint16 *)tcp, ntohs(pseudo.pseudo_length), &sum);
    return in_cksum((uint16 *)&pseudo, sizeof(pseudo), &sum);
}


void alignptrs(struct ip **ip, struct tcp **tcp, struct icmp **icmp,
               struct ip **icmpip, struct tcp **icmptcp, char *pkt) {

    if (ip != NULL) {
        *ip = (struct ip *)pkt;
    } else {
        *ip = NULL;
    }
    if (*ip != NULL && tcp != NULL && (*ip)->ip_proto == IPPROTO_TCP) {
        (*tcp) = (struct tcp *)(((char *)(*ip)) + ((*ip)->ip_hlen << 2));
    } else {
        if (tcp != NULL) *tcp = NULL;
    }

    if (*ip != NULL && icmp != NULL && (*ip)->ip_proto == IPPROTO_ICMP) {
        (*icmp) = (struct icmp *)(((char *)(*ip)) + ((*ip)->ip_hlen << 2));
        alignptrs(icmpip, icmptcp, NULL, NULL, NULL, (*icmp)->icmp_payload);
    } else {
        if (icmp != NULL) *icmp = NULL;
        if (icmpip != NULL) *icmpip = NULL;
        if (icmptcp != NULL) *icmptcp = NULL;
    }
}

void pktinit_const(struct ip *ip, struct tcp *tcp, struct sockaddr_in *caddr) {
    tcp->tcp_win = htons(1024);
    tcp->tcp_opt.stunt = opt_stunt;
    tcp->tcp_opt.stunt.s_ip = caddr->sin_addr.S_addr;
    tcp->tcp_opt.stunt.s_port = caddr->sin_port;

    ip->ip_ver = 4;
    ip->ip_hlen = sizeof(struct ip) >> 2;
    ip->ip_ttl = 48;
    ip->ip_ident = htons(ipid++);
    ip->ip_proto = IPPROTO_TCP;
}

int pktinit_syn(char *buf, unsigned int buflen, struct sockaddr_in *dst, struct sockaddr_in *src, struct sockaddr_in *caddr, int seq) {
    struct ip *ip = (struct ip *)buf;
    struct tcp *tcp = (struct tcp *)&buf[sizeof(struct ip)];

    if (buflen < sizeof(struct ip) + sizeof(struct tcp)) return 0;

    memset(buf, 0, buflen);

    pktinit_const(ip, tcp, caddr);

    tcp->tcp_srcport = src->sin_port;
    tcp->tcp_dstport = dst->sin_port;
    tcp->tcp_seq = seq;
    tcp->tcp_flags = TCP_SYN;

    tcp->tcp_hlen = tcphlen(tcp) >> 2;
    ip->ip_totlen = htons((tcp->tcp_hlen << 2) + (ip->ip_hlen << 2));

    ip->ip_src = src->sin_addr.S_addr;
    ip->ip_dst = dst->sin_addr.S_addr;

    tcp->tcp_check = tcpcheck(ip);

    return ntohs(ip->ip_totlen);

}

int pktinit_ack(char *buf, unsigned int buflen, char *rcv, struct sockaddr_in *caddr) {
    struct ip *ip = (struct ip *)buf;
    struct tcp *tcp = (struct tcp *)&buf[sizeof(struct ip)];
    struct ip *rip;
    struct tcp *rtcp;
    struct tcp_opt_timestamp *opt_ts;

    alignptrs(&rip, &rtcp, NULL, NULL, NULL, rcv);

    if (buflen < sizeof(struct ip) + sizeof(struct tcp)) return 0;
    if (rtcp == NULL) return 0;

    memset(buf, 0, buflen);

    pktinit_const(ip, tcp, caddr);

    tcp->tcp_srcport = rtcp->tcp_dstport;
    tcp->tcp_dstport = rtcp->tcp_srcport;
    tcp->tcp_seq = rtcp->tcp_ack;
    nextseq(&tcp->tcp_ack, NULL, rip, rtcp);
    tcp->tcp_flags = TCP_ACK;
    if ((opt_ts = (struct tcp_opt_timestamp *)
         tcpfindopt(rtcp, TCP_OPT_TIMESTAMP)) != NULL) {

        tcp->tcp_opt.stunt.next.ts.t_kind = TCP_OPT_TIMESTAMP;
        tcp->tcp_opt.stunt.next.ts.t_len = TCP_OPTLEN_TIMESTAMP;
        tcp->tcp_opt.stunt.next.ts.t_val = time(NULL);
        tcp->tcp_opt.stunt.next.ts.t_echo = opt_ts->t_val;

    }

    tcp->tcp_hlen = tcphlen(tcp) >> 2;
    ip->ip_totlen = htons((tcp->tcp_hlen << 2) + (ip->ip_hlen << 2));

    ip->ip_src = rip->ip_dst;
    ip->ip_dst = rip->ip_src;

    tcp->tcp_check = tcpcheck(ip);

    return ntohs(ip->ip_totlen);

}

int pktinit_finack(char *buf, unsigned int buflen, char *rcv, struct sockaddr_in *caddr) {
    struct ip *ip;
    struct tcp *tcp;
    int r = pktinit_ack(buf, buflen, rcv, caddr);
    alignptrs(&ip, &tcp, NULL, NULL, NULL, buf);
    tcp->tcp_flags |= TCP_FIN;
    tcp->tcp_check = tcpcheck(ip);
    return r;
}

int pktinit_rstack(char *buf, unsigned int buflen, char *rcv, struct sockaddr_in *caddr) {
    struct ip *ip;
    struct tcp *tcp;
    int r = pktinit_ack(buf, buflen, rcv, caddr);
    alignptrs(&ip, &tcp, NULL, NULL, NULL, buf);
    tcp->tcp_flags |= TCP_RST;
    tcp->tcp_check = tcpcheck(ip);
    return r;
}

int pktinit_synack(char *buf, unsigned int buflen, char *rcv, struct sockaddr_in *caddr, int seq) {
    struct ip *ip;
    struct tcp *tcp;
    int r = pktinit_ack(buf, buflen, rcv, caddr);
    alignptrs(&ip, &tcp, NULL, NULL, NULL, buf);
    tcp->tcp_seq = seq;
    tcp->tcp_flags |= TCP_SYN;
    tcp->tcp_check = tcpcheck(ip);
    return r;
}

int pktfix_mkkeepalive(char *buf) {
    struct ip *ip;
    struct tcp *tcp;
    int s;
    alignptrs(&ip, &tcp, NULL, NULL, NULL, buf);
    s = ntohs(ip->ip_totlen)+1;
    tcp->tcp_seq = htonl(ntohl(tcp->tcp_seq) - 1);
    ip->ip_totlen = htons(s);
    tcp->tcp_check = tcpcheck(ip);
    return s;
}

int pktinit_icmp(char *buf, unsigned int buflen, char *rcv, struct sockaddr_in *caddr, int type, int code) {
    struct ip *ip = (struct ip *)buf;
    struct icmp *icmp = (struct icmp *)&buf[sizeof(struct ip)];
    struct ip *rip;
    struct tcp *rtcp;
    unsigned short s;
    int zero = 0;

    alignptrs(&rip, &rtcp, NULL, NULL, NULL, rcv);

    if (rtcp == NULL) return 0;
    if (buflen < sizeof(struct ip) + sizeof(struct icmp) + ntohs(rip->ip_totlen)) return 0;


    memset(buf, 0, buflen);

    ip->ip_ver = 4;
    ip->ip_hlen = sizeof(struct ip) >> 2;
    ip->ip_ttl = 48;
    ip->ip_ident = htons(ipid++);
    ip->ip_proto = IPPROTO_ICMP;
    ip->ip_src = rip->ip_dst;
    ip->ip_dst = rip->ip_src;

    s = MIN(ntohs(rip->ip_totlen), 64);

    icmp->icmp_type = type;
    icmp->icmp_code = code;
    memcpy(icmp->icmp_payload, rcv, s);
    icmp->icmp_check = in_cksum((uint16 *)icmp, sizeof(struct icmp) + s, &zero);

    ip->ip_totlen = htons(sizeof(struct icmp) + (ip->ip_hlen << 2) + s);

    return ntohs(ip->ip_totlen);

}



pcap_if_t *devices = NULL;

pcap_if_t *getdevices() {
    char errbuf[PCAP_ERRBUF_SIZE];
    if (devices == NULL) {
        pcap_findalldevs(&devices, errbuf);
    }
    return devices;
}

void populate_devices() {
    pcap_if_t *device = getdevices();
    int i = 0;

    memset(usable_devices, 0, sizeof(usable_devices));

    if (device == NULL) {
        return;
    }

    do {
        pcap_addr_t *a = device->addresses;
        do {
            if (a == NULL || device->flags & PCAP_IF_LOOPBACK) break;
            if (a->addr != NULL) {
                struct sockaddr *s = a->addr;
                if (s->sa_family != AF_INET) continue;
                strncpy(usable_devices[i++], device->name, sizeof(usable_devices[0]));
                break;
            }
        } while ((a = a->next) != NULL);
    } while ((device = device->next) != NULL && i < MAX_DEVICES);
}



int getifaddr(char *ip, int size, char **ifacep) {
    const char *iface = *ifacep;
    pcap_if_t *t;
    char errbuf[PCAP_ERRBUF_SIZE];
    int i = atoi(iface);

    snprintf(errbuf, PCAP_ERRBUF_SIZE, "%d", i);

    if (i < 0 || strcmp(errbuf, iface) != 0) {
        i = -1;
    }

    if (devices == NULL) {
        pcap_findalldevs(&devices, errbuf);
    }

    if (devices == NULL) {
        dbgtrace();
        return -1;
    }

    t = devices;
    do {
        if (i == 0 || (t->name != NULL && strcmp(iface, t->name) == 0)) {
            pcap_addr_t *a = t->addresses;
            do {
                if (a == NULL) break;
                if (a->addr != NULL) {
                    struct sockaddr *s = a->addr;
                    if (s->sa_family != AF_INET) continue;
                    strncpy(ip, inet_ntoa(((struct sockaddr_in *)s)->sin_addr), size);
                    *ifacep = t->name;
                    return 0;
                }
            } while ((a = a->next) != NULL);
            dbgtrace();
            return -1;
        }
        i--;
    } while ((t = t->next) != NULL);
    return -1;
}

void iterattrinit(struct attribute **attr, struct message *msg) {
    if (msg->m_len == 0) *attr = NULL;
    else *attr = (struct attribute *)msg->m_data;
}

void iterattr(struct attribute **attr, struct message *msg) {
    *attr = (struct attribute *)(((char *)*attr) + (*attr)->a_len + LEN_ATTRHDR);
    if (((char *)*attr - (char *)msg->m_data) > msg->m_len)
        *attr = NULL;
}

#ifdef WIN32

int my_errno;

int *_my_errno() {
    my_errno = errno != 0 ? errno : WSAGetLastError();
    return &my_errno;
}

#endif

