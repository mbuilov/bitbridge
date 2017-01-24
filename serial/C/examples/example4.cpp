/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example4.cpp */

#include <stdio.h>

#include "gen_example.h"
#include "bitbridge_memstack_ref.inl"

/* example 4:
 - use c++ helpers
 - use memstack arena allocator which may reference source memory instead of copying it
 - serialize and then deserialize tree of structures
 - pass bridge allocator implicitly via BRIDGE_DEFAULT_ALLOCATOR macro

 1) initialize local bridge memstack allocator
 2) fill test persons tree
 3) serialize it
 4) deserialize it
 5) cleanup
*/

static void *serialize(const struct tree &tree, size_t &packed_size, struct bridge_allocator &ac)
{
	union bridge_pack_conv_info pci;
	pci.init(/*convert:*/false, ac);
	{
		void *mem = tree.pack(pci);
		packed_size = pci.size();
		return mem;
	}
}

static struct tree *deserialize(const void **mem, size_t packed_size, struct bridge_allocator &ac)
{
	union bridge_unpack_conv_info uci;
	uci.init(*mem, packed_size, /*convert:*/false, ac);
	{
		struct tree *tree = tree::unpack_new(uci);
		*mem = uci.from();
		return tree;
	}
}

static bool init_car(struct car &c, int k, struct bridge_allocator &ac)
{
	if (!c.set_name(ac.ref_str_const(
		0 == (k & 3) ? "porsche" :
		1 == (k & 3) ? "mitsubishi" :
		2 == (k & 3) ? "linfan" :
		"lada"
	))) {
		printf("failed to take a reference to string constant!");
		return false;
	}
	if (!c.new_rmodel(ac)) {
		printf("failed to allocate string container!");
		return false;
	}
	if (!c.rmodel_set(ac.ref_str_const(
		0 == (k % 3) ? "truck" :
		1 == (k % 3) ? "hatchback" :
		"cabriolet"
	))) {
		printf("failed to take a reference to string constant!");
		return false;
	}
	c.set_diesel(0 == (k & 1)).
		set_turbo(0 == (k & 3));
	return true;
}

static struct person *create_person(int id, struct bridge_allocator &ac)
{
	struct person *p = person::ac_new(ac);
	if (!p) {
		printf("failed to allocate new person!");
		return NULL;
	}
	p->set_id(id);
	{
		char name_buf[256];
		(void)sprintf(name_buf, "person %d", id);
		if (!p->set_name(ac.copy_str(name_buf))) {
			printf("failed to copy string!");
			goto err;
		}
	}
	{
		size_t n_cars = (unsigned)id % 11;
		if (n_cars) {
			if (!p->new_acars(n_cars, ac)) {
				printf("failed to allocate array of cars!");
				goto err;
			}
			{
				for (size_t i = 0, n = p->get_acars_count(); i < n; i++) {
					if (!init_car(p->acars_get(i), id + (int)i, ac))
						goto err;
				}
			}
		}
	}
	p->set_male(!!(id & 1));
	if (!p->new_rbirth_month_year(ac)) {
		printf("failed to allocate birth_day_year!");
		goto err;
	}
	p->rbirth_month_year_set(0, (short)((id*3) % 12)).
		rbirth_month_year_set(1, (short)((id*2) % 9999));
	return p;
err:
	p->ac_delete(ac);
	return NULL;
}

static bool create_tree(struct tree **parent, int id, unsigned count, struct bridge_allocator &ac)
{
	for (;;) {
		struct tree *t = tree::ac_new(ac);
		if (!t) {
			printf("failed to allocate new tree!");
			return false;
		}
		*parent = t;
		if (!t->set_rp(create_person(id++, ac)))
			return false;
		if (!--count)
			return true;
		if (count > 1) {
			unsigned left = count >> 1;
			if (!create_tree(&t->oleft, id, left, ac))
				return false;
			id += (int)left;
			count -= left;
		}
		parent = &t->oright;
	}
}

static bool compare_cars(const struct car &c1, const struct car &c2)
{
	if (strcmp(c1.get_name(), c2.get_name())) {
		printf("wrong deserialized car name!");
		return false;
	}
	if (strcmp(c1.rmodel_get(), c2.rmodel_get())) {
		printf("wrong deserialized model name!");
		return false;
	}
	if (c1.get_diesel() != c2.get_diesel()) {
		printf("wrong deserialized diesel property!");
		return false;
	}
	if (c1.get_turbo() != c2.get_turbo()) {
		printf("wrong deserialized turbo property!");
		return false;
	}
	return true;
}

static bool compare_persons(const struct person &p1, const struct person &p2)
{
	if (p1.get_id() != p2.get_id()) {
		printf("wrong deserialized person id!");
		return false;
	}
	if (strcmp(p1.get_name(), p2.get_name())) {
		printf("wrong deserialized person name!");
		return false;
	}
	if (p1.get_acars_count() != p2.get_acars_count()) {
		printf("wrong number of cars!");
		return false;
	}
	{
		for (size_t i = 0, n = p1.get_acars_count(); i < n; i++) {
			if (!compare_cars(p1.acars_get(i), p2.acars_get(i)))
				return false;
		}
	}
	if (p1.get_male() != p2.get_male()) {
		printf("wrong deserialized male property()!");
		return false;
	}
	if (p1.rbirth_month_year_get(0) != p2.rbirth_month_year_get(0)) {
		printf("wrong deserialized birth month!");
		return false;
	}
	if (p1.rbirth_month_year_get(1) != p2.rbirth_month_year_get(1)) {
		printf("wrong deserialized birth year!");
		return false;
	}
	return true;
}

static bool compare_trees(const struct tree *t1, const struct tree *t2)
{
	for (;;) {
		if (!t1 != !t2) {
			printf("wrong number of persons in deserialized tree!");
			return false;
		}
		if (!t1)
			return true;
		if (!compare_persons(t1->rp_get(), t2->rp_get()))
			return false;
		if (!compare_trees(t1->get_oleft(), t2->get_oleft()))
			return false;
		t1 = t1->get_oright();
		t2 = t2->get_oright();
	}
}

static void print_car(const struct car &c)
{
	printf("\n\tcar:");
	printf("\n\t\tname = %s", c.get_name());
	printf("\n\t\tmodel = %s", c.rmodel_get());
	printf("\n\t\tdiesel = %s", c.get_diesel() ? "true" : "false");
	printf("\n\t\tturbo = %s", c.get_turbo() ? "true" : "false");
}

static void print_person(const struct person &p)
{
	printf("\nperson:");
	printf("\n\tid = %d", p.get_id());
	printf("\n\tname = %s", p.get_name());
	{
		for (size_t i = 0, n = p.get_acars_count(); i < n; i++)
			print_car(p.acars_get(i));
	}
	printf("\n\tgender = %s", p.get_male() ? "male" : "female");
	printf("\n\tbirth month = %d", p.rbirth_month_year_get(0));
	printf("\n\tbirth year = %d", p.rbirth_month_year_get(1));
}

static void print_tree(const struct tree *t)
{
	while (t) {
		print_person(t->rp_get());
		print_tree(t->get_oleft());
		t = t->get_oright();
	}
}

int main(int argc, char *argv[])
{
	int err = 1;
	struct bridge_memstack_ref_allocator mrac;
	mrac.init();
	mrac.ac.enable_log(false);
	memstack_disable_log(&mrac.ms);
	{
		struct tree *tree = NULL;
		if (!create_tree(&tree, 0, 10000, mrac.ac))
			goto err0;
		{
			size_t packed_size;
			void *mem = serialize(*tree, packed_size, mrac.ac);
			if (!mem) {
				printf("failed to serialize!");
				goto err1;
			}
			{
				const void *from = mem;
				struct tree *tree2 = deserialize(&from, packed_size, mrac.ac);
				if (!tree2) {
					printf("failed to deserialize!");
					goto err2;
				}
				if (from != (char*)mem + packed_size) {
					printf("wrong packed size!");
					goto err3;
				}
				if (!compare_trees(tree, tree2))
					goto err3;
				if (argc > 1)
					print_tree(tree2);
				err = 0; /* ok */
err3:
				/* really doesn't need to free any memory here if using memstack allocator
				  - all memory will be freed by mrac.destroy() */
				tree2->ac_delete(mrac.ac);
			}
err2:
			/* really doesn't need to free any memory here if using memstack allocator
			  - all memory will be freed by mrac.destroy() */
			mrac.ac.free(mem);
		}
err1:
		/* really doesn't need to free any memory here if using memstack allocator
		  - all memory will be freed by mrac.destroy() */
		tree->ac_delete(mrac.ac);
err0:
		;
	}
	mrac.destroy();
	printf(err ? "\nfailed\n" : "\nok\n");
	(void)argv;
	return err;
}
