/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/* bitbridge.c */

/* bitbridge.h suppresses some warnings
  - restore warning level after including it */

#define BRIDGE_LIBRARY_IMPL 1

#ifdef __clang__
#pragma clang diagnostic push
#endif

#ifdef _MSC_VER
#pragma warning(push)
#endif

#include "bitbridge/bitbridge.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_ppint_packed_size(INT32_TYPE n)
{
	unsigned INT32_TYPE x = (unsigned INT32_TYPE)n;
	if (x > 0x3FFFu) {
		if (x > 0x1FFFFFu) {
			if (x > 0xFFFFFFFu)
				return 5;
			return 4;
		}
		return 3;
	}
	if (x > 0x7Fu)
		return 2;
	return 1;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_pplong_packed_size(INT64_TYPE n)
{
	unsigned INT64_TYPE x = (unsigned INT64_TYPE)n;
	if (x > 0x7FFFFFFFFull) {
		if (x > 0x1FFFFFFFFFFFFull) {
			if (x > 0xFFFFFFFFFFFFFFull)
				return 9;
			return 8;
		}
		if (x > 0x3FFFFFFFFFFull)
			return 7;
		return 6;
	}
	if (x > 0x3FFFu) {
		if (x > 0x1FFFFFu) {
			if (x > 0xFFFFFFFu)
				return 5;
			return 4;
		}
		return 3;
	}
	if (x > 0x7Fu)
		return 2;
	return 1;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_ppshort(char *mem, INT16_TYPE n)
{
	unsigned char *m = (unsigned char*)mem;
	(void)sizeof(int[1-2*(sizeof(unsigned) < sizeof(INT16_TYPE))]); /* check that sizeof(unsigned) >= sizeof(INT16_TYPE) */
	{
		unsigned x = (unsigned INT16_TYPE)n;
		while (x > 127u) {
			*m++ = (unsigned char)((x | 128) & (unsigned char)~(unsigned char)0);
			x = (unsigned)(x >> 7);
		}
		*m++ = (unsigned char)(x & (unsigned char)~(unsigned char)0);
		return (char*)m;
	}
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_ppint(char *mem, INT32_TYPE n)
{
	unsigned char *m = (unsigned char*)mem;
	unsigned INT32_TYPE x = (unsigned INT32_TYPE)n;
	while (x > 127u) {
		*m++ = (unsigned char)((x | 128) & (unsigned char)~(unsigned char)0);
		x = (unsigned INT32_TYPE)(x >> 7);
	}
	*m++ = (unsigned char)(x & (unsigned char)~(unsigned char)0);
	return (char*)m;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_pplong(char *mem, INT64_TYPE n)
{
	unsigned char *m = (unsigned char*)mem;
	unsigned char *lim = m + 8; /* 9-th x encoded in 8 bits */
	unsigned INT64_TYPE x = (unsigned INT64_TYPE)n;
	while (x > 127u) {
		*m++ = (unsigned char)((x | 128u) & (unsigned char)~(unsigned char)0);
		x = (unsigned INT64_TYPE)(x >> 7u);
		if (m == lim)
			break;
	}
	*m++ = (unsigned char)(x & (unsigned char)~(unsigned char)0);
	return (char*)m;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_no_lim(const char *mem, INT16_TYPE *n/*out*/)
{
	const unsigned char *m = (const unsigned char*)mem;
	(void)sizeof(int[1-2*(sizeof(unsigned) < sizeof(INT16_TYPE))]); /* check that sizeof(unsigned) >= sizeof(INT16_TYPE) */
	{
		unsigned x = (unsigned)*m++;
		if (x > 0x7Fu) {
			x = (unsigned)(x ^ ((0u + (unsigned)*m++) << 7));
			if (x > 0x3FFFu)
				*n = (INT16_TYPE)(x ^ ((0u + (unsigned)*m++) << 14) ^ 0x4080u);
			else
				*n = (INT16_TYPE)(x ^ 0x80u);
		}
		else
			*n = (INT16_TYPE)x;
	}
	return (const char*)m;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort(const char *mem, const char *lim, INT16_TYPE *n/*out*/)
{
	if ((bridge_counter_t)(lim - mem) >= 3u)
		return _bridge_unpack_ppshort_no_lim(mem, n/*out*/);
	if (_br_unlikely(mem == lim))
		return NULL;
	{
		const unsigned char *_lim = (const unsigned char*)lim;
		const unsigned char *m = (const unsigned char*)mem;
		(void)sizeof(int[1-2*(sizeof(unsigned) < sizeof(INT16_TYPE))]); /* check that sizeof(unsigned) >= sizeof(INT16_TYPE) */
		{
			unsigned x = (unsigned)*m++;
			if (x > 0x7Fu) {
				if (_br_unlikely(m == _lim))
					return NULL;
				x = (unsigned)(x ^ ((0u + (unsigned)*m++) << 7));
				if (x > 0x3FFFu) {
					if (_br_unlikely(m == _lim))
						return NULL;
					*n = (INT16_TYPE)(x ^ ((0u + (unsigned)*m++) << 14) ^ 0x4080u);
				}
				else
					*n = (INT16_TYPE)(x ^ 0x80u);
			}
			else
				*n = (INT16_TYPE)x;
		}
		return (const char*)m;
	}
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_no_lim(const char *mem, INT32_TYPE *n/*out*/)
{
	const unsigned char *m = (const unsigned char*)mem;
	unsigned INT32_TYPE x = (unsigned INT32_TYPE)*m++;
	if (x > 0x7Fu) {
		x = (unsigned INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 7));
		if (x > 0x3FFFu) {
			x = (unsigned INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 14));
			if (x > 0x1FFFFFu) {
				x = (unsigned INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 21));
				if (x > 0xFFFFFFFu)
					*n = (INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 28) ^ 0x10204080u);
				else
					*n = (INT32_TYPE)(x ^ 0x204080u);
			}
			else
				*n = (INT32_TYPE)(x ^ 0x4080u);
		}
		else
			*n = (INT32_TYPE)(x ^ 0x80u);
	}
	else
		*n = (INT32_TYPE)x;
	return (const char*)m;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint(const char *mem, const char *lim, INT32_TYPE *n/*out*/)
{
	if ((bridge_counter_t)(lim - mem) >= 5u)
		return _bridge_unpack_ppint_no_lim(mem, n/*out*/);
	if (_br_unlikely(mem == lim))
		return NULL;
	{
		const unsigned char *_lim = (const unsigned char*)lim;
		const unsigned char *m = (const unsigned char*)mem;
		unsigned INT32_TYPE x = (unsigned INT32_TYPE)*m++;
		if (x > 0x7Fu) {
			if (_br_unlikely(m == _lim))
				return NULL;
			x = (unsigned INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 7));
			if (x > 0x3FFFu) {
				if (_br_unlikely(m == _lim))
					return NULL;
				x = (unsigned INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 14));
				if (x > 0x1FFFFFu) {
					if (_br_unlikely(m == _lim))
						return NULL;
					x = (unsigned INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 21));
					if (x > 0xFFFFFFFu) {
						if (_br_unlikely(m == _lim))
							return NULL;
						*n = (INT32_TYPE)(x ^ ((0u + (unsigned INT32_TYPE)*m++) << 28) ^ 0x10204080u);
					}
					else
						*n = (INT32_TYPE)(x ^ 0x204080u);
				}
				else
					*n = (INT32_TYPE)(x ^ 0x4080u);
			}
			else
				*n = (INT32_TYPE)(x ^ 0x80u);
		}
		else
			*n = (INT32_TYPE)x;
		return (const char*)m;
	}
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_no_lim(const char *mem, INT64_TYPE *n/*out*/)
{
	const unsigned char *m = (const unsigned char*)mem;
	unsigned INT64_TYPE x = (unsigned INT64_TYPE)*m++;
	if (x > 0x7Fu) {
		x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 7));
		if (x > 0x3FFFu) {
			x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 14));
			if (x > 0x1FFFFFu) {
				x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 21));
				if (x > 0xFFFFFFFu) {
					x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 28));
					if (x > 0x7FFFFFFFFull) {
						x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 35));
						if (x > 0x3FFFFFFFFFFull) {
							x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 42));
							if (x > 0x1FFFFFFFFFFFFull) {
								x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 49));
								if (x > 0xFFFFFFFFFFFFFFull)
									*n = (INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 56) ^ 0x102040810204080ull);
								else
									*n = (INT64_TYPE)(x ^ 0x2040810204080ull);
							}
							else
								*n = (INT64_TYPE)(x ^ 0x40810204080ull);
						}
						else
							*n = (INT64_TYPE)(x ^ 0x810204080ull);
					}
					else
						*n = (INT64_TYPE)(x ^ 0x10204080u);
				}
				else
					*n = (INT64_TYPE)(x ^ 0x204080u);
			}
			else
				*n = (INT64_TYPE)(x ^ 0x4080u);
		}
		else
			*n = (INT64_TYPE)(x ^ 0x80u);
	}
	else
		*n = (INT64_TYPE)x;
	return (const char*)m;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong(const char *mem, const char *lim, INT64_TYPE *n/*out*/)
{
	if ((bridge_counter_t)(lim - mem) >= 9u)
		return _bridge_unpack_pplong_no_lim(mem, n/*out*/);
	if (_br_unlikely(mem == lim))
		return NULL;
	{
		const unsigned char *_lim = (const unsigned char*)lim;
		const unsigned char *m = (const unsigned char*)mem;
		unsigned INT64_TYPE x = (unsigned INT64_TYPE)*m++;
		if (x > 0x7Fu) {
			if (_br_unlikely(m == _lim))
				return NULL;
			x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 7));
			if (x > 0x3FFFu) {
				if (_br_unlikely(m == _lim))
					return NULL;
				x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 14));
				if (x > 0x1FFFFFu) {
					if (_br_unlikely(m == _lim))
						return NULL;
					x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 21));
					if (x > 0xFFFFFFFu) {
						if (_br_unlikely(m == _lim))
							return NULL;
						x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 28));
						if (x > 0x7FFFFFFFFull) {
							if (_br_unlikely(m == _lim))
								return NULL;
							x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 35));
							if (x > 0x3FFFFFFFFFFull) {
								if (_br_unlikely(m == _lim))
									return NULL;
								x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 42));
								if (x > 0x1FFFFFFFFFFFFull) {
									if (_br_unlikely(m == _lim))
										return NULL;
									x = (unsigned INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 49));
									if (x > 0xFFFFFFFFFFFFFFull) {
										if (_br_unlikely(m == _lim))
											return NULL;
										*n = (INT64_TYPE)(x ^ ((0u + (unsigned INT64_TYPE)*m++) << 56) ^ 0x102040810204080ull);
									}
									else
										*n = (INT64_TYPE)(x ^ 0x2040810204080ull);
								}
								else
									*n = (INT64_TYPE)(x ^ 0x40810204080ull);
							}
							else
								*n = (INT64_TYPE)(x ^ 0x810204080ull);
						}
						else
							*n = (INT64_TYPE)(x ^ 0x10204080u);
					}
					else
						*n = (INT64_TYPE)(x ^ 0x204080u);
				}
				else
					*n = (INT64_TYPE)(x ^ 0x4080u);
			}
			else
				*n = (INT64_TYPE)(x ^ 0x80u);
		}
		else
			*n = (INT64_TYPE)x;
		return (const char*)m;
	}
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_bit_array(char *A_Restrict mem, const unsigned char arr[], unsigned bit_count)
{
	mem = _bridge_pack_bit_counter(mem, bit_count);
	if (bit_count) {
		bridge_counter_t bit_bytes = bridge_nonzero_bit_array_size(bit_count);
		BRIDGE_MEMCPY(mem, arr, bit_bytes);
		mem += bit_bytes;
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_bit_array(const char *mem,
	bridge_counter_t bit_bytes, unsigned char **s_array, BRIDGE_ALLOCATOR_ARG(ac))
{
	const char *m = mem;
	mem = m + bit_bytes;
	{
		void *a = ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), m, bit_bytes, BRIDGE_ALLOC_REF_COPY_ALIGNED1);
		bridge_allocator_dec_level(ac);
		if (_br_unlikely(!a))
			return NULL;
		*s_array = (unsigned char*)a;
	}
	return mem;
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static const char *_bridge_unpack_bit_array_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Out_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned *s_bit_count,
	A_Out A_Post A_At(*A_Curr, A_When(!*s_bit_count, A_Null) A_When(*s_bit_count, A_Notnull)) unsigned char **s_array,
	BRIDGE_ALLOCATOR_ARG(ac))
{
	INT32_TYPE bit_count;
	mem = _bridge_unpack_counter(mem, lim, &bit_count);
	if (_br_unlikely(!mem))
		return NULL;
	if (_br_unlikely(((void)0,BRIDGE_BIT_COUNT_MAX) < (unsigned INT32_TYPE)bit_count))
		return NULL;
	*s_bit_count = (unsigned)bit_count;
	if (bit_count) {
		bridge_counter_t bit_bytes = bridge_nonzero_bit_array_size((unsigned)bit_count);
		if (_br_unlikely(!upk_check_mem_lim(mem, lim, bit_bytes)))
			return NULL;
		mem = _bridge_unpack_bit_array(mem, bit_bytes, s_array, BRIDGE_PASS_ALLOCATOR(ac));
	}
	else
		*s_array = NULL;
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_bit_array(const char *mem, const char *lim,
	unsigned *s_bit_count, unsigned char **s_array, BRIDGE_ALLOCATOR_ARG(ac))
{
	const char *ret = _bridge_unpack_bit_array_(mem, lim, s_bit_count, s_array, BRIDGE_PASS_ALLOCATOR_THROUGH(ac));
	bridge_allocator_dec_level(ac);
	return ret;
}

/* assume each array element takes at least one byte in packed form */
#define BRIDGE_ARRAY_MAX_SIZE (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_add_str_array_packed_size(unsigned c, char *const *arr, char *const *arr_end)
{
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(_bridge_append_str_packed_size(&c, 0, *arr)))
			return ~0u; /* too big item */
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_add_pstr_array_packed_size(unsigned c, char *const *arr, char *const *arr_end)
{
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(_bridge_append_pstr_packed_size(&c, 0, *arr)))
			return ~0u; /* too big item */
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_add_str_array_packed_size(unsigned c, char *const *arr, char *const *arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE) &&
		_br_likely(_br_of_add(&c, bridge_counter_packed_size((bridge_counter_t)count))))
		return _bridge_add_str_array_packed_size(c, arr, arr_end);
	return ~0u; /* too big array */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_add_pstr_array_packed_size(unsigned c, char *const *arr, char *const *arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE) &&
		_br_likely(_br_of_add(&c, bridge_counter_packed_size((bridge_counter_t)count))))
		return _bridge_add_pstr_array_packed_size(c, arr, arr_end);
	return ~0u; /* too big array */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_str(char *A_Restrict mem, const char *str)
{
	size_t len = str ? BRIDGE_STRLEN(str) : 0;
	BRIDGE_ASSERT(len < BRIDGE_ARRAY_MAX_SIZE); /* checked in _bridge_append_str_packed_size()/_bridge_add_str_array_packed_size() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)len);
	if (len) {
		char *m = mem;
		mem += (bridge_counter_t)len + 1/*'\0'*/;
		BRIDGE_MEMCPY(m, str, (bridge_counter_t)len + 1/*'\0'*/);
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_pstr(char *A_Restrict mem, const char *str)
{
	if (str)
		mem = (char*)BRIDGE_STPCPY(mem, str) + 1/*'\0'*/;
	else
		*mem++ = '\0';
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_str_array(char *A_Restrict mem, char *const *arr, char *const *arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_str(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_pstr_array(char *A_Restrict mem, char *const *arr, char *const *arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_pstr(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_str_array(char *A_Restrict mem, char *const *arr, char *const *arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* checked in bridge_add_str_array_packed_size() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_str_array(mem, arr, arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_pstr_array(char *A_Restrict mem, char *const *arr, char *const *arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* checked in bridge_add_pstr_array_packed_size() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_pstr_array(mem, arr, arr_end);
	return mem;
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Post_satisfies(return <= lim) A_Post_satisfies(return > mem)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static const char *_bridge_unpack_and_check_simple_var_type_counter(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(>,0) size_t item_size, A_Out A_Deref_out_range(<=,BRIDGE_ARRAY_MAX_SIZE) bridge_counter_t *count)
{
	INT32_TYPE _count;
	mem = _bridge_unpack_counter(mem, lim, &_count/*out*/);
	if (_br_likely(mem) &&
		_br_likely((unsigned INT32_TYPE)_count <= BRIDGE_ARRAY_MAX_SIZE) &&
		_br_likely((unsigned INT32_TYPE)_count <= (size_t)~(size_t)0/item_size))
	{
		*count = (bridge_counter_t)(unsigned INT32_TYPE)_count;
		return mem;
	}
	return NULL;
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static const char *_bridge_unpack_str_1(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Outptr_result_z char **str, BRIDGE_ALLOCATOR_ARG(ac))
{
	INT32_TYPE _len;
	mem = _bridge_unpack_counter(mem, lim, &_len/*out*/);
	if (_br_unlikely(!mem))
		return NULL;
	/* if _len != 0, then expecting '\0'-terminated string */
	if (_len && _br_unlikely(
		(unsigned INT32_TYPE)_len >= (bridge_counter_t)(lim - mem) ||
		mem[(bridge_counter_t)(unsigned INT32_TYPE)_len]))
	{
		return NULL;
	}
	/* NOTE: there are may be more '\0'-s inside the string, this is _not_ checked */
	{
		bridge_counter_t len = (bridge_counter_t)(unsigned INT32_TYPE)_len;
		const char *m = mem - !len;
		mem += len + !!len;
		{
			char *s = (char*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), m, len + 1/*'\0'*/, BRIDGE_ALLOC_REF_COPY_STRINGZ);
			if (_br_unlikely(!s))
				return NULL;
			*str = s;
		}
	}
	return mem;
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static const char *_bridge_unpack_pstr_1(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Outptr_result_z char **str, BRIDGE_ALLOCATOR_ARG(ac))
{
	const char *p = (const char*)BRIDGE_MEMCHR(mem, '\0', (size_t)(lim - mem));
	if (_br_unlikely(!p))
		return NULL;
	BRIDGE_ASSERT((size_t)(p - mem) <= BRIDGE_MAX - 1u/*'\0'*/); /* (lim - mem) <= BRIDGE_MAX */
	/* NOTE: there are no '\0'-s inside the string */
	{
		bridge_counter_t sz = (bridge_counter_t)(p - mem) + 1/*'\0'*/;
		const char *m = mem;
		mem += sz;
		{
			char *s = (char*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), m, sz, BRIDGE_ALLOC_REF_COPY_STRINGZ);
			if (_br_unlikely(!s))
				return NULL;
			*str = s;
		}
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_(const char *mem, const char *lim,
	char **str, BRIDGE_ALLOCATOR_ARG(ac))
{
	const char *ret = _bridge_unpack_str_1(mem, lim, str, BRIDGE_PASS_ALLOCATOR_THROUGH(ac));
	bridge_allocator_dec_level(ac);
	return ret;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_(const char *mem, const char *lim,
	char **str, BRIDGE_ALLOCATOR_ARG(ac))
{
	const char *ret = _bridge_unpack_pstr_1(mem, lim, str, BRIDGE_PASS_ALLOCATOR_THROUGH(ac));
	bridge_allocator_dec_level(ac);
	return ret;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_array_(const char *mem, const char *lim,
	bridge_counter_t count, char *arr[/*count*/], char ***arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	char **const _arr_end = arr + count;
	do {
		mem = _bridge_unpack_str_(mem, lim, arr, BRIDGE_PASS_ALLOCATOR(ac));
		if (_br_unlikely(!mem))
			break; /* failed */
	} while (++arr < _arr_end);
	*arr_end = arr; /* always terminate array - to correctly free partially filled array */
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_array_(const char *mem, const char *lim,
	bridge_counter_t count, char *arr[/*count*/], char ***arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	char **const _arr_end = arr + count;
	do {
		mem = _bridge_unpack_pstr_(mem, lim, arr, BRIDGE_PASS_ALLOCATOR(ac));
		if (_br_unlikely(!mem))
			break; /* failed */
	} while (++arr < _arr_end);
	*arr_end = arr; /* always terminate array - to correctly free partially filled array */
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

/* allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
A_Post_satisfies(*arr < *arr_end) A_On_failure(A_Post_satisfies(*arr <= *arr_end))
static const char *_bridge_unpack_str_array1(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_On_failure(A_Outptr_result_maybenull) char ***arr,
	A_Outptr A_On_failure(A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull))) char ***arr_end,
	BRIDGE_ALLOCATOR_ARG(ac))
{
	void *a = ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_MEM);
	if (_br_unlikely(!a)) {
		*arr = *arr_end = NULL; /* for _bridge_unpack_str_array_domains() */
		return NULL; /* failed */
	}
	*arr = (char**)a;
	mem = _bridge_unpack_str_array_(mem, lim, count, *arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
	return mem; /* NULL if failed */
}

/* allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
A_Post_satisfies(*arr < *arr_end) A_On_failure(A_Post_satisfies(*arr <= *arr_end))
static const char *_bridge_unpack_pstr_array1(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_On_failure(A_Outptr_result_maybenull) char ***arr,
	A_Outptr A_On_failure(A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull))) char ***arr_end,
	BRIDGE_ALLOCATOR_ARG(ac))
{
	void *a = ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_MEM);
	if (_br_unlikely(!a)) {
		*arr = *arr_end = NULL; /* for _bridge_unpack_pstr_array_domains() */
		return NULL; /* failed */
	}
	*arr = (char**)a;
	mem = _bridge_unpack_pstr_array_(mem, lim, count, *arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_str_array(const char *mem, const char *lim,
	char ***arr, char ***arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_str_array1(mem, lim, count, arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
		bridge_allocator_dec_level(ac);
	}
	else
		*arr = *arr_end = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_pstr_array(const char *mem, const char *lim,
	char ***arr, char ***arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_pstr_array1(mem, lim, count, arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
		bridge_allocator_dec_level(ac);
	}
	else
		*arr = *arr_end = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_array_domains(const char *mem, const char *lim, bridge_counter_t count,
	char ***arr, char ***arr_end, bridge_counter_t domain, BRIDGE_ALLOCATOR_ARG(ac))
{
	mem = _bridge_unpack_str_array1(mem, lim, count, arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!mem)) {
		bridge_counter_t remain = (bridge_counter_t)(*arr_end - *arr) % domain;
		BRIDGE_ASSERT(!remain || (*arr && *arr_end));
		if (remain) {
			bridge_counter_t to_zero = domain - remain;
			char **e = *arr_end;
			*arr_end = e + to_zero;
			memset(e, 0, sizeof(*e)*to_zero);
		}
	}
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_array_domains(const char *mem, const char *lim, bridge_counter_t count,
	char ***arr, char ***arr_end, bridge_counter_t domain, BRIDGE_ALLOCATOR_ARG(ac))
{
	mem = _bridge_unpack_pstr_array1(mem, lim, count, arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!mem)) {
		bridge_counter_t remain = (bridge_counter_t)((*arr_end - *arr) % domain);
		BRIDGE_ASSERT(!remain || (*arr && *arr_end));
		if (remain) {
			bridge_counter_t to_zero = domain - remain;
			char **e = *arr_end;
			*arr_end = e + to_zero;
			memset(e, 0, sizeof(*e)*to_zero);
		}
	}
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_array(const char *mem, const char *lim,
	bridge_counter_t count, char ***arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	char **arr_end;
	const char *ret = _bridge_unpack_str_array_domains(mem, lim, count, arr, &arr_end, count, BRIDGE_PASS_ALLOCATOR(ac));
	bridge_allocator_dec_level(ac);
	return ret;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_array(const char *mem, const char *lim,
	bridge_counter_t count, char ***arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	char **arr_end;
	const char *ret = _bridge_unpack_pstr_array_domains(mem, lim, count, arr, &arr_end, count, BRIDGE_PASS_ALLOCATOR(ac));
	bridge_allocator_dec_level(ac);
	return ret;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS void _bridge_destroy_ptr_array(void **arr, void **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	for (; arr < arr_end; arr++)
		ac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), *arr);
	bridge_allocator_dec_level(ac);
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS void _bridge_delete_ptr_array(void **arr, void **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	_bridge_destroy_ptr_array(arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
	ac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), arr);
	bridge_allocator_dec_level(ac);
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS void _bridge_delete_ptr_cont(void **container, BRIDGE_ALLOCATOR_ARG(ac))
{
	if (container) {
		void *mem = *container;
		ac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), mem);
		ac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), container);
	}
	bridge_allocator_dec_level(ac);
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_pshort_array_packed_size(const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	unsigned c = 0;
	size_t count = (size_t)(arr_end - arr);
	if (count >= 8) {
		unsigned n = (count <= ~0u/3u) ? (unsigned)count : (~0u/3u);
		const INT16_TYPE *_arr_end = arr + (n - 8u);
		BRIDGE_ASSERT(arr);
		do {
			c += bridge_pshort_packed_size(arr[0]);
			c += bridge_pshort_packed_size(arr[1]);
			c += bridge_pshort_packed_size(arr[2]);
			c += bridge_pshort_packed_size(arr[3]);
			c += bridge_pshort_packed_size(arr[4]);
			c += bridge_pshort_packed_size(arr[5]);
			c += bridge_pshort_packed_size(arr[6]);
			c += bridge_pshort_packed_size(arr[7]);
			arr += 8;
		} while (arr <= _arr_end);
	}
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(!_br_of_add(&c, bridge_pshort_packed_size(*arr))))
			return ~0u;
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_pint_array_packed_size(const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	unsigned c = 0;
	size_t count = (size_t)(arr_end - arr);
	if (count >= 4) {
		unsigned n = (count <= ~0u/5u) ? (unsigned)count : (~0u/5u);
		const INT32_TYPE *_arr_end = arr + (n - 4u);
		BRIDGE_ASSERT(arr);
		do {
			c += bridge_pint_packed_size(arr[0]);
			c += bridge_pint_packed_size(arr[1]);
			c += bridge_pint_packed_size(arr[2]);
			c += bridge_pint_packed_size(arr[3]);
			arr += 4;
		} while (arr <= _arr_end);
	}
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(!_br_of_add(&c, bridge_pint_packed_size(*arr))))
			return ~0u;
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_plong_array_packed_size(const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	unsigned c = 0;
	size_t count = (size_t)(arr_end - arr);
	if (count >= 4) {
		unsigned n = (count <= ~0u/9u) ? (unsigned)count : (~0u/9u);
		const INT64_TYPE *_arr_end = arr + (n - 4u);
		BRIDGE_ASSERT(arr);
		do {
			c += bridge_plong_packed_size(arr[0]);
			c += bridge_plong_packed_size(arr[1]);
			c += bridge_plong_packed_size(arr[2]);
			c += bridge_plong_packed_size(arr[3]);
			arr += 4;
		} while (arr <= _arr_end);
	}
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(!_br_of_add(&c, bridge_plong_packed_size(*arr))))
			return ~0u;
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_ppshort_array_packed_size(const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	unsigned c = 0;
	size_t count = (size_t)(arr_end - arr);
	if (count >= 4) {
		unsigned n = (count <= ~0u/3u) ? (unsigned)count : (~0u/3u);
		const INT16_TYPE *_arr_end = arr + (n - 4u);
		BRIDGE_ASSERT(arr);
		do {
			c += bridge_ppshort_packed_size(arr[0]);
			c += bridge_ppshort_packed_size(arr[1]);
			c += bridge_ppshort_packed_size(arr[2]);
			c += bridge_ppshort_packed_size(arr[3]);
			arr += 4;
		} while (arr <= _arr_end);
	}
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(!_br_of_add(&c, bridge_ppshort_packed_size(*arr))))
			return ~0u;
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_ppint_array_packed_size(const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	unsigned c = 0;
	size_t count = (size_t)(arr_end - arr);
	if (count >= 4) {
		unsigned n = (count <= ~0u/5u) ? (unsigned)count : (~0u/5u);
		const INT32_TYPE *_arr_end = arr + (n - 4u);
		BRIDGE_ASSERT(arr);
		do {
			c += bridge_ppint_packed_size(arr[0]);
			c += bridge_ppint_packed_size(arr[1]);
			c += bridge_ppint_packed_size(arr[2]);
			c += bridge_ppint_packed_size(arr[3]);
			arr += 4;
		} while (arr <= _arr_end);
	}
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(!_br_of_add(&c, bridge_ppint_packed_size(*arr))))
			return ~0u;
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned _bridge_pplong_array_packed_size(const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	unsigned c = 0;
	size_t count = (size_t)(arr_end - arr);
	if (count >= 4) {
		unsigned n = (count <= ~0u/9u) ? (unsigned)count : (~0u/9u);
		const INT64_TYPE *_arr_end = arr + (n - 4u);
		BRIDGE_ASSERT(arr);
		do {
			c += bridge_pplong_packed_size(arr[0]);
			c += bridge_pplong_packed_size(arr[1]);
			c += bridge_pplong_packed_size(arr[2]);
			c += bridge_pplong_packed_size(arr[3]);
			arr += 4;
		} while (arr <= _arr_end);
	}
	for (; arr < arr_end; arr++) {
		if (_br_unlikely(!_br_of_add(&c, bridge_pplong_packed_size(*arr))))
			return ~0u;
	}
	return c;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_pshort_array_packed_size(const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE)) {
		unsigned c = bridge_counter_packed_size((bridge_counter_t)count);
		if (_br_likely(_br_of_add(&c, _bridge_pshort_array_packed_size(arr, arr_end))))
			return c;
	}
	return ~0u;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_pint_array_packed_size(const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE)) {
		unsigned c = bridge_counter_packed_size((bridge_counter_t)count);
		if (_br_likely(_br_of_add(&c, _bridge_pint_array_packed_size(arr, arr_end))))
			return c;
	}
	return ~0u;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_plong_array_packed_size(const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE)) {
		unsigned c = bridge_counter_packed_size((bridge_counter_t)count);
		if (_br_likely(_br_of_add(&c, _bridge_plong_array_packed_size(arr, arr_end))))
			return c;
	}
	return ~0u;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_ppshort_array_packed_size(const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE)) {
		unsigned c = bridge_counter_packed_size((bridge_counter_t)count);
		if (_br_likely(_br_of_add(&c, _bridge_ppshort_array_packed_size(arr, arr_end))))
			return c;
	}
	return ~0u;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_ppint_array_packed_size(const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE)) {
		unsigned c = bridge_counter_packed_size((bridge_counter_t)count);
		if (_br_likely(_br_of_add(&c, _bridge_ppint_array_packed_size(arr, arr_end))))
			return c;
	}
	return ~0u;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS unsigned bridge_pplong_array_packed_size(const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_likely(count <= BRIDGE_ARRAY_MAX_SIZE)) {
		unsigned c = bridge_counter_packed_size((bridge_counter_t)count);
		if (_br_likely(_br_of_add(&c, _bridge_pplong_array_packed_size(arr, arr_end))))
			return c;
	}
	return ~0u;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_pshort_array(char *A_Restrict mem, const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_pshort(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_pint_array(char *A_Restrict mem, const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_pint(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_plong_array(char *A_Restrict mem, const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_plong(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_ppshort_array(char *A_Restrict mem, const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_ppshort(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_ppint_array(char *A_Restrict mem, const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_ppint(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *_bridge_pack_pplong_array(char *A_Restrict mem, const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	for (; arr < arr_end; arr++)
		mem = _bridge_pack_pplong(mem, *arr);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_pshort_array(char *A_Restrict mem, const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* must be checked in bridge_pshort_array_packed_size_() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_pshort_array(mem, arr, arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_pint_array(char *A_Restrict mem, const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* must be checked in bridge_pint_array_packed_size_() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_pint_array(mem, arr, arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_plong_array(char *A_Restrict mem, const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* must be checked in bridge_plong_array_packed_size_() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_plong_array(mem, arr, arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_ppshort_array(char *A_Restrict mem, const INT16_TYPE *arr, const INT16_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* must be checked in bridge_ppshort_array_packed_size_() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_ppshort_array(mem, arr, arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_ppint_array(char *A_Restrict mem, const INT32_TYPE *arr, const INT32_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* must be checked in bridge_ppint_array_packed_size_() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_ppint_array(mem, arr, arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS char *bridge_pack_pplong_array(char *A_Restrict mem, const INT64_TYPE *arr, const INT64_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	BRIDGE_ASSERT(count <= BRIDGE_ARRAY_MAX_SIZE); /* must be checked in bridge_pplong_array_packed_size_() */
	mem = _bridge_pack_counter(mem, (bridge_counter_t)count);
	mem = _bridge_pack_pplong_array(mem, arr, arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pshort_array_no_lim(const char *mem, bridge_counter_t count, INT16_TYPE arr[/*count*/])
{
	const INT16_TYPE *const arr_end = arr + count;
	do {
		mem = _bridge_unpack_pshort_no_lim(mem, arr/*out*/);
	} while (++arr < arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pint_array_no_lim(const char *mem, bridge_counter_t count, INT32_TYPE arr[/*count*/])
{
	const INT32_TYPE *const arr_end = arr + count;
	do {
		mem = _bridge_unpack_pint_no_lim(mem, arr/*out*/);
	} while (++arr < arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_plong_array_no_lim(const char *mem, bridge_counter_t count, INT64_TYPE arr[/*count*/])
{
	const INT64_TYPE *const arr_end = arr + count;
	do {
		mem = _bridge_unpack_plong_no_lim(mem, arr/*out*/);
	} while (++arr < arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_array_no_lim(const char *mem, bridge_counter_t count, INT16_TYPE arr[/*count*/])
{
	const INT16_TYPE *const arr_end = arr + count;
	do {
		mem = _bridge_unpack_ppshort_no_lim(mem, arr/*out*/);
	} while (++arr < arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_array_no_lim(const char *mem, bridge_counter_t count, INT32_TYPE arr[/*count*/])
{
	const INT32_TYPE *const arr_end = arr + count;
	do {
		mem = _bridge_unpack_ppint_no_lim(mem, arr/*out*/);
	} while (++arr < arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_array_no_lim(const char *mem, bridge_counter_t count, INT64_TYPE arr[/*count*/])
{
	const INT64_TYPE *const arr_end = arr + count;
	do {
		mem = _bridge_unpack_pplong_no_lim(mem, arr/*out*/);
	} while (++arr < arr_end);
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pshort_array_(const char *mem, const char *lim,
	bridge_counter_t count, INT16_TYPE arr[/*count*/])
{
	const INT16_TYPE *const arr_end = arr + count;
	bridge_counter_t no_lim_count = (bridge_counter_t)(lim - mem)/3u;
	if (no_lim_count) {
		if (no_lim_count > count)
			no_lim_count = count;
		{
			INT16_TYPE *_arr = arr;
			arr = _arr + no_lim_count;
			mem = _bridge_unpack_pshort_array_no_lim(mem, no_lim_count, _arr);
		}
	}
	for (; arr < arr_end; arr++) {
		mem = _bridge_unpack_pshort(mem, lim, arr/*out*/);
		if (_br_unlikely(!mem))
			return NULL;
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pint_array_(const char *mem, const char *lim,
	bridge_counter_t count, INT32_TYPE arr[/*count*/])
{
	const INT32_TYPE *const arr_end = arr + count;
	bridge_counter_t no_lim_count = (bridge_counter_t)(lim - mem)/5u;
	if (no_lim_count) {
		if (no_lim_count > count)
			no_lim_count = count;
		{
			INT32_TYPE *_arr = arr;
			arr = _arr + no_lim_count;
			mem = _bridge_unpack_pint_array_no_lim(mem, no_lim_count, _arr);
		}
	}
	for (; arr < arr_end; arr++) {
		mem = _bridge_unpack_pint(mem, lim, arr/*out*/);
		if (_br_unlikely(!mem))
			return NULL;
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_plong_array_(const char *mem, const char *lim,
	bridge_counter_t count, INT64_TYPE arr[/*count*/])
{
	const INT64_TYPE *const arr_end = arr + count;
	bridge_counter_t no_lim_count = (bridge_counter_t)(lim - mem)/9u;
	if (no_lim_count) {
		if (no_lim_count > count)
			no_lim_count = count;
		{
			INT64_TYPE *_arr = arr;
			arr = _arr + no_lim_count;
			mem = _bridge_unpack_plong_array_no_lim(mem, no_lim_count, _arr);
		}
	}
	for (; arr < arr_end; arr++) {
		mem = _bridge_unpack_plong(mem, lim, arr/*out*/);
		if (_br_unlikely(!mem))
			return NULL;
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_array_(const char *mem, const char *lim,
	bridge_counter_t count, INT16_TYPE arr[/*count*/])
{
	const INT16_TYPE *const arr_end = arr + count;
	bridge_counter_t no_lim_count = (bridge_counter_t)(lim - mem)/3u;
	if (no_lim_count) {
		if (no_lim_count > count)
			no_lim_count = count;
		{
			INT16_TYPE *_arr = arr;
			arr = _arr + no_lim_count;
			mem = _bridge_unpack_ppshort_array_no_lim(mem, no_lim_count, _arr);
		}
	}
	for (; arr < arr_end; arr++) {
		mem = _bridge_unpack_ppshort(mem, lim, arr/*out*/);
		if (_br_unlikely(!mem))
			return NULL;
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_array_(const char *mem, const char *lim,
	bridge_counter_t count, INT32_TYPE arr[/*count*/])
{
	const INT32_TYPE *const arr_end = arr + count;
	bridge_counter_t no_lim_count = (bridge_counter_t)(lim - mem)/5u;
	if (no_lim_count) {
		if (no_lim_count > count)
			no_lim_count = count;
		{
			INT32_TYPE *_arr = arr;
			arr = _arr + no_lim_count;
			mem = _bridge_unpack_ppint_array_no_lim(mem, no_lim_count, _arr);
		}
	}
	for (; arr < arr_end; arr++) {
		mem = _bridge_unpack_ppint(mem, lim, arr/*out*/);
		if (_br_unlikely(!mem))
			return NULL;
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_array_(const char *mem, const char *lim,
	bridge_counter_t count, INT64_TYPE arr[/*count*/])
{
	const INT64_TYPE *const arr_end = arr + count;
	bridge_counter_t no_lim_count = (bridge_counter_t)(lim - mem)/9u;
	if (no_lim_count) {
		if (no_lim_count > count)
			no_lim_count = count;
		{
			INT64_TYPE *_arr = arr;
			arr = _arr + no_lim_count;
			mem = _bridge_unpack_pplong_array_no_lim(mem, no_lim_count, _arr);
		}
	}
	for (; arr < arr_end; arr++) {
		mem = _bridge_unpack_pplong(mem, lim, arr/*out*/);
		if (_br_unlikely(!mem))
			return NULL;
	}
	return mem;
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pshort_array(const char *mem, const char *lim,
	bridge_counter_t count, INT16_TYPE **arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	*arr = (INT16_TYPE*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_REF_ALIGNED2);
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!*arr))
		return NULL; /* failed */
	return _bridge_unpack_pshort_array_(mem, lim, count, *arr); /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pint_array(const char *mem, const char *lim,
	bridge_counter_t count, INT32_TYPE **arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	*arr = (INT32_TYPE*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_REF_ALIGNED4);
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!*arr))
		return NULL; /* failed */
	return _bridge_unpack_pint_array_(mem, lim, count, *arr); /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_plong_array(const char *mem, const char *lim,
	bridge_counter_t count, INT64_TYPE **arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	*arr = (INT64_TYPE*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_REF_ALIGNED8);
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!*arr))
		return NULL; /* failed */
	return _bridge_unpack_plong_array_(mem, lim, count, *arr); /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_array(const char *mem, const char *lim,
	bridge_counter_t count, INT16_TYPE **arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	*arr = (INT16_TYPE*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_REF_ALIGNED2);
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!*arr))
		return NULL; /* failed */
	return _bridge_unpack_ppshort_array_(mem, lim, count, *arr); /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_array(const char *mem, const char *lim,
	bridge_counter_t count, INT32_TYPE **arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	*arr = (INT32_TYPE*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_REF_ALIGNED4);
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!*arr))
		return NULL; /* failed */
	return _bridge_unpack_ppint_array_(mem, lim, count, *arr); /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_array(const char *mem, const char *lim,
	bridge_counter_t count, INT64_TYPE **arr, BRIDGE_ALLOCATOR_ARG(ac))
{
	*arr = (INT64_TYPE*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), NULL, sizeof(**arr)*count, BRIDGE_ALLOC_REF_ALIGNED8);
	bridge_allocator_dec_level(ac);
	if (_br_unlikely(!*arr))
		return NULL; /* failed */
	return _bridge_unpack_pplong_array_(mem, lim, count, *arr); /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_pshort_array(const char *mem, const char *lim,
	INT16_TYPE **arr, INT16_TYPE **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_pshort_array(mem, lim, count, arr, BRIDGE_PASS_ALLOCATOR(ac));
		BRIDGE_ASSERT(count);
		*arr_end = *arr + count;
	}
	else
		*arr_end = *arr = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_pint_array(const char *mem, const char *lim,
	INT32_TYPE **arr, INT32_TYPE **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_pint_array(mem, lim, count, arr, BRIDGE_PASS_ALLOCATOR(ac));
		BRIDGE_ASSERT(count);
		*arr_end = *arr + count;
	}
	else
		*arr_end = *arr = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_plong_array(const char *mem, const char *lim,
	INT64_TYPE **arr, INT64_TYPE **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_plong_array(mem, lim, count, arr, BRIDGE_PASS_ALLOCATOR(ac));
		BRIDGE_ASSERT(count);
		*arr_end = *arr + count;
	}
	else
		*arr_end = *arr = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_ppshort_array(const char *mem, const char *lim,
	INT16_TYPE **arr, INT16_TYPE **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_ppshort_array(mem, lim, count, arr, BRIDGE_PASS_ALLOCATOR(ac));
		BRIDGE_ASSERT(count);
		*arr_end = *arr + count;
	}
	else
		*arr_end = *arr = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_ppint_array(const char *mem, const char *lim,
	INT32_TYPE **arr, INT32_TYPE **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_ppint_array(mem, lim, count, arr, BRIDGE_PASS_ALLOCATOR(ac));
		BRIDGE_ASSERT(count);
		*arr_end = *arr + count;
	}
	else
		*arr_end = *arr = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS const char *bridge_unpack_pplong_array(const char *mem, const char *lim,
	INT64_TYPE **arr, INT64_TYPE **arr_end, BRIDGE_ALLOCATOR_ARG(ac))
{
	bridge_counter_t count;
	mem = _bridge_unpack_and_check_simple_var_type_counter(mem, lim, sizeof(**arr), &count);
	if (_br_likely(mem) && count) {
		mem = _bridge_unpack_pplong_array(mem, lim, count, arr, BRIDGE_PASS_ALLOCATOR(ac));
		BRIDGE_ASSERT(count);
		*arr_end = *arr + count;
	}
	else
		*arr_end = *arr = NULL;
	bridge_allocator_dec_level(ac);
	return mem; /* NULL if failed */
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS void exchange2_unaligned_array(const void *src/*==dst?*/, void *dst/*==src?*/, size_t count/*0?*/)
{
	const size_t c = count >> 1;
	if (c) {
		size_t i = 0;
		for (; i < c; i++) {
			union {
				UINT32_TYPE t;
				struct _bridge_s4 tt;
			} u;
			u.tt = ((const struct _bridge_s4*)src)[i];
			u.t = hswap4(bswap4(u.t));
			((struct _bridge_s4*)dst)[i] = u.tt;
		}
	}
	if (count & 1) {
		union {
			UINT16_TYPE t;
			struct _bridge_s2 tt;
		} u;
		u.tt = ((const struct _bridge_s2*)src)[count - 1];
		u.t = bswap2(u.t);
		((struct _bridge_s2*)dst)[count - 1] = u.tt;
	}
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS void exchange4_unaligned_array(const void *src/*==dst?*/, void *dst/*==src?*/, size_t count/*0?*/)
{
	const size_t c = count >> 1;
	if (c) {
		size_t i = 0;
		for (; i < c; i++) {
			union {
				UINT64_TYPE t;
				struct _bridge_s8 tt;
			} u;
			u.tt = ((const struct _bridge_s8*)src)[i];
			u.t = hswap8(bswap8(u.t));
			((struct _bridge_s8*)dst)[i] = u.tt;
		}
	}
	if (count & 1) {
		union {
			UINT32_TYPE t;
			struct _bridge_s4 tt;
		} u;
		u.tt = ((const struct _bridge_s4*)src)[count - 1];
		u.t = bswap4(u.t);
		((struct _bridge_s4*)dst)[count - 1] = u.tt;
	}
}

A_Use_decl_annotations
BITBRIDGE_EXPORTS void exchange8_unaligned_array(const void *src/*==dst?*/, void *dst/*==src?*/, size_t count/*0?*/)
{
	size_t i = 0;
	for (; i < count; i++) {
		union {
			UINT64_TYPE t;
			struct _bridge_s8 tt;
		} u;
		u.tt = ((const struct _bridge_s8*)src)[i];
		u.t = bswap8(u.t);
		((struct _bridge_s8*)dst)[i] = u.tt;
	}
}
