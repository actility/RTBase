
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
 *	Horloge au millieme de secondes
 */


#include <stdio.h>
/*
 *	Utilisation de la fonction gettimeofday()
 */


#include <time.h>
#include <sys/time.h>

time_t	RtlBaseSec;

void	rtl_timespec(struct timespec *tv)
{
	struct	timeval	now;
	struct	timezone tz;
	gettimeofday(&now,&tz);
	tv->tv_sec	= now.tv_sec;
	tv->tv_nsec	= now.tv_usec * 1000;
}

time_t	rtl_time(time_t *part)
{
	return	time(part);
}

time_t	rtl_tmms()	/* TS */
{
	struct timeval tv;
	struct timezone tz;
	time_t	ret;

	gettimeofday(&tv,&tz);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;

#ifdef SCO32v4
	tv.tv_usec%=1000000;
#endif

	ret	= (tv.tv_usec/1000)+(1000*tv.tv_sec);

	return	ret;
}

void	rtl_timebase(struct timespec *tv)
{
	rtl_timespec(tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv->tv_sec;
	}
	tv->tv_sec -= RtlBaseSec;
}

void	rtl_timebaseVal(time_t *sec,time_t *ns)
{
	struct timespec	tv;
	rtl_timespec(&tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;

	*sec	= tv.tv_sec;
	*ns	= tv.tv_nsec;
}



#if	0
// horloge en millis secondes
time_t	rtl_timems(time_t *base,time_t *etms,time_t *dtms)	/* TS */
{
	struct timeval tv;
	struct timezone tz;
	time_t	ret;

	gettimeofday(&tv,&tz);
	if(!*base)
	{
		*base	= tv.tv_sec;
	}
	tv.tv_sec -= *base;

#ifdef SCO32v4
	tv.tv_usec%=1000000;
#endif

	ret	= (tv.tv_usec/1000)+(1000*tv.tv_sec);

	*dtms	= (ret - *etms);
	*etms	= ret;

	return	ret;
}

// horloge en centis secondes
time_t	rtl_timecs(time_t *base,time_t *etcs,time_t *dtcs)	/* TS */
{
	struct timeval tv;
	struct timezone tz;
	time_t	ret;

	gettimeofday(&tv,&tz);
	if(!*base)
	{
		*base	= tv.tv_sec;
	}
	tv.tv_sec -= *base;

	ret	= (tv.tv_usec/10000)+(100*tv.tv_sec);

	*dtcs	= (ret - *etcs);
	*etcs	= ret;

	return	ret;
}
#endif
