/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example3.c */

#include <stdio.h>

#include "gen_example.h"
#include "bitbridge_memstack_ref.inl"

/* example 3:
 - use memstack arena allocator which may reference source memory instead of copying it
 - serialize and then deserialize list of simple structures
 - pass bridge allocator implicitly via BRIDGE_DEFAULT_ALLOCATOR macro

 1) initialize local bridge memstack allocator
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
			printf("failed to allocate new person!");
			break;
		}
		*tail = p;
		tail = &p->onext;
		p->id = (int)n;
		{
			char name_buf[] = "person 1048575";
			(void)sprintf(name_buf + str_buf_length("person "), "%u", n & 1048575);
			p->name = bridge_copy_str(name_buf);
			if (!p->name) {
				printf("failed to copy string constant!");
				break;
			}
		}
		if (!(n % 3)) {
			struct hobby *hobby = person_new_ohobby(p);
			if (!hobby) {
				printf("failed to allocate hobby structure!");
				break;
			}
			/* string constant may always be referenced - it never will be deleted */
			switch (n % 5) {
				case 0: hobby->name = bridge_ref_str_const("football"); break;
				case 1: hobby->name = bridge_ref_str_const("volleyball"); break;
				case 2: hobby->name = bridge_ref_str_const("basketball"); break;
				case 3: hobby->name = bridge_ref_str_const("handball"); break;
				case 4: hobby->name = bridge_ref_str_const("swimming"); break;
			}
			if (!hobby->name) {
				printf("failed to reference hobby name!");
				break;
			}
		}
		p->male = (n & 1) ? 1u : 0u;
		{
			int (*birth_month_year)[2] = person_new_rbirth_month_year(p);
			if (!birth_month_year) {
				printf("failed to allocate birth_day_year!");
				break;
			}
			(*birth_month_year)[0] = (int)((n*3) % 12);
			(*birth_month_year)[1] = (int)((n*2) % 9999);
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
			printf("wrong number of persons in deserialized list!");
			return -1;
		}
		if (!p1)
			break;
		if (p1->id != p2->id) {
			printf("wrong deserialized person id!");
			return -1;
		}
		if (strcmp(p1->name, p2->name)) {
			printf("wrong deserialized person name!");
			return -1;
		}
		if (!p1->ohobby != !p2->ohobby) {
			printf("wrong person hobby in deserialized list");
			return -1;
		}
		if (p1->ohobby && strcmp(p1->ohobby->name, p2->ohobby->name)) {
			printf("wrong hobby name in deserialized list");
			return -1;
		}
		if (p1->male != p2->male) {
			printf("wrong male bit in deserialized list");
			return -1;
		}
		if ((*p1->rbirth_month_year)[0] != (*p2->rbirth_month_year)[0]) {
			printf("wrong birth month in deserialized list");
			return -1;
		}
		if ((*p1->rbirth_month_year)[1] != (*p2->rbirth_month_year)[1]) {
			printf("wrong birth year in deserialized list");
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
		if (persons->ohobby)
			printf("\n\thobby = %s", persons->ohobby->name);
		printf("\n\tgender = %s", persons->male ? "male" : "female");
		printf("\n\tbirth month = %d", (*persons->rbirth_month_year)[0]);
		printf("\n\tbirth year = %d", (*persons->rbirth_month_year)[1]);
		persons = persons->onext;
	}
}

int main(int argc, char *argv[])
{
	int err = 1;
	struct bridge_memstack_ref_allocator mrac = BRIDGE_MEMSTACK_REF_ALLOCATOR_INITIALIZER;
	bridge_allocator_enable_log(&mrac.ac, /*enable:*/0);
	memstack_disable_log(&mrac.ms);
	#define BRIDGE_DEFAULT_ALLOCATOR (&mrac.ac)
	{
		struct person *persons = create_persons_list(100000, BRIDGE_DEFAULT_ALLOCATOR);
		if (!persons)
			goto err0;
		{
			size_t packed_size;
			void *mem = serialize(persons, &packed_size, BRIDGE_DEFAULT_ALLOCATOR);
			if (!mem) {
				printf("failed to serialize!");
				goto err1;
			}
			{
				const void *from = mem;
				struct person *persons2 = deserialize(&from, packed_size, BRIDGE_DEFAULT_ALLOCATOR);
				if (!persons2) {
					printf("failed to deserialize!");
					goto err2;
				}
				if (from != (char*)mem + packed_size) {
					printf("wrong packed size!");
					goto err3;
				}
				if (compare_persons_lists(persons, persons2))
					goto err3;
				if (argc < 2)
					print_persons(persons2);
				err = 0; /* ok */
err3:
				/* really doesn't need to free any memory here if using memstack allocator
				  - all memory will be freed by bridge_memstack_ref_allocator_destroy() */
				person_delete(persons2);
			}
err2:
			/* really doesn't need to free any memory here if using memstack allocator
			  - all memory will be freed by bridge_memstack_ref_allocator_destroy() */
			bridge_free(mem);
		}
err1:
		/* really doesn't need to free any memory here if using memstack allocator
		  - all memory will be freed by bridge_memstack_ref_allocator_destroy() */
		person_delete(persons);
err0:
		;
	}
	#undef BRIDGE_DEFAULT_ALLOCATOR
	bridge_memstack_ref_allocator_destroy(&mrac);
	if (err)
		printf("\nfailed\n");
	else if (argc < 3)
		printf("\nok\n");
	(void)argv;
	return err;
}
