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

#ifndef __PRINT_H
#define __PRINT_H

#include "stunt.h"

extern void (*dbgmsg)(struct message *msg);
extern int (*_dbgtrace)(FILE *out, const char *c, ...);

void nodebug_message(struct message *msg);
void display_help(char *argv0);
void print_bindings();
void print_filtering();
void print_timers();
void print_ppred();
void print_fingerprint(char *file);
void display_devices();

void start_etacount();
void stop_etacount();

#define MAXLEN_URI  2048

extern char uri[MAXLEN_URI];

#endif
