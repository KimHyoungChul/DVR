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

#ifndef __CLIENT_H
#define __CLIENT_H

#define VERSION        "0.1"

#define RESULT_URL     "http://nutss.gforge.cis.cornell.edu/stunt-results.php"
#define STUNTD_IP      "128.84.154.62"


#define NUM_TRY_PORTS  7
#define NUM_TRY_EACH   3

#define MAXLEN_ERRBUF   256
#define MAXLEN_FILTER   256
#define MAX_TMTESTS     10
#define MAX_INT         (((~0) ^ (((unsigned int)(~0)) >> 1)) - 1)

#define MAX_TNAT_IP     8
#define MAXLEN_FILE     256

#define THRESH_PORTASSIGN   50

#define FWTEST_TIMEO 30000
#define FWTEST_DELAY 500

#define PPTEST_TIMEO 10000

#define DELAY_NONE   0


#define CNG_IP_PORT (CNG_IP | CNG_PORT)



#define error(r,x)          do {    \
    dbgtrace();                     \
    errline = __LINE__;             \
    errfile = __FILE__;             \
    errfunction = __FUNCTION__;     \
    strncpy(errbuf, x, MAXLEN_ERRBUF);    \
    return r;                       \
} while (0)

extern int errline;
extern char *errfile, *errfunction;
extern char errbuf[MAXLEN_ERRBUF];




#undef TRUE
#undef FALSE

typedef enum {FALSE = 0, TRUE = 1, MAYBE = 42} bool;
typedef enum {P_LOW, P_HIGH, P_DYN} port_type;

struct bindres {
    unsigned int s_addr;
    unsigned short s_port;
    unsigned int m_addr;
    unsigned short m_port;
    unsigned int d_addr;
    unsigned short d_port;
};

enum {
    CPT_FWTEST_FIRST = 0x0,
    FWTEST_UNSOLICITED,
    FWTEST_SYN_RST_SYN,
    FWTEST_SYN_TTL_SYN,
    FWTEST_SYN_UNREACH_SYN,
    FWTEST_SYN_DROP_SYN,
    FWTEST_SYN_RST_SYNACK,
    FWTEST_SYN_TTL_SYNACK,
    FWTEST_SYN_UNREACH_SYNACK,
    FWTEST_SYN_DROP_SYNACK,
    FWTEST_ESTD_SYN_ADDRESS,
    FWTEST_ESTD_SYN_ANY,
    CPT_FWTEST_LAST,

    CPT_TMTEST_FIRST,
    TMTEST_SYN,
    TMTEST_ESTD,
    TMTEST_FIN,
    TMTEST_RST,
    CPT_TMTEST_LAST
};

struct timer {
    int min;
    int max;
};

struct captres {
    int test;
    void (*change_req)(struct message *msg);
    char pcap_filter[MAXLEN_FILTER];
    unsigned int timeoms;
    unsigned int delayms;
    unsigned int start;
    struct sockaddr_in *l_addr;
    pcap_t      *handle;
    pthread_t   tid;
    struct message at_server;
    struct message at_client;
};

struct timeres {
    struct captres *captres;
    int delays;
    int timeos;
    bool close;
};

struct NAT {
    struct {

        // Binding related
        uint32  ip[MAX_TNAT_IP];
        uint32  local_ip[MAX_TNAT_IP];

        bool port_preserving;
        enum {
            PR_LOW_HIGH_DYNAMIC = 0x7,
            PR_LOW_HIGH         = 0x3,
            PR_HIGH_DYNAMIC     = 0x6,
            PR_DYNAMIC          = 0x4,
            PR_HIGH             = 0x2,
            PR_LOW              = 0x1,
            PR_NOT_TESTED_LOW   = 0x8,
            PR_NOT              = 0x0
        } port_range_preserved;
        bool port_overloading;
        enum {
            PY_EVENODD = 0x1,
            PY_NEXTHIGHER = 0x02
        } port_parity;
        enum {
            NB_INDEPENDENT,
            NB_ADDRESS,
            NB_ADDRESS_PORT,
            NB_PORT,
            NB_NEITHER
        } nat_binding;
        int  binding_delta;

        // Filtering and mangling related
        enum {
            MG_IP_TTL = 0x1,
            MG_TCP_SEQ = 0x2,
            MG_ICMP_PAYLOAD = 0x4,
            MG_IP_PAYLOAD = 0x8
        } mangles;
        enum {
            FW_ACCEPT = 0x0,
            FW_RST,
            FW_ICMP,
            FW_DROP
        } fw_unsolicited,             // unsolicited SYN
          fw_syn_rst_syn,             // SYN out. RST in. SYN in?
          fw_syn_ttl_syn,             // SYN out. ICMP TTL in. SYN in?
          fw_syn_unreach_syn,         // SYN out. ICMP UNREACHABLE in. SYN in?
          fw_syn_drop_syn,            // SYN out. SYN in?
          fw_syn_rst_synack,          // SYN out. RST in. SYNACK in?
          fw_syn_ttl_synack,          // SYN out. ICMP TTL in. SYNACK in?
          fw_syn_unreach_synack,      // SYN out. ICMP UNREACHABLE in. SYNACK in?
          fw_syn_drop_synack,         // SYN out. SYNACK in?
          fw_estd_syn_address,        // established ip/port. SYN from ip/port'
          fw_estd_syn_any             // established ip/port. SYN from ip'/port'
          ;
        bool hairpin;

        // Timer related
        struct timer timer_syn, timer_estd, timer_fin, timer_rst;

        float ppred_rate;

    } tcp;
};

extern struct NAT nat;
extern int superuser;
extern char laddress[3][21];
extern char raddress[21];
extern int port;
extern SOCKET serv;
extern char devstr[3][64];

extern int timeleft;

#endif
