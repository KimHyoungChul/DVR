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

#ifndef __PACKETS_H
#define __PACKETS_H

#include "client.h"

void pktadd_unsolicited(struct message *msg);
void pktadd_synrstsyn(struct message *msg);
void pktadd_synttlsyn(struct message *msg);
void pktadd_synunreachsyn(struct message *msg);
void pktadd_syndropsyn(struct message *msg);
void pktadd_synrstsynack(struct message *msg);
void pktadd_synttlsynack(struct message *msg);
void pktadd_synunreachsynack(struct message *msg);
void pktadd_syndropsynack(struct message *msg);
void pktadd_estdsynaddress(struct message *msg);
void pktadd_estdsynaddrport(struct message *msg);
void pktadd_tmsyn(struct message *msg);
void pktadd_tmestd(struct message *msg);
void pktadd_tmfin(struct message *msg);
void pktadd_tmrst(struct message *msg);

void msginit_capreq(struct message *req, struct captres *res);

#endif
