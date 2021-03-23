
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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>



/**
 ** @brief Formats an ISO time
 ** @param A time_t (Unix time)
 ** @param The output buffer
 ** @return void
 **/
void rtl_gettimeofday_to_iso8601date(struct timeval *tv, struct timezone *tz, char *buf) {
	struct tm sttm;
        struct tm *tm = localtime_r(&tv->tv_sec,&sttm);
        int h, m;
        h = tm->tm_gmtoff/3600;
        m = (tm->tm_gmtoff - h*3600) / 60;
        char sign = (h<0) ? '-':'+';
        sprintf (buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03ld%c%02d:%02d",
                tm->tm_year + 1900, tm->tm_mon + 1,
                tm->tm_mday, tm->tm_hour, tm->tm_min, 
		tm->tm_sec, tv->tv_usec / 1000,
                sign, abs(h), abs(m));
}

void rtl_getCurrentIso8601date(char *buf) {
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        rtl_gettimeofday_to_iso8601date(&tv, &tz, buf);
}

char *findSign(char *pt) {
	while (*pt) {
		if (*pt == '+')
			return pt;
		if (*pt == '-')
			return pt;
		pt++;
	}
	return NULL;
}

static char *__ok = "0123456789-:TZ.+";

static bool isBadChar(char *iso) {
	while	(*iso) {
		if	(!strchr(__ok, *iso))
			return true;
		iso ++;
	}
	return false;
}

// 2016-08-26T08:55:47
// 2016-01-11T14:28:00+02:00
time_t rtl_iso8601_to_Unix(char *iso, int convertUTC) {
	struct tm tm;

	if	(!iso || (strlen(iso) < 19) || (strlen(iso) > 32) || isBadChar(iso))
		return 0;
	if	((iso[4] != '-') || (iso[7] != '-') || (iso[10] != 'T') || (iso[13] != ':') || (iso[16] != ':'))
		return 0;

	memset (&tm, 0, sizeof(tm));

	tm.tm_year = (iso[0]-'0')*1000 + (iso[1]-'0')*100 + (iso[2]-'0') * 10 + (iso[3]-'0') - 1900;
	tm.tm_mon = (iso[5]-'0')*10 + (iso[6]-'0') - 1;
	tm.tm_mday = (iso[8]-'0')*10 + (iso[9]-'0');
	tm.tm_isdst = -1;
	tm.tm_hour = (iso[11]-'0')*10 + (iso[12]-'0');
	tm.tm_min = (iso[14]-'0')*10 + (iso[15]-'0');

	// Seconds are decimal
	char tmp[7];
	strncpy (tmp, iso+17, 6);
	tmp[6] = 0;
	double seconds = atof(tmp);
	tm.tm_sec	= (int)seconds;

	/* determine number of seconds since the epoch, i.e. Jan 1st 1970 */
	time_t epoch_seconds = timegm(&tm);
	if	(convertUTC) {
		char *sign = findSign(iso+19);
		if	(sign) {
			char *hours = sign+1;
			char *minutes = strchr(sign+1, ':') ? sign+4:sign+3;	// base or extended format
			int offset = ((hours[0]-'0')*10 + (hours[1]-'0')) * 3600;
			offset	+= ((minutes[0]-'0')*10 + (minutes[1]-'0')) * 60;
			if	(*sign == '+')
				epoch_seconds	-= offset;
			else
				epoch_seconds	+= offset;
		}
	}
	return epoch_seconds;
}

int rtl_iso8601ToTv(char *iso, struct timeval *tv, int convertUTC) {
	struct tm tm;

	if	(!iso || (strlen(iso) < 17) || (strlen(iso) > 32) || isBadChar(iso))
		return -1;

	memset (&tm, 0, sizeof(tm));

	tm.tm_year = (iso[0]-'0')*1000 + (iso[1]-'0')*100 + (iso[2]-'0') * 10 + (iso[3]-'0') - 1900;
	tm.tm_mon = (iso[5]-'0')*10 + (iso[6]-'0') - 1;
	tm.tm_mday = (iso[8]-'0')*10 + (iso[9]-'0');
	tm.tm_isdst = -1;
	tm.tm_hour = (iso[11]-'0')*10 + (iso[12]-'0');
	tm.tm_min = (iso[14]-'0')*10 + (iso[15]-'0');

	// Seconds are decimal
	char tmp[7];
	memcpy (tmp, iso+17, 6);
	tmp[6] = 0;
	double seconds = atof(tmp);
	tm.tm_sec	= (int)seconds;
	int usec = atoi(tmp+3);

	/* determine number of seconds since the epoch, i.e. Jan 1st 1970 */
	time_t epoch_seconds = timegm(&tm);

	if	(convertUTC) {
		char *sign = findSign(iso+19);
		if	(sign) {
			int offset;
			offset	= ((sign[1]-'0')*10 + (sign[2]-'0')) * 3600;
			offset	+= ((sign[4]-'0')*10 + (sign[5]-'0')) * 60;
			if	(*sign == '+')
				epoch_seconds	-= offset;
			else
				epoch_seconds	+= offset;
		}
	}
	tv->tv_sec	= epoch_seconds;
	tv->tv_usec = usec;
	return 0;
}

void rtl_gmtToIso8601(char *buf) {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	struct tm *tm = gmtime(&tv.tv_sec);
	sprintf (buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
		tm->tm_year + 1900, tm->tm_mon + 1,
		tm->tm_mday, tm->tm_hour, tm->tm_min,
		tm->tm_sec, tv.tv_usec / 1000);
}

void rtl_nanoToIso8601(time_t sec, uint32_t nsec, char *buf) {
	struct tm *tm = gmtime(&sec);
	sprintf (buf, "%04d-%02d-%02dT%02d:%02d:%02d.%09uZ",
		tm->tm_year + 1900, tm->tm_mon + 1,
		tm->tm_mday, tm->tm_hour, tm->tm_min,
		tm->tm_sec, nsec);
}

void rtl_nanoToIso8601Local(time_t sec, uint32_t nsec, char *buf) {
	struct tm sttm;
	struct tm *tm = localtime_r(&sec,&sttm);
	int h, m;
	h = tm->tm_gmtoff/3600;
	m = (tm->tm_gmtoff - h*3600) / 60;
	char sign = (h<0) ? '-':'+';
	sprintf (buf, "%04d-%02d-%02dT%02d:%02d:%02d.%09u%c%02d:%02d",
		tm->tm_year + 1900, tm->tm_mon + 1,
		tm->tm_mday, tm->tm_hour, tm->tm_min,
		tm->tm_sec, nsec,
		sign, abs(h), abs(m));
}
