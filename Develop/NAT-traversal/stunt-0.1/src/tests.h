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

#ifndef __TESTS_H
#define __TESTS_H

int test_bindings();
int test_firewall();
int test_timers();
int test_ppred();

void analyze_bindings();
void analyze_filtering();
void analyze_timers();
void analyze_ppred();


void update_timeleft();

extern int timeleft_binding;
extern int timeleft_filter;
extern int timeleft_timer;
extern int timeleft_ppred;

extern int binding_interval;
extern int ppred_interval;
extern int ppred_duration;


extern struct captres captures[CPT_TMTEST_LAST + 1];
extern struct timeres timers[MAX_TMTESTS];

#endif

