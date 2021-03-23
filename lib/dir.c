
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
 *	Consultation des entrees d'un repertoire
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>


typedef	struct
{
	DIR	*p_sysdir;
	char	p_dir[PATH_MAX];
	char	p_file[PATH_MAX];
}	t_path;

void	*rtl_openDir(char *path)	/* TS */
{
	t_path	*dir	= NULL;

	dir	= (t_path *)malloc(sizeof(t_path));
	if	(!dir)
	{
		return NULL;
	}
	if(!(dir->p_sysdir=opendir(path)))
	{
		free	(dir);
		return NULL;
	}
	strcpy	(dir->p_dir,path);
	return (void *)dir;
}

char	*rtl_readAbsDir(void *pdir)	/* TS */
{
	struct dirent *ent;
	t_path	*dir	= (t_path *)pdir;

	if(!dir)
		return	NULL;

	if((ent=readdir(dir->p_sysdir)))
	{
		sprintf	(dir->p_file,"%s/%s",dir->p_dir,ent->d_name);
//printf("filename '%s'\n",dir->p_file);
		return dir->p_file;
	}
	return NULL;
}

char	*rtl_readDir(void *pdir)	/* TS */
{
	struct dirent *ent;
	t_path	*dir	= (t_path *)pdir;

	if(!dir)
		return	NULL;

	if((ent=readdir(dir->p_sysdir)))
	{
		sprintf	(dir->p_file,"%s/%s",dir->p_dir,ent->d_name);
//printf("filename '%s'\n",ent->d_name);
		return ent->d_name;
	}
	return NULL;
}

void	rtl_closeDir(void *pdir)	/* TS */
{
	t_path	*dir	= (t_path *)pdir;

	if(!dir)
		return;
	closedir(dir->p_sysdir);
	free(dir);
}



