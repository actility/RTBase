
/*
 * Copyright (c) 2021 Actility. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by ACTILITY.
 * 4. Neither the name of ACTILITY  nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ACTILITY  "AS IS" AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ACTILITY  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <signal.h>

#define	SZPOLL	100

#if	0
           struct pollfd {
               int   fd;         /* file descriptor */
               short events;     /* requested events */
               short revents;    /* returned events */
           };
#endif

typedef struct pollfd POLLFD;
typedef struct
{
	int	(*fctevent)();
	int	(*fctrequest)();
	void	*ref;
	void	*ref2;
} HAND;

typedef struct polltable
{
	POLLFD	*tbPoll;
	HAND	*handPoll;
	int	szTbPoll;
	int	nbPoll;
} polltable_t;


static int rtl_pollFind(polltable_t *table, int fd);


void	*rtl_pollInit()	/* TS */
{
	polltable_t *table;
	table	= calloc(1,sizeof(polltable_t));
	return table;
}

/*
 *
 *	Ajoute une entree dans la table de polling
 *
 */

int	rtl_pollAdd(void *ptbl,int fd,			/* TS */
		int (*fctevent)(void *,int,void *,void *,int),
		int (*fctrequest)(void *,int,void *,void *,int),
		void * ref,void * ref2)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	POLLFD *poll;
	int	i;

	if(table->nbPoll==table->szTbPoll)
	{
		i=table->szTbPoll;
		if(!i)
		{
			table->szTbPoll=SZPOLL;
			table->handPoll=(HAND *)malloc(table->szTbPoll*sizeof(HAND));
			table->tbPoll=(POLLFD *)malloc(table->szTbPoll*sizeof(POLLFD));
		}
		else
		{
			table->szTbPoll+=SZPOLL;
			table->handPoll=(HAND *)realloc(table->handPoll,table->szTbPoll*sizeof(HAND));
			table->tbPoll=(POLLFD *)realloc(table->tbPoll,table->szTbPoll*sizeof(POLLFD));
		}

		if	(!table->handPoll || !table->tbPoll)
		{
			return	-1;
		}
		for	(poll=table->tbPoll+i; i<table->szTbPoll; i++,poll++)
		{
			poll->fd	= -1;
			poll->events	= 0;
			poll->revents	= 0;
		}
	}

	for	(poll=table->tbPoll,i=0; i<table->szTbPoll; i++,poll++)
	{
		if(poll->fd<0)
		{
			poll->fd	= fd;
			poll->events	= POLLIN;
			poll->revents	= 0;
			table->handPoll[i].fctevent	= fctevent;
			table->handPoll[i].fctrequest	= fctrequest;
			if (fctrequest)
				poll->events	= 0;
			table->handPoll[i].ref	= ref;
			table->handPoll[i].ref2	= ref2;
			table->nbPoll++;
			return i;
		}
	}
	return -1;
}

int	rtl_pollChg(void *ptbl, int fd,int (*fctevent)(void *,int,void *,void *,int), void *ref, void *ref2)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	i	= rtl_pollFind(table,fd);
	if	(i < 0)
		return	-1;

	table->handPoll[i].fctevent	= fctevent;
	table->handPoll[i].ref	= ref;
	table->handPoll[i].ref2	= ref2;
	return 0;
}

/*
 *
 *	recherche d'un file-descriptor ds la table
 *
 */

static int rtl_pollFind(polltable_t *table, int fd)	/* TS */
{
	POLLFD *poll;
	int	i;

	for(i=0,poll=table->tbPoll;i<table->szTbPoll;i++,poll++)
	{
		if(poll->fd==fd)
		{
			return i;
		}
	}
	return -1;
}

/*
 *
 *	Change le mode de polling
 *
 */

int	rtl_pollSetEvt(void *ptbl, int fd,int mode)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if((i=rtl_pollFind(table,fd))>=0)
	{
		table->tbPoll[i].events	= mode & (POLLIN|POLLOUT|POLLPRI|POLLERR);
		return 0;
	}
	return -1;
}

int	rtl_pollSetEvt2(void *ptbl, int pos,int mode)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	if	(pos < 0 || pos >= table->szTbPoll)
		return	-1;
	table->tbPoll[pos].events	= mode & (POLLIN|POLLOUT|POLLPRI|POLLERR);
	return 0;
}


/*
 *
 *	Retourne les events de polling
 *
 */

int	rtl_pollGetEvt(void *ptbl, int fd)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if((i=rtl_pollFind(table,fd))>=0)
	{
		return table->tbPoll[i].revents;
	}
	return 0;
}

int	rtl_pollGetEvt2(void *ptbl, int pos)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	if	(pos < 0 || pos >= table->szTbPoll)
		return	-1;
	return table->tbPoll[pos].revents;
}

/*
 *
 *	Retourne le mode de polling
 *
 */

int	rtl_pollGetMode(void *ptbl, int fd)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if((i=rtl_pollFind(table,fd))>=0)
	{
		return table->tbPoll[i].events;
	}
	return 0;
}

int	rtl_pollGetMode2(void *ptbl, int pos)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	if	(pos < 0 || pos >= table->szTbPoll)
		return	-1;
	return table->tbPoll[pos].events;
}


/*
 *
 *	Retire un file-descriptor de la liste
 *
 */

int	rtl_pollRmv(void *ptbl, int fd)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if	(fd < 0)
		return	-1;

	i	= rtl_pollFind(table,fd);
	if(i<0)
	{
		return	-1;
	}

	table->tbPoll[i].fd		= -1;
	table->tbPoll[i].events		= 0;
	table->handPoll[i].fctevent	= NULL;
	table->handPoll[i].fctrequest	= NULL;
	table->nbPoll--;
	return	0;
}

/*
 *
 *	Scrutation des file-descriptors et appel des procedures
 *
 */

static int rtl_pollRequest(polltable_t *table)	/* TS */
{
	int	i;
	int	nb=0;
	int	ret;

	for (i=0; i<table->szTbPoll; i++)
	{
//		if (table->handPoll[i].fctrequest && (table->tbPoll[i].fd >= 0))
		if ((table->tbPoll[i].fd >= 0) && 
					table->handPoll[i].fctrequest)
		{
			ret =	table->handPoll[i].fctrequest(table,
					table->tbPoll[i].fd,
					table->handPoll[i].ref,
					table->handPoll[i].ref2,
					table->tbPoll[i].events);
			if (ret>=0)
				table->tbPoll[i].events=ret;
			nb++;
		}
	}
	return nb;
}

static int rtl_pollScan(polltable_t *table,int resetevent)	/* TS */
{
	int	i;
	int	nb=0;

	for	(i=0; i<table->szTbPoll; i++)
	{
		if	((table->tbPoll[i].fd >= 0) && 
						!table->handPoll[i].fctevent)
		{
			table->tbPoll[i].fd	= -1;
			table->tbPoll[i].events	= 0;
			table->tbPoll[i].revents= 0;
		}
		if	(table->tbPoll[i].revents & (POLLIN|POLLOUT|POLLPRI|POLLERR))
		{
//			if	((table->tbPoll[i].fd<0) || !table->handPoll[i].fctevent)
			if	((table->tbPoll[i].fd>=0) && table->handPoll[i].fctevent)
				table->handPoll[i].fctevent(table,table->tbPoll[i].fd,table->handPoll[i].ref,table->handPoll[i].ref2,table->tbPoll[i].revents);

			table->tbPoll[i].revents=0;
			if	(resetevent)
				table->tbPoll[i].events=0;
			nb++;
		}
	}
	return nb;
}


/*
 *	Appel des callbacks pour valoriser les events attendus
 *	Attente de donnees sur un des fd
 *
 */

int	rtl_poll(void *ptbl, int timeout)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	rc;

	if(table->nbPoll<0)
		return -1 ;

	rtl_pollRequest(table);
	rc	= poll(table->tbPoll,table->szTbPoll,timeout);

	if	(rc == 0)
	{
		return	0;
	}

	if	(rc < 0)
	{
		if	(errno == EINVAL)
		{
			return -EINVAL;
		}
		return -1;
	}
	return rtl_pollScan(table,0);
}

/*
 *	PAS d'appel des callbacks pour valoriser les events attendus
 *		=> les events attendus doivent etre geres au fil de l'eau
 *	Attente de donnees sur un des fd
 *
 */

int	rtl_pollRaw(void *ptbl, int timeout)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	rc;

	if(table->nbPoll<0)
		return -1 ;

	rc	= poll(table->tbPoll,table->szTbPoll,timeout);

	if	(rc == 0)
	{
		return	0;
	}

	if	(rc < 0)
	{
		if	(errno == EINVAL)
		{
			return -EINVAL;
		}
		return -1;
	}
	return rtl_pollScan(table,0);
}
