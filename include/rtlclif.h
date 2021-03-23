
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

#ifndef	RTL_CLIF_H
#define	RTL_CLIF_H


// a buffer waiting for "poll write event" on a tcp/socket for CLI
typedef	struct	s_lout
{
	struct list_head list;	// the chain list of pending data
	char	*o_buf;		// a buffer
	int	o_len;		// lenght of o_buf
}	t_clout;

// a CLI connection on admin port
typedef	struct
{
	int	cl_fd;		// associated socket
	int	cl_pos;		// position in cl_cmd
	char	cl_cmd[1000];	// the user input accumulated until '\n' | '\r'
	time_t	cl_lasttcpread;	// time of last remote read
	time_t	cl_lasttcpwrite;// time of last remote write
	int	cl_nbkey;	// number of retry for password
	int	cl_okkey;	// password ok
	t_clout	cl_lout;	// list of pending data
	char	cl_data[256];	// sensor address to sniff ('*' == all)
}	t_clif;


#endif
