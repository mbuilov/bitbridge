#ifndef BITBRIDGE_HEAP_INL_INCLUDED
#define BITBRIDGE_HEAP_INL_INCLUDED

/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/* bitbridge_heap.inl */

/* bridge heap allocator:
  - allocates memory in heap via standard malloc()/free() */

#include "bitbridge_comn.h"

#if !defined(BRIDGE_MALLOC) || !defined(BRIDGE_FREE)
#include <stdlib.h>
#define BRIDGE_MALLOC(sz) malloc(sz)
#define BRIDGE_FREE(mem)  free(mem)
#endif /* !defined(BRIDGE_MALLOC) || !defined(BRIDGE_FREE) */

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4505) /* Unreferenced local function has been removed */
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct bridge_heap_allocator {
	struct bridge_allocator ac;
	char s[1];
#ifdef __cplusplus
	inline void init();
	inline void destroy();
#endif
};

/* initializer for struct bridge_heap_allocator */
#define BRIDGE_HEAP_ALLOCATOR_INITIALIZER { \
	BRIDGE_ALLOCATOR_INITIALIZER( \
		bridge_heap_alloc_cb, \
		bridge_heap_free_cb, \
		bridge_allocator_log_cb \
	), {'\0'} \
}

/* alloc_cb() for bridge heap allocator */
A_Nonnull_arg(1) A_Check_return A_Ret_maybenull
static void *_bridge_heap_alloc_cb(
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
	enum bridge_alloc_type at,
	A_In_opt_z char *emptyz/*empty string,NULL?*/)
{
	void *mem = NULL;
	BRIDGE_DEBUG_ARGS_VOID /* ignore info about origin of the call */
	if (BRIDGE_ALLOC_STRING_BITS == ((unsigned)at & BRIDGE_ALLOC_STRING_BITS)) {
		BRIDGE_ASSERT(source);
		BRIDGE_ASSERT(
			BRIDGE_ALLOC_REF_COPY_STRINGZ == at ||
			BRIDGE_ALLOC_COPY_STRINGZ     == at ||
			BRIDGE_ALLOC_COPY_SUBSTR      == at);
		if (!sz) {
			/* sz not computed yet, compute it now */
			BRIDGE_ASSERT(
				BRIDGE_ALLOC_REF_COPY_STRINGZ == at ||
				BRIDGE_ALLOC_COPY_STRINGZ     == at);
			sz = BRIDGE_STRLEN((const char*)source) + 1u/*'\0'*/;
			BRIDGE_ASSERT(sz); /* assume no overflow in '+1u' */
		}
		if (1u == sz && emptyz) {
			at = BRIDGE_ALLOC_MEM; /* don't copy source string */
			mem = emptyz; /* just reference '\0' somewhere in allocator structure */
		}
	}
	if (!mem)
		mem = BRIDGE_MALLOC(sz);
	return BRIDGE_ALLOCATOR_COPY_RETURN(ac, mem, source, sz, at, /*ac_allows_ref_source:*/0);
}

/* free_cb() for bridge heap allocator */
A_Nonnull_arg(1)
static void _bridge_heap_free_cb(
	BRIDGE_ALLOCATOR_ARG(ac),
	A_Pre_opt_valid A_Post_ptr_invalid void *mem,
	A_In_opt_z char *emptyz/*empty string,NULL?*/)
{
	BRIDGE_DEBUG_ARGS_VOID /* ignore info about origin of the call */
	BRIDGE_LOG_FREE(ac, mem);
	/* don't free mem if it points into an allocator structure */
	if (mem != emptyz)
		BRIDGE_FREE(mem);
}

/* alloc_cb() for struct bridge_heap_allocator */
A_Nonnull_arg(1) A_Check_return A_Ret_maybenull
static void *bridge_heap_alloc_cb(
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
	enum bridge_alloc_type at)
{
	void *a = ac;
	struct bridge_heap_allocator *hac = (struct bridge_heap_allocator*)a;
	char *emptyz = hac->s; /* just reference '\0' at end of struct bridge_heap_allocator */
	return _bridge_heap_alloc_cb(BRIDGE_PASS_ALLOCATOR_THROUGH(&hac->ac), source, sz, at, emptyz);
}

/* free_cb() for struct bridge_heap_allocator */
A_Nonnull_arg(1)
static void bridge_heap_free_cb(
	BRIDGE_ALLOCATOR_ARG(ac),
	A_Pre_opt_valid A_Post_ptr_invalid void *mem)
{
	void *a = ac;
	struct bridge_heap_allocator *hac = (struct bridge_heap_allocator*)a;
	char *emptyz = hac->s; /* just reference '\0' at end of struct bridge_heap_allocator */
	_bridge_heap_free_cb(BRIDGE_PASS_ALLOCATOR_THROUGH(&hac->ac), mem, emptyz);
}

/* initialize heap allocator */
A_Nonnull_all_args
static inline void bridge_heap_allocator_init(A_Post_valid struct bridge_heap_allocator *hac/*out*/)
{
	bridge_allocator_init(&hac->ac, bridge_heap_alloc_cb, bridge_heap_free_cb, NULL);
	hac->s[0] = '\0';
}

/* destroy heap allocator */
A_Nonnull_all_args
static void bridge_heap_allocator_destroy(A_Pre_valid A_Post_invalid struct bridge_heap_allocator *hac)
{
	bridge_allocator_destroy(&hac->ac);
}

#ifdef __cplusplus
inline void bridge_heap_allocator::init()
{
	bridge_heap_allocator_init(this);
}
inline void bridge_heap_allocator::destroy()
{
	bridge_heap_allocator_destroy(this);
}
#endif

/* suppress warnings about unused functions */
typedef int __fake__bridge_heap_alloc_cb_t[sizeof(&_bridge_heap_alloc_cb)];
typedef int __fake__bridge_heap_free_cb_t[sizeof(&_bridge_heap_free_cb)];
typedef int __fake_bridge_heap_alloc_cb_t[sizeof(&bridge_heap_alloc_cb)];
typedef int __fake_bridge_heap_free_cb_t[sizeof(&bridge_heap_free_cb)];
typedef int __fake_bridge_heap_allocator_init_t[sizeof(&bridge_heap_allocator_init)];
typedef int __fake_bridge_heap_allocator_destroy_t[sizeof(&bridge_heap_allocator_destroy)];

#ifdef __cplusplus
}
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* BITBRIDGE_HEAP_INL_INCLUDED */
