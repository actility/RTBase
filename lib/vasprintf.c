
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
#include <stdarg.h>
#include <string.h>

/*
 *	This is a dynamic sprintf, allocating and reallocating buffer on demand
 * size = 0 means no limitation
 */
int rtl_vasnprintf(char **buf, size_t size, const char *format, va_list arg) {
	char _tmp[4096];
	int rv = vsnprintf(_tmp, 4096, format, arg);
	if (rv < 0) return -1;

	int szbuf = *buf ? strlen(*buf) : 0;

	// Size limitation
	if	((size > 0) && (rv > size - szbuf))
		rv	= size - szbuf;

	if	(*buf) {
		*buf	= realloc(*buf, szbuf+rv+10);
	}
	else {
		*buf	= malloc(rv+10);
		**buf = 0;
	}

	if	(!*buf)
		return -1;
	strcat (*buf+szbuf, _tmp);
	return rv;
}

int rtl_asnprintf(char **buf, size_t size, const char *format, ...) {
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = rtl_vasnprintf(buf, size, format, arg);
	va_end(arg);
	return rv;
}

int rtl_asprintf(char **buf, const char *format, ...) {
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = rtl_vasnprintf(buf, 0, format, arg);
	va_end(arg);
	return rv;
}

int rtl_vasprintf(char **buf, const char *format, va_list arg) {
	return rtl_vasnprintf (buf, 0, format, arg);
}
