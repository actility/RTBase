
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
#include <unistd.h>


#include <time.h>
#include <sys/time.h>

#include "rtlbase.h"

extern	time_t	RtlBaseSec;

#ifdef  WIRMAV2
#undef  CLOCK_MONOTONIC_RAW	// does not work on wirmav2 ...	
#endif


#if	defined(_POSIX_TIMERS) && defined(_POSIX_MONOTONIC_CLOCK)
#ifdef	CLOCK_MONOTONIC_RAW
static	char *TimeType	= "@(#) Actility clock_gettime_CLOCK_MONOTONIC_RAW";
#else
static	char *TimeType	= "@(#) Actility clock_gettime_POSIX_MONOTONIC_CLOCK";
#endif
#else
static	char *TimeType	= "@(#) Actility gettimeofday";
#endif

char    *rtl_timespecmonoType()
{
        return  TimeType;
}


#if	defined(_POSIX_TIMERS) && defined(_POSIX_MONOTONIC_CLOCK)
void	rtl_timespecmono(struct timespec *tv)
{
#ifdef	CLOCK_MONOTONIC_RAW
	clock_gettime(CLOCK_MONOTONIC_RAW,tv);
#else
	clock_gettime(CLOCK_MONOTONIC,tv);
#endif
}
#else
void	rtl_timespecmono(struct timespec *tv)
{
	struct	timeval		now;
	gettimeofday(&now,NULL);
	tv->tv_sec	= now.tv_sec;
	tv->tv_nsec	= now.tv_usec * 1000;
}
#endif

time_t	rtl_timemono(time_t *part)
{
	struct timespec	tv;

	rtl_timespecmono(&tv);
	if	(part)
	{
		*part	= tv.tv_sec;
	}

	return	tv.tv_sec;
}

time_t	rtl_tmmsmono()
{
	struct timespec	tv;
	time_t		ret;

	rtl_timespecmono(&tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;
	ret	= (tv.tv_nsec/1000000)+(1000*tv.tv_sec);
	return	ret;

}

void	rtl_timebasemono(struct timespec *tv)
{
	rtl_timespecmono(tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv->tv_sec;
	}
	tv->tv_sec -= RtlBaseSec;
}

void	rtl_timebasemonoVal(time_t *sec,time_t *ns)
{
	struct timespec	tv;
	rtl_timespecmono(&tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;

	*sec	= tv.tv_sec;
	*ns	= tv.tv_nsec;
}

void	rtl_timespecaddms(struct timespec *tv,int ms)
{
	tv->tv_sec	+= ms / 1000;
	tv->tv_nsec	+= (ms % 1000) * 1000000;
	if	(tv->tv_nsec >= 1000000000)
	{
		tv->tv_sec	+= 1;
		tv->tv_nsec	-= 1000000000;
	}
}

int	rtl_timespeccmp(struct timespec *tv1, struct timespec *tv2)
{
	return	RTL_TIMESPECMP(tv1,tv2);
}
