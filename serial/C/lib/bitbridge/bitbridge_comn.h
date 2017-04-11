#ifndef BITBRIDGE_CONM_H_INCLUDED
#define BITBRIDGE_CONM_H_INCLUDED

/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/* bitbridge_comn.h */

/* bridge common interface */

#include "sal_defs.h"

/* bridge runtime library version */
#define BRIDGE_RUNTIME_LIB_VERSION 1

#if !defined(BRIDGE_STRLEN) || !defined(BRIDGE_MEMCHR) || !defined(BRIDGE_STPCPY) || !defined(BRIDGE_MEMCPY) || !defined(BRIDGE_MEMSET)
#include <string.h>
#ifndef BRIDGE_STRLEN
#define BRIDGE_STRLEN(s) strlen(s)
#endif
#ifndef BRIDGE_MEMCHR
#define BRIDGE_MEMCHR(m,c,s) memchr(m,c,s)
#endif
#ifndef BRIDGE_STPCPY
#ifndef _WIN32
#include <unistd.h> /* for stpcpy() */
#endif
#if defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200809L)
#define BRIDGE_STPCPY(d,s) stpcpy(d,s)
#else /* _POSIX_VERSION < 200809L */
A_Nonnull_all_args A_Check_return A_Ret_never_null A_Ret_range(>=,d)
static inline char *BRIDGE_STPCPY(A_Notnull char *A_Restrict d, A_In_z const char *A_Restrict s)
{
	return strcpy(d, s) + BRIDGE_STRLEN(s);
}
#endif /* _POSIX_VERSION < 200809L */
#endif /* !BRIDGE_STPCPY */
#ifndef BRIDGE_MEMCPY
#define BRIDGE_MEMCPY(dst, src, sz) memcpy(dst, src, sz)
#endif
#ifndef BRIDGE_MEMSET
#define BRIDGE_MEMSET(mem, ch, sz) memset(mem, ch, sz)
#endif
#endif /* !BRIDGE_STRLEN || !BRIDGE_MEMCHR || !BRIDGE_STPCPY || !BRIDGE_MEMCPY || !BRIDGE_MEMSET */

#ifdef BITBRIDGE_DEBUG
#ifdef ASSERT
#define BRIDGE_ASSERT(cond) ASSERT(cond)
#else /* !ASSERT */
#include <assert.h>
#define BRIDGE_ASSERT(cond) assert(cond)
#endif /* !ASSERT */
#else /* !BITBRIDGE_DEBUG */
#define BRIDGE_ASSERT(cond) ASSUME(cond)
#endif /* !BITBRIDGE_DEBUG */

/* define BITBRIDGE_DEBUG to print traces of memory alloc/free calls */
#ifdef BITBRIDGE_DEBUG
#define BRIDGE_DEBUG_ARGS_VOID      (void)file, (void)line;
#define BRIDGE_DEBUG_ARGS_DECL      , const char *file, unsigned line
#define BRIDGE_DEBUG_ARGS_PASS      , file, line
#define BRIDGE_DEBUG_ARGS           , __FILE__, __LINE__
#define __BRIDGE_EXPORT_SUFFIX(a,r) a##v##r##_debug
#else
#define BRIDGE_DEBUG_ARGS_VOID
#define BRIDGE_DEBUG_ARGS_DECL
#define BRIDGE_DEBUG_ARGS_PASS
#define BRIDGE_DEBUG_ARGS
#define __BRIDGE_EXPORT_SUFFIX(a,r) a##v##r
#endif

#define _BRIDGE_EXPORT_SUFFIX(a,r) __BRIDGE_EXPORT_SUFFIX(a,r)
#define BRIDGE_EXPORT_SUFFIX(a)    _BRIDGE_EXPORT_SUFFIX(a,BRIDGE_RUNTIME_LIB_VERSION)

#include "bitbridge/bitbridge_alloc.h"

/* define fixed-width types: INT64_TYPE, INT32_TYPE, INT16_TYPE, INT8_TYPE */

#ifndef INT64_TYPE
#define INT64_TYPE long long
#endif

#ifndef INT32_TYPE
#define INT32_TYPE int
#endif

#ifndef INT16_TYPE
#define INT16_TYPE short
#endif

#ifndef INT8_TYPE
#define INT8_TYPE signed char
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* check type sizes in bits */
typedef int _CHAR_is_not_8_bits_[1-2*(255 != (unsigned char)~(unsigned char)0)];
typedef int _INT8_is_not_8_bits_[1-2*(1 != sizeof(INT8_TYPE))];
typedef int _INT16_is_not_16_bits_[1-2*(2 != sizeof(INT16_TYPE))];
typedef int _INT32_is_not_32_bits_[1-2*(4 != sizeof(INT32_TYPE))];
typedef int _INT64_is_not_64_bits_[1-2*(8 != sizeof(INT64_TYPE))];

/* maximum number of elements in bridge array,
  also this is maximum packed size of bridge structure */
#define BRIDGE_MAX ((unsigned int)(0x7FFFFFFFu & ~0u) - 8u)

/* maximum number of bits in packed bridge structure */
#define BRIDGE_BIT_COUNT_MAX ((unsigned int)(0xFFFFFFFFu & ~0u))

/* BRIDGE_MAX must not be greater than the maximum size_t value */
typedef int _BRIDGE_MAX_greater_than_size_t_[1-2*(BRIDGE_MAX > (size_t)~(size_t)0)];

/* some checks are not valid if BRIDGE_MAX is too small */
typedef int _BRIDGE_MAX_too_small_[1-2*(BRIDGE_MAX < 0xFFFFu - 8u)];

/* structures used in bridge pack/unpack interface */

/* just pack */
union bridge_pack_info {
	struct {
		struct bridge_allocator *ac; /* memory allocator for allocating packed data buffer */
		unsigned size;               /* packed size, <= BRIDGE_MAX after successfull packing */
	} pi;
#ifdef __cplusplus
	inline void init(A_In struct bridge_allocator &ac);
	A_Check_return
	size_t size() const {
		return pi.size;
	}
#endif
};

/* pack with converting numbers byte-order */
union bridge_pack_conv_info {
	struct {
		struct bridge_allocator *ac; /* memory allocator for allocating packed data buffer */
		unsigned size;               /* packed size, <= BRIDGE_MAX after successfull packing */
		int convert;                 /* if non-zero, then swap numbers byte-order while packing */
	} pi;
	union bridge_pack_info p;
#ifdef __cplusplus
	inline void init(bool convert, A_In struct bridge_allocator &ac);
	A_Check_return
	size_t size() const {
		return pi.size;
	}
#endif
};

/* initialize union bridge_pack_info */
A_Nonnull_all_args
static inline void bridge_pack_info_init(A_Post_valid union bridge_pack_info *pi, A_In struct bridge_allocator *ac)
{
	pi->pi.ac = ac;
}

/* initialize union bridge_pack_conv_info */
A_Nonnull_all_args
static inline void bridge_pack_conv_info_init(A_Post_valid union bridge_pack_conv_info *pci, int convert, A_In struct bridge_allocator *ac)
{
	bridge_pack_info_init(&pci->p, ac);
	pci->pi.convert = convert;
}

#ifdef __cplusplus
inline void bridge_pack_info::init(A_In struct bridge_allocator &ac)
{
	bridge_pack_info_init(this, &ac);
}
inline void bridge_pack_conv_info::init(bool convert, A_In struct bridge_allocator &ac)
{
	bridge_pack_conv_info_init(this, convert ? 1 : 0, &ac);
}
#endif

struct bridge_unpack_src {
	const void *from;      /* packed data to unpack from */
	const void *limit;     /* packed data limit */
#ifdef __cplusplus
	A_Nonnull_all_args
	inline void init(A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/);
#endif
};

A_Nonnull_all_args
static inline void bridge_unpack_src_init(A_Post_valid struct bridge_unpack_src *s, A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/)
{
	s->from = mem;
	s->limit = (const char*)mem + sz;
}

#ifdef __cplusplus
A_Nonnull_all_args
inline void bridge_unpack_src::init(A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/)
{
	bridge_unpack_src_init(this, mem, sz);
}
#endif

/* just unpack */
union bridge_unpack_info {
	struct {
		struct bridge_allocator *ac; /* memory allocator for allocating unpacked structures */
		struct bridge_unpack_src us; /* source of unpacked data */
	} ui;
#ifdef __cplusplus
	A_Nonnull_all_args
	inline void init(A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/, A_In struct bridge_allocator &ac);
	A_Check_return
	const void *from() const {
		return ui.us.from;
	}
#endif
};

/* unpack with converting numbers byte-order */
union bridge_unpack_conv_info {
	struct {
		struct bridge_allocator *ac; /* memory allocator for allocating unpacked structures */
		struct bridge_unpack_src us; /* source of unpacked data */
		int convert;                 /* if non-zero, then swap numbers byte-order while unpacking */
	} ui;
	union bridge_unpack_info u;
#ifdef __cplusplus
	A_Nonnull_all_args
	inline void init(A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/, bool convert, A_In struct bridge_allocator &ac);
	A_Check_return
	const void *from() const {
		return ui.us.from;
	}
#endif
};

/* initialize union bridge_unpack_info */
A_Nonnull_all_args
static inline void bridge_unpack_info_init(
	A_Post_valid union bridge_unpack_info *ui,
	A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/, A_In struct bridge_allocator *ac)
{
	ui->ui.ac = ac;
	bridge_unpack_src_init(&ui->ui.us, mem, sz);
}

/* initialize union bridge_unpack_conv_info */
A_Nonnull_all_args
static inline void bridge_unpack_conv_info_init(
	A_Post_valid union bridge_unpack_conv_info *uci,
	A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/, int convert, A_In struct bridge_allocator *ac)
{
	uci->ui.ac = ac;
	bridge_unpack_src_init(&uci->ui.us, mem, sz);
	uci->ui.convert = convert;
}

#ifdef __cplusplus
A_Nonnull_all_args
inline void bridge_unpack_info::init(A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/, A_In struct bridge_allocator &ac)
{
	bridge_unpack_info_init(this, mem, sz, &ac);
}
A_Nonnull_all_args
inline void bridge_unpack_conv_info::init(A_In_reads_bytes(sz) const void *mem, size_t sz/*0?*/, bool convert, A_In struct bridge_allocator &ac)
{
	bridge_unpack_conv_info_init(this, mem, sz, convert ? 1 : 0, &ac);
}
#endif /* __cplusplus */

/* wrappers for tracing calls of allocator's callbacks */
#define BRIDGE_WRAP_PACK_INFO(in)        _bridge_wrap_pack_info(in BRIDGE_DEBUG_ARGS) BRIDGE_DEBUG_ARGS
#define BRIDGE_WRAP_PACK_CONV_INFO(in)   _bridge_wrap_pack_conv_info(in BRIDGE_DEBUG_ARGS) BRIDGE_DEBUG_ARGS
#define BRIDGE_WRAP_UNPACK_INFO(in)      _bridge_wrap_unpack_info(in BRIDGE_DEBUG_ARGS) BRIDGE_DEBUG_ARGS
#define BRIDGE_WRAP_UNPACK_CONV_INFO(in) _bridge_wrap_unpack_conv_info(in BRIDGE_DEBUG_ARGS) BRIDGE_DEBUG_ARGS

A_Nonnull_all_args A_Check_return A_Ret_range(==,in) A_Ret_never_null
static inline union bridge_pack_info *_bridge_wrap_pack_info(union bridge_pack_info *in BRIDGE_DEBUG_ARGS_DECL)
{
	bridge_log_trace(BRIDGE_PASS_ALLOCATOR_THROUGH(in->pi.ac));
	return in;
}

A_Nonnull_all_args A_Check_return A_Ret_range(==,in) A_Ret_never_null
static inline union bridge_pack_conv_info *_bridge_wrap_pack_conv_info(union bridge_pack_conv_info *in BRIDGE_DEBUG_ARGS_DECL)
{
	bridge_log_trace(BRIDGE_PASS_ALLOCATOR_THROUGH(in->pi.ac));
	return in;
}

A_Nonnull_all_args A_Check_return A_Ret_range(==,in) A_Ret_never_null
static inline union bridge_unpack_info *_bridge_wrap_unpack_info(union bridge_unpack_info *in BRIDGE_DEBUG_ARGS_DECL)
{
	bridge_log_trace(BRIDGE_PASS_ALLOCATOR_THROUGH(in->ui.ac));
	return in;
}

A_Nonnull_all_args A_Check_return A_Ret_range(==,in) A_Ret_never_null
static inline union bridge_unpack_conv_info *_bridge_wrap_unpack_conv_info(union bridge_unpack_conv_info *in BRIDGE_DEBUG_ARGS_DECL)
{
	bridge_log_trace(BRIDGE_PASS_ALLOCATOR_THROUGH(in->ui.ac));
	return in;
}

/* zero memory */
A_Nonnull_all_args
static inline void _bridge_memzero(A_Notnull void *mem, A_In_range(>,0) size_t sz)
{
	BRIDGE_MEMSET(mem, 0, sz);
}

/* check that pointer is non-NULL */
A_Nonnull_all_args
static inline void _bridge_nonnull(A_Notnull void *s)
{
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif
	BRIDGE_ASSERT(s);
	(void)s;
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
}

/* check that pointers are non-NULL */
A_Nonnull_all_args
static inline void _bridge_ac_nonnull(A_Notnull void *s, A_Notnull struct bridge_allocator *ac)
{
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif
	BRIDGE_ASSERT(s);
	BRIDGE_ASSERT(ac);
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
	(void)s, (void)ac;
}

/* bits processing helpers */

/* get bit value */
A_Nonnull_all_args A_Check_return
static inline int bridge_get_bit(A_At(bits + (bit >> 3), A_In_reads(1)) const unsigned char bits[], unsigned bit)
{
	return !!(bits[bit >> 3u] & (1u << (bit & 7u)));
}

/* set bit value */
A_Nonnull_all_args A_Ret_range(==,set)
static inline int bridge_set_bit(A_At(bits + (bit >> 3), A_Inout_updates_all(1)) unsigned char bits[], unsigned bit, int set)
{
	unsigned byte = bit >> 3u;
	unsigned b = 1u << (bit & 7u);
	if (set)
		bits[byte] = (unsigned char)((bits[byte] | b) & (unsigned char)~(unsigned char)0);
	else
		bits[byte] = (unsigned char)((bits[byte] & ~b) & (unsigned char)~(unsigned char)0);
	return set;
}

/* get number of bytes neeed to hold given non-zero number of bits */
A_Check_return A_Ret_range(==, ((bit_count - 1u) >> 3u) + 1u)
static inline unsigned bridge_bytes_for_nonzero_bits(A_In_range(>,0) unsigned bit_count)
{
	BRIDGE_ASSERT(bit_count);
	return ((bit_count - 1u) >> 3u) + 1u;
}

/* get number of bytes neeed to hold given number of bits */
/* warning: bit_count used twice! */
#define _bridge_bytes_for_bits(bit_count) ((((bit_count) & 1u) + ((bit_count) >> 1u) + 3u) >> 2u)

/* get number of bytes neeed to hold given number of bits */
A_Check_return A_Ret_range(==, ((bit_count & 1u) + (bit_count >> 1u) + 3u) >> 2u)
static inline unsigned bridge_bytes_for_bits(unsigned bit_count)
{
	return _bridge_bytes_for_bits(bit_count);
}

/* get number of elements in array */
#define bridge_array_count(a) (size_t)(a##_end - a)

#ifdef __cplusplus
}
#endif

#endif /* BITBRIDGE_CONM_H_INCLUDED */
