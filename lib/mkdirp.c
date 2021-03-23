
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
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static int	_rtl_mkdir_(char *deb,char *fin)	/* TS */
{
	char	str[512];
	int	nb;

	strcpy(str,deb);
	str[fin-deb]=0;		// TODO ...
	if(mkdir(str,0777)<0)
	{
		if(errno==EEXIST)
		{
			return 0;
		}
		if(errno!=ENOENT)
		{
			return -1;
		}
		for(fin--;fin>deb && *fin!='/';fin--);
		if(fin<=deb)
		{
			return -1;
		}
		if((nb=_rtl_mkdir_(deb,fin))<0)
		{
			return -1;
		}
		if(mkdir(str,0777)<0)
		{
			if(errno==EEXIST)
				return	0;
			return -1;
		}
		return nb+1;
	}
	return 1;
}

/*
 *
 *	Cette fonction cree un repertoire et tous les repertoires
 *	intermediaires, si possible.
 *	
 *	Le nombre de repertoire crees est retourne, ou '-1' si une
 *	erreur survient. Dans ce cas, 'errno' contient le code
 *	d'erreur (voir fonction systeme mkdir()).
 *
 */

int	rtl_mkdirp(char *path)	/* TS */
{
	return _rtl_mkdir_(path,path+strlen(path));
}


