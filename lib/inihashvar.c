
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
#include 	<sys/timeb.h>
#include 	<sys/stat.h>

#include	<sys/time.h>
#include	<sys/resource.h>
#include	<unistd.h>
#include	<pthread.h>

#include	"rtlbase.h"
#include	"rtlhtbl.h"

char	*rtl_iniStr(void *ht,char *sec,int index,char *var,char *def)
{
	char	path[256];
	char	tmp[256];
	char	*pt;
	char	*val;

	pt	= path;
	path[0]	= '\0';
	if	(sec && *sec)
		strcat(path,sec);	
	if	(index >= 0)
	{
		sprintf	(tmp,":%d",index);
		strcat(path,tmp);	
	}
	if	(strlen(path))
		strcat(path,".");
	strcat(path,var);	
	if	(*pt == '.')
		pt++;

	val	= rtl_htblGet(ht,pt);
	if	(!val)
		return	def;
	return	val;
}

int	rtl_iniInt(void *ht,char *sec,int index,char *var,int def)
{
	char	more[256];
	char	*val;
	int	ret	= 0;
	int	hex	= 0;

	val	= rtl_iniStr(ht,sec,index,var,NULL);
	if	(!val || !*val)
		return	def;

retry:
	if	(*val == '0' && (*(val+1) == 'x' || *(val+1) == 'X'))
	{
		more[0]	= '\0';
		sscanf(val+2,"%x%s",&hex,more);
//RTL_TRDBG(1,"'%s:%d.%s' 0x%x\n",sec,index,var,hex);
		ret	= ret | hex;
		if	(strlen(more))
		{
			val	= more;
			if	(*val == '|')
			{
				val++;
				goto	retry;
			}
			return	ret;
		}
		return	ret;
	}
	ret	= atoi(val);
	if	(ret == 0)
	{
		if	(strcasecmp(val,"true") == 0)
			return	1;
		if	(strcasecmp(val,"false") == 0)
			return	0;
	}
	return	ret;
}

static	char *CfgCBIniFctVar(void *user,char *var)
{
	char	*val;
	val	= rtl_htblGet(user,var);
	if	(!val)
		val	= (char *)getenv(var);
	if	(!val)
		val	= NULL;
	return	val;
}

int rtl_iniLoadCB(void *user,const char *section,const char *name,const char *value)
{
	char	bvar[256];
	char	buf[256];
	char	*var;
	char	*nvalue;


	nvalue	= (char *)value;
	sprintf	(bvar,"%s.%s",section,name);
	var	= bvar;
	if	(*var == '.')	// because section can be empty
		var++;

	if	(!nvalue||!*nvalue)
	{
		nvalue	= "";
	}

//	RTL_TRDBG(9,"var='%s' val='%s'\n",var,nvalue);
	if	(nvalue && *nvalue && *nvalue == '$')
	{

		nvalue	= rtl_evalPath2(nvalue,buf,sizeof(buf)-1,
							CfgCBIniFctVar,user);
	}
	rtl_htblRemove(user,var);
	rtl_htblInsert(user,var,(void *)strdup(nvalue));
	return	1;
}
