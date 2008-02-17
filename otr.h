  /********************************************************************\
  * BitlBee -- An IRC to other IM-networks gateway                     *
  *                                                                    *
  * Copyright 2002-2008 Wilmer van der Gaast and others                *
  \********************************************************************/

/*
  OTR support (cf. http://www.cypherpunks.ca/otr/)
  
  2008, Sven Moritz Hallberg <pesco@khjk.org>
  (c) and funded by stonedcoder.org
*/

/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License with
  the Debian GNU/Linux distribution in /usr/share/common-licenses/GPL;
  if not, write to the Free Software Foundation, Inc., 59 Temple Place,
  Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef BITLBEE_PROTOCOLS_OTR_H
#define BITLBEE_PROTOCOLS_OTR_H

#include "bitlbee.h"


// forward decls to avoid mutual dependencies
struct irc;
struct im_connection;
struct account;

// 'otr' root command, hooked up in root_commands.c
void cmd_otr(struct irc *, char **args);


#ifdef WITH_OTR
#include <libotr/proto.h>
#include <libotr/message.h>
#include <libotr/privkey.h>

/* representing a keygen job */
typedef struct kg {
	char *accountname;
	char *protocol;
	
	struct kg *next;
} kg_t;

/* struct to encapsulate our book keeping stuff */
typedef struct otr {
	OtrlUserState us;
	pid_t keygen;    /* pid of keygen slave (0 if none) */
	FILE *to;        /* pipe to keygen slave */
	FILE *from;      /* pipe from keygen slave */
	
	/* active keygen job (NULL if none) */
	char *sent_accountname;
	char *sent_protocol;
	
	/* keygen jobs waiting to be sent to slave */
	kg_t *todo;
} otr_t;

/* called from main() */
void otr_init(void);

/* called from irc_new()/irc_free() */
otr_t *otr_new();
void otr_free(otr_t *otr);

/* called by storage_* functions */
void otr_load(struct irc *irc);
void otr_save(struct irc *irc);
void otr_remove(const char *nick);
void otr_rename(const char *onick, const char *nnick);

/* called from account_add() */
int otr_check_for_key(struct account *a);

/* called from imcb_buddy_msg() */
char *otr_handle_message(struct im_connection *ic, const char *handle,
	const char *msg);
	
/* called from imc_buddy_msg() */
int otr_send_message(struct im_connection *ic, const char *handle, const char *msg,
	int flags);

#else

typedef void otr_t;
typedef void *OtrlMessageAppOps;

#define otr_init() {}
#define otr_new() (NULL)
#define otr_free(otr) {}
#define otr_load(irc) {}
#define otr_save(irc) {}
#define otr_remove(nick) {}
#define otr_rename(onick,nnick) {}
#define otr_check_for_key(acc) (0)
#define otr_handle_message(ic,handle,msg) (g_strdup(msg))
#define otr_send_message(ic,h,m,f) (ic->acc->prpl->buddy_msg(ic,h,m,f))

void cmd_otr_nosupport(void *, char **);

#endif
#endif
