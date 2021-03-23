
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

#ifndef	RTL_BASE_H
#define	RTL_BASE_H

#include	<unistd.h>
#include	<time.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<sys/timeb.h>
#include	<limits.h>


#define	RTL_WITH_XLAP	// xlap available
#define	RTL_WITH_EPOLL	// epoll available
#define	RTL_WITH_ELAP	// elap available (xlap+epoll)

#define	RTL_STRERRNO	strerror(errno)

/*
*/

#define RTL_TRUNLIMITED INT_MAX
#define RTL_TRDEFAULT   1*1000*1000

#define	RTL_TRDBG(lev,...) \
{\
	extern int TraceDebug;\
	extern int TraceLevel;\
	if (TraceLevel >= (lev))\
	{\
		rtl_tracedbg((lev),TraceDebug <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
	}\
}

#define	RTL_TRMOD(lev,...) \
{\
	extern int TraceDebug;\
	extern int TraceLevel;\
	if (TraceLevel >= (lev))\
	{\
		rtl_tracedbg((lev),TraceDebug <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
	}\
	else\
	if (TraceDebug && TraceDebug == TraceModule)\
	{\
		int t;\
		t = -100 - (lev);\
		rtl_tracedbg(-100,1 <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
	}\
}



#ifndef	RTL_BASE_TYPE
#define	RTL_BASE_TYPE
#define	int32	int
#define	uint32	unsigned int
#endif

#ifndef	RTL_LIST_HEAD_STRUCT
#define	RTL_LIST_HEAD_STRUCT
struct list_head {
	struct list_head *next, *prev;
};
#endif

int	rtl_init();
char	*rtl_version();

void	*rtl_openDir(char *path);
char	*rtl_readDir(void *dir);
char	*rtl_readAbsDir(void *dir);
void	rtl_closeDir(void *dir);
int	rtl_mkdirp(char *path);
char	*rtl_basename(char *fullName);
char	*rtl_suffixname(char *fullName);
char	*rtl_dirname(char *fullName,char *pathName);
void	rtl_checkPathName(char *path);
void	rtl_catPathNames(char *left,char *right);
void	rtl_absToRelPath(char *path,char *pathRef,char *pathRet);

void	*rtl_pollInit();
int	rtl_pollAdd(void *tbl,int fd,
		int (*fevt)(void *,int,void *,void *,int),
		int (*freq)(void *,int,void *,void *,int),
		void *ref,void *ref2);
int	rtl_pollSetEvt(void *tbl, int fd,int mode);
int	rtl_pollSetEvt2(void *tbl, int pos,int mode);
int	rtl_pollGetEvt(void *tbl, int fd);
int	rtl_pollGetEvt2(void *ptbl, int pos);
int	rtl_pollGetMode(void *tbl, int fd);
int	rtl_pollGetMode2(void *ptbl, int pos);
int	rtl_pollRmv(void *tbl, int fd);
int	rtl_poll(void *tbl, int timeout);
int	rtl_pollRaw(void *ptbl, int timeout);


pid_t	rtl_gettid();
unsigned long rtl_vsize(int pid);
unsigned int rtl_cpuloadavg();
int	rtl_pseudothreadid();
void	rtl_enablecore(int flg);
void	rtl_docore();
char	*rtl_aaaammjj(time_t t,char *dst);
char	*rtl_hhmmss(time_t t,char *dst);
char	*rtl_aaaammjjhhmmss(time_t t,char *dst);
char	*rtl_hhmmssms(char *dst);

int	rtl_tracemutex();
void	rtl_traceunlock(void);
void	rtl_tracelevel(int level);
void	rtl_tracesizemax(int sz);
void	rtl_tracecommitsizemax();
int	rtl_tracefilesize(FILE *f);
FILE	*rtl_tracecurrentfile();
void	rtl_tracerotate(char *logFiles);
void	rtl_trace(int t,char *fmt,...);
void	rtl_tracenotime(int t,char *fmt,...);
void	rtl_tracedbg(int t,char *file,int line,char *fmt,...);
void	rtl_tracedbg2(int t,char *file,int line,char *fmt,...);
void rtl_traceInfoInt64(unsigned long info);
void rtl_traceInfoInt32(unsigned int info);
void rtl_traceInfoStr(char *info);
int	rtl_tracedayopen();
void	rtl_traceBackup(int flag);
void	rtl_tracefslimits(double low,double high);
double	rtl_tracefsperused();
void	rtl_traceIsoTimeFormat(int f);



void	rtl_timespec(struct timespec *tv);
time_t	rtl_time(time_t *part);			// == time(2)
time_t	rtl_tmms();
void	rtl_timebase(struct timespec *tv);
void	rtl_timebaseVal(time_t *sec,time_t *ns);
#if	0
time_t	rtl_timems(time_t *base,time_t *etms,time_t *dtms);
time_t	rtl_timecs(time_t *base,time_t *etcs,time_t *dtcs);
#endif

void	rtl_timespecmono(struct timespec *tv);
time_t	rtl_timemono(time_t *part);
time_t	rtl_tmmsmono();
void	rtl_timebasemono(struct timespec *tv);
void	rtl_timebasemonoVal(time_t *sec,time_t *ns);
int	rtl_timespeccmp(struct timespec *tv1, struct timespec *tv2);
void	rtl_timespecaddms(struct timespec *tv,int ms);

#define	RTL_TIMESPECMP(t1,t2)\
	((t1)->tv_sec > (t2)->tv_sec ? +1000000000 :\
	((t1)->tv_sec < (t2)->tv_sec ? -1000000000 :\
			(t1)->tv_nsec - (t2)->tv_nsec))


char *rtl_binToStr(unsigned char *bin,int lg,char dst[],int max);
unsigned char	*rtl_strToBin(char *str,unsigned char bin[],int *lg);

char	*rtl_evalPath(char *in,char *out,int lgmax,char *(*fctvar)(char *v));
char	*rtl_evalPath2(char *in,char *out,int lgmax,char *(*fctvar)(void *user,char *v),void *user);


long rtl_base64_encode (char *to, const char *from, unsigned int len);
long rtl_base64_decode (char *to, const char *from, unsigned int len);



int rtl_ftime (struct timeb *timebuf);
void rtl_gettimeofday_to_iso8601date(struct timeval *tv, struct timezone *tz, char *buf);
void rtl_getCurrentIso8601date(char *buf);
time_t rtl_iso8601_to_Unix(char *iso, int convertUTC);
int rtl_iso8601ToTv(char *iso, struct timeval *tv, int convertUTC);
void rtl_nanoToIso8601(time_t sec, unsigned int nsec, char *buf);
void rtl_nanoToIso8601Local(time_t sec, unsigned int nsec, char *buf);


/* Parse given INI-style file. May have [section]s, name=value pairs
   (whitespace stripped), and comments starting with ';' (semicolon). Section
   is "" if name=value pair parsed before any section heading. name:value
   pairs are also supported as a concession to Python's ConfigParser.

   For each name=value pair parsed, call handler function with given user
   pointer as well as section, name, and value (data only valid for duration
   of handler call). Handler should return nonzero on success, zero on error.

   Returns 0 on success, line number of first error on parse error (doesn't
   stop on first error), -1 on file open error, or -2 on memory allocation
   error (only when INI_USE_STACK is zero).
*/
int rtl_iniParse(const char* filename,
              int (*handler)(void* user, const char* section,
                             const char* name, const char* value),
              void* user);

/* Same as ini_parse(), but takes a FILE* instead of filename. This doesn't
   close the file when it's finished -- the caller must do that. */
int rtl_iniParseFile(FILE* file,
                   int (*handler)(void* user, const char* section,
                                  const char* name, const char* value),
                   void* user);

char *rtl_iniStr(void *ht,char *sec,int index,char *var,char *def);
int rtl_iniInt(void *ht,char *sec,int index,char *var,int def);
int rtl_iniLoadCB(void *user,const char *section,const char *name,const char *value);
void rtl_gmtToIso8601(char *buf);


void	rtl_spin_lock_gcc(int *lock);
void	rtl_spin_unlock_gcc(int *lock);

int	rtl_addrOverlap(const char *a,const char *b,size_t sz);
void	*rtl_memcpy(void *dest,const void *src,size_t n);
unsigned int rtl_jenkins_hash(const unsigned char* key, size_t length);
u_int rtl_vmstatus(int pid,u_int *data,u_int *stk,u_int *exe,u_int *lib);

#endif
