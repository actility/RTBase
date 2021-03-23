
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

#ifndef	RTL_EPOLL_H
#define	RTL_EPOLL_H

#include	<sys/epoll.h>

#define	RTL_EPTB_MAGIC	0x740411
#define	RTL_EPHD_MAGIC	0x960210

typedef struct epollhand
{
	unsigned int	hd_magic;
	int		hd_fd;
	unsigned int	hd_serial;
	int		(*hd_fctevent)();
	int		(*hd_fctrequest)();	// too bad when using epoll ...
	void		*hd_ref;
	void		*hd_ref2;
	unsigned int	hd_events;	// last events returned
	unsigned int	hd_revents;	// last events requested
} t_epollhand;

typedef struct epolltable
{
	unsigned int	ep_magic;
	int		ep_fd;
	int		ep_nbFd;
	int		ep_szTb;
	t_epollhand	**ep_hdTb;
	unsigned int	ep_walkrr;
} t_epolltable;


/* epoll.c */
void *rtl_epollInit(void);
int rtl_epollAdd(void *ptbl, int fd, int (*fctevent)(void *, int, void *, void *, int), int (*fctrequest)(void *, int, void *, void *, int), void *ref, void *ref2);
void *rtl_epollHandle(void *ptbl, int fd);
void *rtl_epollHandle2(void *ptbl, int pos);
unsigned int rtl_epollSerial(void *ptbl, int fd);
int rtl_epollSetEvt(void *ptbl, int fd, int mode);
int rtl_epollSetEvt2(void *ptbl, int pos, int mode);
int rtl_epollGetEvt(void *ptbl, int fd);
int rtl_epollGetEvt2(void *ptbl, int pos);
int rtl_epollGetMode(void *ptbl, int fd);
int rtl_epollGetMode2(void *ptbl, int pos);
int rtl_epollRmv(void *ptbl, int fd);
int rtl_epoll(void *ptbl, int timeout);
int rtl_epollRaw(void *ptbl, int timeout);

int rtl_epollWalk(void *ptbl,int per,int (*fctwalk)(time_t now,void *ptbl,void *hand));

#endif
