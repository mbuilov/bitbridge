/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example1.c */

#include <stdio.h>

#include "gen_example.h"
#include "bitbridge_heap.inl"

/* example 1:
 - use heap allocator
 - serialize and then deserialize simple structure
 - pass bridge allocator explicitly

 1) initialize local bridge heap allocator
 2) fill test person structure
 3) serialize it
 4) deserialize it
 5) cleanup
*/

static void *serialize(const struct person *p, size_t *packed_size, struct bridge_allocator *ac)
{
	union bridge_pack_conv_info pci;
	bridge_pack_conv_info_init(&pci, /*convert:*/0, ac);
	{
		void *mem = person_pack(p, &pci);
		*packed_size = pci.pi.size;
		return mem;
	}
}

static struct person *deserialize(const void **mem, size_t packed_size, struct bridge_allocator *ac)
{
	union bridge_unpack_conv_info uci;
	bridge_unpack_conv_info_init(&uci, *mem, packed_size, /*convert:*/0, ac);
	{
		struct person *p = person_unpack(&uci);
		*mem = uci.ui.us.from;
		return p;
	}
}

static void print_person(const struct person *p)
{
	printf("\nperson:");
	printf("\n\tid = %d", p->id);
	printf("\n\tname = %s", p->name);
}

int main(int argc, char *argv[])
{
	int err = 1;
	struct bridge_heap_allocator hac;
	bridge_heap_allocator_init(&hac);
	bridge_allocator_enable_log(&hac.ac, /*enable:*/0);
	{
		struct person p;
		person_init(&p);
		p.id = 123;
		/* string constant may always be referenced - it never will be deleted */
		p.name = bridge_ac_ref_str_buf("person name", &hac.ac);
		if (!p.name) {
			printf("failed to copy string constant!");
			goto err1;
		}
		{
			size_t packed_size;
			void *mem = serialize(&p, &packed_size, &hac.ac);
			if (!mem) {
				printf("failed to serialize!");
				goto err1;
			}
			{
				const void *from = mem;
				struct person *p2 = deserialize(&from, packed_size, &hac.ac);
				if (!p2) {
					printf("failed to deserialize!");
					goto err2;
				}
				if (from != (char*)mem + packed_size) {
					printf("wrong packed size!");
					goto err3;
				}
				if (p.id != p2->id) {
					printf("wrong deserialized person id!");
					goto err3;
				}
				if (strcmp(p.name, p2->name)) {
					printf("wrong deserialized person name!");
					goto err3;
				}
				print_person(p2);
				err = 0; /* ok */
err3:
				person_ac_delete(p2, &hac.ac);
			}
err2:
			bridge_ac_free(mem, &hac.ac);
		}
err1:
		person_ac_destroy(&p, &hac.ac);
	}
	bridge_heap_allocator_destroy(&hac);
	printf(err ? "\nfailed\n" : "\nok\n");
	(void)argc, (void)argv;
	return err;
}
