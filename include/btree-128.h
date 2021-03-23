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

extern struct btree_geo btree_geo128;

struct btree_head128 { struct btree_head h; };

static inline int btree_init128(struct btree_head128 *head)
{
	return btree_init(&head->h);
}

static inline void btree_destroy128(struct btree_head128 *head)
{
	btree_destroy(&head->h);
}

static inline void *btree_lookup128(struct btree_head128 *head, uint64_t k1, uint64_t k2)
{
	uint64_t key[2] = {k1, k2};
	return btree_lookup(&head->h, &btree_geo128, (unsigned long *)&key);
}

static inline void *btree_get_prev128(struct btree_head128 *head,
				      uint64_t *k1, uint64_t *k2)
{
	uint64_t key[2] = {*k1, *k2};
	void *val;

	val = btree_get_prev(&head->h, &btree_geo128,
			     (unsigned long *)&key);
	*k1 = key[0];
	*k2 = key[1];
	return val;
}

static inline int btree_insert128(struct btree_head128 *head, uint64_t k1, uint64_t k2,
				  void *val)
{
	uint64_t key[2] = {k1, k2};
	return btree_insert(&head->h, &btree_geo128,
			    (unsigned long *)&key, val);
}

static inline int btree_update128(struct btree_head128 *head, uint64_t k1, uint64_t k2,
				  void *val)
{
	uint64_t key[2] = {k1, k2};
	return btree_update(&head->h, &btree_geo128,
			    (unsigned long *)&key, val);
}

static inline void *btree_remove128(struct btree_head128 *head, uint64_t k1, uint64_t k2)
{
	uint64_t key[2] = {k1, k2};
	return btree_remove(&head->h, &btree_geo128, (unsigned long *)&key);
}

static inline void *btree_last128(struct btree_head128 *head, uint64_t *k1, uint64_t *k2)
{
	uint64_t key[2];
	void *val;

	val = btree_last(&head->h, &btree_geo128, (unsigned long *)&key[0]);
	if (val) {
		*k1 = key[0];
		*k2 = key[1];
	}

	return val;
}

static inline int btree_merge128(struct btree_head128 *target,
				 struct btree_head128 *victim)
{
	return btree_merge(&target->h, &victim->h, &btree_geo128);
}

void visitor128(void *elem, unsigned long opaque, unsigned long *__key,
		size_t index, void *__func);

typedef void (*visitor128_t)(void *elem, unsigned long opaque,
			     uint64_t key1, uint64_t key2, size_t index);

static inline size_t btree_visitor128(struct btree_head128 *head,
				      unsigned long opaque,
				      visitor128_t func2)
{
	return btree_visitor(&head->h, &btree_geo128, opaque,
			     visitor128, func2);
}

static inline size_t btree_grim_visitor128(struct btree_head128 *head,
					   unsigned long opaque,
					   visitor128_t func2)
{
	return btree_grim_visitor(&head->h, &btree_geo128, opaque,
				  visitor128, func2);
}

#define btree_for_each_safe128(head, k1, k2, val)	\
	for (val = btree_last128(head, &k1, &k2);	\
	     val;					\
	     val = btree_get_prev128(head, &k1, &k2))

