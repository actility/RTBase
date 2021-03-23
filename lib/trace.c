
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <time.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>

#include "rtlbase.h"

#define	ABS(x)		((x) > 0 ? (x) : -(x))


static	int	TraceLevel	= 0;
static	int	TraceRotate	= 0;
static	char	TraceFiles[512];

static	int	SZMAX	= RTL_TRUNLIMITED; // no limit during starting of LRR

static	pthread_mutex_t	CS;
static	char	CSDone	= 0;

static	void	InitCS(void)
{

	if	( CSDone )
		return;
	if (pthread_mutex_init(&CS,NULL))
	{
		fprintf(stderr,"'%s' %d error while initializing mutex '%s' !!!\n",
			__FILE__,__LINE__,RTL_STRERRNO);
		return;
	}
	CSDone	= 1;
}

#if	0	// useless avoid warning
static	void	*GetCS(void)
{
	if (!CSDone)
		return	NULL;
	return	&CS;
}
#endif

static	void	EnterCS(void)
{
	if (!CSDone)
		return;
	if (pthread_mutex_lock(&CS))
	{
		fprintf(stderr,"'%s' %d error while locking mutex '%s' !!!\n",
			__FILE__,__LINE__,RTL_STRERRNO);
	}


}

static	void	LeaveCS(void)
{
	if (!CSDone)
		return;
	pthread_mutex_unlock(&CS);
}

void	rtl_traceunlock(void)
{
	LeaveCS();
}

int	rtl_tracemutex()
{
	InitCS();
	return	1;
}

void	rtl_tracelevel(int level)
{
	TraceLevel	= level;
}

void	rtl_tracesizemax(int sz)
{
    SZMAX = sz;
}

void	rtl_tracecommitsizemax(void)
{
    // call after reading conf files
    // if still unlimited, force the default

	if (SZMAX == RTL_TRUNLIMITED) {
		SZMAX = RTL_TRDEFAULT;
    }
    rtl_tracedbg(0, __FILE__, __LINE__, "TRACE used szmax: %d\n", SZMAX);
}

static int g_backup = 0;

void rtl_traceBackup(int flag) {
	g_backup = flag;
}

// RDTP-12864
// 0: default time format with rtl_hhmmssms()
// 1: iso8601 time format with rtl_getCurrentIso8601date()
static	int	IsoTimeFormat	= 0;	

void	rtl_traceIsoTimeFormat(int f)
{
	IsoTimeFormat	= f;
}

// RDTP-9309
static	double	FsSzMaxLow	= 0.0;
static	double	FsSzMaxHigh	= 0.0;
static	double	FsSzUsed	= 0;	// [0..100]%
static	int	FsSzState	= 0;

void	rtl_tracefslimits(double low,double high)
{
	FsSzMaxLow	= low;
	FsSzMaxHigh	= high;
	FsSzState	= 0;
}

double	rtl_tracefsperused()
{
	return	FsSzUsed;
}

static	int	FsSzMax(FILE *f)
{
	struct	statfs	vfs;
	char	*change	= NULL;

	if	(FsSzMaxLow <= 0.0 || FsSzMaxHigh <= 0.0)
		return	0;

	if	(!f)
		return	0;

	if	(fstatfs(fileno(f),&vfs) != 0)
		return	0;

	if	(getuid() == 0)
	{ // f_bfree : free blocks for root user
		FsSzUsed	=
		(100.0*(double)(vfs.f_blocks - vfs.f_bfree) / 
					(double)vfs.f_blocks);
	}
	else
	{ // f_bavail : free blocks for standard user
		FsSzUsed	=
		(100.0*(double)(vfs.f_blocks - vfs.f_bavail) / 
					(double)vfs.f_blocks);
	}


#if	0
printf	("fssize=%ld fsfree=%ld fsavail=%ld\n",
				vfs.f_blocks,vfs.f_bfree,vfs.f_bavail);
printf("fsszused=%f [%f..%f]\n",FsSzUsed,FsSzMaxLow,FsSzMaxHigh);
#endif

	change	= NULL;
	switch	(FsSzState)
	{
	case	0:	// nominal state
		if	(FsSzUsed >= FsSzMaxHigh)
		{	// high limit reached	=> block traces
			FsSzState	= 1;
			change		= "block";
		}
	break;
	case	1:	// traces locked
		if	(FsSzUsed <= FsSzMaxLow)
		{	// low limit reached	=> unblock traces
			FsSzState	= 0;
			change		= "unblock";
		}
	break;
	}
	if	(change)
	{
		time_t	gmtTime;
		struct tm locTime;
		time(&gmtTime);
		localtime_r (&gmtTime, &locTime);
fprintf(f,"### %s traces : %04d%02d%02d ", change,
		locTime.tm_year+1900,locTime.tm_mon+1,locTime.tm_mday);
fprintf(f," %02d:%02d:%02d\n",
		locTime.tm_hour,locTime.tm_min,locTime.tm_sec);
fprintf(f,"### pid=%d szfs=%f [%f..%f]\n",getpid(),FsSzUsed,FsSzMaxLow,FsSzMaxHigh);
		fflush(f);
	}

	return	FsSzState;
}

static	int	SzMax(FILE *f)
{
	int	szMax;
	struct stat st;

	szMax	= 1;
	if (f)
	{
		if	( fstat(fileno(f),&st) == 0 )
		{
			if	( st.st_size < SZMAX )
				szMax	= 0;
			else if (g_backup) {
				char new[PATH_MAX], dirn[512], basen[128];
				time_t t = time(0);
				struct tm tm;
				gmtime_r (&t, &tm);

				// dirname and basename may modified the parameter, use a copy
				strcpy(new, TraceFiles);
				strcpy(dirn, dirname(new));
				strcpy(new, TraceFiles);
				strcpy(basen, basename(new));
				sprintf(new, "%s/bkp_%s.%04d%02d%02d_%02d%02d%02d", 
					dirn, basen,
					tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
					tm.tm_hour, tm.tm_min, tm.tm_sec);
				rename(TraceFiles, new);
			}
		}
	}
	return	szMax;
}

int	rtl_tracefilesize(FILE *f)
{
	return SzMax(f);
}

static	int	day_open = -1;

int	rtl_tracedayopen()
{
	return day_open;
}

static	FILE	*HistoLog(char *logFiles)
{
	char	tmp[1024];

	static	FILE* f = NULL;
	static	int	rotatecount = 1;
	time_t	gmtTime;
	struct tm locTime;
	int	day_full = 0;

#define WDAY(d)	(d?d:7)

	if (!TraceRotate || !logFiles)
		return	stderr;

	if (f && FsSzMax(f))
		return	f;

	time(&gmtTime);
//	locTime = *localtime(&gmtTime);
	localtime_r (&gmtTime, &locTime);

	if (f && (day_open == WDAY(locTime.tm_wday)) && SzMax(f))
	{
		day_full = 1;
	}

	if (!f || day_full || (day_open != WDAY(locTime.tm_wday)))
	{
		char *ext;
		char *mode="w+";
		struct stat st;
		struct tm flocTime;
		int append;
#if	0
		if (f)
		{
			fclose(f);
			f = NULL;
		}
#endif
		if (day_open != WDAY(locTime.tm_wday))
			rotatecount	= 1;
		day_open = WDAY(locTime.tm_wday);
		sprintf(tmp, "%s", logFiles);
		ext = strrchr(tmp, '.');
		if (ext)
			*ext = '\0';
		ext = strrchr(logFiles, '.');
		if (!ext)
			ext = ".log";
//		sprintf(tmp, "%s_%02d%s", tmp, day_open, ext);
		sprintf(tmp+strlen(tmp), "_%02d%s", day_open, ext);

		mode = "w+";
		if (day_full)
		{
			unlink(tmp);
			rotatecount++;
		}
		append = 0;
		if (stat(tmp, &st) == 0)
		{
//			flocTime = localtime(&st.st_mtime);
			localtime_r (&st.st_mtime, &flocTime);
			if (locTime.tm_yday == flocTime.tm_yday && locTime.tm_year == flocTime.tm_year)
			{
				mode = "a+";
				append	= 1;
			}
		}
#if	0
		f = fopen(tmp, mode);
		if (!f)
		{
			char	dir[512];
			rtl_dirname(tmp, dir);
			rtl_mkdirp(dir);
			f = fopen(tmp, mode);
		}
#endif
		f = f ? freopen(tmp, mode, f) : fopen(tmp, mode);
		if (!f)
		{
			char	dir[512];
			rtl_dirname(tmp, dir);
			rtl_mkdirp(dir);
			f = f ? freopen(tmp, mode, f) : fopen(tmp, mode);
#ifdef	FD_CLOEXEC
			if	(f)
			{
				int	fd;
				// child process do not inherit trace files fds
				fd	= fileno(f);
				if	(fd >= 0)
				fcntl(fd,F_SETFD,fcntl(fd,F_GETFD)|FD_CLOEXEC);
			}
#endif
		}
		if (day_full)
		{
			ext	= "rotate";
		}
		else
		{
			if (!append)
				ext = "start";
			else
				ext = "restart";
		}
		if (f)
		{
			unlink(logFiles);
			if (link(tmp,logFiles)) {;}	// just avoid warning
		}
		if (f && !SzMax(f))
		{
			char strmax[50] = "unlimited (config files not yet read)";
			if (SZMAX != RTL_TRUNLIMITED) {
				sprintf(strmax, "%d", SZMAX);
            }
fprintf(f,"################################################################\n");
fprintf(f,"### %s(%d) : %04d%02d%02d ", ext,rotatecount,
		locTime.tm_year+1900,locTime.tm_mon+1,locTime.tm_mday);
fprintf(f," %02d:%02d:%02d\n",
		locTime.tm_hour,locTime.tm_min,locTime.tm_sec);
fprintf(f,"### pid=%d szmax=%s\n",getpid(),strmax);
fprintf(f,"################################################################\n");
			fflush(f);
		}
	}
	if	( !f )
		return	stderr;
	return	f;
}

FILE *rtl_tracecurrentfile()
{
	return	HistoLog(TraceFiles);
}

void	rtl_tracerotate(char *logFiles)	/* !TS */
{
	TraceRotate	= 1;
	strcpy	(TraceFiles,logFiles);
	HistoLog(logFiles);
}

void	rtl_vtrace(int t,int withTime,char *fmt, va_list listArg) /* TS */
{
	char	tmp[64];

	EnterCS();
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		FILE	*f;

		f	= HistoLog(TraceFiles);
		if	( FsSzState || SzMax(f) )
		{
			LeaveCS();
			return;
		}
		if	(withTime)
		{
			if	(IsoTimeFormat)
			{
				rtl_getCurrentIso8601date(tmp);
				fprintf (f, "%s ", tmp);
			}
			else
				fprintf (f, "%s", rtl_hhmmssms(tmp));
		}
			
//		fprintf (f, " (%04d) ", rtl_pseudothreadid());
		fprintf (f, " (%05d) ", rtl_gettid());

		vfprintf (f, fmt, listArg);
		fflush	(f) ;
	}
	LeaveCS();
}

void	rtl_tracedbg(int t,char *file,int line,char *fmt,...)	/* TS */
{
	char	tmp[64];

	EnterCS();
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		va_list	listArg;
		FILE	*f;

		f	= HistoLog(TraceFiles);
		if	( FsSzState || SzMax(f) )
		{
			LeaveCS();
			return;
		}
		if	(IsoTimeFormat)
		{
			rtl_getCurrentIso8601date(tmp);
			fprintf (f, "%s ", tmp);
		}
		else
			fprintf (f, "%s", rtl_hhmmssms(tmp));
			
//		fprintf (f, " (%04d) ", rtl_pseudothreadid());
		fprintf (f, " (%05d) ", rtl_gettid());

		if	(file && *file)
		{
			fprintf (f, "[%s:%d] ", file, line);
		}

		va_start(listArg,fmt);
		vfprintf (f, fmt, listArg);
		va_end(listArg);
		fflush	(f) ;
	}
	LeaveCS();
}

static char *_levels[] = { "ERR", "WAR", "  2", "  3", "  4", "INF", "  6", "  7", "  8", "DET", "DBG" };
__thread char _Info[16+1] = "";

void rtl_traceInfoInt64(unsigned long info) {
	sprintf (_Info, "%016lx", info);
}

void rtl_traceInfoInt32(unsigned int info) {
	sprintf (_Info, "%08x", info);
}

void rtl_traceInfoStr(char *info) {
	if	(!info || !*info)
		_Info[0] = 0;
	else {
		strncat (_Info, info, 16);
		_Info[16] = 0;
	}
}

void	rtl_tracedbg2(int t,char *file,int line,char *fmt,...)	/* TS */
{
	EnterCS();
	//if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		va_list	listArg;
		FILE	*f;

		f	= HistoLog(TraceFiles);
		if	( FsSzState || SzMax(f) )
		{
			LeaveCS();
			return;
		}
		char name[64+2];
		char tmp[64];
		char fmtdate[128];
		if	(IsoTimeFormat)
		{
			rtl_getCurrentIso8601date(tmp);
			sprintf (fmtdate, "%s ", tmp);
		}
		else
			sprintf (fmtdate, "%s", rtl_hhmmssms(tmp));
		pthread_getname_np(pthread_self(), name, 32);
		if (*_Info)
			sprintf (name+strlen(name), ":%s", _Info);
		strcat(name, ")");
		fprintf (f, "%s[%s] (%-30.30s ", fmtdate, _levels[t], name);

		if	(file && *file)
			fprintf (f, "[%s:%d] ", file, line);

		va_start(listArg,fmt);
		vfprintf (f, fmt, listArg);
		va_end(listArg);
		fflush	(f) ;
	}
	LeaveCS();
}

void	rtl_trace(int t,char *fmt,...)	/* TS */
{
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		va_list	listArg;

		va_start(listArg,fmt);
		rtl_vtrace (t, 1, fmt, listArg);
		va_end(listArg);
	}
}


void	rtl_tracenotime(int t,char *fmt,...)	/* TS */
{
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		va_list	listArg;

		va_start(listArg,fmt);
		rtl_vtrace (t, 0, fmt, listArg);
		va_end(listArg);
	}
}
