#ifndef BITBRIDGE_MEMSTACK_REF_INL_INCLUDED
#define BITBRIDGE_MEMSTACK_REF_INL_INCLUDED

/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/* bitbridge_memstack_ref.inl */

/* bridge memory stack ref-allocator:
  - allocates memory via memstack_push(), doesn't frees memory;
  - allows references to source memory */

#include "memstack.h"
#include "bitbridge_comn.h"

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

struct bridge_memstack_ref_allocator {
	struct bridge_allocator ac;
	struct memstack ms;
	char s[1];
#ifdef __cplusplus
	inline void init();
	inline void destroy();
#endif
};

/* initializer for struct bridge_memstack_ref_allocator */
#define BRIDGE_MEMSTACK_REF_ALLOCATOR_INITIALIZER { \
	BRIDGE_ALLOCATOR_INITIALIZER( \
		bridge_memstack_ref_alloc_cb, \
		bridge_memstack_ref_free_cb, \
		bridge_allocator_log_cb \
	), MEMSTACK_STATIC_INITIALIZER, {'\0'} \
}

/* alloc_cb() for bridge memstack ref allocator */
A_Nonnull_arg(1) A_Check_return A_Ret_maybenull
static void *_bridge_memstack_ref_alloc_cb(
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
	A_Inout struct memstack *st,
	A_In_opt_z char *emptyz/*empty string,NULL?*/)
{
	void *mem;
	switch (at) {
		case BRIDGE_ALLOC_REF_COPY_STRINGZ:
			/* if sz is not computed or sz > 1 - just reference the source memory,
			   else - reference '\0' somewhere in allocator structure */
			if (sz) {
		case BRIDGE_ALLOC_COPY_STRINGZ:
				if (!sz) {
					/* sz not computed yet, compute it now, later will copy the source string */
					sz = BRIDGE_STRLEN((const char*)source) + 1u/*'\0'*/;
					BRIDGE_ASSERT(sz); /* assume no overflow in '+1u' */
				}
				/* fall through */
		case BRIDGE_ALLOC_COPY_SUBSTR:
				if (1u == sz && emptyz) {
					at = BRIDGE_ALLOC_MEM; /* don't copy source string */
					mem = emptyz; /* just reference '\0' somewhere in allocator structure */
					break;
				}
			}
			/* fall through */
		case BRIDGE_ALLOC_REF_ALIGNED1:
		case BRIDGE_ALLOC_REF_ALIGNED2:
		case BRIDGE_ALLOC_REF_ALIGNED4:
		case BRIDGE_ALLOC_REF_ALIGNED8:
		case BRIDGE_ALLOC_REF_COPY_ALIGNED1:
		case BRIDGE_ALLOC_REF_COPY_ALIGNED2:
		case BRIDGE_ALLOC_REF_COPY_ALIGNED4:
		case BRIDGE_ALLOC_REF_COPY_ALIGNED8:
		case BRIDGE_ALLOC_MEM:
		case BRIDGE_ALLOCZ_MEM:
		case BRIDGE_ALLOC_PACK:
		default:
			mem = bridge_check_ref_alignment(source, at);
			/* never copy source string for BRIDGE_ALLOC_REF_COPY_STRINGZ */
			BRIDGE_ASSERT(BRIDGE_ALLOC_REF_COPY_STRINGZ != at || mem == source);
			if (!mem) {
#ifdef BITBRIDGE_DEBUG
				mem = _memstack_push_(st, sz MEMSTACK_DEBUG_ARGS_PASS);
#else
				mem = memstack_push(st, sz);
#endif
			}
	}
	return BRIDGE_ALLOCATOR_COPY_RETURN(ac, mem, source, sz, at, /*ac_allows_ref_source:*/2);
}

/* alloc_cb() for struct bridge_memstack_ref_allocator */
A_Nonnull_arg(1) A_Check_return A_Ret_maybenull
static void *bridge_memstack_ref_alloc_cb(
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
	struct bridge_memstack_ref_allocator *mrac = (struct bridge_memstack_ref_allocator*)a;
	struct memstack *st = &mrac->ms;
	char *emptyz = mrac->s; /* just reference '\0' at end of struct bridge_memstack_ref_allocator */
	return _bridge_memstack_ref_alloc_cb(BRIDGE_PASS_ALLOCATOR_THROUGH(&mrac->ac), source, sz, at, st, emptyz);
}

/* free_cb() for struct bridge_memstack_ref_allocator */
A_Nonnull_arg(1)
static void bridge_memstack_ref_free_cb(
	BRIDGE_ALLOCATOR_ARG(ac),
	A_Pre_opt_valid A_Post_ptr_invalid void *mem)
{
	BRIDGE_DEBUG_ARGS_VOID /* ignore info about origin of the call */
	BRIDGE_LOG_FREE(ac, mem);
	/* really don't free anything */
}

/* initialize memstack ref allocator */
A_Nonnull_all_args
static inline void bridge_memstack_ref_allocator_init(A_Post_valid struct bridge_memstack_ref_allocator *mrac/*out*/)
{
	bridge_allocator_init(&mrac->ac, bridge_memstack_ref_alloc_cb, bridge_memstack_ref_free_cb, NULL);
	memstack_init(&mrac->ms);
	mrac->s[0] = '\0';
}

/* destroy memstack ref allocator: free memory allocated by memstack */
A_Nonnull_all_args
static void bridge_memstack_ref_allocator_destroy(A_Pre_valid A_Post_invalid struct bridge_memstack_ref_allocator *mrac)
{
	memstack_destroy(&mrac->ms);
	bridge_allocator_destroy(&mrac->ac);
}

#ifdef __cplusplus
inline void bridge_memstack_ref_allocator::init()
{
	bridge_memstack_ref_allocator_init(this);
}
inline void bridge_memstack_ref_allocator::destroy()
{
	bridge_memstack_ref_allocator_destroy(this);
}
#endif

/* suppress warnings about unused functions */
typedef int __fake__bridge_memstack_ref_alloc_cb_t[sizeof(&_bridge_memstack_ref_alloc_cb)];
typedef int __fake_bridge_memstack_ref_alloc_cb_t[sizeof(&bridge_memstack_ref_alloc_cb)];
typedef int __fake_bridge_memstack_ref_free_cb_t[sizeof(&bridge_memstack_ref_free_cb)];
typedef int __fake_bridge_memstack_ref_allocator_init_t[sizeof(&bridge_memstack_ref_allocator_init)];
typedef int __fake_bridge_memstack_ref_allocator_destroy_t[sizeof(&bridge_memstack_ref_allocator_destroy)];

#ifdef __cplusplus
}
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* BITBRIDGE_MEMSTACK_REF_INL_INCLUDED */
