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


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>

#ifdef WIN32
#define uriadd1(x)   snprintf(&uri[strlen(uri)], MAXLEN_URI - strlen(uri), x "")
#define uriadd2(x,y)   snprintf(&uri[strlen(uri)], MAXLEN_URI - strlen(uri), x "", y)
#define uriadd4(x,y,z,a)   snprintf(&uri[strlen(uri)], MAXLEN_URI - strlen(uri), x "", y, z, a)
#define uriadd5(x,y,z,a,b)   snprintf(&uri[strlen(uri)], MAXLEN_URI - strlen(uri), x "", y, z, a, b)
#endif

#ifdef LINUX
#define uriadd(x,...)   (snprintf(&uri[strlen(uri)], MAXLEN_URI - strlen(uri), x, ##__VA_ARGS__))
#define uriadd1         uriadd
#define uriadd2         uriadd
#define uriadd4         uriadd
#define uriadd5         uriadd
#endif

char uri[MAXLEN_URI];

int nodebug_trace(FILE *out, const char *c, ...);
void (*dbgmsg)(struct message *msg) = nodebug_message;
int (*_dbgtrace)(FILE *out, const char *c, ...) = nodebug_trace;

void nodebug_message(struct message *msg) {
}


int nodebug_trace(FILE *out, const char *c, ...) {
    return 0;
}

char *bool2string(bool b) {
    switch(b) {
        case TRUE: return "Yes";
        case FALSE: return "No";
        case MAYBE: return "Not tested";
    }
    return NULL;
}

void display_devices() {
    pcap_if_t *device = getdevices();
    int i = 0;

    if (device == NULL) {
        fprintf(stderr, "Error opening device list. Need root priviledges.");
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

void display_help(char *argv0) {
    printf("Usage: \n");
    printf("        %s [options] [-i iface [-i iface]] server\n", argv0);
    printf("        %s -i help\n", argv0);
    printf("Options:\n");
    printf("-i, --%-20s interface e.g. eth0, eth0:1\n", "interface iface");
    printf("-p, --%-20s port e.g. 3478\n", "port port");
    printf("-v, --%-20s increase verbosity\n", "debug");
    printf("-b, --%-20s test port binding behaviour\n", "test-bindings");
    printf("-f, --%-20s test packet filtering behaviour\n", "test-filtering");
    printf("-t, --%-20s test timers\n", "test-timers");
    printf("-r, --%-20s test port-prediction\n", "test-portpred");
    printf("-o, --%-20s write fingerprint to file\n", "output file");
    printf("    --%-20s timer value for SYN test in seconds\n", "timer-syn sec");
    printf("    --%-20s timer value for ESTD test in seconds\n", "timer-estd sec");
    printf("    --%-20s timer value for FIN test in seconds\n", "timer-fin sec");
    printf("    --%-20s timer value for RST test in seconds\n", "timer-rst sec");
    printf("    --%-20s timer value for port-prediction test in seconds\n", "timer-portpred sec");
    printf("    --%-20s binding test interval in seconds\n", "binding-interval sec");
    printf("    --%-20s port prediction interval in seconds\n", "portpred-interval sec");
}

void print_bindings() {
    int comma = 0, k;

    uriadd1("&ip=");
    printf("%-23s: ", "IP");
    comma = 0;
    for(k=0; k<MAX_TNAT_IP; k++) {
        if (nat.tcp.ip[k] == 0) break;
        if (comma) {
            printf(", ");
        }
        printf("%d.%d.%d.%d", NIPQUAD(nat.tcp.ip[k]));
        uriadd2("%d.%d.%d.%d,", NIPQUAD(nat.tcp.ip[k]));
    }
    printf("\n");

    uriadd1("&lip=");
    printf("%-23s: ", "Local IP");
    comma = 0;
    for(k=0; k<MAX_TNAT_IP; k++) {
        if (nat.tcp.local_ip[k] == 0) break;
        if (comma) {
            printf(", ");
        }
        printf("%d.%d.%d.%d", NIPQUAD(nat.tcp.local_ip[k]));
        uriadd2("%d.%d.%d.%d,", NIPQUAD(nat.tcp.local_ip[k]));
    }
    printf("\n");


    printf("%-23s: %s\n", "Binding fixed",
           nat.tcp.nat_binding == NB_INDEPENDENT ? "Always (NB=I)" :
           nat.tcp.nat_binding == NB_ADDRESS ? "Same IP (NB=AD)" :
           nat.tcp.nat_binding == NB_PORT ? "Same Port (NB=P)" :
           nat.tcp.nat_binding == NB_ADDRESS_PORT ? "Same Port (NB=ADP)" :
           nat.tcp.nat_binding == NB_NEITHER ? "Never (NB=SD)" :
           ""
           );
    uriadd2("&nb=%s",
           nat.tcp.nat_binding == NB_INDEPENDENT ? "i" :
           nat.tcp.nat_binding == NB_ADDRESS ? "a" :
           nat.tcp.nat_binding == NB_PORT ? "p" :
           nat.tcp.nat_binding == NB_ADDRESS_PORT ? "ap" :
           nat.tcp.nat_binding == NB_NEITHER ? "s" :
           ""
          );


    if (nat.tcp.nat_binding != NB_INDEPENDENT && nat.tcp.binding_delta == 0) {
        printf("%-23s: %s\n", "Binding delta", "Random");
        uriadd1("&bd=r");
    } else {
        printf("%-23s: %d\n", "Binding delta", nat.tcp.binding_delta);
        uriadd2("&bd=%d", nat.tcp.binding_delta);
    }

    comma = 0;
    uriadd1("&pr=");
    printf("%-23s: ", "Port ranges preserved");
    if (nat.tcp.port_range_preserved & PR_LOW) {printf("Low"); uriadd1("l,"); comma = 1;}
    if (nat.tcp.port_range_preserved & PR_HIGH) {printf("%sHigh", comma ? ", ":""); uriadd1("h,"); comma = 1;}
    if (nat.tcp.port_range_preserved & PR_DYNAMIC) {printf("%sDynamic", comma ? ", ":""); uriadd1("d,"); comma = 1;}
    if (nat.tcp.port_range_preserved & PR_NOT_TESTED_LOW) {printf("%sNot tested: Low", comma ? "; ":""); uriadd1("nl,"); comma = 1;}
    printf("\n");

    printf("%-23s: %s\n", "Port preserving", bool2string(nat.tcp.port_preserving));
    if (nat.tcp.port_preserving != MAYBE) uriadd2("&pp=%s", nat.tcp.port_preserving == TRUE ? "t" : "f");

    printf("%-23s: %s\n", "Port overloading", bool2string(nat.tcp.port_overloading));
    if (nat.tcp.port_overloading != MAYBE) uriadd2("&po=%s", nat.tcp.port_overloading == TRUE ? "t" : "f");

    printf("%-23s: %s\n", "Hairpinning", bool2string(nat.tcp.hairpin));
    if (nat.tcp.hairpin != MAYBE) uriadd2("&h=%s", nat.tcp.hairpin == TRUE ? "t" : "f");

    comma = 0;
    uriadd1("&ppp=");
    printf("%-23s: ", "Port parity preserved");
    if (nat.tcp.port_parity & PY_EVENODD) {printf("Even and Odd"); uriadd1("eo,"); comma = 1;}
    if (nat.tcp.port_parity & PY_NEXTHIGHER) {printf("%sNext Higher", comma ? ", ":""); uriadd1("nh,"); comma = 1;}
    printf("\n");
}

void print_filtering() {
    int slash, comma, res;

    for(res=0; res<=FW_DROP; res++) {
        char *resstr, *ustr;

        switch(res) {
            case FW_ACCEPT: resstr = "Accepts incoming"; ustr="fa"; break;
            case FW_DROP: resstr = "Drops incoming"; ustr="fd"; break;
            case FW_RST: resstr = "RSTs incoming"; ustr="fr"; break;
            case FW_ICMP: resstr = "ICMPs incoming"; ustr="fi"; break;
        }

        uriadd2("&%s=", ustr);
        if (nat.tcp.fw_syn_rst_synack == res) uriadd1("ra,");
        if (nat.tcp.fw_syn_ttl_synack == res) uriadd1("ta,");
        if (nat.tcp.fw_syn_unreach_synack == res) uriadd1("ua,");
        if (nat.tcp.fw_syn_drop_synack == res) uriadd1("da,");
        if (nat.tcp.fw_syn_rst_syn == res) uriadd1("rs,");
        if (nat.tcp.fw_syn_ttl_syn == res) uriadd1("ts,");
        if (nat.tcp.fw_syn_unreach_syn == res) uriadd1("us,");
        if (nat.tcp.fw_syn_drop_syn == res) uriadd1("ds,");
        if (nat.tcp.fw_unsolicited == res) uriadd1("s,");
        if (nat.tcp.fw_estd_syn_address == res) uriadd1("ep,");
        if (nat.tcp.fw_estd_syn_any == res) uriadd1("eip,");

        if (res != FW_ACCEPT &&
            nat.tcp.fw_syn_rst_synack != res &&
            nat.tcp.fw_syn_ttl_synack != res &&
            nat.tcp.fw_syn_unreach_synack != res &&
            nat.tcp.fw_syn_drop_synack != res &&
            nat.tcp.fw_syn_rst_syn != res &&
            nat.tcp.fw_syn_ttl_syn != res &&
            nat.tcp.fw_syn_unreach_syn != res &&
            nat.tcp.fw_syn_drop_syn != res &&
            nat.tcp.fw_unsolicited != res &&
            nat.tcp.fw_estd_syn_any != res &&
            nat.tcp.fw_estd_syn_address != res) continue;

        printf("%-23s: ", resstr);


        if (res == FW_ACCEPT) {
            comma = 1;
            if (nat.tcp.fw_unsolicited == res) {
                printf("Unsolicited SYNs (EF=O)");
                uriadd1("&ef=o");
            } else if (nat.tcp.fw_estd_syn_any == res) {
                printf("New SYNs from ANY host once established (EF=I)");
                uriadd1("&ef=i");
            } else if (nat.tcp.fw_estd_syn_address == res) {
                printf("New SYNs from same IP once established (EF=AD)");
                uriadd1("&ef=a");
            } else {
                printf("Non-SYN packets on established connection (EF=ADP)");
                uriadd1("&ef=ap");
            }
        } else {
            comma = 1;
            if (nat.tcp.fw_estd_syn_address == res) printf("New SYNs from same IP once established");
            else if (nat.tcp.fw_estd_syn_any == res) printf("New SYNs from different IP once established");
            else if (nat.tcp.fw_unsolicited == res) printf("Unsolicited SYNs");
            else comma = 0;
        }

        if (nat.tcp.fw_syn_rst_syn == res ||
            nat.tcp.fw_syn_ttl_syn == res ||
            nat.tcp.fw_syn_unreach_syn == res ||
            nat.tcp.fw_syn_drop_syn == res) {

            if (comma == 1) printf("%-26s", "\n");
            comma = 1;
            printf("SYN after ");
            slash = 0;
            if (nat.tcp.fw_syn_rst_syn == res) {printf("RST"); slash = 1;}
            if (nat.tcp.fw_syn_ttl_syn == res) {printf("%sICMP TTL", slash ? ", " : ""); slash = 1;}
            if (nat.tcp.fw_syn_unreach_syn == res) {printf("%sICMP UNREACHABLE", slash ? ", " : ""); slash = 1;}
            if (nat.tcp.fw_syn_drop_syn == res) {printf("%sDROP", slash ? ", " : ""); slash = 1;}
            printf(" for hole-punching SYN");
        }


        if (nat.tcp.fw_syn_rst_synack == res ||
            nat.tcp.fw_syn_ttl_synack == res ||
            nat.tcp.fw_syn_unreach_synack == res ||
            nat.tcp.fw_syn_drop_synack == res) {

            if (comma == 1) printf("%-26s", "\n");
            comma = 1;
            printf("SYNACK after ");
            slash = 0;
            if (nat.tcp.fw_syn_rst_synack == res) {printf("RST"); slash = 1;}
            if (nat.tcp.fw_syn_ttl_synack == res) {printf("%sICMP TTL", slash ? ", " : ""); slash = 1;}
            if (nat.tcp.fw_syn_unreach_synack == res) {printf("%sICMP UNREACHABLE", slash ? ", " : ""); slash = 1;}
            if (nat.tcp.fw_syn_drop_synack == res) {printf("%sDROP", slash ? ", " : ""); slash = 1;}
            printf(" for hole-punching SYN");
        }

        printf("\n");
    }

    comma = 0;
    printf("%-23s: ", "Mangles");
    uriadd1("&mg=");
    if (nat.tcp.mangles & MG_IP_TTL) {printf("IP TTL"); uriadd1("tt,"); comma = 1;}
    if (nat.tcp.mangles & MG_TCP_SEQ) {printf("%sTCP Seq#", comma ? ", ":""); uriadd1("ts,"); comma = 1;}
    if (nat.tcp.mangles & MG_ICMP_PAYLOAD) {printf("%sICMP Payload", comma ? ", ":""); uriadd1("ic,"); comma = 1;}
    if (nat.tcp.mangles & MG_IP_PAYLOAD) {printf("%sIP Payload", comma ? ", ":""); uriadd1("tp,"); comma = 1;}
    printf("\n");
}

void print_timers() {
    int i;
    char *c, *u;
    struct timer *p;

    for(i=0; i<4; i++) {
        p = NULL;
        switch(i) {
            case 0: p = &nat.tcp.timer_syn;  c = "SYN_SENT timer", u="ts"; break;
            case 1: p = &nat.tcp.timer_estd;  c = "ESTABLISHED timer", u="te"; break;
            case 2: p = &nat.tcp.timer_fin;  c = "FIN timer", u="tf"; break;
            case 3: p = &nat.tcp.timer_rst;  c = "RST timer", u="tr"; break;
        }

        printf("%-23s: ", c);
        if (p->max == MAX_INT) {
            if (p->min < 0) {
                printf("Not tested");
                p = NULL;
            } else {
                printf("Atleast %ds", p->min);
            }
        } else if (p->min <= 0) {
            printf("Less than %ds", p->max);
        } else {
            printf("Between %ds and %ds", p->min, p->max);
        }
        printf("\n");
        if (p) uriadd4("&%s=%d,%d", u, p->min, p->max == MAX_INT ? -1 : p->max);
    }

}

void print_ppred() {
    printf("%-23s: ", "Port prediction");
    if (nat.tcp.ppred_rate < 0) {
        printf("Not tested");
    } else {
        printf("%d%%", (int) (nat.tcp.ppred_rate * 100));
        uriadd2("&ppr=%f", nat.tcp.ppred_rate);
    }
    printf("\n");
}

static unsigned char *getuuid() {
    static unsigned char mac[18];
    unsigned char macn[6];
    int i;

#ifdef WIN32
    UUID uuid;
    UuidCreateSequential(&uuid);

    for (i=2; i<8; i++)
        macn[i - 2] = uuid.Data4[i];
#endif


#ifdef LINUX
    struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s;
    int ok = 0;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s==-1) {
        return NULL;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);

    IFR = ifc.ifc_req;
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++) {
        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) {
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
                    ok = 1;
                    break;
                }
            }
        }
    }

    close(s);
    if (ok) {
        bcopy(ifr.ifr_hwaddr.sa_data, macn, 6);
    }
    else {
        return NULL;
    }
#endif

    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
             macn[0], macn[1], macn[2],
             macn[3], macn[4], macn[5]);
    return mac;
}

void print_fingerprint(char *file) {
    int fd;
    char *s = getuuid();
    char u[1024];

    if (s != NULL) {
        uriadd2("&id=%s", s);
    }

    snprintf(u, sizeof(u), RESULT_URL "?v=%s%s\n", VERSION, uri);
    strncpy(uri, u, sizeof(uri));
    if (file != NULL && strlen(file) > 0) {
        unlink(file);
        fd = open(file, O_CREAT | O_WRONLY | O_BINARY | O_TRUNC, S_IWRITE | S_IREAD);
        if (fd != -1) {
            fwrite(fd, uri, (int)strlen(uri));
            close(fd);
        }
    }

}




pthread_t etatid;
THREAD_START count_eta(void *arg) {
    int reset = 0, help = 0;
    while (timeleft != 0) {
        int s = timeleft - time(NULL);
        if (s <= 0) {
            timeleft = time(NULL) + 60;
            s = 60;
            reset++;
            if (reset > 5 && help == 0) {
                printf("\r                                                           \n"
                       "The test seems to be taking much longer than expected.\n"
                       "This may be due to a programming bug or an unexpected situation.\n"
                       "Please visit the FAQ at http://nutss.gforge.cis.cornell.edu/stunt-faq.php\n"
                       "to check for solutions or email saikat" "@" "cs.cornell.edu\n\n");
                help = 1;
            }
        }
        printf("\rEstimated time remaining: %dh %02dm %02ds",
               s / 3600, (s % 3600) / 60, (s % 60));
        fflush(stdout);
        usleep(500000);
    }
    pthread_exit(0);
}

void start_etacount() {
    pthread_create(&etatid, NULL, count_eta, NULL);
}

void stop_etacount() {
    timeleft = 0;
    pthread_join(etatid, NULL);
    printf("\r%45s\r","");
}


