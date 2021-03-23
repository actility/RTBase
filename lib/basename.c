
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
 *	Manipulation des noms de fichier
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#undef	WIN32


#define	ISSEP(c)		(((c)=='/')||((c)=='\\'))

#ifdef WIN32
#define	PATHSEPARATOR		'\\'
#define PATHITEMCMP(c1,c2)	((ISSEP(c1)&&(ISSEP(c2)))?(1):(toupper(c1)==toupper(c2)))
#else
#define	PATHSEPARATOR		'/'
#define PATHITEMCMP(c1,c2)	((ISSEP(c1)&&(ISSEP(c2)))?(1):((c1)==(c2)))
#endif

/*
 *
 *	Retourne le dernier element d'un chemin
 *
 */

char	*rtl_basename(char *fullName)	/* TS */
{
	register char 	*pt;

	for(pt=fullName;*pt;pt++)
	{
		if(ISSEP(*pt))
		{
			fullName=pt+1;
		}
	}
	return fullName;
}

/*
 *
 *	Retourne le dernier suffixe d'un nom de fichier
 *
 */

char	*rtl_suffixname(char *fullName)	/* TS */
{
	register char *old=0L;

	while(*fullName)
	{
		if(ISSEP(*fullName))
		{
			old=NULL;
		}
		if(*fullName=='.')
		{
			old=fullName;
		}
		fullName++;
	}
	return old;
}

/*
 *
 *	Retourne le chemin d'acces d'un fichier
 *
 */

char	*rtl_dirname(char *fullName,char *pathName)	/* TS */
{
	register char 	*pt,*max,*buf;

	max	= rtl_basename(fullName)-1;
	buf	= pathName;
	for(pt=fullName;pt<max;pt++)
	{
		*buf++ = *pt;
	}
	*buf = 0;
	if(ISSEP(*fullName) && (*pathName)==0)
	{
		*pathName   = PATHSEPARATOR;
		pathName[1] = 0;
	}
	return pathName;
}

/*
 *
 *	Adapte un nom de fichier au format de l'OS cible
 *
 *	Le path en entree peut etre constitue de '/' et de '\'
 *	En sortie, le path est constitue de '/' sous unix, 
 *	et de '\' sous DOS
 *
 *	ex:	C:\ROOT/tools/xsb
 *		-> C:/ROOT/tools/xsb sous UNIX
 *		-> C:\ROOT\tools\xsb sous DOS
 *
 */

void	rtl_checkPathName(char *path)	/* TS */
{
	while(*path)
	{
		if(!ISSEP(*path))
		{
			path++;
			continue;
		}
		*path++=PATHSEPARATOR;
	}
}

/*
 *
 *	Concatenation de deux morceaux de path
 *
 *
 */

void	rtl_catPathNames(char *left,char *right)	/* TS */
{
	char	*pt;

	if(*left)
	{
		pt	= left+strlen(left)-1;
		if(!ISSEP(*pt))
		{
			pt[1]=PATHSEPARATOR;
			pt[2]=0;
		}
	}
	strcat(left,right);
}

/*
 *
 *	Calcul un chemin relatif pour passer d'un chemin a un autre
 *
 *	path		chemin a acceder
 *	pathRef		chemin depuis lequel on veut acceder a 'path'
 *	pathRet		chemin relatif en retour
 *
 */


void rtl_absToRelPath(char *path,char *pathRef,char *pathRet)	/* TS */
{
	char	*pt,*ref;
	char	*newPt,*newRef="";
	int	commun;
	static	char	separator[1+1] = { PATHSEPARATOR, '\0' };


	*pathRet=0;
	/* Recherche de la partie commune */
	commun=0;
	newPt=NULL;
	for(pt=path,ref=pathRef;*pt && PATHITEMCMP(*pt,*ref);pt++,ref++)
	{
		commun++;
		if(ISSEP(*pt))
		{
			newPt=pt+1;
			newRef=ref+1;
		}
	}
	if(!commun)
	{
		strcpy(pathRet,path);
		return;
	}
	if(commun==(int)strlen(path) && commun==(int)strlen(pathRef))
	{
		strcpy(pathRet,".");
		return;
	}
	if(!newPt)
	{
		return;
	}

	/* Calcul du chemin relatif */

	/* on remonte dans la partie diff jusqu'a la partie commune	*/
// printf("newPt=%s newRef=%s\n",newPt,newRef);
	*pathRet=0;
	for(pt=newRef;pt&&*pt;pt++)
	{
		if(ISSEP(*pt))
		{
			if(*pathRet==0)
			{
				strcat(pathRet,"..");
				strcat(pathRet,separator);
			}
			strcat(pathRet,"..");
			strcat(pathRet,separator);
		}
	}
	if(*pathRet==0)
	{
		strcat(pathRet,"..");
		strcat(pathRet,separator);
	}

	/* on rajoute la partie commune	*/
	strcat(pathRet,newPt);
/*
	printf("path=%s => %s\n",path,newPt);
	printf("pathRef=%s => %s\n",pathRef,newRef);
	printf("ret=%s\n",pathRet);
*/
}


