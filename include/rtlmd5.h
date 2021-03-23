
/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * See md5.c for more information.
 */

#ifndef RTL_MD5_H
#define RTL_MD5_H

/* Any 32-bit or wider unsigned integer data type will do */
typedef unsigned int MD5_u32plus;

typedef struct {
	MD5_u32plus lo, hi;
	MD5_u32plus a, b, c, d;
	unsigned char buffer[64];
	MD5_u32plus block[16];
} t_MD5_CTX;

void rtl_MD5_Init(t_MD5_CTX *ctx);
void rtl_MD5_Update(t_MD5_CTX *ctx, const void *data, unsigned long size);
void rtl_MD5_Final(unsigned char *result, t_MD5_CTX *ctx);

unsigned char *rtl_MD5_Digest(const unsigned char *d, unsigned long n,
		unsigned char *md);	// == openssl/MD5()

unsigned int rtl_MD5_Hashfunc_mem(void *key,int lg);
unsigned int rtl_MD5_Hashfunc_str(void *key);	// considered as char *

#endif
