#ifndef TEST_FN1_H_INCLUDED
#define TEST_FN1_H_INCLUDED

/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* test_fn1.h */

struct bridge_allocator;
struct memstack;

int test_fn1(
	struct bridge_allocator *struct_allocator,
	struct bridge_allocator *pack_allocator,
	struct bridge_allocator *unpack_allocator,
	struct memstack *sost,
	int convert);

#endif /* TEST_FN1_H_INCLUDED */
