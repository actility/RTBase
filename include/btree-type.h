
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

#define __BTREE_TP(pfx, type, sfx)	pfx ## type ## sfx
#define _BTREE_TP(pfx, type, sfx)	__BTREE_TP(pfx, type, sfx)
#define BTREE_TP(pfx)			_BTREE_TP(pfx, BTREE_TYPE_SUFFIX,)
#define BTREE_FN(name)			BTREE_TP(btree_ ## name)
#define BTREE_TYPE_HEAD			BTREE_TP(struct btree_head)
#define VISITOR_FN			BTREE_TP(visitor)
#define VISITOR_FN_T			_BTREE_TP(visitor, BTREE_TYPE_SUFFIX, _t)

BTREE_TYPE_HEAD {
	struct btree_head h;
};

static inline int BTREE_FN(init)(BTREE_TYPE_HEAD *head)
{
	return btree_init(&head->h);
}

static inline void BTREE_FN(destroy)(BTREE_TYPE_HEAD *head)
{
	btree_destroy(&head->h);
}

static inline int BTREE_FN(merge)(BTREE_TYPE_HEAD *target,
				  BTREE_TYPE_HEAD *victim)
{
	return btree_merge(&target->h, &victim->h, BTREE_TYPE_GEO);
}

#if (BITS_PER_LONG > BTREE_TYPE_BITS)
static inline void *BTREE_FN(lookup)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key)
{
	unsigned long _key = key;
	return btree_lookup(&head->h, BTREE_TYPE_GEO, &_key);
}

static inline int BTREE_FN(insert)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key,
				   void *val)
{
	unsigned long _key = key;
	return btree_insert(&head->h, BTREE_TYPE_GEO, &_key, val);
}

static inline int BTREE_FN(update)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key,
		void *val)
{
	unsigned long _key = key;
	return btree_update(&head->h, BTREE_TYPE_GEO, &_key, val);
}

static inline void *BTREE_FN(remove)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key)
{
	unsigned long _key = key;
	return btree_remove(&head->h, BTREE_TYPE_GEO, &_key);
}

static inline void *BTREE_FN(last)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE *key)
{
	unsigned long _key;
	void *val = btree_last(&head->h, BTREE_TYPE_GEO, &_key);
	if (val)
		*key = _key;
	return val;
}

static inline void *BTREE_FN(get_prev)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE *key)
{
	unsigned long _key = *key;
	void *val = btree_get_prev(&head->h, BTREE_TYPE_GEO, &_key);
	if (val)
		*key = _key;
	return val;
}
#else
static inline void *BTREE_FN(lookup)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key)
{
	return btree_lookup(&head->h, BTREE_TYPE_GEO, (unsigned long *)&key);
}

static inline int BTREE_FN(insert)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key,
			   void *val)
{
	return btree_insert(&head->h, BTREE_TYPE_GEO, (unsigned long *)&key,
			    val);
}

static inline int BTREE_FN(update)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key,
		void *val)
{
	return btree_update(&head->h, BTREE_TYPE_GEO, (unsigned long *)&key, val);
}

static inline void *BTREE_FN(remove)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE key)
{
	return btree_remove(&head->h, BTREE_TYPE_GEO, (unsigned long *)&key);
}

static inline void *BTREE_FN(last)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE *key)
{
	return btree_last(&head->h, BTREE_TYPE_GEO, (unsigned long *)key);
}

static inline void *BTREE_FN(get_prev)(BTREE_TYPE_HEAD *head, BTREE_KEYTYPE *key)
{
	return btree_get_prev(&head->h, BTREE_TYPE_GEO, (unsigned long *)key);
}
#endif

void VISITOR_FN(void *elem, unsigned long opaque, unsigned long *key,
		size_t index, void *__func);

typedef void (*VISITOR_FN_T)(void *elem, unsigned long opaque,
			     BTREE_KEYTYPE key, size_t index);

static inline size_t BTREE_FN(visitor)(BTREE_TYPE_HEAD *head,
				       unsigned long opaque,
				       VISITOR_FN_T func2)
{
	return btree_visitor(&head->h, BTREE_TYPE_GEO, opaque,
			     visitorl, func2);
}

static inline size_t BTREE_FN(grim_visitor)(BTREE_TYPE_HEAD *head,
					    unsigned long opaque,
					    VISITOR_FN_T func2)
{
	return btree_grim_visitor(&head->h, BTREE_TYPE_GEO, opaque,
				  visitorl, func2);
}

#undef VISITOR_FN
#undef VISITOR_FN_T
#undef __BTREE_TP
#undef _BTREE_TP
#undef BTREE_TP
#undef BTREE_FN
#undef BTREE_TYPE_HEAD
#undef BTREE_TYPE_SUFFIX
#undef BTREE_TYPE_GEO
#undef BTREE_KEYTYPE
#undef BTREE_TYPE_BITS
