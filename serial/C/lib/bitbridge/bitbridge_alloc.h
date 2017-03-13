#ifndef BITBRIDGE_ALLOC_H_INCLUDED
#define BITBRIDGE_ALLOC_H_INCLUDED

/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/* bitbridge_alloc.h */

/* bridge allocator interface */

#ifndef BITBRIDGE_CONM_H_INCLUDED
#error do not #include this file directly, #include "bitbridge/bitbridge_comn.h" instead
#endif

/* assume BRIDGE_DEFAULT_ALLOCATOR is a reference to struct bridge_allocator, then: */

/* allocate memory via BRIDGE_DEFAULT_ALLOCATOR */
#define bridge_alloc(sz)                    bridge_ac_alloc(sz, BRIDGE_DEFAULT_ALLOCATOR)

/* allocate memory via BRIDGE_DEFAULT_ALLOCATOR, then zero-initialize allocated memory */
#define bridge_allocz(sz)                   bridge_ac_allocz(sz, BRIDGE_DEFAULT_ALLOCATOR)

/* allocate memory via BRIDGE_DEFAULT_ALLOCATOR, then copy given '\0'-terminated string into it */
#define bridge_copy_str(str)                bridge_ac_copy_str(str, BRIDGE_DEFAULT_ALLOCATOR)

/* take a reference to given '\0'-terminated string,
  if BRIDGE_DEFAULT_ALLOCATOR doesn't allows references, then allocate new memory and copy source string into it */
#define bridge_ref_str(str)                 bridge_ac_ref_str(str, BRIDGE_DEFAULT_ALLOCATOR)

/* same as bridge_copy_str()/bridge_ref_str(),
  but may provide size of source '\0'-terminated string (string length + 1 for terminating '\0')
  NOTE: assume there are no '\0'-characters inside source string */
#define bridge_copy_str_size(str, sz/*0?*/) bridge_ac_copy_str_size(str, sz, BRIDGE_DEFAULT_ALLOCATOR)
#define bridge_ref_str_size(str, sz/*0?*/)  bridge_ac_ref_str_size(str, sz, BRIDGE_DEFAULT_ALLOCATOR)

/* copy or take a reference to string constant or '\0'-terminated string buffer
  NOTE: string constants like "aaa" are '\0'-terminated in C
  NOTE: it is allowed to pass characters buffer initialized with string constant, for example:
   const char buf[] = "aaaa";
   bridge_copy_str_buf(buf);
  NOTE: assume there are no '\0'-characters inside passed string buffer and buffer is '\0'-terminated */
#define bridge_copy_str_buf(str)            bridge_ac_copy_str_buf(str, BRIDGE_DEFAULT_ALLOCATOR)
#define bridge_ref_str_buf(str)             bridge_ac_ref_str_buf(str, BRIDGE_DEFAULT_ALLOCATOR)
#define bridge_copy_str_const(str)          bridge_ac_copy_str_const(str, BRIDGE_DEFAULT_ALLOCATOR)
#define bridge_ref_str_const(str)           bridge_ac_ref_str_const(str, BRIDGE_DEFAULT_ALLOCATOR)

/* make a copy of substring: str may be not '\0'-terminated, len - substring length */
#define bridge_copy_sub_str(str, len/*0?*/) bridge_ac_copy_sub_str(str, len, BRIDGE_DEFAULT_ALLOCATOR)

/* free memory allocated via BRIDGE_DEFAULT_ALLOCATOR
  NOTE: assume allocator doesn't allows references, because it's not allowed to free references */
#define bridge_free(mem)                    bridge_ac_free(mem, BRIDGE_DEFAULT_ALLOCATOR)

/* alloc/free with custom allocator reference */
#define bridge_ac_alloc(sz, ac)                    _br_alloc_mem(sz, BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_allocz(sz, ac)                   _br_allocz_mem(sz, BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_copy_str(str, ac)                _br_copy_str(str, BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_ref_str(str, ac)                 _br_ref_str(str, BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_copy_str_size(str, sz/*0?*/, ac) _br_copy_str_size(str, sz, BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_ref_str_size(str, sz/*0?*/, ac)  _br_ref_str_size(str, sz, BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_copy_str_buf(str, ac)            _br_copy_str_size(str, str_buf_size(str), BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_ref_str_buf(str, ac)             _br_ref_str_size(str, str_buf_size(str), BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_copy_str_const(str, ac)          _br_copy_str_size(str, str_const_size(str), BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_ref_str_const(str, ac)           _br_ref_str_size(str, str_const_size(str), BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_copy_sub_str(str, len, ac)       _br_copy_sub_str(str, len, BRIDGE_WRAP_ALLOCATOR(ac))
#define bridge_ac_free(mem, ac)                    _bridge_free_mem(mem, BRIDGE_WRAP_ALLOCATOR(ac))

/* get size of string constant or character array, including '\0'-terminator, so result is always > 0
   NOTE: doesn't compiles if 'str' is not a string constant like "aaa" or character array like str[] = "aaa"; */
#ifdef __cplusplus
#define str_buf_size(str) (sizeof(str) + 0*sizeof((&(str) - (const char(*)[sizeof(str)/sizeof((str)[0])])0)))
#elif defined(__GNUC__)
#define str_buf_size(str) (sizeof(str) + 0*sizeof(&(str) - __builtin_choose_expr( \
	__builtin_types_compatible_p(__typeof__(&(str)), char(*)[sizeof(str)/sizeof((str)[0])]), \
	(char(*)[sizeof(str)/sizeof((str)[0])])0, (const char(*)[sizeof(str)/sizeof((str)[0])])0)))
#else
#define str_buf_size(str) sizeof(str)
#endif

/* get length of string constant or character array, not including '\0'-terminator, so length may be 0
  NOTE: doesn't compiles if 'str' is not a string constant like "aaa" or character array like str[] = "aaa"; */
#define str_buf_length(str) (str_buf_size(str) - 1/*'\0'*/)

/* get size/length of string constant */
#define str_const_size(str)   str_buf_size(str)
#define str_const_length(str) str_buf_length(str)

#ifdef __cplusplus
extern "C" {
#endif

/* wrappers for tracing calls of allocator's callbacks */
#define BRIDGE_ALLOCATOR_ARG(ac)               A_Inout struct bridge_allocator *ac BRIDGE_DEBUG_ARGS_DECL
#define BRIDGE_WRAP_ALLOCATOR(ac)              bridge_log_trace(ac BRIDGE_DEBUG_ARGS) BRIDGE_DEBUG_ARGS
#define BRIDGE_PASS_ALLOCATOR(ac)              bridge_log_trace(ac BRIDGE_DEBUG_ARGS) BRIDGE_DEBUG_ARGS_PASS
#define BRIDGE_PASS_ALLOCATOR_CONTAINER(ac,cn) (bridge_log_trace(ac BRIDGE_DEBUG_ARGS), cn) BRIDGE_DEBUG_ARGS_PASS
#define BRIDGE_PASS_ALLOCATOR_THROUGH(ac)      ac BRIDGE_DEBUG_ARGS_PASS

/* bit indicating that source memory pointer is not NULL and
  if source address alignment is unacceptable for given allocation type,
  then source memory should be copied to newly allocated memory block */
#define BRIDGE_ALLOC_COPY_BIT /*0100*/4u

/* bits that are set for BRIDGE_ALLOC_REF_COPY_STRINGZ, BRIDGE_ALLOC_COPY_STRINGZ and BRIDGE_ALLOC_COPY_SUBSTR values */
#define BRIDGE_ALLOC_STRING_BITS /*1100*/12u

/* NOTE: 14 allocation types: numbers 9 and 15 are reserved */
enum bridge_alloc_type {

	/* allocate memory block for a structure with largest simple type of 1,2,4 or 8 bytes,
	  allocator may return the source pointer (only if it's not NULL)
	  indicating that source memory should be referenced by the caller,
	  else allocator should allocate new memory block for filling it by the caller */
	BRIDGE_ALLOC_REF_ALIGNED1      = /*0000*/0,
	BRIDGE_ALLOC_REF_ALIGNED2      = /*0001*/1,
	BRIDGE_ALLOC_REF_ALIGNED4      = /*0010*/2,
	BRIDGE_ALLOC_REF_ALIGNED8      = /*0011*/3,

	/* same as BRIDGE_ALLOC_REF_ALIGNED{1,2,4,8},
	  but copy source memory if new memory block was allocated,
	  source pointer != NULL */
	BRIDGE_ALLOC_REF_COPY_ALIGNED1 = /*0100*/4, /* (BRIDGE_ALLOC_REF_ALIGNED1 | BRIDGE_ALLOC_COPY_BIT) */
	BRIDGE_ALLOC_REF_COPY_ALIGNED2 = /*0101*/5, /* (BRIDGE_ALLOC_REF_ALIGNED2 | BRIDGE_ALLOC_COPY_BIT) */
	BRIDGE_ALLOC_REF_COPY_ALIGNED4 = /*0110*/6, /* (BRIDGE_ALLOC_REF_ALIGNED4 | BRIDGE_ALLOC_COPY_BIT) */
	BRIDGE_ALLOC_REF_COPY_ALIGNED8 = /*0111*/7, /* (BRIDGE_ALLOC_REF_ALIGNED8 | BRIDGE_ALLOC_COPY_BIT) */

	/* allocate memory for '\0'-terminated string,
	  allocator may return the source '\0'-terminated string pointer indicating that it should be referenced by the caller,
	  else allocator should allocate new memory block and copy '\0'-terminated string into it;
	  NOTE: sz - if non-zero, then the size of string, including terminating '\0', if zero - then size is not computed yet */
	BRIDGE_ALLOC_REF_COPY_STRINGZ  = /*1101*/13, /* BRIDGE_ALLOC_COPY_BIT is set */

	/* same as BRIDGE_ALLOC_REF_COPY_STRINGZ,
	  but always do a copy of the source string */
	BRIDGE_ALLOC_COPY_STRINGZ      = /*1100*/12, /* BRIDGE_ALLOC_COPY_BIT is set */

	/* same as BRIDGE_ALLOC_COPY_STRINGZ,
	  but source string is not '\0'-terminated, sz must be non-zero */
	BRIDGE_ALLOC_COPY_SUBSTR       = /*1110*/14, /* BRIDGE_ALLOC_COPY_BIT is set */

	/* allocate memory chunk to unpack structures to (for example - for array of pointers),
	  source pointer is NULL */
	BRIDGE_ALLOC_MEM               = /*1010*/10,

	/* same as BRIDGE_ALLOC_MEM, but also zero-iniialize allocated memory chunk,
	  source pointer is NULL */
	BRIDGE_ALLOCZ_MEM              = /*1000*/8,

	/* allocate new single memory block to pack structures to,
	  source pointer is NULL */
	BRIDGE_ALLOC_PACK              = /*1011*/11
};

/* forward declaration */
struct bridge_allocator;

typedef
	/* allocate memory, returns NULL if failed */
	A_Nonnull_arg(1)
	void *(*bridge_allocator_alloc_cb_t)(
		BRIDGE_ALLOCATOR_ARG(ac),
		A_When(
			BRIDGE_ALLOC_REF_ALIGNED1 == at ||
			BRIDGE_ALLOC_REF_ALIGNED2 == at ||
			BRIDGE_ALLOC_REF_ALIGNED4 == at ||
			BRIDGE_ALLOC_REF_ALIGNED8 == at, A_Readable_bytes(sz))
		A_When(
			BRIDGE_ALLOC_REF_COPY_ALIGNED1 == at ||
			BRIDGE_ALLOC_REF_COPY_ALIGNED2 == at ||
			BRIDGE_ALLOC_REF_COPY_ALIGNED4 == at ||
			BRIDGE_ALLOC_REF_COPY_ALIGNED8 == at, A_In_reads_bytes(sz))
		A_When(
			BRIDGE_ALLOC_REF_COPY_STRINGZ == at ||
			BRIDGE_ALLOC_COPY_STRINGZ     == at, A_In_reads_bytes(sz) A_In_z)
		A_When(
			BRIDGE_ALLOC_COPY_SUBSTR == at, A_In_reads_bytes(sz - 1))
		A_When(
			BRIDGE_ALLOC_MEM  == at ||
			BRIDGE_ALLOCZ_MEM == at ||
			BRIDGE_ALLOC_PACK == at, A_Null)
		const void *source/*!=NULL if BRIDGE_ALLOC_COPY_BIT is set, else may be NULL*/,
		A_When(
			BRIDGE_ALLOC_REF_COPY_STRINGZ != at &&
			BRIDGE_ALLOC_COPY_STRINGZ     != at, A_In_range(>,0))
		size_t sz/*0?*/,
		enum bridge_alloc_type at);

typedef
	/* free memory allocated by bridge_allocator_alloc_cb_t */
	A_Nonnull_arg(1)
	void (*bridge_allocator_free_cb_t)(
		BRIDGE_ALLOCATOR_ARG(ac),
		A_Pre_opt_valid A_Post_ptr_invalid void *mem/*NULL?*/);

typedef
	/* log alloc/free operation backtrace,
	  if ac == mem then intermediate call (sz == 0),
	  if sz == 0 then freeing (NULL?) mem,
	  if sz != 0 then allocated (NULL?) mem */
	A_Nonnull_arg(1)
	void (*bridge_allocator_log_cb_t)(
		BRIDGE_ALLOCATOR_ARG(ac),
		size_t sz/*0?*/,
		A_Maybenull void *mem/*NULL?*/);

/* bridge allocator - used to allocate memory while packing/unpacking bridge structures */
struct bridge_allocator {

	/* allocator callback */
	bridge_allocator_alloc_cb_t alloc_cb;

	/* deallocator callback,
	  may be NULL if memory does not need to be freed (memory may be allocated on stack) */
	bridge_allocator_free_cb_t free_cb;

#ifdef BITBRIDGE_DEBUG
	/* log callback */
	bridge_allocator_log_cb_t log_cb;
	int trace_level;
#endif

#ifdef __cplusplus

	/* init allocator */
	A_Nonnull_arg(1)
	inline void init(
		A_In bridge_allocator_alloc_cb_t alloc_cb/*!=NULL*/,
		A_In_opt bridge_allocator_free_cb_t free_cb/*NULL?*/,
		A_In_opt bridge_allocator_log_cb_t log_cb = NULL);

	/* destroy allocator */
	inline void destroy();

	/* enable/disable allocator's memory operations backtraces */
	inline void enable_log(bool enable);

	/* allocate memory,
	  if z == true, then zero-initialize allocated memory */
	A_Ret_restrict A_Check_return A_Ret_maybenull A_Post_writable_byte_size(sz)
	inline void *alloc(A_In_range(>,0) size_t sz/*>0*/, bool z = false);

	/* allocate memory, then copy given '\0'-terminated string into it
	  NOTE: may provide size of source '\0'-terminated string (string length + 1 for terminating '\0')
	  NOTE: assume there are no '\0'-characters inside source string */
	A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
	inline char *copy_str(A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/, size_t sz = 0);

	/* take a reference to given '\0'-terminated string,
	  if allocator doesn't allows references, then allocate new memory and copy source string into it
	  NOTE: may provide size of source '\0'-terminated string (string length + 1 for terminating '\0')
	  NOTE: assume there are no '\0'-characters inside source string */
	A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
	inline char *ref_str(A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/, size_t sz = 0);

	/* copy string constant */
	A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
	char *copy_str_const(A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/)
	{
		return copy_str(str, BRIDGE_STRLEN(str) + 1/*'\0'*/);
	}

	/* take ref of string constant */
	A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
	char *ref_str_const(A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/)
	{
		return ref_str(str, BRIDGE_STRLEN(str) + 1/*'\0'*/);
	}

	/* make a copy of substring: str may be not '\0'-terminated, len - substring length */
	A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
	inline char *copy_sub_str(
		A_In_reads_bytes(len) const char *str/*!=NULL*/,
		A_In_range(<,(size_t)~(size_t)0) size_t len/*0?*/);

	/* free memory allocated via alloc()
	  NOTE: assume allocator doesn't allows references, because it's not allowed to free references */
	A_Nonnull_arg(2)
	inline void free(A_Pre_opt_valid A_Post_ptr_invalid void *mem/*NULL?*/);

#endif /* __cplusplus */
};

/* allocator initialization helper */
#ifdef BITBRIDGE_DEBUG
/* NOTE: by default, trace_level initialized with sign bit set - log is disabled */
#define BRIDGE_ALLOCATOR_INITIALIZER(alloc_cb, free_cb, log_cb) {alloc_cb, free_cb, log_cb, (int)~(~0u >> 1)}
#else
#define BRIDGE_ALLOCATOR_INITIALIZER(alloc_cb, free_cb, log_cb) {alloc_cb, free_cb}
#endif

#include "bitbridge/bitbridge_log_cb.inl"

/* initialize allocator */
A_Nonnull_arg(1) A_Nonnull_arg(2)
static inline void bridge_allocator_init(
	A_Post_valid struct bridge_allocator *ac/*out*/,
	A_In bridge_allocator_alloc_cb_t alloc_cb_/*!=NULL*/,
	A_In_opt bridge_allocator_free_cb_t free_cb_/*NULL?*/,
	A_In_opt bridge_allocator_log_cb_t log_cb_/*NULL?*/)
{
	ac->alloc_cb = alloc_cb_;
	ac->free_cb = free_cb_;
#ifdef BITBRIDGE_DEBUG
	ac->log_cb = log_cb_ ? log_cb_ : bridge_allocator_log_cb;
	ac->trace_level = (int)~(~0u >> 1);
#endif
	(void)log_cb_;
}

/* for convenience, does nothing */
A_Nonnull_all_args
static inline void bridge_allocator_destroy(A_Pre_valid A_Post_invalid struct bridge_allocator *ac)
{
	(void)ac;
}

#ifdef __cplusplus

A_Nonnull_arg(1)
inline void bridge_allocator::init(
	A_In bridge_allocator_alloc_cb_t alloc_cb_/*!=NULL*/,
	A_In_opt bridge_allocator_free_cb_t free_cb_/*NULL?*/,
	A_In_opt bridge_allocator_log_cb_t log_cb_/*NULL?*/)
{
	bridge_allocator_init(this, alloc_cb_, free_cb_, log_cb_);
}

inline void bridge_allocator::destroy()
{
	bridge_allocator_destroy(this);
}

#endif /* __cplusplus */

/* enable/disable allocator's memory operations backtraces */
A_Nonnull_all_args
static inline void bridge_allocator_enable_log(A_Inout struct bridge_allocator *ac, int enable)
{
	(void)ac, (void)enable;
#ifdef BITBRIDGE_DEBUG
	if (enable)
		ac->trace_level = (int)((unsigned)ac->trace_level & (~0u >> 1));
	else
		ac->trace_level = (int)((unsigned)ac->trace_level | ~(~0u >> 1));
#endif
}

#ifdef __cplusplus
inline void bridge_allocator::enable_log(bool enable)
{
	bridge_allocator_enable_log(this, enable ? 1 : 0);
}
#endif

/* simple helper to log intermediate call of alloc/free operations */
A_Nonnull_all_args A_Ret_never_null A_Ret_range(==,ac)
static inline struct bridge_allocator *bridge_log_trace(BRIDGE_ALLOCATOR_ARG(ac))
{
	BRIDGE_DEBUG_ARGS_VOID
#ifdef BITBRIDGE_DEBUG
	if (ac->trace_level >= 0)
		ac->log_cb(BRIDGE_PASS_ALLOCATOR_THROUGH(ac), /*sz:*/0, /*mem:*/ac);
	ac->trace_level++;
#endif
	return ac;
}

/* simple helper to log alloc operation call */
A_Nonnull_arg(1)
static inline void _bridge_log_alloc(
	BRIDGE_ALLOCATOR_ARG(ac),
	A_In_range(>,0) size_t sz,
	A_Maybenull void *mem)
{
	(void)ac, (void)mem, (void)sz;
	BRIDGE_DEBUG_ARGS_VOID
#ifdef BITBRIDGE_DEBUG
	if (ac->trace_level >= 0)
		ac->log_cb(BRIDGE_PASS_ALLOCATOR_THROUGH(ac), sz, mem);
	ac->trace_level--;
#endif
}

/* simple helper to log free operation call */
A_Nonnull_arg(1)
static inline void _bridge_log_free(
	BRIDGE_ALLOCATOR_ARG(ac),
	A_Maybenull void *mem)
{
	(void)ac, (void)mem;
	BRIDGE_DEBUG_ARGS_VOID
#ifdef BITBRIDGE_DEBUG
	if (ac->trace_level >= 0)
		ac->log_cb(BRIDGE_PASS_ALLOCATOR_THROUGH(ac), /*sz:*/0, mem);
	ac->trace_level--;
#endif
}

/* decrement offset of printed allocator call traces */
A_Nonnull_all_args
static inline void bridge_allocator_dec_level(A_Inout struct bridge_allocator *ac)
{
	(void)ac;
#ifdef BITBRIDGE_DEBUG
	BRIDGE_ASSERT((~0u >> 1) & (unsigned)ac->trace_level); /* ignore sign bit */
	ac->trace_level--;
#endif
}

/* static assert */
#define __BRIDGE_TYPEDEF_CHECK(expr, line) typedef int _br_check_at_##line[1-2*!(expr)]
#define _BRIDGE_TYPEDEF_CHECK(expr, line)  __BRIDGE_TYPEDEF_CHECK(expr, line)
#define BRIDGE_TYPEDEF_CHECK(expr)         _BRIDGE_TYPEDEF_CHECK(expr, __LINE__)

/* check BRIDGE_ALLOC_... enum values */
BRIDGE_TYPEDEF_CHECK(1 == (1 << BRIDGE_ALLOC_REF_ALIGNED1));
BRIDGE_TYPEDEF_CHECK(2 == (1 << BRIDGE_ALLOC_REF_ALIGNED2));
BRIDGE_TYPEDEF_CHECK(4 == (1 << BRIDGE_ALLOC_REF_ALIGNED4));
BRIDGE_TYPEDEF_CHECK(8 == (1 << BRIDGE_ALLOC_REF_ALIGNED8));

/* check that BRIDGE_ALLOC_COPY_BIT is not set */
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_REF_ALIGNED1 & BRIDGE_ALLOC_COPY_BIT));
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_REF_ALIGNED2 & BRIDGE_ALLOC_COPY_BIT));
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_REF_ALIGNED4 & BRIDGE_ALLOC_COPY_BIT));
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_REF_ALIGNED8 & BRIDGE_ALLOC_COPY_BIT));
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_MEM          & BRIDGE_ALLOC_COPY_BIT));
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOCZ_MEM         & BRIDGE_ALLOC_COPY_BIT));
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_PACK         & BRIDGE_ALLOC_COPY_BIT));

/* check that BRIDGE_ALLOC_COPY_BIT is set */
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_REF_COPY_ALIGNED1 & BRIDGE_ALLOC_COPY_BIT);
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_REF_COPY_ALIGNED2 & BRIDGE_ALLOC_COPY_BIT);
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_REF_COPY_ALIGNED4 & BRIDGE_ALLOC_COPY_BIT);
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_REF_COPY_ALIGNED8 & BRIDGE_ALLOC_COPY_BIT);
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_REF_COPY_STRINGZ  & BRIDGE_ALLOC_COPY_BIT);
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_COPY_STRINGZ      & BRIDGE_ALLOC_COPY_BIT);
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_COPY_SUBSTR       & BRIDGE_ALLOC_COPY_BIT);

/* check that BRIDGE_ALLOC_REF_COPY_ALIGNED? is the BRIDGE_ALLOC_REF_ALIGNED? + BRIDGE_ALLOC_COPY_BIT */
BRIDGE_TYPEDEF_CHECK((BRIDGE_ALLOC_REF_ALIGNED1 | BRIDGE_ALLOC_COPY_BIT) == BRIDGE_ALLOC_REF_COPY_ALIGNED1);
BRIDGE_TYPEDEF_CHECK((BRIDGE_ALLOC_REF_ALIGNED2 | BRIDGE_ALLOC_COPY_BIT) == BRIDGE_ALLOC_REF_COPY_ALIGNED2);
BRIDGE_TYPEDEF_CHECK((BRIDGE_ALLOC_REF_ALIGNED4 | BRIDGE_ALLOC_COPY_BIT) == BRIDGE_ALLOC_REF_COPY_ALIGNED4);
BRIDGE_TYPEDEF_CHECK((BRIDGE_ALLOC_REF_ALIGNED8 | BRIDGE_ALLOC_COPY_BIT) == BRIDGE_ALLOC_REF_COPY_ALIGNED8);

/* check BRIDGE_ALLOC_REF_COPY_STRINGZ, BRIDGE_ALLOC_COPY_STRINGZ and BRIDGE_ALLOC_COPY_SUBSTR enum values */
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_REF_COPY_STRINGZ & 1);
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_COPY_STRINGZ & 1));
BRIDGE_TYPEDEF_CHECK(!(BRIDGE_ALLOC_COPY_SUBSTR & 1));

/* check that (BRIDGE_ALLOC_COPY_STRINGZ | 1) == BRIDGE_ALLOC_REF_COPY_STRINGZ */
BRIDGE_TYPEDEF_CHECK((BRIDGE_ALLOC_COPY_STRINGZ | 1) == BRIDGE_ALLOC_REF_COPY_STRINGZ);

/* check that BRIDGE_ALLOC_STRING_BITS are set for string operations */
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_STRING_BITS == (BRIDGE_ALLOC_STRING_BITS & BRIDGE_ALLOC_REF_COPY_STRINGZ));
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_STRING_BITS == (BRIDGE_ALLOC_STRING_BITS & BRIDGE_ALLOC_COPY_STRINGZ));
BRIDGE_TYPEDEF_CHECK(BRIDGE_ALLOC_STRING_BITS == (BRIDGE_ALLOC_STRING_BITS & BRIDGE_ALLOC_COPY_SUBSTR));

A_Const_function A_Nonnull_all_args A_Check_return A_Ret_range(==,source) A_Ret_never_null
static inline void *bridge_ref_source(A_Notnull const void *source)
{
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif
	BRIDGE_ASSERT(source);
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
	{
		union {
			const void *c;
			void *v;
		} u = {source};
		BRIDGE_ASSERT(u.v == source);
		return u.v; /* reference the source memory */
	}
}

/* helper for struct bridge_allocator's alloc_cb() implementation allowing to reference source memory:
  check if source memory address is properly aligned for given allocation type,
  if yes - returns source memory pointer,
  if no  - returns NULL, alloc_cb() should try to allocate fresh memory */
A_Check_return A_Ret_maybenull
A_When(return != NULL, A_Post_satisfies(return == source))
A_When(BRIDGE_ALLOC_REF_COPY_STRINGZ == at, A_Post_satisfies(return == source))
static inline void *bridge_check_ref_alignment(
	A_When(
		BRIDGE_ALLOC_REF_ALIGNED1 == at ||
		BRIDGE_ALLOC_REF_ALIGNED2 == at ||
		BRIDGE_ALLOC_REF_ALIGNED4 == at ||
		BRIDGE_ALLOC_REF_ALIGNED8 == at, A_Maybenull)
	A_When(
		BRIDGE_ALLOC_REF_COPY_ALIGNED1 == at ||
		BRIDGE_ALLOC_REF_COPY_ALIGNED2 == at ||
		BRIDGE_ALLOC_REF_COPY_ALIGNED4 == at ||
		BRIDGE_ALLOC_REF_COPY_ALIGNED8 == at, A_Notnull)
	A_When(
		BRIDGE_ALLOC_REF_COPY_STRINGZ == at ||
		BRIDGE_ALLOC_COPY_STRINGZ     == at, A_In_z)
	A_When(
		BRIDGE_ALLOC_COPY_SUBSTR == at, A_Notnull)
	A_When(
		BRIDGE_ALLOC_MEM  == at ||
		BRIDGE_ALLOCZ_MEM == at ||
		BRIDGE_ALLOC_PACK == at, A_Null)
	const void *source/*!=NULL if BRIDGE_ALLOC_COPY_BIT is set, else may be NULL*/,
	enum bridge_alloc_type at)
{
	unsigned a = (unsigned)at & ~BRIDGE_ALLOC_COPY_BIT;
	switch (a) {
		case BRIDGE_ALLOC_REF_ALIGNED2:
		case BRIDGE_ALLOC_REF_ALIGNED4:
		case BRIDGE_ALLOC_REF_ALIGNED8:
			/* source may be NULL, then will fall through */
#ifndef BRIDGE_ALLOW_MISALIGNED_ACCESS
			if ((((const char*)source - (const char*)0) & ~0u) != (1u << a))
				break; /* cannot reference the source: bad alignment */
#endif
			/* fall through */
		case BRIDGE_ALLOC_REF_ALIGNED1:
			if (source) {
		case BRIDGE_ALLOC_REF_COPY_STRINGZ & ~BRIDGE_ALLOC_COPY_BIT:
				BRIDGE_ASSERT(source);
				return bridge_ref_source(source); /* reference the source memory */
			}
			break; /* cannot reference the source: source is NULL */
		/*case BRIDGE_ALLOCZ_MEM:*/
		case BRIDGE_ALLOC_COPY_STRINGZ & ~BRIDGE_ALLOC_COPY_BIT:
			/* don't reference the source string, must allocate new memory chunk and copy source string into it */
			BRIDGE_ASSERT((BRIDGE_ALLOC_COPY_STRINGZ == at) ^ !source);
			break;
		/*case BRIDGE_ALLOC_COPY_SUBSTR & ~BRIDGE_ALLOC_COPY_BIT:*/
		case BRIDGE_ALLOC_MEM:
		case BRIDGE_ALLOC_PACK:
			BRIDGE_ASSERT((BRIDGE_ALLOC_COPY_SUBSTR == at) ^ !source);
			break;
		default:
			BRIDGE_ASSERT(0); /* unreachable */
	}
	return NULL; /* either bad alignment or source is NULL */
}

/* helper for struct bridge_allocator's alloc_cb() implementation: copy source memory if required */
/* ac_allows_ref_source - non-zero if allocator allows to reference source memory - e.g. allocator calls bridge_check_ref_alignment() */
/* ac_allows_ref_source == 2 if allocator also always references source memory for BRIDGE_ALLOC_REF_COPY_STRINGZ - e.g. mem == source */
/* NOTE: for at == BRIDGE_ALLOC_COPY_STRINGZ sz must be computed (must be non-zero) */
/* NOTE: for at == BRIDGE_ALLOC_REF_COPY_STRINGZ sz must be computed (must be non-zero) if
  will copy the source string (ac_allows_ref_source != 2 && (!ac_allows_ref_source || mem != source)) */
A_Nonnull_arg(1) A_Check_return A_Ret_maybenull A_Post_satisfies(return == mem)
static inline void *_bridge_allocator_copy_return(
	BRIDGE_ALLOCATOR_ARG(ac),
	A_Maybenull void *mem/*NULL if alloc failed*/,
	A_When(
		BRIDGE_ALLOC_REF_ALIGNED1 == at ||
		BRIDGE_ALLOC_REF_ALIGNED2 == at ||
		BRIDGE_ALLOC_REF_ALIGNED4 == at ||
		BRIDGE_ALLOC_REF_ALIGNED8 == at, A_Readable_bytes(sz))
	A_When(
		BRIDGE_ALLOC_REF_COPY_ALIGNED1 == at ||
		BRIDGE_ALLOC_REF_COPY_ALIGNED2 == at ||
		BRIDGE_ALLOC_REF_COPY_ALIGNED4 == at ||
		BRIDGE_ALLOC_REF_COPY_ALIGNED8 == at, A_In_reads_bytes(sz))
	A_When(
		BRIDGE_ALLOC_REF_COPY_STRINGZ == at ||
		BRIDGE_ALLOC_COPY_STRINGZ     == at, A_In_reads_bytes(sz) A_In_z)
	A_When(
		BRIDGE_ALLOC_COPY_SUBSTR == at, A_In_reads_bytes(sz - 1))
	const void *source/*!=NULL if BRIDGE_ALLOC_COPY_BIT is set, else may be NULL*/,
	A_When(
		BRIDGE_ALLOC_REF_COPY_STRINGZ != at ||
		(ac_allows_ref_source != 2 && (!ac_allows_ref_source || mem != source)), A_In_range(>,0))
	size_t sz/*0?*/,
	enum bridge_alloc_type at,
	A_In_range(0,2) int ac_allows_ref_source)
{
	_bridge_log_alloc(ac BRIDGE_DEBUG_ARGS_PASS, sz, mem);
	if (mem) {
		switch (at) {
			case BRIDGE_ALLOC_REF_ALIGNED1:
			case BRIDGE_ALLOC_REF_ALIGNED2:
			case BRIDGE_ALLOC_REF_ALIGNED4:
			case BRIDGE_ALLOC_REF_ALIGNED8:
				/* source == NULL? */
				break;
			case BRIDGE_ALLOC_COPY_SUBSTR:
				BRIDGE_ASSERT(sz);
				((char*)mem)[--sz] = '\0';
				goto _copy_source;
			case BRIDGE_ALLOC_REF_COPY_ALIGNED1:
			case BRIDGE_ALLOC_REF_COPY_ALIGNED2:
			case BRIDGE_ALLOC_REF_COPY_ALIGNED4:
			case BRIDGE_ALLOC_REF_COPY_ALIGNED8:
			case BRIDGE_ALLOC_REF_COPY_STRINGZ:
				/* source != NULL */
				BRIDGE_ASSERT(source);
				if ((BRIDGE_ALLOC_REF_COPY_STRINGZ == at && 2 == ac_allows_ref_source) ||
					(ac_allows_ref_source && mem == source))
				{
					break; /* not need to copy source memory - it is referenced */
				}
				/* fall through */
			case BRIDGE_ALLOC_COPY_STRINGZ:
				BRIDGE_ASSERT(sz);
_copy_source:
				BRIDGE_ASSERT(source);
				return BRIDGE_MEMCPY(mem, source, sz/*0?*/);
			case BRIDGE_ALLOCZ_MEM:
				BRIDGE_MEMSET(mem, 0, sz/*>0*/);
				/* fall through */
			case BRIDGE_ALLOC_MEM:
			case BRIDGE_ALLOC_PACK:
				BRIDGE_ASSERT(sz);
			default:
				break;
		}
	}
	return mem; /* NULL? */
}

/* for use in allocator's alloc_cb(): log allocation and copy source memory if needed */
#define BRIDGE_ALLOCATOR_COPY_RETURN(ac, mem, source, sz, at, ac_allows_ref_source) \
	_bridge_allocator_copy_return(ac BRIDGE_DEBUG_ARGS, mem, source, sz, at, ac_allows_ref_source)

/* for use in allocator's free_cb(): log deallocation */
#define BRIDGE_LOG_FREE(ac, mem) _bridge_log_free(ac BRIDGE_DEBUG_ARGS, mem)

A_Nonnull_all_args A_Ret_restrict A_Check_return A_Ret_maybenull A_Post_writable_byte_size(sz)
static inline void *_br_alloc_mem(A_In_range(>,0) size_t sz/*>0*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	void *mem = ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), /*source:*/NULL, sz, BRIDGE_ALLOC_MEM);
	bridge_allocator_dec_level(ac);
	return mem;
}

A_Nonnull_all_args A_Ret_restrict A_Check_return A_Ret_maybenull A_Post_writable_byte_size(sz)
static inline void *_br_allocz_mem(A_In_range(>,0) size_t sz/*>0*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	void *mem = ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), /*source:*/NULL, sz, BRIDGE_ALLOCZ_MEM);
	bridge_allocator_dec_level(ac);
	return mem;
}

A_Nonnull_arg(2)
static inline void _bridge_free_mem(A_Pre_opt_valid A_Post_ptr_invalid void *mem/*NULL?*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	ac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), mem);
	bridge_allocator_dec_level(ac);
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
static inline char *_br_copy_str(A_In_z const char *str/*'\0'-terminated*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	char *copy = (char*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), /*source:*/str, /*sz:*/0/*not computed*/, BRIDGE_ALLOC_COPY_STRINGZ);
	bridge_allocator_dec_level(ac);
	return copy;
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
static inline char *_br_ref_str(A_In_z const char *str/*'\0'-terminated*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	char *ref = (char*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), /*source:*/str, /*sz:*/0/*not computed*/, BRIDGE_ALLOC_REF_COPY_STRINGZ);
	bridge_allocator_dec_level(ac);
	return ref;
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
static inline char *_br_copy_str_size(
	A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/,
	size_t sz/*0?*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	char *copy = (char*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), /*source:*/str, sz/*0 if not computed*/, BRIDGE_ALLOC_COPY_STRINGZ);
	bridge_allocator_dec_level(ac);
	return copy;
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
static inline char *_br_ref_str_size(
	A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/,
	size_t sz/*0?*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	char *ref = (char*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), /*source:*/str, sz/*0 if not computed*/, BRIDGE_ALLOC_REF_COPY_STRINGZ);
	bridge_allocator_dec_level(ac);
	return ref;
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
static inline char *_br_copy_sub_str(
	A_In_reads_bytes(len) const char *str/*!=NULL*/,
	A_In_range(<,(size_t)~(size_t)0) size_t len/*0?*/, BRIDGE_ALLOCATOR_ARG(ac))
{
	char *copy = (char*)ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(ac), /*source:*/str, len + 1/*'\0'*/, BRIDGE_ALLOC_COPY_SUBSTR);
	bridge_allocator_dec_level(ac);
	return copy;
}

#ifdef __cplusplus

A_Ret_restrict A_Check_return A_Ret_maybenull A_Post_writable_byte_size(sz)
inline void *bridge_allocator::alloc(A_In_range(>,0) size_t sz/*>0*/, bool z)
{
	return z ? bridge_ac_allocz(sz, this) : bridge_ac_alloc(sz, this);
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
inline char *bridge_allocator::copy_str(A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/, size_t sz/*0?*/)
{
	return bridge_ac_copy_str_size(str, sz/*0?*/, this);
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
inline char *bridge_allocator::ref_str(A_In_reads_bytes(sz) A_In_z const char *str/*'\0'-terminated*/, size_t sz/*0?*/)
{
	return bridge_ac_ref_str_size(str, sz/*0?*/, this);
}

A_Nonnull_all_args A_Check_return A_Ret_maybenull_z
inline char *bridge_allocator::copy_sub_str(
	A_In_reads_bytes(len) const char *str/*!=NULL*/,
	A_In_range(<,(size_t)~(size_t)0) size_t len/*0?*/)
{
	return bridge_ac_copy_sub_str(str, len, this);
}

A_Nonnull_arg(2)
inline void bridge_allocator::free(A_Pre_opt_valid A_Post_ptr_invalid void *mem/*NULL?*/)
{
	bridge_ac_free(mem, this);
}

#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif

#endif /* BITBRIDGE_ALLOC_H_INCLUDED */
