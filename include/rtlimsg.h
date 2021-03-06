
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

#ifndef	RTL_IMSG_H
#define	RTL_IMSG_H

#define	IMSG_DATA_SZ	256

#define	IMSG_MSG	0
#define	IMSG_TIMER	1
#define	IMSG_BOTH	2


// static (or header) part of the message, fixed size less than IMSG_DATA_SZ
#define	im_data		im_u._im_data_
#define	im_head		im_u._im_data_

typedef	struct // P : private attributes, U : user attributes
{
/*P*/	struct	list_head list;		// chain list of messages
/*P*/	unsigned int	im_serial;	// self serial number
/*P*/	char		im_timer;	// is a timer
/*P*/	char		im_delayed;	// is a delayed message
/*P*/	time_t		im_start;	// start at time (now in sec)
/*P*/	time_t 		im_end;		// end at time (now in sec)
/*P*/	int		im_datasz;	// size of allocated data
/*P*/	void		*im_dataptr;	// free required if im_datasz > 0

/*U*/	int		im_class;	// user define
/*U*/	int		im_type;	// user define

/*U*/	void		*im_from;	// address of source object
/*U*/	unsigned int	*im_serialfrom;	// serial number of im_from
/*U*/	void		*im_to;		// address of dest object
/*U*/	unsigned int	im_serialto;	// serial number of im_to


	// for static data message less than sizeof(im_u) in this case 
	// im_datasz == 0 and im_dataptr == &im_data[0]
	// im_datasz im_dataptr im_data are set with functions
	// rtl_imsg[Set|Cpy|Dup]Data()

	union	im_u
	{
/*P*/		void	*_im_struct_;
/*U*/		char	_im_data_[IMSG_DATA_SZ];	
	}	im_u;
/*P*/	time_t 		im_startns;	// start at time (in nsec)
/*P*/	time_t 		im_endns;	// end at time (in nsec)
/*P*/	time_t		im_diffms;	// initial value & diff (in msec)
	
}	t_imsg;

#define	RTL_IQMSG_NO_FLAGS		0

#ifdef	RTL_INTERNAL_IQMSG_STRUCT
typedef	struct
{
	t_imsg	iq_msglist;	// internal messages list
	t_imsg	iq_timerlist;	// internal timers list
	int	iq_msgcount;
	int	iq_timercount;
	int	iq_count;
	pthread_mutex_t	iq_CS;
	pthread_cond_t	iq_CD;
	char	iq_CSDone;
	u_int	iq_flags;
}	t_iqmsg;
#endif


void	rtl_timerResol(int res);
void	*rtl_imsgInitIq();
int	rtl_imsgFreeIq(void *iq);
int	rtl_imsgCount(void *iq);
int	rtl_timerCount(void *iq);
int	rtl_imsgVerifCount(void *iq);
int	rtl_timerVerifCount(void *iq);

t_imsg	*rtl_timerAlloc(int class,int type,time_t delta,void *to,uint32 serial);
t_imsg	*rtl_imsgAlloc(int class,int type,void *to,uint32 serialto);

t_imsg	*rtl_imsgLetData(t_imsg *msg);
t_imsg	*rtl_imsgSetData(t_imsg *msg,void *data);
t_imsg	*rtl_imsgCpyData(t_imsg *msg,void *data,int sz);
t_imsg	*rtl_imsgDupData(t_imsg *msg,void *data,int sz);
t_imsg	*rtl_imsgAppData(t_imsg *msg,void *data,int sz);

int	rtl_imsgAdd(void *iq,t_imsg *msg);
int	rtl_imsgAddDelayed(void *iq,t_imsg *msg,time_t delta);

t_imsg	*rtl_imsgGet(void *iq,int mode);
int	rtl_imsgGetArray(void *piq,int mode,t_imsg *array[],int nbmax);
t_imsg	*rtl_imsgWait(void *iq,int mode);
t_imsg	*rtl_imsgWaitTime(void *piq,int mode,int *ms);

int	rtl_imsgFree(t_imsg *msg);
void	rtl_imsgCancelDelay(t_imsg *msg);
int	rtl_imsgRemove(void *piq,t_imsg *rmv);
int	rtl_imsgRemoveTo(void *piq,void *to);
int	rtl_imsgRemoveToType(void *piq,void *to,int class,int type);
int	rtl_imsgRemoveAll(void *piq);
int     rtl_imsgFastRemove(void *piq,t_imsg *rmv);

void	rtl_imsgDump(t_imsg *msg,FILE *f);
void	rtl_imsgDumpIq(void *piq,FILE *f);

#endif
