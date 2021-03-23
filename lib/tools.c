
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

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include 	<time.h>
#include 	<ctype.h>
#include 	<sys/timeb.h>
#include 	<sys/stat.h>

#include	<sys/time.h>
#include	<sys/resource.h>
#include	<unistd.h>
#include	<pthread.h>

#include	<sys/types.h>
#include	<sys/syscall.h>

#define		ABS(x)	((x) > 0 ? (x) : -(x))

// has ftime() is deprecated rewrite it with gettimeofday()
// to avoid warnings about uses of ftime()
int rtl_ftime (struct timeb *timebuf)
{
	struct timeval tv;
	struct timezone tz;

	if (gettimeofday (&tv, &tz) < 0)
		return -1;
	timebuf->time = tv.tv_sec;
	timebuf->millitm = (tv.tv_usec + 500) / 1000;
	if (timebuf->millitm == 1000)
	{
		++timebuf->time;
		timebuf->millitm = 0;
	}
	timebuf->timezone = tz.tz_minuteswest;
	timebuf->dstflag = tz.tz_dsttime;
	return 0;
}

u_int rtl_vmstatus(int pid,u_int *data,u_int *stk,u_int *exe,u_int *lib)
{
	char	buff[512];
	char	path[512];
	size_t	len;
	FILE	*f;
	char	*line;

	u_int	vmSize	= 0;
	int	nb = 1;

	if	(data)	{ *data = 0; nb++; }
	if	(stk)	{ *stk = 0; nb++; }
	if	(exe)	{ *exe = 0; nb++; }
	if	(lib)	{ *lib = 0; nb++; }
		

	if	(pid == 0)
		sprintf	(path,"/proc/self/status");
	else
		sprintf	(path,"/proc/%d/status",pid);

	f	= fopen(path,"r");
	if	(!f)
		return	0;

	line	= &buff[0];
	while	(nb > 0 && getline(&line,&len,f) != -1)
	{
		if	(!strncmp(line,"VmSize:",7))
		{
			vmSize	= (u_int)strtoul(line+7,NULL,10);
			nb--;
		} else
		if	(!strncmp(line,"VmData:",7))
		{
			if	(data)
			{
				*data	= (u_int)strtoul(line+7,NULL,10);
				nb--;
			}
		} else
		if	(!strncmp(line,"VmStk:",6))
		{
			if	(stk)
			{
				*stk	= (u_int)strtoul(line+6,NULL,10);
				nb--;
			}
		} else
		if	(!strncmp(line,"VmExe:",6))
		{
			if	(exe)
			{
				*exe	= (u_int)strtoul(line+6,NULL,10);
				nb--;
			}
		} else
		if	(!strncmp(line,"VmLib:",6))
		{
			if	(lib)
			{
				*lib	= (u_int)strtoul(line+6,NULL,10);
				nb--;
			}
		}
	}

	fclose(f);

	return	vmSize;
}

typedef	struct
{
	unsigned long size,resident,share,text,lib,data,dt;
}	t_statm;

unsigned long rtl_vsize(int pid)
{
	t_statm	result;
	char	path[512];
	FILE	*f;

	if	(pid == 0)
		sprintf	(path,"/proc/self/statm");
	else
		sprintf	(path,"/proc/%d/statm",pid);

	f	= fopen(path,"r");
	if	(!f)
		return	0;

	if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld",
		&result.size,&result.resident,&result.share,
		&result.text,&result.lib,&result.data,&result.dt))
	{
		fclose(f);
		return	0;
	}

	fclose(f);
	return	(result.size*1024*4);	// nbre de pages de 4Ko
}

unsigned int rtl_cpuloadavg()
{
	char	path[512];
	FILE	*f;
	float	result;

	sprintf	(path,"/proc/loadavg");
	f	= fopen(path,"r");
	if	(!f)
		return	0;

	if(1 != fscanf(f,"%f",&result))
	{
		fclose(f);
		return	0;
	}

	fclose(f);
	return	(unsigned long)(result*100);
}

int	rtl_pseudothreadid()	/* TS */
{
	return	(unsigned int) ((unsigned long)pthread_self()%9973);
}

void	rtl_enablecore(int flg)	/* TS */
{
	struct	rlimit	lim;

	if	( flg )
	{
		lim.rlim_cur	= RLIM_INFINITY;
		lim.rlim_max	= RLIM_INFINITY;
	}
	else
	{
		lim.rlim_cur	= 0;
		lim.rlim_max	= 0;
	}
	setrlimit(RLIMIT_CORE,&lim);
}

int	rtl_docore()
{
	int	ret;
	// TODO
	// fork() + abort()


	ret	= fork();
	if	(ret < 0)	// parent
		return	ret;	// error
	if	(ret > 0)	// parent
		return	ret;	// pid of child

	rtl_enablecore(1);
	// child
	abort();	
}


char	*rtl_aaaammjj(time_t t,char *buf)	/* TS */
{
	struct	timeb	tp ;
	struct	tm	stdt;
	struct	tm	*dt;

	if (t == 0)
	{
		rtl_ftime	(&tp) ;
		dt	= localtime_r(&tp.time,&stdt);
	}
	else
		dt	= localtime_r(&t,&stdt);
	sprintf(buf,"%04d%02d%02d", dt->tm_year+1900,dt->tm_mon+1,dt->tm_mday);

	return	buf ;
}

char	*rtl_hhmmss(time_t t,char *buf)		/* TS */
{
	struct	timeb	tp ;
	struct	tm	stdt;
	struct	tm	*dt;

	if (t == 0)
	{
		rtl_ftime	(&tp) ;
		dt	= localtime_r(&tp.time,&stdt);
	}
	else
		dt	= localtime_r(&t,&stdt);
	sprintf(buf,"%02d:%02d:%02d",dt->tm_hour,dt->tm_min,dt->tm_sec);

	return	buf ;
}

char	*rtl_aaaammjjhhmmss(time_t t,char *buf)	/* TS */
{
	struct	timeb	tp ;
	struct	tm	stdt;
	struct	tm	*dt;

	if (t == 0)
	{
		rtl_ftime	(&tp) ;
		dt	= localtime_r(&tp.time,&stdt);
	}
	else
		dt	= localtime_r(&t,&stdt);
	sprintf(buf,"%04d/%02d/%02d %02d:%02d:%02d",
		dt->tm_year+1900,dt->tm_mon+1,dt->tm_mday,
		dt->tm_hour,dt->tm_min,dt->tm_sec);

	return	buf ;
}

char	*rtl_hhmmssms(char *buf)	/* TS */
{
	struct	timeb	tp ;
	struct	tm	stdt;
	struct	tm	*dt;

	memset	(&tp,0,sizeof(tp));
	rtl_ftime	(&tp) ;
	dt	= localtime_r(&tp.time,&stdt);
	sprintf(buf,"%02d:%02d:%02d.%03d ",
		dt->tm_hour,dt->tm_min,dt->tm_sec,tp.millitm);

	return	buf ;
}

char *rtl_binToStr(unsigned char *bin,int lg,char dst[],int max)	/* TS */
{
	int	i;
	int	j = 0;

	static	char	*tb	= "0123456789abcdef";

	for (i = 0 ; i < lg && j < max ; i++)
	{
		dst[j++]	= tb[(unsigned char)(bin[i]/16)];
		dst[j++]	= tb[(unsigned char)(bin[i]%16)];
//printf("%c %c\n",dst[j-2],dst[j-1]);
	}
	dst[j++]	= '\0';
	return	dst;
}

unsigned char	*rtl_strToBin(char *str,unsigned char bin[],int *lg)	/* TS */
{
	int	max = *lg;
	int	i = 0;

	*lg	= 0;
	while (str && *str && i < max)
	{
		int	x;
		char	tmp[3];

		if (!isxdigit(*str))	{ str++ ; continue; }

		tmp[0]	= *str++;
		if (!*str)	break;
		tmp[1]	= *str++;
		tmp[2]	= '\0';
		if (sscanf(tmp,"%x",&x) == 1)
		{
			bin[i]	= (unsigned char)x;
			i++;
		}
		else
			break;
	}
	*lg	= i;
#if	0
printf("[");
for (i = 0 ; i < *lg ; i++)
	printf("%02x ",bin[i]);
printf("]\n");
#endif
	return	bin;
}

pid_t	rtl_gettid()
{
	static	__thread pid_t tid = 0;
	if	(tid == 0)
	{
		tid	= (pid_t)syscall(SYS_gettid);
	}
	return	tid;
}

int	rtl_addrOverlap(const char *a,const char *b,size_t sz)
{
	return	(a <= b && a+sz > b) || (b <= a && b+sz > a);
}

void	*rtl_memcpy(void *dest,const void *src,size_t n)
{
	if	(rtl_addrOverlap((const char *)dest,(const char *)src,n))
		return	memmove(dest,src,n);

	return	memcpy(dest,src,n);
}

unsigned int rtl_jenkins_hash(const unsigned char* key, size_t length) 
{
	size_t i = 0;
	unsigned int hash = 0;
	while (i != length) 
	{
		hash += key[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

#ifdef	rtl_tools_MAIN
int	main(int argc,char *argv[])
{
	u_int	vmsize;
	u_int	vmdata;
	u_int	vmstk;
	u_int	vmexe;
	u_int	vmlib;
	vmsize	= rtl_vmstatus(0,&vmdata,&vmstk,&vmexe,&vmlib);

	printf	("VmSize: %u\n",vmsize);
	printf	("VmData: %u\n",vmdata);
	printf	("VmStk: %u\n",vmstk);
	printf	("VmExe: %u\n",vmexe);
	printf	("VmLib: %u\n",vmlib);
	printf	("\n");

	printf	("Check with:\n");
	char	cmd[512];
	sprintf	(cmd,"cat /proc/%d/status | grep Vm",getpid());
	(void)system(cmd);
}
#endif
