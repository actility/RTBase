
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
 *	Remplacement des sequences ${...}
 */

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

static	char	*rtl_getvar2(void *user,char *name)
{
	char	*val ;

	if	( !name || !*name )
		return	"" ;
	val	= (char *)getenv(name) ;
	if	( !val )
		return	"" ;
	return	val ;
}

/*
 *	cherche si *c est egal a un des car de lst
 */

static	char	*rtl_isIn(char *c,char *lst)
{
	while	(lst && *lst)
	{
		if	( *c == *lst )
			return	c;
		lst++;
	}
	return	NULL;
}

/*
 *	cherche dans *in le 1er car. qui est egal a un des car. de *end
 */

static	char	*rtl_skipto(char *in,char *end)
{
	char	*pt ;
	char	*pos ;

	pt	= in;
	while	(*pt)
	{
		pos	= rtl_isIn(pt,end) ;
		if	( pos )
		{
			return	pos ;
		}
		pt++ ;
	}
	return	strchr(in,0) ;
}


/*
 *	copie la chaine <in> dans la chaine <out> en evaluant les
 *	variables $XXXX par getenv()
 *
 *	<in> ne peut etre une variable static
 *
 *	les formes reconnues sont :
 *		${NOM_VAR}
 *		$(NOM_VAR)
 *
 *	si fctvar(void *user,char *name) != NULL il est possible de specifier
 *	une fonction d'evaluation de variables , qui doit retourner
 *	le contenu de la variable name ou la chaine vide .
 */

char	*rtl_evalPath2( char *in, char *out, int lgmax, char *(*fctvar)(void *user,char *v),void *user)
{
	int	lg	= 0 ;
	char	*svout	= out ;
	char	*ptvar ;
	char	*ptin ;
	char	*ptskip = NULL ;
	char	*value ;
	int	lgvalue ;
	char	varname[128];
	int	l;

	if	(!out)
		return	NULL;

	if	( fctvar == NULL )
		fctvar	= rtl_getvar2 ;
	*out	= '\0' ;

	while	( *in && lg < lgmax )
	{
		if	( *in == '$' && (*(in+1) == '{' || *(in+1) == ')'))
		{	/* une variable	*/
			in++ ;
			switch	(*in)
			{
			case	'{' : ptskip	= "}" ; in++ ;
			break ;
			case	'(' : ptskip	= ")" ; in++ ;
			break ;
			}
			ptvar	= varname;
			ptin	= rtl_skipto(in,ptskip) ;
			if	(!ptin || !*ptin)
				break;
			l	= (ptin - in)+1;
			if	(l <= 0 || l >= sizeof(varname))
				break;
			strncpy	(varname,in,l);
			varname[l-1]	= '\0';
			value	= (*fctvar)(user,ptvar) ;
			if	( !value || !*value )
				value	= "" ;
//printf("varname='%s' => val='%s'\n",varname,value);

			lgvalue	= strlen(value) ;
			if	( lgvalue > 0 )
			{
				if	( lg + lgvalue >= lgmax )
				{
					lgvalue	= lgmax - lg ;
				}
				memcpy	(out,value,lgvalue) ;
				out	+= lgvalue ;
				lg	+= lgvalue ;
			}
			if	( *ptin )
			{
				if	( *ptin == '$' )
				{
					in	= ptin ;
				}
				else
				{
					in	= ptin+1 ;
				}
			}
			else
				break ;
		}
		else
		{	/* caractere normal a recopier */
			*out	= *in ;
			in++ ;
			out++ ;
			lg++ ;
		}
	}
	*out	= '\0' ;
	return	svout ;
}


