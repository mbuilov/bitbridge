/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* bridge_test.c */

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#include "bitbridge_heap.inl"
#include "bitbridge_memstack.inl"
#include "bitbridge_memstack_ref.inl"

#include "test_fn1.h"

#ifdef BITBRIDGE_DEBUG
#define LOOP_COUNT 1
#else
#define LOOP_COUNT 1000
#endif

static inline void crt_check_memory(void)
{
#ifdef WIN32
#ifdef _DEBUG
	_ASSERTE(_CrtCheckMemory());
#endif
#endif
}

/* enable to detect heap corruption and memory leaks in debug mode */
static inline void crt_enable_debug_memory(void)
{
#ifdef WIN32
#ifdef _DEBUG
	int f = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
#if 0
	_CRTDBG_ALLOC_MEM_DF        - enable debug heap allocations
	_CRTDBG_LEAK_CHECK_DF       - turn on leak-checking at program exit
	_CRTDBG_DELAY_FREE_MEM_DF   - keep freed memory blocks in linked list
	_CRTDBG_CHECK_CRT_DF        - debug internal memory operations in runtime libraries
	_CRTDBG_CHECK_ALWAYS_DF     - check memory on every alloc/free request
	_CRTDBG_CHECK_EVERY_16_DF   - check memory on every 16-th alloc/free request
	_CRTDBG_CHECK_EVERY_128_DF  - check memory on every 128-th alloc/free request
	_CRTDBG_CHECK_EVERY_1024_DF - check memory on every 1024-th alloc/free request
	_CRTDBG_CHECK_DEFAULT_DF    - disable memory checks
#endif
	f &= ~(_CRTDBG_CHECK_CRT_DF | _CRTDBG_CHECK_ALWAYS_DF);
	f |= _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_128_DF;
	_CrtSetDbgFlag(f);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
#endif
}

static int test1(unsigned loop_count)
{
	struct bridge_heap_allocator ha = BRIDGE_HEAP_ALLOCATOR_INITIALIZER;
	struct bridge_memstack_allocator ma = BRIDGE_MEMSTACK_ALLOCATOR_INITIALIZER;
	struct bridge_memstack_ref_allocator mra = BRIDGE_MEMSTACK_REF_ALLOCATOR_INITIALIZER;
#if 0
	memstack_enable_log(&ma.ms);
	memstack_enable_log(&mra.ms);
#endif
	/*bridge_allocator_enable_log(&ha.ac, 1);*/
	{
		int err = 0; /* ok */
		unsigned i = 0;
		for (; i < loop_count; i++) {
			if (test_fn1(
				/*struct_allocator:*/&ha.ac,
				/*pack_allocator:*/&ha.ac,
				/*unpack_allocator:*/&ha.ac,
				/*sost:*/&mra.ms,
				/*convert:*/0
			)) {
				err = 1; /* failed */
				break;
			}
			memstack_cleanup(&ma.ms);
			memstack_cleanup(&mra.ms);
			if (test_fn1(
				/*struct_allocator:*/&ma.ac,
				/*pack_allocator:*/&ma.ac,
				/*unpack_allocator:*/&ma.ac,
				/*sost:*/&mra.ms,
				/*convert:*/1
			)) {
				err = 2; /* failed */
				break;
			}
			memstack_cleanup(&ma.ms);
			memstack_cleanup(&mra.ms);
			if (test_fn1(
				/*struct_allocator:*/&ha.ac,
				/*pack_allocator:*/&ma.ac,
				/*unpack_allocator:*/&mra.ac,
				/*sost:*/&mra.ms,
				/*convert:*/0
			)) {
				err = 3; /* failed */
				break;
			}
			memstack_cleanup(&ma.ms);
			memstack_cleanup(&mra.ms);
		}
		bridge_heap_allocator_destroy(&ha);
		bridge_memstack_allocator_destroy(&ma);
		bridge_memstack_ref_allocator_destroy(&mra);
		return err;
	}
}

int main(int argc, char *argv[])
{
	int err;
	(void)argc, (void)argv;
	crt_enable_debug_memory();
	err = test1(LOOP_COUNT);
	crt_check_memory();
	if (err)
		printf("\n!FAILED TEST: %d\n", err);
	else
		printf("ALL TESTS PASSED\n");
	return err;
}
