#ifndef BITBRIDGE_LOG_CB_INL_INCLUDED
#define BITBRIDGE_LOG_CB_INL_INCLUDED

/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/* bitbridge_log_cb.inl */

/* implementation of log_cb() for generic struct bridge_allocator */

#ifdef BITBRIDGE_DEBUG

#if !defined(BRIDGE_DEBUG_PRINTF_AT) || !defined(BRIDGE_DEBUG_PRINTF_FREE) || !defined(BRIDGE_DEBUG_PRINTF_ALLOC)

#include <stdio.h> /* for fprintf() called from bridge_allocator_log_cb() */

#define BRIDGE_DEBUG_PRINTF_AT(ac, level_str, file, line) \
	fprintf(stderr, "bridge %p: %sat %s:%u\n", (void*)(ac), level_str, file, line)

#define BRIDGE_DEBUG_PRINTF_FREE(ac, level_str, mem, file, line) \
	fprintf(stderr, "bridge %p: %sfree %p at %s:%u\n", (void*)(ac), level_str, mem, file, line)

#define BRIDGE_DEBUG_PRINTF_ALLOC(ac, level_str, mem, sz, file, line) \
	fprintf(stderr, "bridge %p: %salloc %p[%lu] at %s:%u\n", (void*)(ac), level_str, mem, \
		(unsigned long)((sz) & ((unsigned long)~(unsigned long)0)), file, line)

#endif /* BRIDGE_DEBUG_PRINTF_AT, BRIDGE_DEBUG_PRINTF_FREE, BRIDGE_DEBUG_PRINTF_ALLOC */

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

/* log_cb() for generic struct bridge_allocator */
A_Nonnull_arg(1)
static void bridge_allocator_log_cb(
	A_Inout struct bridge_allocator *ac,
	A_In_z const char *file,
	unsigned line,
	size_t sz,
	void *mem)
{
	static const char level_buf[] = "                                                                  ";
	const char *level_str = level_buf;
	if ((unsigned)ac->trace_level < sizeof(level_buf) - 1u/*'\0'*/)
		level_str += sizeof(level_buf) - 1u/*'\0'*/ - (unsigned)ac->trace_level;
	if (ac == mem)
		BRIDGE_DEBUG_PRINTF_AT(ac, level_str, file, line);
	else if (!sz)
		BRIDGE_DEBUG_PRINTF_FREE(ac, level_str, mem, file, line);
	else
		BRIDGE_DEBUG_PRINTF_ALLOC(ac, level_str, mem, sz, file, line);
}

/* don't warn about unused functions */
typedef int __fake_bridge_allocator_log_cb_t[sizeof(&bridge_allocator_log_cb)];

#ifdef __cplusplus
}
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* BITBRIDGE_DEBUG */

#endif /* BITBRIDGE_LOG_CB_INL_INCLUDED */
