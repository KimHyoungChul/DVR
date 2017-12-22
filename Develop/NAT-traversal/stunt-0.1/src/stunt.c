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
#include <sys/types.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "compat.h"
#include "client.h"
#include "print.h"
#include "tests.h"

FILE *dbgout = NULL;
char *dbgfile;

int errline;
char *errfile, *errfunction;
char errbuf[MAXLEN_ERRBUF];

char file[MAXLEN_FILE];
char usable_devices[MAX_DEVICES][MAXLEN_IFACE];

int timeleft;

char raddress[21];
char laddress[3][21] = {"0.0.0.0", "", ""};
char devstr[3][64];
int port = PORT_STUNTD;
int superuser = 0;
SOCKET serv = -1;

bool do_test_bindings = FALSE;
bool do_test_firewall = FALSE;
bool do_test_timers = FALSE;
bool do_test_ppred = FALSE;

bool user_debug = FALSE;

struct NAT nat;

void parse_args(int argc, char **argv) {
    int c;
    int a=0;
    int v=0;
    int t=0;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"interface", 1, 0, 'i'},
            {"port", 1, 0, 'p'},
            {"help", 0, 0, 'h'},
            {"test-bindings", 0, 0, 'b'},
            {"test-filtering", 0, 0, 'f'},
            {"test-timers", 0, 0, 't'},
            {"test-portpred", 0, 0, 'r'},
            {"debug", 0, 0, 'v'},
            {"timer-syn", 1, 0, 1},
            {"timer-estd", 1, 0, 1},
            {"timer-fin", 1, 0, 1},
            {"timer-rst", 1, 0, 1},
            {"timer-portpred", 1, 0, 4},
            {"binding-interval", 1, 0, 2},
            {"portpred-interval", 1, 0, 3},
            {"output", 1, 0, 'o'},
            {"debug-file", 1, 0, 5},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "i:p:hbrftvo:", long_options, &option_index);

        if (c == -1) break;

        switch(c) {

            case 1:
                {
                    int test, tm = atoi(optarg);
                    bool fc;
                    if (t == MAX_TMTESTS || tm < 4) break;

                    if (strcmp(long_options[option_index].name,"timer-syn") == 0) {
                        test = TMTEST_SYN;
                        fc = FALSE;
                    } else if (strcmp(long_options[option_index].name,"timer-estd") == 0) {
                        test = TMTEST_ESTD;
                        fc = FALSE;
                    } else if (strcmp(long_options[option_index].name,"timer-fin") == 0) {
                        test = TMTEST_FIN;
                        fc = TRUE;
                    } else if (strcmp(long_options[option_index].name,"timer-rst") == 0) {
                        test = TMTEST_RST;
                        fc = FALSE;
                    }

                    {
                        struct timeres ts = {&captures[test], tm, tm+10, fc};
                        timers[t++] = ts;
                    }
                }
                break;

            case 2:
                binding_interval = atoi(optarg) * 1000000;
                timeleft_binding = 5 + atoi(optarg) * NUM_TRY_PORTS * NUM_TRY_EACH * 4;
                break;

            case 3:
                ppred_interval = atoi(optarg);
                break;

            case 4:
                ppred_duration = atoi(optarg);
                timeleft_ppred = ppred_duration;
                break;

            case 5:
                dbgfile = optarg;
                dbgout = fopen(optarg, "w+");
                break;

            case 'i':
                if (strcmp(optarg, "help") == 0) {
                    display_devices();
                    exit(1);
                } else if (getifaddr(laddress[a], sizeof(laddress[0]), &optarg) == -1) {
                    fprintf(stderr, "Ignoring non-existant interface %s\n", optarg);
                } else {
                    strncpy(devstr[a], optarg, sizeof(devstr[0]));
                    a++;
                }
                break;

            case 'p':
                port = atoi(optarg);
                break;

            case 'b':
                do_test_bindings = TRUE;
                break;

            case 'f':
                do_test_firewall = TRUE;
                break;

            case 'r':
                do_test_ppred = TRUE;
                break;

            case 't':
                do_test_timers = TRUE;
                break;

            case 'v':
                v++;
                if (v > 1) dbgmsg = debug_message;
                if (v > 0) {
                    _dbgtrace = fprintf;
                    user_debug = TRUE;
                }
                break;

            case 'o':
                strncpy(file, optarg, MAXLEN_FILE);
                break;

            case '?':
            case 'h':
                display_help(argv[0]);
                exit(1);
        }
    }

    if (optind < argc) {
        struct hostent *ent = gethostbyname(argv[optind]);
        if (ent != NULL) {
            snprintf(raddress, sizeof(raddress), "%d.%d.%d.%d",
                   NIPQUAD(*(int *)ent->h_addr_list[0]));
        } else {
            fprintf(stderr, "Cannot resolve %s\n", argv[optind]);
            display_help(argv[0]);
            exit(1);
        }
    } else {
            strncpy(raddress, STUNTD_IP, sizeof(raddress));
    }

    if (dbgout == NULL) {
        dbgout = stderr;
    }

    if (a < 1) {
        int i = 0;
        struct sockaddr_in raddr, laddr;
        SOCKET sock = -1;

        populate_devices();
        printf("Determining network interface to use ... ");
        fflush(stdout);
        while (strlen(usable_devices[i]) != 0 && a == 0) {
            int r;

            optarg = devstr[a];
            strncpy(devstr[a], usable_devices[i], sizeof(devstr[0]));
            getifaddr(laddress[a], sizeof(laddress[i]), &optarg);

            laddr.sin_family = AF_INET;
            laddr.sin_addr.S_addr = inet_addr(laddress[a]);
            laddr.sin_port = 0;

            raddr.sin_family = AF_INET;
            raddr.sin_addr.S_addr = inet_addr(raddress);
            raddr.sin_port = htons(port);

            r = (int)(sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));
            if (r != -1) r = bind(sock, (struct sockaddr *)&laddr, sizeof(laddr));
            if (r != -1) r = connect(sock, (struct sockaddr *)&raddr, sizeof(raddr));
            if (r != -1) a++;

            closesocket(sock);
            i++;
        }
        printf("done\n");
    }

    if (a < 1) {
        display_help(argv[0]);
        exit(1);
    }

    if (do_test_ppred == TRUE) {
        do_test_bindings = TRUE;
    }

    if (do_test_bindings == FALSE &&
        do_test_firewall == FALSE &&
        do_test_timers == FALSE &&
        do_test_ppred == FALSE) {

        do_test_bindings = TRUE;
        do_test_firewall = TRUE;
        do_test_timers = TRUE;
        do_test_ppred = TRUE;
    }

    if (t == 0) {
        struct timeres timers_default[] = {
#ifdef WIN32
            {&captures[TMTEST_SYN]  , 67 , 77 , FALSE }  ,
            {&captures[TMTEST_ESTD] , 7197 , 7207 , FALSE }  ,
            {&captures[TMTEST_ESTD] , 897 , 907 , FALSE }  ,
            {&captures[TMTEST_FIN]  , 57 , 67 , TRUE  }  ,
            {&captures[TMTEST_RST]  , 57 , 67 , FALSE }
#endif
#ifdef LINUX
            {&captures[TMTEST_SYN]  , 149 , 159 , FALSE }  ,
            {&captures[TMTEST_ESTD] , 7197 , 7207 , FALSE }  ,
            {&captures[TMTEST_ESTD] , 897 , 907 , FALSE }  ,
            {&captures[TMTEST_FIN]  , 57 , 67 , TRUE  }  ,
            {&captures[TMTEST_RST]  , 57 , 67 , FALSE }
#endif
        };
        t = elemof(timers_default);
        memcpy(timers, timers_default, MIN(sizeof(timers), sizeof(timers_default)));
    }

    timeleft_timer = 0;
    while (t > 0) {
        timeleft_timer += timers[--t].timeos;
    }

    if (do_test_bindings == FALSE) timeleft_binding = 0;
    if (do_test_firewall == FALSE) timeleft_filter = 0;
    if (do_test_timers == FALSE) timeleft_timer = 0;
    if (do_test_ppred == FALSE) timeleft_ppred = 0;

    if (!superuser && (do_test_firewall == TRUE || do_test_timers == TRUE)) {
        fprintf(stderr, "stunt: Need root privileges\n");
        exit(1);
    }

}

void launch() {
    char res;
    char cmd[1024];
    unsigned int i;
    printf("\nSubmit NAT fingerprint for statistics? (Y/n) ");
    res = tolower(fgetc(stdin));

    if (res != 'n') {
        // Changing & -> _ is necessary for windows. Useful for both.
        printf("\nTo complete the submission, please wait for the STUNT result \n"
               "submission webpage to load in your web browser. You will be asked\n"
               "to enter details about your NAT and your contact information.\n"
               "To complete the submission, click 'submit' on that page once you\n"
               "have entered all the available information.\n");

        for(i=0; i<strlen(uri); i++) if (uri[i] == '&') uri[i] = '_';
#ifdef WIN32
        snprintf(cmd, sizeof(cmd), "start %s", uri);
        system(cmd);
#endif

#ifdef LINUX
        snprintf(cmd, sizeof(cmd), "htmlview \"%s\" || mozilla \"%s\"", uri, uri);
        if (fork() == 0) {
            execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
            exit(1);
        }
#endif

        if (file != NULL && strlen(file) > 0) {
            printf("\nA copy of the results is stored in the file: %s\n"
                   "If the web-based submission process does not work, please email\n"
                   "this file to saikat" "@" "cs.cornell.edu along with the vendor and\n"
                   "model number of your NAT.\n", file);

        }
    } else if (file != NULL && strlen(file) > 0) {
        printf("\nThe test results have been saved to the file: %s\n"
               "If you wish to submit the results later, please email this file\n"
               "to saikat" "@" "cs.cornell.edu along with the vendor and model number\n"
               "of your NAT.\n", file);
    }
}


int main(int argc, char **argv) {
    int e = 0;

#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        perror("WSAStartup");
        exit(1);
    }

    superuser = 1;
#endif

#ifdef LINUX
    superuser = (getuid() == 0);
#endif

    parse_args(argc, argv);

    update_timeleft();
    srandom(time(NULL));

    printf("The test is about to begin. The following tests shall be performed:\n\n");
    if (do_test_bindings) printf("  Bindings (~ %dmin %dsec)\t\tTests the TCP port binding behaviour\n",
                                 timeleft_binding / 60, timeleft_binding % 60);
    if (do_test_firewall) printf("  Filtering (~ %dmin %dsec)\t\tTests TCP packet filtering\n",
                                 timeleft_filter / 60, timeleft_filter % 60);
    if (do_test_timers) printf("  Timers (~ %dmin %dsec)\t\tTests TCP binding timers\n",
                                 timeleft_timer / 60, timeleft_timer % 60);
    if (do_test_ppred) printf("  Port prediction (~ %dmin %dsec)\tTests TCP binding prediction rate\n",
                                 timeleft_ppred / 60, timeleft_ppred % 60);
    printf("\nDetailed explanations of the tests and NAT characteristics inferred from\n"
           "them can be found at http://nutss.gforge.cis.cornell.edu/stunt-client.php\n\n");

    start_etacount();

    if (do_test_bindings == TRUE) {
        if (test_bindings() == -1) {
            do_test_bindings = FALSE;
            e |= 0x01;
            fprintf(stderr, "\nstunt: error testing bindings, while %s\n", errbuf);
            if (user_debug == TRUE) printf("stunt: error reported in function %s at line %d in file %s\n", errfunction, errline, errfile);
        }
        timeleft_binding = 0;
        update_timeleft();
    }

    strcpy(errbuf, "");

    if (do_test_firewall == TRUE) {
        if (test_firewall() == -1) {
            do_test_firewall = FALSE;
            e |= 0x02;
            fprintf(stderr, "\nstunt: error testing filtering, while %s\n", errbuf);
            if (user_debug == TRUE) fprintf(stderr, "stunt: error reported in function %s at line %d in file %s\n", errfunction, errline, errfile);
        }
        timeleft_filter = 0;
        update_timeleft();
    }

    strcpy(errbuf, "");

    if (do_test_timers == TRUE) {
        if (test_timers() == -1) {
            do_test_timers = FALSE;
            e |= 0x04;
            fprintf(stderr, "\nstunt: error testing timers, while %s\n", errbuf);
            if (user_debug == TRUE) fprintf(stderr, "stunt: error reported in function %s at line %d in file %s\n", errfunction, errline, errfile);
        }
        timeleft_timer = 0;
        update_timeleft();
    }

    if (do_test_ppred == TRUE) {
        if (test_ppred() == -1) {
            do_test_ppred = FALSE;
            e |= 0x08;
            fprintf(stderr, "\nstunt: error testing port prediction, while %s\n", errbuf);
            if (user_debug == TRUE) fprintf(stderr, "stunt: error reported in function %s at line %d in file %s\n", errfunction, errline, errfile);
        }
        timeleft_ppred = 0;
        update_timeleft();
    }

    memset(&nat, 0, sizeof(nat));
    stop_etacount();
    printf("\r%40s\n", "");

    if (do_test_bindings == TRUE) {
        analyze_bindings();
        print_bindings();
    }

    if (do_test_firewall == TRUE) {
        analyze_filtering();
        print_filtering();
    }

    if (do_test_timers == TRUE) {
        analyze_timers();
        print_timers();
    }

    if (do_test_ppred == TRUE) {
        analyze_ppred();
        print_ppred();
    }

    if (do_test_bindings == TRUE ||
        do_test_firewall == TRUE ||
        do_test_timers == TRUE ||
        do_test_ppred == TRUE) {
        print_fingerprint(file);
        launch();
    }

    if (e != 0) {
        printf("\nAn ERROR occured during the test. Please check the FAQ at \n"
               "http://nutss.gforge.cis.cornell.edu/stunt-faq.php on how to \n"
               "fix this error. Alternatively ");
        if (dbgout != stderr) {
            printf("please email the file \n"
                   "%s to saikat" "@" "cs.cornell.edu\n"
                   "for diagnosis.\n", dbgfile);
        }else if (_dbgtrace == fprintf) {
            printf("please email the output \n"
                   "to saikat" "@" "cs.cornell.edu for diagnosis.\n");
        } else {
            printf("please rerun the test\n"
                   "passing the options: -v --debug-file debug.txt\n");
        }
    }


    printf("\nThank you for running the STUNT client.\n");

    exit(e);
}
