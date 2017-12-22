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

#ifndef __STUNT_H
#define __STUNT_H

#define STUNT_MAGIC               0x0c

#define TYPE_NOP                    0x0000
#define TYPE_NULL_REQ               0x0000
#define TYPE_NULL_RES               0x0100
#define TYPE_NULL_ERR               0x0110

#define TYPE_BINDING_REQ        0x0001
#define TYPE_BINDING_RES        0x0101
#define TYPE_BINDING_ERR        0x0111

#define TYPE_CAPTURE_REQ        0x0003
#define TYPE_CAPTURE_RES        0x0103
#define TYPE_CAPTURE_ACK        0x0203
#define TYPE_CAPTURE_ERR        0x0113

#define ATTR_MAPPED_ADDRESS     0x0001
#define ATTR_RESPONSE_ADDRESS   0x0002
#define ATTR_SOURCE_ADDRESS     0x0004
#define ATTR_CHANGED_ADDRESS    0x0005
#define ATTR_CHANGE_REQUEST     0x0003
#define ATTR_ERROR_CODE         0x0009
#define ATTR_UNKNOWN_ATTRIBUTES 0x000a
#define ATTR_REFLECTED_FROM     0x000b
#define ATTR_XOR_MAPPED_ADDRESS 0x0020
#define ATTR_SERVER             0x0022
#define ATTR_OPTIONAL           0x7fff

#define ATTR_PACKET_REQUEST     0x0100
#define ATTR_CAPTURE_TIMEOUTS   0x0101
#define ATTR_CAPTURED_PACKET    0x0102

#define ERR_NONE                0x0000
#define ERR_UNSUPPORTED         0x0001

#define PKTREQ_SEND             0x20
#define PKTREQ_WAIT             0x00
#define PKTREQ_REXMITACKED      0x10
#define PKTREQ_DELAY            0x08
#define PKTREQ_ALTADDR          0x04
#define PKTREQ_ALTPORT          0x02

#define PKTREQ_KEEPALIVE        0x01

#define CNG_IP                  0x00000004
#define CNG_PORT                0x00000002


#define ADDR_IPv4               0x01
#define ADDR_IPv6               0x02


#define CPT_FROM_SERVER         0x00
#define CPT_TO_SERVER           0x01
#define CPTMASK_DIRN            0x01

#define IPPROTO_NULL            0x00
#define IPPROTO_SUPPLIED        0xFF


typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

struct message {
    uint16 m_type;
    uint16 m_len;
    uint8 m_xid[16];
    uint8 m_data[MAXLEN_VALUES];
};

union addr_ipv4 {
    struct {
        uint8 A, B, C, D;
    } bytes;
    struct {
        uint32 s_addr;
    } word;
    uint8 raw[4];
};

union addr_ipv6 {
    struct {
        uint8 A1, A2, A3, A4;
        uint8 B1, B2, B3, B4;
        uint8 C1, C2, C3, C4;
        uint8 D1, D2, D3, D4;
    } bytes;
    struct {
        uint32 A, B, C, D;
    } words;
    uint8 raw[16];
};

struct attr_address {
    uint8 _pad;
    uint8 a_type;
    uint16 a_port;
    union {
        union addr_ipv4 ipv4;
        union addr_ipv6 ipv6;
    } a_addr;
};

struct attr_error {
    uint16 _pad;
    uint8 e_class;
    uint8 e_number;
    uint8 e_reason[0];
};

struct attr_server {
    uint8 s_details[0];
};

struct attr_capturetimeouts {
    uint32 c_timeoms;
    uint32 c_delayms;
};

struct attr_packetreq {
    uint8  p_proto;
    uint8  p_type;
    uint8  p_code;
    uint8  p_flags;
    uint8  p_data[0];
};

struct attr_packet {
    uint8  p_type;
    uint8  p_times;
    uint16 p_time;
    uint8  p_data[0];
};

struct attr_unknown {
    uint16 u_attr[0];
};

struct attr_change_request {
    uint32 c_change;
};

struct attribute {
    uint16 a_type;
    uint16 a_len;
    union {
        struct attr_address address;
        struct attr_error   error;
        struct attr_capturetimeouts capture;
        struct attr_packet  packet;
        struct attr_packetreq  packetreq;
        struct attr_server  server;
        struct attr_unknown unknown;
        struct attr_change_request  change_request;
        uint32              word;
        uint8               raw[MAXLEN_ATTR];
    } a_data;
};

#define LEN_MSGHDR     (offsetof(struct message, m_data))
#define LEN_ATTRHDR    (offsetof(struct attribute, a_data))
#define LEN_PACKETHDR  (offsetof(struct attr_packet, p_data))
#define LEN_PACKETDATA (MAXLEN_ATTR - offsetof(struct attr_packet, p_data))
#define LEN_ERRORHDR   (offsetof(struct attr_error, e_reason))
#define PORT_STUNTD    3478

#endif
