
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "rtlclif.h"


#define	CLI_MAX_FD	32
#define	TCP_FRAME_SZ	1024



static	t_clif	*TbCli[CLI_MAX_FD];
static	int	FdListen = -1;
static	int	(*UserCmdFct)(t_clif *,char *);

static	int	TcpKeepAlive	= 1;
#if	0
static	int	TcpKeepIdle	= 60;
static	int	TcpKeepIntvl	= 10;
static	int	TcpKeepCnt	= 2;
#endif


/*!
 *
 * @brief set keep alive attributs on a fd socket
 * Actualy we dont change system attributs, we just set keep alive mode.
 * @param fd the socket fd
 * @return void
 * 
 */
static	void	AdmKeepAlive(int fd)
{
	if	( TcpKeepAlive > 0 )
	{
		setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,
				(char *)&TcpKeepAlive,sizeof(TcpKeepAlive));
#if	0
		if	( TcpKeepIdle > 0 )
		{
			setsockopt(fd,IPPROTO_TCP,TCP_KEEPIDLE,
			(char *)&TcpKeepIdle,sizeof(TcpKeepIdle));
		}
		if	( TcpKeepIntvl > 0 )
		{
			setsockopt(fd,IPPROTO_TCP,TCP_KEEPINTVL,
			(char *)&TcpKeepIntvl,sizeof(TcpKeepIntvl));
		}
		if	( TcpKeepCnt > 0 )
		{
			setsockopt(fd,IPPROTO_TCP,TCP_KEEPCNT,
			(char *)&TcpKeepCnt,sizeof(TcpKeepCnt));
		}
#endif
	}
}

/*!
 *
 * @brief Call back function when we need to store data waiting to be sent
 * @param cl the destination connection
 * @param buf the data to store
 * @param len the lenght of data
 * @return void
 * 
 */
static	void	AdmAddBuf(t_clif *cl,char *buf,int len)
{
	int	fd;
	t_clout	*out;

	fd	= cl->cl_fd;

	RTL_TRDBG(9,"Adm add buffer lg=%d\n",len);
	if	(!buf || len <= 0)
		return;

	// buf deja decoupe en morceaux plus petits que TCP_FRAME_SZ
	out	= (t_clout *)malloc(sizeof(t_clout));
	if	( !out )
		return;

	out->o_len	= len;
	out->o_buf	= (char *)malloc(len);
	if	( !out->o_buf )
		return;
	memcpy	(out->o_buf,buf,len);

	list_add_tail( &(out->list), &(cl->cl_lout.list) );
}
