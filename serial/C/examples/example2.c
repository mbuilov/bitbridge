/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example2.c */

#include <stdio.h>

#include "gen_example.h"
#include "bitbridge/bitbridge_memstack.inl"

/* example 2:
 - use memstack arena allocator
 - serialize and then deserialize list of simple structures
 - pass bridge allocator implicitly via BRIDGE_DEFAULT_ALLOCATOR macro

 1) initialize local bridge heap allocator
 2) fill test persons list
 3) serialize it
 4) deserialize it
 5) cleanup
*/

static void *serialize(const struct person *persons, size_t *packed_size, struct bridge_allocator *ac)
{
	union bridge_pack_conv_info pci;
	bridge_pack_conv_info_init(&pci, /*convert:*/0, ac);
	{
		void *mem = person_pack(persons, &pci);
		*packed_size = pci.pi.size;
		return mem;
	}
}

static struct person *deserialize(const void **mem, size_t packed_size, struct bridge_allocator *ac)
{
	union bridge_unpack_conv_info uci;
	bridge_unpack_conv_info_init(&uci, *mem, packed_size, /*convert:*/0, ac);
	{
		struct person *persons = person_unpack(&uci);
		*mem = uci.ui.us.from;
		return persons;
	}
}

static struct person *create_persons_list(unsigned count, struct bridge_allocator *ac)
{
#define BRIDGE_DEFAULT_ALLOCATOR ac
	struct person *persons;
	struct person **tail = &persons;
	unsigned n = 0;
	for (; n < count; n++) {
		struct person *p = person_new();
		if (!p) {
			fprintf(stderr, "failed to allocate new person!");
			break;
		}
		*tail = p;
		tail = &p->onext;
		p->id = (int)n;
		{
			char name_buf[] = "person 65535";
			int len = sprintf(name_buf + str_buf_length("person "), "%u", n & 65535);
			p->name = bridge_copy_str_size(name_buf, str_buf_length("person ") + (unsigned)len + 1u/*'\0'*/);
			if (!p->name) {
				fprintf(stderr, "failed to copy string constant!");
				break;
			}
		}
	}
	*tail = NULL; /* terminate list */
	if (n != count) {
		person_delete(persons);
		return NULL;
	}
	return persons;
#undef BRIDGE_DEFAULT_ALLOCATOR
}

static int compare_persons_lists(const struct person *p1, const struct person *p2)
{
	for (;;) {
		if (!p1 != !p2) {
			fprintf(stderr, "wrong number of persons in deserialized list!");
			return -1;
		}
		if (!p1)
			break;
		if (p1->id != p2->id) {
			fprintf(stderr, "wrong deserialized person id!");
			return -1;
		}
		if (strcmp(p1->name, p2->name)) {
			fprintf(stderr, "wrong deserialized person name!");
			return -1;
		}
		p1 = p1->onext;
		p2 = p2->onext;
	}
	return 0; /* ok */
}

static void print_persons(const struct person *persons)
{
	while (persons) {
		printf("\nperson:");
		printf("\n\tid = %d", persons->id);
		printf("\n\tname = %s", persons->name);
		persons = persons->onext;
	}
}

int main(int argc, char *argv[])
{
	int err = 1;
	struct bridge_memstack_allocator mac = BRIDGE_MEMSTACK_ALLOCATOR_INITIALIZER;
	bridge_allocator_enable_log(&mac.ac, /*enable:*/0);
	memstack_enable_log(&mac.ms, 0);
	#define BRIDGE_DEFAULT_ALLOCATOR (&mac.ac)
	{
		struct person *persons = create_persons_list(100, BRIDGE_DEFAULT_ALLOCATOR);
		if (!persons)
			goto err0;
		{
			size_t packed_size;
			void *mem = serialize(persons, &packed_size, BRIDGE_DEFAULT_ALLOCATOR);
			if (!mem) {
				fprintf(stderr, "failed to serialize!");
				goto err1;
			}
			{
				const void *from = mem;
				struct person *persons2 = deserialize(&from, packed_size, BRIDGE_DEFAULT_ALLOCATOR);
				if (!persons2) {
					fprintf(stderr, "failed to deserialize!");
					goto err2;
				}
				if (from != (char*)mem + packed_size) {
					fprintf(stderr, "wrong packed size!");
					goto err3;
				}
				if (compare_persons_lists(persons, persons2))
					goto err3;
				if (argc < 2)
					print_persons(persons2);
				err = 0; /* ok */
err3:
				/* really doesn't need to free any memory here if using memstack allocator
				  - all memory will be freed by bridge_memstack_allocator_destroy() */
				person_delete(persons2);
			}
err2:
			/* really doesn't need to free any memory here if using memstack allocator
			  - all memory will be freed by bridge_memstack_allocator_destroy() */
			bridge_free(mem);
		}
err1:
		/* really doesn't need to free any memory here if using memstack allocator
		  - all memory will be freed by bridge_memstack_allocator_destroy() */
		person_delete(persons);
err0:
		;
	}
	#undef BRIDGE_DEFAULT_ALLOCATOR
	bridge_memstack_allocator_destroy(&mac);
	if (err)
		fprintf(stderr, "\nfailed\n");
	else if (argc < 3)
		printf("\nok\n");
	(void)argv;
	return err;
}
