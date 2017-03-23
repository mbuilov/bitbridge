#ifndef BITBRIDGE_H_INCLUDED
#define BITBRIDGE_H_INCLUDED

/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/* bitbridge.h */

/* internal header used by generated bridge pack()/unpack() functions */

#include "bitbridge/bitbridge_comn.h"

#ifndef BRIDGE_LIBRARY_IMPL
#ifndef BRIDGE_NEED_RUNTIME_LIB
#error "BRIDGE_NEED_RUNTIME_LIB is not defined! (this header should be included only from bridge-generated source)"
#endif
#if BRIDGE_NEED_RUNTIME_LIB > BRIDGE_RUNTIME_LIB_VERSION
#error "too old bridge runtime library!"
#endif
#endif /* BRIDGE_LIBRARY_IMPL */

#include "bswaps.h"

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define _br_likely(expr)   __builtin_expect(!!(expr),1)
#define _br_unlikely(expr) __builtin_expect(!!(expr),0)
#else
#define _br_likely(expr)   (expr)
#define _br_unlikely(expr) (expr)
#endif

/* declaration for exported functions, such as __declspec(dllexport)/__declspec(dllimport) or __attribute__((visibility("default"))) */
#ifndef BITBRIDGE_EXPORTS
#define BITBRIDGE_EXPORTS
#endif

/* exported functions are suffixed with library version
 - to protect from linking wrong bridge runtime library */
#define _bridge_unpack_bit_array          BRIDGE_EXPORT_SUFFIX(_bridge_unpack_bit_array)
#define bridge_unpack_bit_array           BRIDGE_EXPORT_SUFFIX(bridge_unpack_bit_array)
#define _bridge_unpack_str_               BRIDGE_EXPORT_SUFFIX(_bridge_unpack_str_)
#define _bridge_unpack_pstr_              BRIDGE_EXPORT_SUFFIX(_bridge_unpack_pstr_)
#define _bridge_unpack_str_array_         BRIDGE_EXPORT_SUFFIX(_bridge_unpack_str_array_)
#define _bridge_unpack_pstr_array_        BRIDGE_EXPORT_SUFFIX(_bridge_unpack_pstr_array_)
#define bridge_unpack_str_array           BRIDGE_EXPORT_SUFFIX(bridge_unpack_str_array)
#define bridge_unpack_pstr_array          BRIDGE_EXPORT_SUFFIX(bridge_unpack_pstr_array)
#define _bridge_unpack_str_array_domains  BRIDGE_EXPORT_SUFFIX(_bridge_unpack_str_array_domains)
#define _bridge_unpack_pstr_array_domains BRIDGE_EXPORT_SUFFIX(_bridge_unpack_pstr_array_domains)
#define _bridge_unpack_str_array          BRIDGE_EXPORT_SUFFIX(_bridge_unpack_str_array)
#define _bridge_unpack_pstr_array         BRIDGE_EXPORT_SUFFIX(_bridge_unpack_pstr_array)
#define _bridge_destroy_ptr_array         BRIDGE_EXPORT_SUFFIX(_bridge_destroy_ptr_array)
#define _bridge_delete_ptr_array          BRIDGE_EXPORT_SUFFIX(_bridge_delete_ptr_array)
#define _bridge_delete_ptr_cont           BRIDGE_EXPORT_SUFFIX(_bridge_delete_ptr_cont)
#define _bridge_unpack_pshort_array       BRIDGE_EXPORT_SUFFIX(_bridge_unpack_pshort_array)
#define _bridge_unpack_pint_array         BRIDGE_EXPORT_SUFFIX(_bridge_unpack_pint_array)
#define _bridge_unpack_plong_array        BRIDGE_EXPORT_SUFFIX(_bridge_unpack_plong_array)
#define _bridge_unpack_ppshort_array      BRIDGE_EXPORT_SUFFIX(_bridge_unpack_ppshort_array)
#define _bridge_unpack_ppint_array        BRIDGE_EXPORT_SUFFIX(_bridge_unpack_ppint_array)
#define _bridge_unpack_pplong_array       BRIDGE_EXPORT_SUFFIX(_bridge_unpack_pplong_array)
#define bridge_unpack_pshort_array        BRIDGE_EXPORT_SUFFIX(bridge_unpack_pshort_array)
#define bridge_unpack_pint_array          BRIDGE_EXPORT_SUFFIX(bridge_unpack_pint_array)
#define bridge_unpack_plong_array         BRIDGE_EXPORT_SUFFIX(bridge_unpack_plong_array)
#define bridge_unpack_ppshort_array       BRIDGE_EXPORT_SUFFIX(bridge_unpack_ppshort_array)
#define bridge_unpack_ppint_array         BRIDGE_EXPORT_SUFFIX(bridge_unpack_ppint_array)
#define bridge_unpack_pplong_array        BRIDGE_EXPORT_SUFFIX(bridge_unpack_pplong_array)

#ifdef __cplusplus
extern "C" {
#endif

/* used to count elements in bridge structures and arrays:
  number of elements in bridge array must not exceed BRIDGE_MAX,
  otherwise it will be impossible to pack such array */
typedef A_In_range(<=,BRIDGE_MAX) unsigned int bridge_counter_t; /* <= BRIDGE_MAX */

/* internal structures used while packing */

union _pk_bits_info {
	struct {
		unsigned char *bits;
		unsigned bit_count;
	} p;
};

union _pk_mem_info {
	struct {
		char *mem;
	} p;
};

union _pk_bits_mem_info {
	struct {
		unsigned char *bits;
		unsigned bit_count;
		char *mem;
	} p;
	struct {
		union _pk_bits_info _b;
	} _pb;
	struct {
		union _pk_bits_info __b;
		union _pk_mem_info _m;
	} _pm;
};

union _pk_mem_conv_info {
	struct {
		char *mem;
		int convert;
	} p;
	struct {
		union _pk_mem_info _m;
	} _pm;
};

union _pk_bits_mem_conv_info {
	struct {
		unsigned char *bits;
		unsigned bit_count;
		char *mem;
		int convert;
	} p;
	struct {
		union _pk_bits_info _b;
	} _pb;
	struct {
		union _pk_bits_info __b;
		union _pk_mem_info _m;
	} _pm;
	struct {
		union _pk_bits_mem_info _bm;
	} _pbm;
	struct {
		union _pk_bits_info __b;
		union _pk_mem_conv_info _mc;
	} _pmc;
};

/* internal structures used while unpacking */

union _upk_bits_info {
	struct {
		const unsigned char *bits;
		unsigned bit_count;
		const char *lim;
	} u;
};

union _upk_mem_info {
	struct {
		const char *lim;
		const char *mem;
	} u;
};

union _upk_bits_mem_info {
	struct {
		const unsigned char *bits;
		unsigned bit_count;
		const char *lim;
		const char *mem;
	} u;
	struct {
		union _upk_bits_info _b;
	} _ub;
	struct {
		const unsigned char *_bits;
		unsigned _bit_count;
		union _upk_mem_info _m;
	} _um;
};

union _upk_mem_conv_info {
	struct {
		const char *lim;
		const char *mem;
		int convert;
	} u;
	struct {
		union _upk_mem_info _m;
	} _um;
};

union _upk_bits_mem_conv_info {
	struct {
		const unsigned char *bits;
		unsigned bit_count;
		const char *lim;
		const char *mem;
		int convert;
	} u;
	struct {
		union _upk_bits_info _b;
	} _ub;
	struct {
		const unsigned char *_bits;
		unsigned _bit_count;
		union _upk_mem_info _m;
	} _um;
	struct {
		union _upk_bits_mem_info _bm;
	} _ubm;
	struct {
		const unsigned char *_bits;
		unsigned _bit_count;
		union _upk_mem_conv_info _mc;
	} _umc;
};

union _upk_mem_conv_allocator_info {
	struct {
		const char *lim;
		const char *mem;
		int convert;
		struct bridge_allocator *ac;
	} u;
	struct {
		union _upk_mem_info _m;
	} _um;
	struct {
		union _upk_mem_conv_info _mc;
	} _umc;
};

union _upk_bits_mem_conv_allocator_info {
	struct {
		const unsigned char *bits;
		unsigned bit_count;
		const char *lim;
		const char *mem;
		int convert;
		struct bridge_allocator *ac;
	} u;
	struct {
		union _upk_bits_info _b;
	} _ub;
	struct {
		const unsigned char *_bits;
		unsigned _bit_count;
		union _upk_mem_info _m;
	} _um;
	struct {
		union _upk_bits_mem_info _bm;
	} _ubm;
	struct {
		const unsigned char *_bits;
		unsigned _bit_count;
		union _upk_mem_conv_info _mc;
	} _umc;
	struct {
		union _upk_bits_mem_conv_info _bmc;
	} _ubmc;
	struct {
		const unsigned char *_bits;
		unsigned _bit_count;
		union _upk_mem_conv_allocator_info _mca;
	} _umca;
};

/* smallest long integer type capable to hold two integers */
#ifndef DOUBLE_INTEGER_TYPE
#if defined(__SIZEOF_LONG__) && defined(__SIZEOF_INT__) && __SIZEOF_LONG__ == 2*(__SIZEOF_INT__)
#define DOUBLE_INTEGER_TYPE long int
#else
#define DOUBLE_INTEGER_TYPE long long int
#endif
#endif /* !DOUBLE_INTEGER_TYPE */

/* compile-time check that DOUBLE_INTEGER_TYPE is of proper size */
typedef int _check_DOUBLE_INTEGER_TYPE_t[1-2*(sizeof(DOUBLE_INTEGER_TYPE) != 2*sizeof(int))];

/* internal structure used while determining packed size */
typedef unsigned DOUBLE_INTEGER_TYPE _bridge_bits_bytes_t;

A_Check_return A_Ret_range(==, bb & ~0u)
static inline unsigned _bridge_bits_bytes_get_bytes(_bridge_bits_bytes_t bb)
{
	return (unsigned)(bb & ~0u);
}

A_Check_return A_Ret_range(==, bb >> sizeof(int)*8)
static inline unsigned _bridge_bits_bytes_get_bits(_bridge_bits_bytes_t bb)
{
	return (unsigned)(bb >> sizeof(int)*8);
}

A_Check_return
static inline _bridge_bits_bytes_t _bridge_bits_bytes_from_bits_and_bytes(unsigned bits, unsigned bytes)
{
	_bridge_bits_bytes_t bb = bits;
	bb <<= sizeof(int)*8;
	return bb | bytes;
}

/* add number n to counter c checking for overflow,
  returns 0 on overflow,
  returns 1 otherwise */
A_Check_return A_Ret_range(0,1) A_Success(return == 1) A_Post_satisfies(*c == A_Old(*c) + n)
static inline int _br_maybe_of_add(A_Inout unsigned *c, unsigned n, int may_overflow)
{
#ifdef _DEBUG
	if (~0u - *c < n) {
		BRIDGE_ASSERT(may_overflow);
		return 0; /* will be overflow */
	}
	(*c) += n;
	return 1; /* ok */
#else
	{
		unsigned nn = (*c) += n;
		BRIDGE_ASSERT(may_overflow || nn >= n);
		if (may_overflow)
			return nn >= n;
		return 1; /* ok */
	}
#endif
}

/* add number n to counter c checking for overflow,
  returns 0 on overflow,
  returns 1 otherwise */
#define _br_of_add(c,n) _br_maybe_of_add(c, n, /*may_overflow:*/1)

/* bits processing */

A_Check_return A_Ret_range(==, ((bit_count - 1u) >> 3u) + 1u)
A_Post_satisfies(return <= BRIDGE_BIT_COUNT_MAX/8 + !!(BRIDGE_BIT_COUNT_MAX & 7))
static inline bridge_counter_t bridge_nonzero_bit_array_size(A_In_range(1,BRIDGE_BIT_COUNT_MAX) unsigned bit_count)
{
	BRIDGE_ASSERT(bit_count && bit_count <= BRIDGE_BIT_COUNT_MAX);
	return (((void)0,~0u) - 7u >= BRIDGE_BIT_COUNT_MAX) ?
		(bit_count + 7u) >> 3u : bridge_bytes_for_nonzero_bits(bit_count);
}

A_Check_return A_Ret_range(==, ((bit_count & 1u) + (bit_count >> 1u) + 3u) >> 2u)
A_Post_satisfies(return <= BRIDGE_BIT_COUNT_MAX/8 + !!(BRIDGE_BIT_COUNT_MAX & 7))
static inline bridge_counter_t bridge_bit_array_size(A_In_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned bit_count)
{
	BRIDGE_ASSERT(bit_count <= BRIDGE_BIT_COUNT_MAX);
	return (((void)0,~0u) - 7u >= BRIDGE_BIT_COUNT_MAX) ?
		(bit_count + 7u) >> 3u : bridge_bytes_for_bits(bit_count);
}

/* check that it's possible to unpack sz packed bytes */
A_Nonnull_all_args
A_Check_return
A_Success(return == 1)
A_Ret_range(0,1)
A_Pre_satisfies(mem <= lim)
A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
A_Post_satisfies(sz <= (bridge_counter_t)(lim - mem))
static inline int upk_check_mem_lim(A_In const char *mem, A_In const char *lim,
	A_In_range(<=,BRIDGE_MAX) bridge_counter_t sz)
{
	BRIDGE_ASSERT((size_t)(lim - mem) <= BRIDGE_MAX);
	return sz <= (bridge_counter_t)(lim - mem);
}

/* check that it's possible to unpack sz*n packed bytes */
A_Nonnull_all_args
A_Check_return
A_Success(return == 1)
A_Ret_range(0,1)
A_Pre_satisfies(mem <= lim)
A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
A_Post_satisfies((unsigned INT32_TYPE)n*sz <= BRIDGE_MAX)
A_Post_satisfies((unsigned INT32_TYPE)n*sz <= (bridge_counter_t)(lim - mem))
static inline int upk_check_mem_lim_n(A_In const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t sz, INT32_TYPE n)
{
	BRIDGE_ASSERT(sz);
	BRIDGE_ASSERT((size_t)(lim - mem) <= BRIDGE_MAX);
	if (sz == 1)
		return (unsigned INT32_TYPE)n <= (bridge_counter_t)(lim - mem);
	return
		_br_likely((unsigned INT32_TYPE)n <= BRIDGE_MAX/sz) &&
		upk_check_mem_lim(mem, lim, (bridge_counter_t)((unsigned INT32_TYPE)n*sz));
}

/* check that it's possible to unpack b packed bits */
A_Nonnull_all_args
A_Check_return
A_Success(return == 1)
A_Ret_range(0,1)
A_Pre_satisfies(bits <= lim)
A_Pre_satisfies((size_t)((const unsigned char*)lim - bits) <= BRIDGE_MAX)
A_Post_satisfies(((((bit_count + b) & 1) + ((bit_count + b) >> 1) + 3) >> 2) <= BRIDGE_MAX)
A_Post_satisfies(((((bit_count + b) & 1) + ((bit_count + b) >> 1) + 3) >> 2) <= (bridge_counter_t)((const unsigned char*)lim - bits))
A_Post_satisfies(bit_count + b <= BRIDGE_BIT_COUNT_MAX)
static inline int upk_check_bits_lim(A_In const unsigned char *bits, A_In const char *lim,
	A_In_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned bit_count, A_In_range(1,BRIDGE_BIT_COUNT_MAX) unsigned b)
{
	BRIDGE_ASSERT(b);
	BRIDGE_ASSERT((size_t)(lim - (const char*)bits) <= BRIDGE_MAX);
	if (BRIDGE_BIT_COUNT_MAX <= ((void)0,~0u) - BRIDGE_BIT_COUNT_MAX) {
		bit_count += b;
		if (_br_unlikely(b > BRIDGE_BIT_COUNT_MAX))
			return 0;
	}
	else if (BRIDGE_BIT_COUNT_MAX != ((void)0,~0u)) {
		if (_br_unlikely(b > BRIDGE_BIT_COUNT_MAX - bit_count))
			return 0;
		bit_count += b;
	}
	else if (_br_unlikely(!_br_of_add(&bit_count, b)))
		return 0;
	return bridge_nonzero_bit_array_size(bit_count) <= (bridge_counter_t)(lim - (const char*)bits);
}

/* statically check that b is non-zero */
#define upk_check_bits_lim_nz(bits, lim, bit_count, b) upk_check_bits_lim(bits, lim, bit_count, b + 0*(unsigned)sizeof(int[1-2*!(b)]))

/* check that it's possible to unpack b*n packed bits */
A_Nonnull_all_args
A_Check_return
A_Success(return == 1)
A_Ret_range(0,1)
A_Pre_satisfies(bits <= lim)
A_Pre_satisfies((size_t)((const unsigned char*)lim - bits) <= BRIDGE_MAX)
A_Post_satisfies(((((bit_count + (unsigned INT32_TYPE)n*b) & 1) +
	((bit_count + (unsigned INT32_TYPE)n*b) >> 1) + 3) >> 2) <= BRIDGE_MAX)
A_Post_satisfies(((((bit_count + (unsigned INT32_TYPE)n*b) & 1) +
	((bit_count + (unsigned INT32_TYPE)n*b) >> 1) + 3) >> 2) <= (bridge_counter_t)((const unsigned char*)lim - bits))
A_Post_satisfies(bit_count + (unsigned INT32_TYPE)n*b <= BRIDGE_BIT_COUNT_MAX)
static inline int upk_check_bits_lim_n(A_In const unsigned char *bits, A_In const char *lim,
	A_In_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned bit_count,
	A_In_range(1,BRIDGE_BIT_COUNT_MAX) unsigned b, A_In_range(1,BRIDGE_MAX) bridge_counter_t n)
{
	BRIDGE_ASSERT(b);
	BRIDGE_ASSERT(n);
	BRIDGE_ASSERT((size_t)(lim - (const char*)bits) <= BRIDGE_MAX);
	if (b == 1) {
		if (BRIDGE_MAX <= ((void)0,~0u) - BRIDGE_BIT_COUNT_MAX) {
			bit_count += n;
			if (_br_unlikely(bit_count > BRIDGE_BIT_COUNT_MAX))
				return 0;
		}
		else if (BRIDGE_BIT_COUNT_MAX != ((void)0,~0u)) {
			if (_br_unlikely(n > BRIDGE_BIT_COUNT_MAX - bit_count))
				return 0;
			bit_count += n;
		}
		else if (_br_unlikely(!_br_of_add(&bit_count, n)))
			return 0;
		return bridge_nonzero_bit_array_size(bit_count) <= (bridge_counter_t)(lim - (const char*)bits);
	}
	return
		_br_likely(n <= BRIDGE_BIT_COUNT_MAX/b) &&
		upk_check_bits_lim(bits, lim, bit_count, n*b);
}

/* assume array item is variable-sized with packed size >= packed,
  check that number of items is in limits and we can allocate an array of that number of items */
A_Check_return
A_Success(return == 1)
A_Ret_range(0,1)
A_Post_satisfies((unsigned INT32_TYPE)c*packed <= BRIDGE_MAX)
A_Post_satisfies((unsigned INT32_TYPE)c*item_size <= (size_t)~(size_t)0)
static inline int _bridge_check_array_counter(
	A_In_range(>,0) size_t item_size, INT32_TYPE c, A_In_range(1,BRIDGE_MAX) bridge_counter_t packed)
{
	return
		_br_likely((unsigned INT32_TYPE)c <= BRIDGE_MAX/packed) &&
		_br_likely((unsigned INT32_TYPE)c <= (size_t)~(size_t)0/item_size);
}

/* assume array item is fixed-sized with packed size == packed,
  check that number of items is in limits and we can allocate an array of that number of items */
A_Nonnull_all_args
A_Check_return
A_Success(return == 1)
A_Ret_range(0,1)
A_Pre_satisfies(mem <= lim)
A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
A_Post_satisfies(item_size*(unsigned INT32_TYPE)c <= (size_t)~(size_t)0)
A_Post_satisfies(packed*(unsigned INT32_TYPE)c <= (bridge_counter_t)(lim - mem))
A_Post_satisfies(packed*(unsigned INT32_TYPE)c <= BRIDGE_MAX)
static inline int _bridge_check_fixed_array_counter(A_In const char *mem, A_In const char *lim,
	A_In_range(>,0) size_t item_size, INT32_TYPE c, A_In_range(1,BRIDGE_MAX) bridge_counter_t packed)
{
	if (_br_unlikely(!upk_check_mem_lim_n(mem, lim, packed, c)))
		return 0;
	/* (unsigned INT32_TYPE)c <= (bridge_counter_t)(lim - mem)/packed, max c == BRIDGE_MAX/packed */
	if (_br_unlikely(
		(size_t)~(size_t)0/item_size < BRIDGE_MAX/packed &&
		(size_t)~(size_t)0/item_size < (bridge_counter_t)(unsigned INT32_TYPE)c))
		return 0;
	return 1; /* ok */
}

/* copy array of unaligned 2,4 or 8-byte integers, swap integers byte order while copying */

/* NOTE: src may be == dst */
A_Nonnull_all_args
BITBRIDGE_EXPORTS void exchange2_unaligned_array(
	A_In_reads_bytes(count*2) const void *src/*==dst?*/, A_Out_writes_bytes_all(count*2) void *dst/*==src?*/, size_t count);

/* NOTE: src may be == dst */
A_Nonnull_all_args
BITBRIDGE_EXPORTS void exchange4_unaligned_array(
	A_In_reads_bytes(count*4) const void *src/*==dst?*/, A_Out_writes_bytes_all(count*4) void *dst/*==src?*/, size_t count);

/* NOTE: src may be == dst */
A_Nonnull_all_args
BITBRIDGE_EXPORTS void exchange8_unaligned_array(
	A_In_reads_bytes(count*8) const void *src/*==dst?*/, A_Out_writes_bytes_all(count*8) void *dst/*==src?*/, size_t count);

/* unpack array of unaligned 2,4 or 8-byte integers, swap integers byte order while copying */

A_Nonnull_all_args
static inline void get_exchange_short_array(
	A_In_reads_bytes(count*sizeof(INT16_TYPE)) const void *src/*==dst?*/,
	A_Out_writes_bytes_all(count*sizeof(INT16_TYPE)) INT16_TYPE *dst/*==src?*/,
	size_t count)
{
	exchange2_unaligned_array(src, dst, count);
}

A_Nonnull_all_args
static inline void get_exchange_int_array(
	A_In_reads_bytes(count*sizeof(INT32_TYPE)) const void *src/*==dst?*/,
	A_Out_writes_bytes_all(count*sizeof(INT32_TYPE)) INT32_TYPE *dst/*==src?*/,
	size_t count)
{
	exchange4_unaligned_array(src, dst, count);
}

A_Nonnull_all_args
static inline void get_exchange_long_array(
	A_In_reads_bytes(count*sizeof(INT64_TYPE)) const void *src/*==dst?*/,
	A_Out_writes_bytes_all(count*sizeof(INT64_TYPE)) INT64_TYPE *dst/*==src?*/,
	size_t count)
{
	exchange8_unaligned_array(src, dst, count);
}

A_Nonnull_all_args
static inline void get_exchange_float_array(
	A_In_reads_bytes(count*sizeof(float)) const void *src/*==dst?*/,
	A_Out_writes_bytes_all(count*sizeof(float)) float *dst/*==src?*/,
	size_t count)
{
	void *d = dst;
	(void)sizeof(int(*)[1-2*(sizeof(float) != sizeof(INT32_TYPE))]);
	get_exchange_int_array(src, (INT32_TYPE*)d, count);
}

A_Nonnull_all_args
static inline void get_exchange_double_array(
	A_In_reads_bytes(count*sizeof(double)) const void *src/*==dst?*/,
	A_Out_writes_bytes_all(count*sizeof(double)) double *dst/*==src?*/,
	size_t count)
{
	void *d = dst;
	(void)sizeof(int(*)[1-2*(sizeof(double) != sizeof(INT64_TYPE))]);
	get_exchange_long_array(src, (INT64_TYPE*)d, count);
}

/* pack array of unaligned 2,4 or 8-byte integers, swap integers byte order while copying */

A_Nonnull_all_args
static inline void put_exchange_short_array(
	A_In_reads_bytes(count*sizeof(INT16_TYPE)) const INT16_TYPE *A_Restrict src/*!=dst*/,
	A_Out_writes_bytes_all(count*sizeof(INT16_TYPE)) void *A_Restrict dst/*!=src*/,
	size_t count)
{
	exchange2_unaligned_array(src, dst, count);
}

A_Nonnull_all_args
static inline void put_exchange_int_array(
	A_In_reads_bytes(count*sizeof(INT32_TYPE)) const INT32_TYPE *A_Restrict src/*!=dst*/,
	A_Out_writes_bytes_all(count*sizeof(INT32_TYPE)) void *A_Restrict dst/*!=src*/,
	size_t count)
{
	exchange4_unaligned_array(src, dst, count);
}

A_Nonnull_all_args
static inline void put_exchange_long_array(
	A_In_reads_bytes(count*sizeof(INT64_TYPE)) const INT64_TYPE *A_Restrict src/*!=dst*/,
	A_Out_writes_bytes_all(count*sizeof(INT64_TYPE)) void *A_Restrict dst/*!=src*/,
	size_t count)
{
	exchange8_unaligned_array(src, dst, count);
}

A_Nonnull_all_args
static inline void put_exchange_float_array(
	A_In_reads_bytes(count*sizeof(float)) const float *A_Restrict src/*!=dst*/,
	A_Out_writes_bytes_all(count*sizeof(float)) void *A_Restrict dst/*!=src*/,
	size_t count)
{
	const void *s = src;
	(void)sizeof(int(*)[1-2*(sizeof(float) != sizeof(INT32_TYPE))]);
	put_exchange_int_array((const INT32_TYPE*)s, dst, count);
}

A_Nonnull_all_args
static inline void put_exchange_double_array(
	A_In_reads_bytes(count*sizeof(double)) const double *A_Restrict src/*!=dst*/,
	A_Out_writes_bytes_all(count*sizeof(double)) void *A_Restrict dst/*!=src*/,
	size_t count)
{
	const void *s = src;
	(void)sizeof(int(*)[1-2*(sizeof(double) != sizeof(INT64_TYPE))]);
	put_exchange_long_array((const INT64_TYPE*)s, dst, count);
}

/* put/get numbers to/from memory buffer */

struct _bridge_s2 {char tt[2];};
struct _bridge_s4 {char tt[4];};
struct _bridge_s8 {char tt[8];};

A_Nonnull_all_args
static inline void _bridge_put_byte_at(A_Out_writes_bytes_all(sizeof(n)) char *at, INT8_TYPE n)
{
	*(INT8_TYPE*)at = n;
}

A_Nonnull_all_args
static inline void _bridge_put_short_at(A_Out_writes_bytes_all(sizeof(n)) char *at, int conv, INT16_TYPE n)
{
	union {
		UINT16_TYPE t;
		struct _bridge_s2 tt;
	} u;
	u.t = conv ? bswap2((UINT16_TYPE)n) : (UINT16_TYPE)n;
	*(struct _bridge_s2*)at = u.tt;
}

A_Nonnull_all_args
static inline void _bridge_put_int_at(A_Out_writes_bytes_all(sizeof(n)) char *at, int conv, INT32_TYPE n)
{
	union {
		UINT32_TYPE t;
		struct _bridge_s4 tt;
	} u;
	u.t = conv ? bswap4((UINT32_TYPE)n) : (UINT32_TYPE)n;
	*(struct _bridge_s4*)at = u.tt;
}

A_Nonnull_all_args
static inline void _bridge_put_long_at(A_Out_writes_bytes_all(sizeof(n)) char *at, int conv, INT64_TYPE n)
{
	union {
		UINT64_TYPE t;
		struct _bridge_s8 tt;
	} u;
	u.t = conv ? bswap8((UINT64_TYPE)n) : (UINT64_TYPE)n;
	*(struct _bridge_s8*)at = u.tt;
}

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(==,A_Old(*mem)) A_Post_satisfies(*mem == A_Old(*mem) + sz)
static inline char *_bridge_advance_mem(A_Inout A_At(*A_Curr, A_Notnull) char **mem, A_In_range(>,0) size_t sz)
{
	char *at = *mem;
	*mem = at + sz;
	return at;
}

A_Nonnull_all_args A_Post_satisfies(*mem == A_Old(*mem) + sizeof(n))
static inline void _bridge_put_byte(A_Inout A_At(*A_Curr, A_Out_writes_bytes_all(sizeof(n))) char **mem, INT8_TYPE n)
{
	_bridge_put_byte_at(_bridge_advance_mem(mem, sizeof(n)), n);
}

A_Nonnull_all_args A_Post_satisfies(*mem == A_Old(*mem) + sizeof(n))
static inline void _bridge_put_short(A_Inout A_At(*A_Curr, A_Out_writes_bytes_all(sizeof(n))) char **mem, int conv, INT16_TYPE n)
{
	_bridge_put_short_at(_bridge_advance_mem(mem, sizeof(n)), conv, n);
}

A_Nonnull_all_args A_Post_satisfies(*mem == A_Old(*mem) + sizeof(n))
static inline void _bridge_put_int(A_Inout A_At(*A_Curr, A_Out_writes_bytes_all(sizeof(n))) char **mem, int conv, INT32_TYPE n)
{
	_bridge_put_int_at(_bridge_advance_mem(mem, sizeof(n)), conv, n);
}

A_Nonnull_all_args A_Post_satisfies(*mem == A_Old(*mem) + sizeof(n))
static inline void _bridge_put_long(A_Inout A_At(*A_Curr, A_Out_writes_bytes_all(sizeof(n))) char **mem, int conv, INT64_TYPE n)
{
	_bridge_put_long_at(_bridge_advance_mem(mem, sizeof(n)), conv, n);
}

A_Nonnull_all_args A_Check_return A_Post_satisfies(*mem == A_Old(*mem) + sizeof(INT8_TYPE))
static inline INT8_TYPE _bridge_get_byte_no_lim(A_Inout A_At(*A_Curr, A_In_reads_bytes(sizeof(INT8_TYPE))) const char **mem)
{
	return (INT8_TYPE)*(*mem)++;
}

A_Nonnull_all_args A_Check_return A_Post_satisfies(*mem == A_Old(*mem) + sizeof(INT16_TYPE))
static inline INT16_TYPE _bridge_get_short_no_lim(A_Inout A_At(*A_Curr, A_In_reads_bytes(sizeof(INT16_TYPE))) const char **mem, int conv)
{
	const void *m = mem;
	union {
		UINT16_TYPE t;
		struct _bridge_s2 tt;
	} u;
	u.tt = *(*(const struct _bridge_s2**)m)++;
	return (INT16_TYPE)(conv ? bswap2(u.t) : u.t);
}

A_Nonnull_all_args A_Check_return A_Post_satisfies(*mem == A_Old(*mem) + sizeof(INT32_TYPE))
static inline INT32_TYPE _bridge_get_int_no_lim(A_Inout A_At(*A_Curr, A_In_reads_bytes(sizeof(INT32_TYPE))) const char **mem, int conv)
{
	const void *m = mem;
	union {
		UINT32_TYPE t;
		struct _bridge_s4 tt;
	} u;
	u.tt = *(*(const struct _bridge_s4**)m)++;
	return (INT32_TYPE)(conv ? bswap4(u.t) : u.t);
}

A_Nonnull_all_args A_Check_return A_Post_satisfies(*mem == A_Old(*mem) + sizeof(INT64_TYPE))
static inline INT64_TYPE _bridge_get_long_no_lim(A_Inout A_At(*A_Curr, A_In_reads_bytes(sizeof(INT64_TYPE))) const char **mem, int conv)
{
	const void *m = mem;
	union {
		UINT64_TYPE t;
		struct _bridge_s8 tt;
	} u;
	u.tt = *(*(const struct _bridge_s8**)m)++;
	return (INT64_TYPE)(conv ? bswap8(u.t) : u.t);
}

A_Nonnull_all_args
static inline void _bridge_put_float_at(A_Out_writes_bytes_all(sizeof(n)) char *at, int conv, float n)
{
	union {
		float f;
		INT32_TYPE i;
	} u;
	u.f = n;
	(void)sizeof(int(*)[1-2*(sizeof(float) != sizeof(INT32_TYPE))]);
	_bridge_put_int_at(at, conv, u.i);
}

A_Nonnull_all_args
static inline void _bridge_put_double_at(A_Out_writes_bytes_all(sizeof(n)) char *at, int conv, double n)
{
	union {
		double d;
		INT64_TYPE l;
	} u;
	u.d = n;
	(void)sizeof(int(*)[1-2*(sizeof(double) != sizeof(INT64_TYPE))]);
	_bridge_put_long_at(at, conv, u.l);
}

A_Nonnull_all_args A_Post_satisfies(*mem == A_Old(*mem) + sizeof(n))
static inline void _bridge_put_float(A_Inout A_At(*A_Curr, A_Out_writes_bytes_all(sizeof(n))) char **mem, int conv, float n)
{
	union {
		float f;
		INT32_TYPE i;
	} u;
	u.f = n;
	(void)sizeof(int(*)[1-2*(sizeof(float) != sizeof(INT32_TYPE))]);
	_bridge_put_int(mem, conv, u.i);
}

A_Nonnull_all_args A_Post_satisfies(*mem == A_Old(*mem) + sizeof(n))
static inline void _bridge_put_double(A_Inout A_At(*A_Curr, A_Out_writes_bytes_all(sizeof(n))) char **mem, int conv, double n)
{
	union {
		double d;
		INT64_TYPE l;
	} u;
	u.d = n;
	(void)sizeof(int(*)[1-2*(sizeof(double) != sizeof(INT64_TYPE))]);
	_bridge_put_long(mem, conv, u.l);
}

A_Nonnull_all_args A_Check_return A_Post_satisfies(*mem == A_Old(*mem) + sizeof(float))
static inline float _bridge_get_float_no_lim(A_Inout A_At(*A_Curr, A_In_reads_bytes(sizeof(float))) const char **mem, int conv)
{
	union {
		float f;
		INT32_TYPE i;
	} u;
	u.i = _bridge_get_int_no_lim(mem, conv);
	return u.f;
}

A_Nonnull_all_args A_Check_return A_Post_satisfies(*mem == A_Old(*mem) + sizeof(double))
static inline double _bridge_get_double_no_lim(A_Inout A_At(*A_Curr, A_In_reads_bytes(sizeof(double))) const char **mem, int conv)
{
	union {
		double d;
		INT64_TYPE i;
	} u;
	u.i = _bridge_get_long_no_lim(mem, conv);
	return u.d;
}

/* pack likely positive integers */

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 3)
BITBRIDGE_EXPORTS char *_bridge_pack_ppshort(A_Out_writes_to_ptr(return) char *mem, INT16_TYPE n);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 5)
BITBRIDGE_EXPORTS char *_bridge_pack_ppint(A_Out_writes_to_ptr(return) char *mem, INT32_TYPE n);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 9)
BITBRIDGE_EXPORTS char *_bridge_pack_pplong(A_Out_writes_to_ptr(return) char *mem, INT64_TYPE n);

/* unpack likely positive integers */

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 3)
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_no_lim(A_In const char *mem, A_Out INT16_TYPE *n/*out*/);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 5)
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_no_lim(A_In const char *mem, A_Out INT32_TYPE *n/*out*/);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 9)
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_no_lim(A_In const char *mem, A_Out INT64_TYPE *n/*out*/);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, mem + 3) A_Post_satisfies(return <= lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim, A_Out INT16_TYPE *n/*out*/);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, mem + 5) A_Post_satisfies(return <= lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim, A_Out INT32_TYPE *n/*out*/);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, mem + 9) A_Post_satisfies(return <= lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim, A_Out INT64_TYPE *n/*out*/);

/* pack integers */

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 3)
static inline char *_bridge_pack_pshort(A_Out_writes_to_ptr(return) char *mem, INT16_TYPE n)
{
	n = (INT16_TYPE)((INT16_TYPE)(unsigned INT16_TYPE)~(unsigned INT16_TYPE)0 &
		((INT16_TYPE)(((unsigned INT16_TYPE)n & (((unsigned INT16_TYPE)~(unsigned INT16_TYPE)0) >> 1)) << 1) ^ (n >> 15)));
	return _bridge_pack_ppshort(mem, n);
}

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 5)
static inline char *_bridge_pack_pint(A_Out_writes_to_ptr(return) char *mem, INT32_TYPE n)
{
	n = (INT32_TYPE)((INT32_TYPE)(unsigned INT32_TYPE)~(unsigned INT32_TYPE)0 &
		((INT32_TYPE)(((unsigned INT32_TYPE)n & (((unsigned INT32_TYPE)~(unsigned INT32_TYPE)0) >> 1)) << 1) ^ (n >> 31)));
	return _bridge_pack_ppint(mem, n);
}

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 9)
static inline char *_bridge_pack_plong(A_Out_writes_to_ptr(return) char *mem, INT64_TYPE n)
{
	n = (INT64_TYPE)((INT64_TYPE)(unsigned INT64_TYPE)~(unsigned INT64_TYPE)0 &
		((INT64_TYPE)(((unsigned INT64_TYPE)n & (((unsigned INT64_TYPE)~(unsigned INT64_TYPE)0) >> 1)) << 1) ^ (n >> 63)));
	return _bridge_pack_pplong(mem, n);
}

/* unpack integers */

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 3)
static inline const char *_bridge_unpack_pshort_no_lim(A_In const char *mem, A_Out INT16_TYPE *n/*out*/)
{
	mem = _bridge_unpack_ppshort_no_lim(mem, n/*out*/);
	*n = (INT16_TYPE)(((INT16_TYPE)((unsigned INT16_TYPE)(*n) >> 1) ^ -(*n & 1)) &
		(INT16_TYPE)(unsigned INT16_TYPE)~(unsigned INT16_TYPE)0);
	return mem;
}

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 5)
static inline const char *_bridge_unpack_pint_no_lim(A_In const char *mem, A_Out INT32_TYPE *n/*out*/)
{
	mem = _bridge_unpack_ppint_no_lim(mem, n/*out*/);
	*n = (INT32_TYPE)(((INT32_TYPE)((unsigned INT32_TYPE)(*n) >> 1) ^ -(*n & 1)) &
		(INT32_TYPE)(unsigned INT32_TYPE)~(unsigned INT32_TYPE)0);
	return mem;
}

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 9)
static inline const char *_bridge_unpack_plong_no_lim(A_In const char *mem, A_Out INT64_TYPE *n/*out*/)
{
	mem = _bridge_unpack_pplong_no_lim(mem, n/*out*/);
	*n = (INT64_TYPE)(((INT64_TYPE)((unsigned INT64_TYPE)(*n) >> 1) ^ -(*n & 1)) &
		(INT64_TYPE)(unsigned INT64_TYPE)~(unsigned INT64_TYPE)0);
	return mem;
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, mem + 3) A_Post_satisfies(return <= lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static inline const char *_bridge_unpack_pshort(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim, A_Out INT16_TYPE *n/*out*/)
{
	mem = _bridge_unpack_ppshort(mem, lim, n/*out*/);
	if (mem) {
		*n = (INT16_TYPE)(((INT16_TYPE)((unsigned INT16_TYPE)(*n) >> 1) ^ -(*n & 1)) &
			(INT16_TYPE)(unsigned INT16_TYPE)~(unsigned INT16_TYPE)0);
	}
	return mem;
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, mem + 5) A_Post_satisfies(return <= lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static inline const char *_bridge_unpack_pint(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim, A_Out INT32_TYPE *n/*out*/)
{
	mem = _bridge_unpack_ppint(mem, lim, n/*out*/);
	if (mem) {
		*n = (INT32_TYPE)(((INT32_TYPE)((unsigned INT32_TYPE)(*n) >> 1) ^ -(*n & 1)) &
			(INT32_TYPE)(unsigned INT32_TYPE)~(unsigned INT32_TYPE)0);
	}
	return mem;
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, mem + 9) A_Post_satisfies(return <= lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static inline const char *_bridge_unpack_plong(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim, A_Out INT64_TYPE *n/*out*/)
{
	mem = _bridge_unpack_pplong(mem, lim, n/*out*/);
	if (mem) {
		*n = (INT64_TYPE)(((INT64_TYPE)((unsigned INT64_TYPE)(*n) >> 1) ^ -(*n & 1)) &
			(INT64_TYPE)(unsigned INT64_TYPE)~(unsigned INT64_TYPE)0);
	}
	return mem;
}

/* determine packed integers size */

A_Check_return A_Ret_range(1,3)
static inline unsigned bridge_ppshort_packed_size(INT16_TYPE n)
{
	(void)sizeof(int[1-2*(sizeof(unsigned) < sizeof(INT16_TYPE))]); /* check that sizeof(unsigned) >= sizeof(INT16_TYPE) */
	{
		unsigned x = (unsigned)n;
		return 1u + (x > 0x7Fu) + (x > 0x3FFFu);
	}
}

A_Const_function A_Check_return A_Ret_range(1,5)
BITBRIDGE_EXPORTS unsigned bridge_ppint_packed_size(INT32_TYPE n);

A_Const_function A_Check_return A_Ret_range(1,9)
BITBRIDGE_EXPORTS unsigned bridge_pplong_packed_size(INT64_TYPE n);

A_Check_return A_Ret_range(1,3)
static inline unsigned bridge_pshort_packed_size(INT16_TYPE n)
{
	n = (INT16_TYPE)((INT16_TYPE)(unsigned INT16_TYPE)~(unsigned INT16_TYPE)0 &
		((INT16_TYPE)(((unsigned INT16_TYPE)n & (((unsigned INT16_TYPE)~(unsigned INT16_TYPE)0) >> 1)) << 1) ^ (n >> 15)));
	return bridge_ppshort_packed_size(n);
}

A_Check_return A_Ret_range(1,5)
static inline unsigned bridge_pint_packed_size(INT32_TYPE n)
{
	n = (INT32_TYPE)((INT32_TYPE)(unsigned INT32_TYPE)~(unsigned INT32_TYPE)0 &
		((INT32_TYPE)(((unsigned INT32_TYPE)n & (((unsigned INT32_TYPE)~(unsigned INT32_TYPE)0) >> 1)) << 1) ^ (n >> 31)));
	return bridge_ppint_packed_size(n);
}

A_Check_return A_Ret_range(1,9)
static inline unsigned bridge_plong_packed_size(INT64_TYPE n)
{
	n = (INT64_TYPE)((INT64_TYPE)(unsigned INT64_TYPE)~(unsigned INT64_TYPE)0 &
		((INT64_TYPE)(((unsigned INT64_TYPE)n & (((unsigned INT64_TYPE)~(unsigned INT64_TYPE)0) >> 1)) << 1) ^ (n >> 63)));
	return bridge_pplong_packed_size(n);
}

/* pack/unpack counter */

/* counter coded in this way:
 * 01111111                                             0-127        (1 byte)
 * 11111111 01111111                                  128-16383      (2 bytes)
 * 11111111 11111111 01111111                       16384-2097151    (3 bytes)
 * 11111111 11111111 11111111 01111111            2097151-268435455  (4 bytes)
 * 11111111 11111111 11111111 11111111 00000111 268435456-2147483647 (5 bytes) */

#define BRIDGE_COUNTER_MAX_PACKED_SIZE (BRIDGE_MAX <= 0x3FFFu ? 2 : BRIDGE_MAX <= 0x1FFFFFu ? 3 : 5)

/* ensure we may always add bridge_counter_t packed size to bridge_counter_t without unsigned integer overflow */
typedef int _bridge_max_is_too_big_[1-2*(BRIDGE_MAX >= ~0u - BRIDGE_COUNTER_MAX_PACKED_SIZE)];

A_Check_return A_Ret_range(1,5)
static inline unsigned bridge_counter_packed_size(A_In_range(<=,BRIDGE_MAX) bridge_counter_t c)
{
	BRIDGE_ASSERT(c <= BRIDGE_MAX);
	if (((void)0,BRIDGE_MAX) <= (unsigned INT16_TYPE)~(unsigned INT16_TYPE)0)
		return bridge_ppshort_packed_size((INT16_TYPE)c);
	return bridge_ppint_packed_size((INT32_TYPE)c);
}

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 5)
static inline char *_bridge_pack_counter(A_Out_writes_to_ptr(return) char *mem, A_In_range(<=,BRIDGE_MAX) bridge_counter_t c)
{
	BRIDGE_ASSERT(c <= BRIDGE_MAX);
	if (((void)0,BRIDGE_MAX) <= (unsigned INT16_TYPE)~(unsigned INT16_TYPE)0)
		return _bridge_pack_ppshort(mem, (INT16_TYPE)c);
	return _bridge_pack_ppint(mem, (INT32_TYPE)c);
}

A_Nonnull_all_args A_Check_return A_Ret_range(mem + 1, mem + 5)
static inline const char *_bridge_unpack_counter_no_lim(A_In const char *mem, A_Out INT32_TYPE *c/*out*/)
{
	return _bridge_unpack_ppint_no_lim(mem, c/*out*/);
}

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, mem + 5)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Post_satisfies(return <= lim)
static inline const char *_bridge_unpack_counter(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim, A_Out INT32_TYPE *c/*out*/)
{
	return _bridge_unpack_ppint(mem, lim, c/*out*/);
}

A_Check_return A_Ret_range(<=, 5 + BRIDGE_BIT_COUNT_MAX/8 + !!(BRIDGE_BIT_COUNT_MAX & 7))
static inline unsigned bridge_bit_array_packed_size(A_In_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned bit_count)
{
	unsigned sz;
	BRIDGE_ASSERT(bit_count <= BRIDGE_BIT_COUNT_MAX);
	sz = (((void)0,BRIDGE_BIT_COUNT_MAX) <= (unsigned INT16_TYPE)~(unsigned INT16_TYPE)0) ?
		bridge_ppshort_packed_size((INT16_TYPE)bit_count) : bridge_ppint_packed_size((INT32_TYPE)bit_count);
	sz += bridge_bit_array_size(bit_count);
	return sz;
}

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(mem + 1, mem + 5)
static inline char *_bridge_pack_bit_counter(
	A_Out_writes_to_ptr(return) char *mem, A_In_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned bit_count)
{
	BRIDGE_ASSERT(bit_count <= BRIDGE_BIT_COUNT_MAX);
	if (((void)0,BRIDGE_BIT_COUNT_MAX) <= (unsigned INT16_TYPE)~(unsigned INT16_TYPE)0)
		return _bridge_pack_ppshort(mem, (INT16_TYPE)bit_count);
	return _bridge_pack_ppint(mem, (INT32_TYPE)bit_count);
}

/* pack number of bits in array */
/* pack array of bits */
A_Nonnull_arg(1) A_Check_return A_Ret_never_null
A_Ret_range(>,mem + (((bit_count & 1) + (bit_count >> 1) + 3) >> 2))
A_Pre_satisfies(!arr == !bit_count)
BITBRIDGE_EXPORTS char *bridge_pack_bit_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_bytes_opt(((bit_count & 1) + (bit_count >> 1) + 3) >> 2) const unsigned char arr[],
	A_In_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned bit_count);

/* allocate and copy array of bits */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(==,mem + bit_bytes)
BITBRIDGE_EXPORTS const char *_bridge_unpack_bit_array(A_In_reads_bytes(bit_bytes) const char *mem,
	A_In_range(1, BRIDGE_BIT_COUNT_MAX/8 + !!(BRIDGE_BIT_COUNT_MAX & 7)) bridge_counter_t bit_bytes,
	A_Outptr A_At(*A_Curr, A_Post_readable_size(bit_bytes)) unsigned char **s_array,
	BRIDGE_ALLOCATOR_ARG(ac));

/* unpack bit counter, allocate and copy array of bits */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
BITBRIDGE_EXPORTS const char *bridge_unpack_bit_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Out_range(<=,BRIDGE_BIT_COUNT_MAX) unsigned *s_bit_count,
	A_Out A_Post A_At(*A_Curr, A_When(!*s_bit_count, A_Null) A_When(*s_bit_count, A_Notnull)) unsigned char **s_array,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline int bridge_append_byte_array_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d,
	A_In_opt const INT8_TYPE *arr, A_In_opt const INT8_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_unlikely(count > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/sizeof(*arr)))
		return -1;
	if (_br_unlikely(!_br_of_add(c, bridge_counter_packed_size((bridge_counter_t)count) - d + (bridge_counter_t)(count*sizeof(*arr)))))
		return -1;
	return 0;
}

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline int bridge_append_short_array_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d,
	A_In_opt const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	/* NOTE: this check is not valid for BRIDGE_MAX == 0xf7, so BRIDGE_MAX must be == 0xfff7 or bigger */
	if (_br_unlikely(count > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/sizeof(*arr)))
		return -1;
	if (_br_unlikely(!_br_of_add(c, bridge_counter_packed_size((bridge_counter_t)count) - d + (bridge_counter_t)(count*sizeof(*arr)))))
		return -1;
	return 0;
}

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline int bridge_append_int_array_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d,
	A_In_opt const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_unlikely(count > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/sizeof(*arr)))
		return -1;
	if (_br_unlikely(!_br_of_add(c, bridge_counter_packed_size((bridge_counter_t)count) - d + (bridge_counter_t)(count*sizeof(*arr)))))
		return -1;
	return 0;
}

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline int bridge_append_long_array_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d,
	A_In_opt const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_unlikely(count > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/sizeof(*arr)))
		return -1;
	if (_br_unlikely(!_br_of_add(c, bridge_counter_packed_size((bridge_counter_t)count) - d + (bridge_counter_t)(count*sizeof(*arr)))))
		return -1;
	return 0;
}

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline int bridge_append_float_array_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d,
	A_In_opt const float *arr, A_In_opt const float *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_unlikely(count > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/sizeof(*arr)))
		return -1;
	if (_br_unlikely(!_br_of_add(c, bridge_counter_packed_size((bridge_counter_t)count) - d + (bridge_counter_t)(count*sizeof(*arr)))))
		return -1;
	return 0;
}

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline int bridge_append_double_array_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d,
	A_In_opt const double *arr, A_In_opt const double *const arr_end)
{
	size_t count = (size_t)(arr_end - arr);
	if (_br_unlikely(count > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/sizeof(*arr)))
		return -1;
	if (_br_unlikely(!_br_of_add(c, bridge_counter_packed_size((bridge_counter_t)count) - d + (bridge_counter_t)(count*sizeof(*arr)))))
		return -1;
	return 0;
}

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0)
static inline int _bridge_append_str_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d, A_In_opt_z const char *str)
{
	size_t len = str ? BRIDGE_STRLEN(str) : 0;
	/* note: need one byte more for terminating '\0' */
	if (_br_unlikely(len >= BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE))
		return -1;
	if (len)
		len += bridge_counter_packed_size((bridge_counter_t)len);
	BRIDGE_ASSERT(len + 1/*'\0'*/ <= BRIDGE_MAX);
	if (_br_unlikely(!_br_of_add(c, (bridge_counter_t)len + 1u/*'\0'*/ - d)))
		return -1;
	return 0;
}

A_Nonnull_arg(1) A_Check_return A_Ret_range(-1,0)
static inline int _bridge_append_pstr_packed_size(A_Inout unsigned *c, A_In_range(0,1) unsigned d, A_In_opt_z const char *str)
{
	size_t sz = (str ? BRIDGE_STRLEN(str) : 0) + 1/*'\0'*/;
	if (_br_unlikely(sz > ((void)0,~0u)))
		return -1;
	if (_br_unlikely(!_br_of_add(c, (unsigned)sz - d)))
		return -1;
	return 0;
}

/* on failure returns value > BRIDGE_MAX (array is too big) */
A_Pure_function A_Check_return A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_add_str_array_packed_size(unsigned c,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

/* on failure returns value > BRIDGE_MAX (array is too big) */
A_Pure_function A_Check_return A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_add_pstr_array_packed_size(unsigned c,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

/* count packed number of array elements */
/* on failure returns value > BRIDGE_MAX (array is too big) */
A_Pure_function A_Check_return A_Ret_range(>,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_add_str_array_packed_size(unsigned c,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

/* count packed number of array elements */
/* on failure returns value > BRIDGE_MAX (array is too big) */
A_Pure_function A_Check_return A_Ret_range(>,0) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_add_pstr_array_packed_size(unsigned c,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

#define _bridge_str_array_packed_size(arr, arr_end) _bridge_add_str_array_packed_size(0, arr, arr_end)
#define _bridge_pstr_array_packed_size(arr, arr_end) _bridge_add_pstr_array_packed_size(0, arr, arr_end)

/* pack string length + '\0'-terminated string */
A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem)
BITBRIDGE_EXPORTS char *_bridge_pack_str(A_Out_writes_to_ptr(return) char *A_Restrict mem, A_In_opt_z const char *str);

/* pack '\0'-terminated string */
A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem)
BITBRIDGE_EXPORTS char *_bridge_pack_pstr(A_Out_writes_to_ptr(return) char *A_Restrict mem, A_In_opt_z const char *str);

/* pack array of strings without elements counter */
A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_str_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

/* pack array of strings without elements counter */
A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_pstr_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

/* pack number of array elements */
/* pack array of strings */
A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_str_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

/* pack number of array elements */
/* pack array of strings */
A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_pstr_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) char *const *arr, A_In_opt char *const *arr_end);

/* unpack string */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Outptr_result_z char **str, BRIDGE_ALLOCATOR_ARG(ac));

/* unpack string */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Outptr_result_z char **str, BRIDGE_ALLOCATOR_ARG(ac));

/* unpack string, NULL'ify destination on error */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static inline const char *_bridge_unpack_str(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Outptr_result_z A_On_failure(A_At(*A_Curr, A_Null)) char **str, BRIDGE_ALLOCATOR_ARG(ac))
{
	mem = _bridge_unpack_str_(mem, lim, str, BRIDGE_PASS_ALLOCATOR(ac));
	if (_br_unlikely(!mem))
		*str = NULL;
	bridge_allocator_dec_level(ac);
	return mem;
}

/* unpack string, NULL'ify destination on error */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
static inline const char *_bridge_unpack_pstr(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Outptr_result_z A_On_failure(A_At(*A_Curr, A_Null)) char **str, BRIDGE_ALLOCATOR_ARG(ac))
{
	mem = _bridge_unpack_pstr_(mem, lim, str, BRIDGE_PASS_ALLOCATOR(ac));
	if (_br_unlikely(!mem))
		*str = NULL;
	bridge_allocator_dec_level(ac);
	return mem;
}

/* unpack array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
A_Post_satisfies(&arr[count] == *arr_end) A_On_failure(A_Post_satisfies(arr <= *arr_end) A_Post_satisfies(*arr_end < &arr[count]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_array_(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) char *arr[/*count*/],
	A_Always(A_Outptr) char ***arr_end, BRIDGE_ALLOCATOR_ARG(ac));

/* unpack array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
A_Post_satisfies(&arr[count] == *arr_end) A_On_failure(A_Post_satisfies(arr <= *arr_end) A_Post_satisfies(*arr_end < &arr[count]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_array_(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) char *arr[/*count*/],
	A_Always(A_Outptr) char ***arr_end, BRIDGE_ALLOCATOR_ARG(ac));

/* unpack array counter, allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
A_Post_satisfies(!*arr || *arr < *arr_end) A_On_failure(A_Post_satisfies(*arr <= *arr_end))
BITBRIDGE_EXPORTS const char *bridge_unpack_str_array(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) char ***arr,
	A_Always(A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull))) char ***arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

/* unpack array counter, allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX)
A_Post_satisfies(!*arr || *arr < *arr_end) A_On_failure(A_Post_satisfies(*arr <= *arr_end))
BITBRIDGE_EXPORTS const char *bridge_unpack_pstr_array(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) char ***arr,
	A_Always(A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull))) char ***arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

/* allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array,
  note: remaining not filled items inside 'domain' array are NULL'ed */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
A_Post_satisfies(*arr < *arr_end) A_On_failure(A_Post_satisfies(*arr <= *arr_end))
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_array_domains(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_On_failure(A_Outptr_result_maybenull) char ***arr,
	A_Outptr A_On_failure(A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull))) char ***arr_end,
	A_In_range(1,count) bridge_counter_t domain, BRIDGE_ALLOCATOR_ARG(ac));

/* allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array,
  note: remaining not filled items inside 'domain' are NULL'ed */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
A_Post_satisfies(*arr < *arr_end) A_On_failure(A_Post_satisfies(*arr <= *arr_end))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_array_domains(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_On_failure(A_Outptr_result_maybenull) char ***arr,
	A_Outptr A_On_failure(A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull))) char ***arr_end,
	A_In_range(1,count) bridge_counter_t domain, BRIDGE_ALLOCATOR_ARG(ac));

/* allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array,
  note: remaining not filled items inside array are NULL'ed */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_str_array(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) char ***arr,
	BRIDGE_ALLOCATOR_ARG(ac));

/* allocate array, then fill array of strings dynamically allocating items,
  returns NULL if failed - caller must destroy partially filled strings array,
  note: remaining not filled items inside array are NULL'ed */
A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pstr_array(
	A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) char ***arr,
	BRIDGE_ALLOCATOR_ARG(ac));

/* free pointers in array */
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS void _bridge_destroy_ptr_array(
	A_When(arr != arr_end, A_At(*A_Curr, A_Pre_opt_valid A_Post_ptr_invalid)) A_In_reads_to_ptr_opt(arr_end) void **arr,
	A_In_opt void **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

/* free pointers in array, then free array itself */
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS void _bridge_delete_ptr_array(
	A_Pre_opt_valid A_Post_ptr_invalid A_In_reads_to_ptr_opt(arr_end) void **arr,
	A_Pre_opt_valid A_Post_ptr_invalid A_In_opt void **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

/* free pointer in container, then free container itself */
BITBRIDGE_EXPORTS void _bridge_delete_ptr_cont(
	A_Pre_opt_valid A_Post_ptr_invalid void **container,
	BRIDGE_ALLOCATOR_ARG(ac));

#define _bridge_destroy_ptr_array_(arr, arr_end, ac) do { \
	void *__a = arr; \
	void *__e = arr_end; \
	/* check that arr is a pointer to pointer and is compatible with arr_end */ \
	(void)(sizeof(**(arr)) + sizeof((*(arr) - *(arr_end)))); \
	_bridge_destroy_ptr_array((void**)__a, (void**)__e, ac); \
} while ((void)0,0)

#define _bridge_delete_ptr_array_(arr, arr_end, ac) do { \
	void *__a = arr; \
	void *__e = arr_end; \
	/* check that arr is a pointer to pointer and is compatible with arr_end */ \
	(void)(sizeof(**(arr)) + sizeof((*(arr) - *(arr_end)))); \
	_bridge_delete_ptr_array((void**)__a, (void**)__e, ac); \
} while ((void)0,0)

#define _bridge_delete_ptr_cont_(container, ac) do { \
	void *__c = container; \
	/* check that container is a pointer to pointer */ \
	(void)sizeof(**(container)); \
	_bridge_delete_ptr_cont((void**)__c, ac); \
} while ((void)0,0)

/* free strings in array */
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline void _bridge_destroy_str_array(
	A_When(arr != arr_end, A_At(*A_Curr, A_Pre_opt_valid A_Post_ptr_invalid)) A_In_reads_to_ptr_opt(arr_end) char **arr,
	A_In_opt char **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac))
{
	_bridge_destroy_ptr_array_(arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
	bridge_allocator_dec_level(ac);
}

/* free strings in array, then free array itself */
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
static inline void _bridge_delete_str_array(
	A_Pre_opt_valid A_Post_ptr_invalid A_In_reads_to_ptr_opt(arr_end) char **arr,
	A_Pre_opt_valid A_Post_ptr_invalid A_In_opt char **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac))
{
	_bridge_delete_ptr_array_(arr, arr_end, BRIDGE_PASS_ALLOCATOR(ac));
	bridge_allocator_dec_level(ac);
}

/* free string in container, then free container itself */
static inline void _bridge_delete_str_cont(
	A_Pre_opt_valid A_Post_ptr_invalid char **container,
	BRIDGE_ALLOCATOR_ARG(ac))
{
	_bridge_delete_ptr_cont_(container, BRIDGE_PASS_ALLOCATOR(ac));
	bridge_allocator_dec_level(ac);
}

/* packed array size without array elements counter */

A_Pure_function A_Check_return A_Ret_range(>=,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_pshort_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>=,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_pint_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>=,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_plong_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>=,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_ppshort_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>=,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_ppint_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>=,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned _bridge_pplong_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

/* packed array size with array elements counter */

A_Pure_function A_Check_return A_Ret_range(>,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_pshort_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_pint_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_plong_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_ppshort_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_ppint_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Pure_function A_Check_return A_Ret_range(>,arr_end - arr) A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS unsigned bridge_pplong_array_packed_size(
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

/* pack array without elements counter */

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_pshort_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_pint_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_plong_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_ppshort_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_ppint_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>=,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *_bridge_pack_pplong_array(A_When(arr < arr_end, A_Out_writes_to_ptr(return)) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

/* pack array with elements counter */

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_pshort_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_pint_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_plong_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_ppshort_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT16_TYPE *arr, A_In_opt const INT16_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_ppint_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT32_TYPE *arr, A_In_opt const INT32_TYPE *const arr_end);

A_Nonnull_arg(1) A_Check_return A_Ret_never_null A_Ret_range(>,mem + (arr_end - arr))
A_Pre_satisfies(arr <= arr_end) A_Pre_satisfies(!arr == !arr_end)
BITBRIDGE_EXPORTS char *bridge_pack_pplong_array(A_Out_writes_to_ptr(return) char *A_Restrict mem,
	A_In_reads_to_ptr_opt(arr_end) const INT64_TYPE *arr, A_In_opt const INT64_TYPE *const arr_end);

/* unpack array items not checking limits */

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(>=,mem + count) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pshort_array_no_lim(A_In const char *mem,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT16_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(>=,mem + count) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pint_array_no_lim(A_In const char *mem,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT32_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(>=,mem + count) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_plong_array_no_lim(A_In const char *mem,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT64_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(>=,mem + count) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_array_no_lim(A_In const char *mem,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT16_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(>=,mem + count) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_array_no_lim(A_In const char *mem,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT32_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(>=,mem + count) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_array_no_lim(A_In const char *mem,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT64_TYPE arr[/*count*/]);

/* unpack array items checking limits */

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pshort_array_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT16_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pint_array_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT32_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_plong_array_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT64_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_array_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT16_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_array_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT32_TYPE arr[/*count*/]);

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(arr[0]))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_array_(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count, A_Out_writes_all(count) INT64_TYPE arr[/*count*/]);

/* allocate array, then unpack array items */

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pshort_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_Post A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) INT16_TYPE **arr,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pint_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_Post A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) INT32_TYPE **arr,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_plong_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_Post A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) INT64_TYPE **arr,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppshort_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_Post A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) INT16_TYPE **arr,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_ppint_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_Post A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) INT32_TYPE **arr,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + count, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Pre_satisfies(count <= (size_t)~(size_t)0/sizeof(**arr))
BITBRIDGE_EXPORTS const char *_bridge_unpack_pplong_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_In_range(1,BRIDGE_MAX) bridge_counter_t count,
	A_Outptr A_Post A_At(*A_Curr, A_Readable_elements(count)) A_On_failure(A_Outptr_result_maybenull) INT64_TYPE **arr,
	BRIDGE_ALLOCATOR_ARG(ac));

/* unpack array elements counter, allocate array, then unpack items */

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Post_satisfies(!*arr || *arr < *arr_end)
BITBRIDGE_EXPORTS const char *bridge_unpack_pshort_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) INT16_TYPE **arr,
	A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull)) INT16_TYPE **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Post_satisfies(!*arr || *arr < *arr_end)
BITBRIDGE_EXPORTS const char *bridge_unpack_pint_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) INT32_TYPE **arr,
	A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull)) INT32_TYPE **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Post_satisfies(!*arr || *arr < *arr_end)
BITBRIDGE_EXPORTS const char *bridge_unpack_plong_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) INT64_TYPE **arr,
	A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull)) INT64_TYPE **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Post_satisfies(!*arr || *arr < *arr_end)
BITBRIDGE_EXPORTS const char *bridge_unpack_ppshort_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) INT16_TYPE **arr,
	A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull)) INT16_TYPE **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Post_satisfies(!*arr || *arr < *arr_end)
BITBRIDGE_EXPORTS const char *bridge_unpack_ppint_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) INT32_TYPE **arr,
	A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull)) INT32_TYPE **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

A_Nonnull_all_args A_Check_return A_Success(return != NULL) A_Ret_range(mem + 1, lim)
A_Pre_satisfies(mem <= lim) A_Pre_satisfies((size_t)(lim - mem) <= BRIDGE_MAX) A_Post_satisfies(!*arr || *arr < *arr_end)
BITBRIDGE_EXPORTS const char *bridge_unpack_pplong_array(A_In_reads_to_ptr(lim) const char *mem, A_In const char *lim,
	A_Always(A_Outptr_result_maybenull) INT64_TYPE **arr,
	A_Out A_Post A_At(*A_Curr, A_When(!*arr, A_Null) A_When(*arr, A_Notnull)) INT64_TYPE **arr_end,
	BRIDGE_ALLOCATOR_ARG(ac));

struct _bridge_s1 {char tt[1];};
struct _bridge_s3 {char tt[3];};
struct _bridge_s5 {char tt[5];};
struct _bridge_s6 {char tt[6];};
struct _bridge_s7 {char tt[7];};

A_Nonnull_all_args
static inline void _br_memcpy1(A_Out_writes_bytes_all(1) void *A_Restrict dst, A_In_reads_bytes(1) const char *A_Restrict src)
{
	*(struct _bridge_s1*)dst = *(const struct _bridge_s1*)src;
}

A_Nonnull_all_args
static inline void _br_memcpy2(A_Out_writes_bytes_all(2) void *A_Restrict dst, A_In_reads_bytes(2) const char *A_Restrict src)
{
	*(struct _bridge_s2*)dst = *(const struct _bridge_s2*)src;
}

A_Nonnull_all_args
static inline void _br_memcpy3(A_Out_writes_bytes_all(3) void *A_Restrict dst, A_In_reads_bytes(3) const char *A_Restrict src)
{
	*(struct _bridge_s3*)dst = *(const struct _bridge_s3*)src;
}

A_Nonnull_all_args
static inline void _br_memcpy4(A_Out_writes_bytes_all(4) void *A_Restrict dst, A_In_reads_bytes(4) const char *A_Restrict src)
{
	*(struct _bridge_s4*)dst = *(const struct _bridge_s4*)src;
}

A_Nonnull_all_args
static inline void _br_memcpy5(A_Out_writes_bytes_all(5) void *A_Restrict dst, A_In_reads_bytes(5) const char *A_Restrict src)
{
	*(struct _bridge_s5*)dst = *(const struct _bridge_s5*)src;
}

A_Nonnull_all_args
static inline void _br_memcpy6(A_Out_writes_bytes_all(6) void *A_Restrict dst, A_In_reads_bytes(6) const char *A_Restrict src)
{
	*(struct _bridge_s6*)dst = *(const struct _bridge_s6*)src;
}

A_Nonnull_all_args
static inline void _br_memcpy7(A_Out_writes_bytes_all(7) void *A_Restrict dst, A_In_reads_bytes(7) const char *A_Restrict src)
{
	*(struct _bridge_s7*)dst = *(const struct _bridge_s7*)src;
}

A_Nonnull_all_args
static inline void _br_memcpy8(A_Out_writes_bytes_all(8) void *A_Restrict dst, A_In_reads_bytes(8) const char *A_Restrict src)
{
	*(struct _bridge_s8*)dst = *(const struct _bridge_s8*)src;
}

/* check that field size == sz, returns field size as unsigned integer */
#define _br_sizeof(field, sz) ((unsigned)(0*sizeof(int[1-2*((~0u & sizeof(field)) != (sz))]) + sizeof(field)))

#ifdef __cplusplus
}
#endif

/* suppress warnings about unused static functions in generated code */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4505) /* Unreferenced local function has been removed */
#endif

#endif /* BITBRIDGE_H_INCLUDED */
