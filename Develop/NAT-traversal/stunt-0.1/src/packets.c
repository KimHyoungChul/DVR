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
#include "compat.h"
#include "packets.h"
#include "client.h"

void msginit_capreq(struct message *req, struct captres *res) {
    struct attribute attr;
    msgzero(req);
    msginit_request(req, TYPE_CAPTURE_REQ);
    res->change_req(req);
    attrinit_capturetimeouts(&attr, res->timeoms, res->delayms);
    msgadd(req, &attr);
}

void pktadd_unsolicited(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_SEND | PKTREQ_DELAY | PKTREQ_ALTADDR); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_WAIT | PKTREQ_ALTADDR); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND | PKTREQ_ALTADDR); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND | PKTREQ_ALTADDR); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT | PKTREQ_ALTADDR); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND | PKTREQ_ALTADDR); msgadd(msg, &attr);
    attrinit_changerequest(&attr, CNG_IP_PORT); msgadd(msg, &attr);
}

void pktadd_synrstsyn(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_RST | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_synttlsyn(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_ICMP, ICMP_TYPE_TTL, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_synunreachsyn(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_ICMP, ICMP_TYPE_UNREACH, ICMP_CODE_HOST_UNREACH, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_syndropsyn(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}
void pktadd_synrstsynack(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_RST | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_synttlsynack(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_ICMP, ICMP_TYPE_TTL, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_synunreachsynack(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_ICMP, ICMP_TYPE_UNREACH, ICMP_CODE_HOST_UNREACH, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_syndropsynack(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_estdsynaddress(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_SEND | PKTREQ_ALTPORT | PKTREQ_DELAY); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_estdsynaddrport(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_SEND | PKTREQ_ALTPORT | PKTREQ_ALTADDR | PKTREQ_DELAY); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_tmsyn(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND | PKTREQ_DELAY); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_tmestd(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, PKTREQ_KEEPALIVE, PKTREQ_SEND | PKTREQ_DELAY | PKTREQ_REXMITACKED); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
}

void pktadd_tmfin(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_FIN | TCP_ACK, 0, PKTREQ_SEND | PKTREQ_DELAY | PKTREQ_REXMITACKED); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_NULL, 0, 0, PKTREQ_WAIT); msgadd(msg, &attr);
}

void pktadd_tmrst(struct message *msg) {
    struct attribute attr;
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_SYN | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_ACK, 0, PKTREQ_WAIT); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_RST | TCP_ACK, 0, PKTREQ_SEND); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_TCP, TCP_RST | TCP_ACK, 0, PKTREQ_SEND | PKTREQ_DELAY); msgadd(msg, &attr);
    attrinit_pktreq(&attr, IPPROTO_NULL, 0, 0, PKTREQ_WAIT); msgadd(msg, &attr);
}



