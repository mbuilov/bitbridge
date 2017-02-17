/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* model.c */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "model.h"

static void init_check_determine_structs_layout(void);
static void determine_convertable_need_destroy_packed_bits_bytes_simple_fields(void);
static void determine_convertable_recursive_need_destroy_packed_bits_bytes_user_type_fields(void);
static void determine_unknown_packed_bits_bytes_recursive_user_types_check_structs(void);
static void determine_tail_recursive(void);
static void priority_sort_fields(void);
static void sort_structs(void);

struct struct_def **structs = NULL;
struct struct_def **structs_end = NULL;

void build_model(void)
{
	init_check_determine_structs_layout();
	determine_convertable_need_destroy_packed_bits_bytes_simple_fields();
	determine_convertable_recursive_need_destroy_packed_bits_bytes_user_type_fields();
	determine_unknown_packed_bits_bytes_recursive_user_types_check_structs();
	determine_tail_recursive();
	priority_sort_fields();
	sort_structs();
}

#ifdef __GNUC__
__attribute__ ((pure))
#endif
static struct struct_def *find_struct(const char *s_name, const char *f_type)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		if (!strcmp((*s)->s_name, f_type))
			return *s;
	}
	err("structure '%s': unresolved user-defined type reference: '%s'\n", s_name, f_type);
}

static void unlimited_sized_struct_check(void);
static void final_determ_structs_layout(void);
static void final_determ_equivalent(void);
static void final_determ_ptr_arr(void);

static void init_check_determine_structs_layout(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		(*s)->tail_recursive        = NULL;
		(*s)->equivalent            = NULL;
		(*s)->s_layout              = S_STATIC1;
		(*s)->bit_offset            = 0;
		(*s)->need_destroy          = 0;
		(*s)->_marked               = 0;
		(*s)->_processed            = 0;
		(*s)->equiv_many            = 0;
		(*s)->recursion_entry       = 0;
		(*s)->convertable           = 0;
		(*s)->unknown_packed_bits   = 0;
		(*s)->unknown_packed_bytes  = 0;
		(*s)->ptr_arr               = 0;
		(*s)->_aux                  = 0;
		(*s)->packed_bits           = 0;
		(*s)->packed_optional_bits  = 0;
		(*s)->packed_bytes          = 0;
		(*s)->packed_optional_bytes = 0;
		{
			struct field_def *const *f = (*s)->fields;
			const char *eq = NULL;
			for (; f < (*s)->fields_end; f++) {
				(*f)->bit = 0;
				(*f)->user_type = is_reserved_type((*f)->type) ? NULL : find_struct((*s)->s_name, (*f)->type);
				if (S_DYNAMIC != (*s)->s_layout) {
					if ((*f)->f_power > F_REQUIRED ||
						field_type_str == (*f)->type ||
						field_type_pstr == (*f)->type ||
						(F_OPTIONAL == (*f)->f_power && (*f)->user_type))
					{
						(*s)->s_layout = S_DYNAMIC;
					}
					else if ((*f)->user_type)
						(*s)->s_layout = (enum struct_layout)(8 | (*s)->s_layout); /* yet undefined layout, will be defined later */
					else if (F_OPTIONAL == (*f)->f_power)
						(*s)->s_layout = (enum struct_layout)(((*s)->s_layout & 8) | S_MONOLITH);
					else if (
						field_type_short == (*f)->type ||
						field_type_pshort == (*f)->type ||
						field_type_ppshort == (*f)->type)
					{
						if (((*s)->s_layout & 7) < S_STATIC2)
							(*s)->s_layout = (enum struct_layout)(((*s)->s_layout & 8) | S_STATIC2);
					}
					else if (
						field_type_int == (*f)->type ||
						field_type_pint == (*f)->type ||
						field_type_ppint == (*f)->type ||
						field_type_float == (*f)->type)
					{
						if (((*s)->s_layout & 7) < S_STATIC4)
							(*s)->s_layout = (enum struct_layout)(((*s)->s_layout & 8) | S_STATIC4);
					}
					else if (
						field_type_long == (*f)->type ||
						field_type_plong == (*f)->type ||
						field_type_pplong == (*f)->type ||
						field_type_double == (*f)->type)
					{
						if (((*s)->s_layout & 7) < S_STATIC8)
							(*s)->s_layout = (enum struct_layout)(((*s)->s_layout & 8) | S_STATIC8);
					}
				}
				if (F_REQUIRED != (*f)->f_power)
					(*s)->equiv_many = 1; /* temporary mark the structure that it have no equivalent type */
				else if (!(*f)->user_type) {
					if (!eq) {
						eq = (*f)->type;
						if (field_type_bit == eq)
							(*s)->equiv_many = 1; /* temporary mark the structure that it have no equivalent type */
					}
					else if (eq != (*f)->type)
						(*s)->equiv_many = 1; /* temporary mark the structure that it have no equivalent type */
				}
				/* check if field is a pointer(s), check (*f)->f_power:
				  --------------------------------------------------------------------------
				  |           |  REQUIRED   |  OPTIONAL  |  POINTER   | ARRAY |  DEFAULT   |
				  |-----------+-------------+------------+------------+-------+------------|
				  | user_type | check later | !S_DYNAMIC | !S_DYNAMIC |  no   | !S_DYNAMIC |
				  | bit       |     no      |     no     | impossible |  no   |     yes    |
				  | byte      |     no      |     no     |     yes    |  no   |     yes    |
				  | str       |     yes     |     yes    |     no     |  no   |     no     |
				  -------------------------------------------------------------------------- */
				if (!(*s)->ptr_arr) {
					if (F_ARRAY == (*f)->f_power)
						(*s)->ptr_arr = 1; /* temporary mark the structure that it can't be destroyed as array of pointers */
					else if (!(*f)->user_type) {
						if (field_type_str == (*f)->type || field_type_pstr == (*f)->type) {
							if (F_REQUIRED != (*f)->f_power && F_OPTIONAL != (*f)->f_power)
								(*s)->ptr_arr = 1; /* temporary mark the structure that it can't be destroyed as array of pointers */
						}
						else if (F_REQUIRED == (*f)->f_power || F_OPTIONAL == (*f)->f_power)
							(*s)->ptr_arr = 1; /* temporary mark the structure that it can't be destroyed as array of pointers */
					}
				}
			}
			if (!(*s)->equiv_many)
				(*s)->equivalent = eq; /* NULL? */
		}
	}
	unlimited_sized_struct_check();
	final_determ_structs_layout();
	final_determ_equivalent();
	final_determ_ptr_arr();
}

struct stack_item {
	struct struct_def *s;
	struct field_def *const *f;
};

static size_t capacity = 0;
static struct stack_item *stack = NULL;

#define STACK_GROW_BY 16

static void grow_stack(void)
{
	if ((size_t)~(size_t)0 - capacity*sizeof(*stack) < STACK_GROW_BY*sizeof(*stack))
		err("stack overflow\n");
	capacity += STACK_GROW_BY;
	stack = (struct stack_item*)mem_realloc(stack, sizeof(*stack)*capacity);
}

#ifdef __GNUC__
__attribute__ ((noreturn))
#endif
static void unlimited_size_error(const struct stack_item *item, size_t depth, const struct struct_def *s, const struct field_def *f)
{
	fprintf(stderr, "error: structure '%s' has unlimited size:\n", f->user_type->s_name);
	if (depth) {
		do {
			fprintf(stderr, "\tstructure '%s' with required field '%s' which is the\n", item->s->s_name, (*item->f)->name);
		} while (++item != stack + depth);
	}
	fprintf(stderr, "\tstructure '%s' with required field '%s' which is the\n", s->s_name, f->name);
	fprintf(stderr, "\tstructure '%s' again\n", f->user_type->s_name);
	exit(-1);
}

static void unlimited_sized_struct_check_inner(struct struct_def *s)
{
	size_t depth = 0;
	struct stack_item *item;
	struct field_def *const *f;
process_struct:
	for (f = s->fields; f < s->fields_end; f++) {
		if ((*f)->user_type && !(*f)->user_type->_processed && F_OPTIONAL != (*f)->f_power && F_ARRAY != (*f)->f_power) {
			/* check stack for infinite-sized struct */
			if (s == (*f)->user_type)
				unlimited_size_error(NULL, 0, s, *f);
			else for (item = stack + depth; item != stack;) {
				if ((--item)->s == (*f)->user_type)
					unlimited_size_error(item, depth, s, *f);
			}
			/* recursive check */
			if (capacity <= depth)
				grow_stack();
			/* save vars into stack */
			item = stack + (depth++);
			item->s = s;
			item->f = f;
			s = (*f)->user_type;
			goto process_struct;
process_struct_return:
			/* restore vars from stack */
			item = stack + (--depth);
			s = item->s;
			f = item->f;
		}
	}
	s->_processed = 1; /* structure was checked for unlimited size */
	if (depth)
		goto process_struct_return;
}

static void unlimited_sized_struct_check(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		if (!(*s)->_processed)
			unlimited_sized_struct_check_inner(*s);
	}
	/* NOTE: all structures are now marked as 'processed' with _processed bit set */
}

static void final_determ_structs_layout_inner(struct struct_def *s)
{
	size_t depth = 0;
	struct stack_item *item;
	struct field_def *const *f;
check_fields:
	for (f = s->fields; f < s->fields_end; f++) {
		if ((*f)->user_type) {
			/* NOTE: (*f)->f_power == F_REQUIRED, else s->s_layout == S_DYNAMIC */
			for (;;) {
				if (S_DYNAMIC == (*f)->user_type->s_layout) {
					s->s_layout = S_DYNAMIC;
					goto next_struct;
				}
				if ((*f)->user_type->s_layout < S_DYNAMIC) {
					if (s->s_layout < (8 | (*f)->user_type->s_layout))
						s->s_layout = (enum struct_layout)(8 | (*f)->user_type->s_layout);
					break; /* check next field */
				}
				/* recursive check - (*f)->user_type->s_layout is undefined yet */
				if (capacity <= depth)
					grow_stack();
				/* save vars into stack */
				item = stack + (depth++);
				item->s = s;
				item->f = f;
				s = (*f)->user_type;
				goto check_fields;
check_fields_return:
				/* restore vars from stack */
				item = stack + (--depth);
				s = item->s;
				f = item->f;
				/* ok, (*f)->user_type->s_layout now must be defined */
			} /* for (;;) */
		}
	}
	s->s_layout = (enum struct_layout)(s->s_layout & 7); /* finally define layout */
next_struct:
	if (depth)
		goto check_fields_return;
}

/* must be called after unlimited_sized_struct_check() */
static void final_determ_structs_layout(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		/* check only structs with undefined layout */
		if ((*s)->s_layout > S_DYNAMIC)
			final_determ_structs_layout_inner(*s);
	}
}

static void final_determ_equivalent_inner(struct struct_def *s)
{
	size_t depth = 0;
	struct stack_item *item;
	struct field_def *const *f;
check_fields:
	for (f = s->fields; f < s->fields_end; f++) {
		if ((*f)->user_type) {
			/* NOTE: (*f)->f_power == F_REQUIRED, else s->equiv_many == 1 */
			for (;;) {
				if ((*f)->user_type->equiv_many) {
					s->equiv_many = 1; /* temporary mark the structure that it have no equivalent type */
					goto next_struct;
				}
				if ((*f)->user_type->_marked) {
					if (!s->equivalent)
						s->equivalent = (*f)->user_type->equivalent;
					else if (s->equivalent != (*f)->user_type->equivalent) {
						s->equiv_many = 1; /* temporary mark the structure that it have no equivalent type */
						goto next_struct;
					}
					break; /* check next field */
				}
				/* recursive check - (*f)->user_type equivalence is undefined yet */
				if (capacity <= depth)
					grow_stack();
				/* save vars into stack */
				item = stack + (depth++);
				item->s = s;
				item->f = f;
				s = (*f)->user_type;
				goto check_fields;
check_fields_return:
				/* restore vars from stack */
				item = stack + (--depth);
				s = item->s;
				f = item->f;
				/* ok, (*f)->user_type equivalence now must be defined */
			} /* for (;;) */
		}
	}
	s->_marked = 1; /* equivalence is finally defined */
next_struct:
	if (depth)
		goto check_fields_return;
}

#ifdef __GNUC__
__attribute__ ((pure))
#endif
static unsigned is_struct_equiv_many(const struct struct_def *s)
{
	for (;;) {
		if (s->fields + 1 != s->fields_end)
			return 1; /* yes, more that one field */
		s = s->fields[0]->user_type;
		if (!s)
			return 0; /* only one field of non-user type */
	}
}

/* must be called after unlimited_sized_struct_check() */
static void final_determ_equivalent(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		/* check only structs that may have equivalent type */
		if (!(*s)->equiv_many && !(*s)->_marked)
			final_determ_equivalent_inner(*s);
	}
	for (s = structs; s < structs_end; s++) {
		if ((*s)->equiv_many) {
			(*s)->equiv_many = 0; /* just unmark */
			(*s)->equivalent = NULL; /* structure have no equivalent type */
		}
		else
			(*s)->equiv_many = 1u & is_struct_equiv_many(*s);
		(*s)->_marked = 0; /* unmark */
	}
}

static void final_determ_ptr_arr_inner(struct struct_def *s)
{
	size_t depth = 0;
	struct stack_item *item;
	struct field_def *const *f;
check_fields:
	for (f = s->fields; f < s->fields_end; f++) {
		if ((*f)->user_type) {
			/* NOTE: (*f)->f_power != F_ARRAY, else s->ptr_arr == 1 */
			if (F_REQUIRED != (*f)->f_power) {
				if (S_DYNAMIC == (*f)->user_type->s_layout) {
					s->ptr_arr = 1; /* temporary mark the structure that it can't be destroyed as array of pointers */
					goto next_struct;
				}
				continue; /* check next field */
			}
			for (;;) {
				if ((*f)->user_type->ptr_arr) {
					s->ptr_arr = 1; /* temporary mark the structure that it can't be destroyed as array of pointers */
					goto next_struct;
				}
				if ((*f)->user_type->_marked)
					break; /* check next field */
				/* recursive check - it's not known yet if (*f)->user_type may be destroyed as array of pointers */
				if (capacity <= depth)
					grow_stack();
				/* save vars into stack */
				item = stack + (depth++);
				item->s = s;
				item->f = f;
				s = (*f)->user_type;
				goto check_fields;
check_fields_return:
				/* restore vars from stack */
				item = stack + (--depth);
				s = item->s;
				f = item->f;
				/* ok, now it is known if (*f)->user_type may be destroyed as array of pointers */
			} /* for (;;) */
		}
	}
	s->_marked = 1; /* finally defined if the structure may be destroyed as array of pointers */
next_struct:
	if (depth)
		goto check_fields_return;
}

#ifdef __GNUC__
__attribute__ ((pure))
#endif
static unsigned is_struct_many_ptrs(const struct struct_def *s)
{
	for (;;) {
		if (s->fields + 1 != s->fields_end)
			return 1; /* yes, more that one pointer */
		s = s->fields[0]->user_type;
		if (!s || !s->ptr_arr)
			return 0; /* only one field of non-user type or non-required field of user type */
	}
}

/* must be called after final_determ_structs_layout() */
static void final_determ_ptr_arr(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		/* check only structs that may be destroyed as array of pointers */
		if (!(*s)->ptr_arr && !(*s)->_marked)
			final_determ_ptr_arr_inner(*s);
	}
	for (s = structs; s < structs_end; s++) {
		(*s)->ptr_arr = 1u & !(*s)->ptr_arr;
		if ((*s)->ptr_arr && !(*s)->equivalent)
			(*s)->equiv_many = 1u & is_struct_many_ptrs(*s);
		(*s)->_marked = 0; /* unmark */
	}
}

#if 0
#define DPR(a,b,c,d,e) printf(a,b,c,d,e)
#else
#define DPR(a,b,c,d,e)
#endif

/* must be called after init_check_determine_structs_layout() */
static void determine_convertable_need_destroy_packed_bits_bytes_simple_fields(void)
{
	/* this procedure ignores user-typed fields - those fields are counted at next step */
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		struct field_def *const *f = (*s)->fields;
		for (; f < (*s)->fields_end; f++) {
			if (!(*f)->user_type && (
				field_type_short  == (*f)->type ||
				field_type_int    == (*f)->type ||
				field_type_long   == (*f)->type ||
				field_type_float  == (*f)->type ||
				field_type_double == (*f)->type))
			{
				(*s)->convertable = 1;
			}
			switch ((*f)->f_power) {
				case F_ARRAY:
					(*s)->need_destroy++; /* NOTE: cannot overflow: one value for each field at maximum */
					/* array counter has variable bytes >= 1 */
					DPR("%s: bytes(%u)+=%u (field: []%s)\n", (*s)->s_name, (*s)->packed_bytes, 1, (*f)->name);
					if (~0u == (*s)->packed_bytes)
						err("structure '%s': too many packed bytes\n", (*s)->s_name);
					(*s)->packed_bytes++;
					/* array has unknown bytes, except for an array of user types having only bits */
					if (!(*f)->user_type || ~0u - (BRIDGE_MAX_COUNTER_PACKED_SIZE - 1u) < (*s)->packed_optional_bytes)
						(*s)->unknown_packed_bytes = 1; /* invalidates (*s)->packed_optional_bytes */
					else {
						DPR("+%s: optional_bytes(%u)+=%u (field: []%s)\n",
							(*s)->s_name, (*s)->packed_optional_bytes, BRIDGE_MAX_COUNTER_PACKED_SIZE - 1u, (*f)->name);
						(*s)->packed_optional_bytes += BRIDGE_MAX_COUNTER_PACKED_SIZE - 1u;
					}
					/* if (*f)->user_type, then determine unknown_packed_bits, unknown_packed_bytes and convertable later */
					break;
				case F_OPTIONAL:
					DPR("%s: bits(%u)+=%u (field: *%s)\n", (*s)->s_name, (*s)->packed_bits, 1, (*f)->name);
					(*s)->packed_bits++; /* NOTE: cannot overflow here - one bit for each field at maximum */
					if ((*f)->user_type || field_type_str == (*f)->type || field_type_pstr == (*f)->type)
						(*s)->need_destroy++; /* NOTE: cannot overflow: one value for each field at maximum */
					else {
						if (~0u == (*s)->bit_offset)
							err("structure '%s': too many bit/optional fields\n", (*s)->s_name);
						(*f)->bit = (*s)->bit_offset++; /* bit number that defines that field is filled in */
					}
					if (field_type_bit == (*f)->type) {
						if (~0u == (*s)->bit_offset)
							err("structure '%s': too many bit/optional fields\n", (*s)->s_name);
						(*s)->bit_offset++; /* store field value in the same bits array that defines which fields are filled in */
						DPR("+%s: optional_bits(%u)+=%u (field: *%s)\n", (*s)->s_name, (*s)->packed_optional_bits, 1, (*f)->name);
						(*s)->packed_optional_bits++; /* NOTE: cannot overflow: one value for each field at maximum */
					}
					else if (field_type_str == (*f)->type || field_type_pstr == (*f)->type) {
						/* packed string has unknown bytes >= 1 */
						(*s)->unknown_packed_bytes = 1; /* invalidates (*s)->packed_optional_bytes */
					}
					else if (!(*f)->user_type) {
						unsigned os =
							field_type_short   == (*f)->type ? 2u :
							field_type_pshort  == (*f)->type ? 3u :
							field_type_ppshort == (*f)->type ? 3u :
							field_type_int     == (*f)->type ? 4u :
							field_type_pint    == (*f)->type ? 5u :
							field_type_ppint   == (*f)->type ? 5u :
							field_type_long    == (*f)->type ? 8u :
							field_type_plong   == (*f)->type ? 9u :
							field_type_pplong  == (*f)->type ? 9u :
							field_type_float   == (*f)->type ? 4u :
							field_type_double  == (*f)->type ? 8u :
							/*field_type_byte  == (*f)->type*/ 1u;
						if (~0u - os < (*s)->packed_optional_bytes)
							(*s)->unknown_packed_bytes = 1; /* invalidates (*s)->packed_optional_bytes */
						else {
							DPR("+%s: optional_bytes(%u)+=%u (field: *%s)\n", (*s)->s_name, (*s)->packed_optional_bytes, os, (*f)->name);
							(*s)->packed_optional_bytes += os;
						}
					}
					/* if (*f)->user_type, then determine unknown_packed_bits, unknown_packed_bytes,
					  packed_optional_bits, packed_optional_bytes and convertable later */
					break;
				case F_REQUIRED:
				case F_POINTER:
				default:
					if ((*f)->f_power > F_REQUIRED || field_type_str == (*f)->type || field_type_pstr == (*f)->type)
						(*s)->need_destroy++; /* NOTE: cannot overflow: one value for each field at maximum */
					if (field_type_str == (*f)->type || field_type_pstr == (*f)->type) {
						/* packed string has unknown bytes >= 1 */
						unsigned pwr = field_pwr(*f);
						DPR("%s: bytes(%u)+=%u (field: %s)\n", (*s)->s_name, (*s)->packed_bytes, pwr, (*f)->name);
						if (~0u - (*s)->packed_bytes < pwr)
							err("structure '%s': too many packed bytes\n", (*s)->s_name);
						(*s)->packed_bytes += pwr;
						(*s)->unknown_packed_bytes = 1; /* invalidates (*s)->packed_optional_bytes */
					}
					else if (field_type_bit == (*f)->type) {
						if ((*f)->f_power > F_REQUIRED) {
							unsigned bb = bytes_for_bits((*f)->f_power);
							DPR("%s: bytes(%u)+=%u (field: %s)\n", (*s)->s_name, (*s)->packed_bytes, bb, (*f)->name);
							if (~0u - (*s)->packed_bytes < bb)
								err("structure '%s': too many packed bytes\n", (*s)->s_name);
							(*s)->packed_bytes += bb;
						}
						else {
							if (~0u == (*s)->bit_offset)
								err("structure '%s': too many bit/optional fields\n", (*s)->s_name);
							(*f)->bit = (*s)->bit_offset++; /* bit number in array of bits that defines bit-field value */
							DPR("%s: bits(%u)+=%u (field: %s)\n", (*s)->s_name, (*s)->packed_bits, 1, (*f)->name);
							(*s)->packed_bits++; /* NOTE: cannot overflow here - one bit for each field at maximum */
						}
					}
					else if (!(*f)->user_type) {
						unsigned pwr = field_pwr(*f);
						{
							unsigned sz =
								field_type_short   == (*f)->type ? 2u :
								field_type_pshort  == (*f)->type ? 1u :
								field_type_ppshort == (*f)->type ? 1u :
								field_type_int     == (*f)->type ? 4u :
								field_type_pint    == (*f)->type ? 1u :
								field_type_ppint   == (*f)->type ? 1u :
								field_type_long    == (*f)->type ? 8u :
								field_type_plong   == (*f)->type ? 1u :
								field_type_pplong  == (*f)->type ? 1u :
								field_type_float   == (*f)->type ? 4u :
								field_type_double  == (*f)->type ? 8u :
								/*field_type_byte  == (*f)->type*/ 1u;
							if (pwr > ~0u/sz)
								err("structure '%s': field '%s': too big fixed array size: %u\n", (*s)->s_name, (*f)->name, pwr);
							sz *= pwr;
							DPR("%s: bytes(%u)+=%u (field: %s)\n", (*s)->s_name, (*s)->packed_bytes, sz, (*f)->name);
							if (~0u - sz < (*s)->packed_bytes)
								err("structure '%s': too many packed bytes\n", (*s)->s_name);
							(*s)->packed_bytes += sz;
						}
						{
							unsigned os =
								field_type_pshort  == (*f)->type ? 2u :
								field_type_ppshort == (*f)->type ? 2u :
								field_type_pint    == (*f)->type ? 4u :
								field_type_ppint   == (*f)->type ? 4u :
								field_type_plong   == (*f)->type ? 8u :
								field_type_pplong  == (*f)->type ? 8u :
								0u;
							if (os) {
								os *= (pwr > ~0u/os) ? 0 : pwr;
								if (!os || ~0u - os < (*s)->packed_optional_bytes)
									(*s)->unknown_packed_bytes = 1; /* invalidates (*s)->packed_optional_bytes */
								else {
									DPR("+%s: optional_bytes(%u)+=%u (field: *%s)\n",
										(*s)->s_name, (*s)->packed_optional_bytes, os, (*f)->name);
									(*s)->packed_optional_bytes += os;
								}
							}
						}
					}
					/* else check if need to destroy field of required user type later */
					/* if (*f)->user_type, then determine unknown_packed_bits, unknown_packed_bytes,
					  packed_optional_bits, packed_optional_bytes, packed_bits, packed_bytes and convertable later */
					break;
			}
		}
		/* total number of bytes needed to store bits */
		(*s)->bit_offset = bytes_for_bits((*s)->bit_offset);
	}
}

static void determine_convertable_recursive_need_destroy_packed_bits_bytes_user_type_fields_inner(struct struct_def *const ss)
{
	size_t depth = 0;
	struct stack_item *item;
	struct field_def *const *f;
	struct struct_def *s = ss;
	int need_destroy_field = 0; /* initialize to avoid bogus compiler warning */
	size_t arr_count = 0;
	size_t opt_count = 0;
	unsigned multiplier = 1;
process_structure:
	s->_marked = 1;
	for (f = s->fields; f < s->fields_end; f++) {
		if ((*f)->user_type) {
			if (s == ss) {
				/* check if ss->need_destroy already accounts current field */
				need_destroy_field = F_REQUIRED != (*f)->f_power;
			}
			if ((*f)->user_type->_processed) {
				/* (*f)->user_type wasn't processed yet */
				if ((*f)->user_type->_marked) {
					(*f)->user_type->recursion_entry = 1;
					if (!need_destroy_field) {
						need_destroy_field = 1; /* mark that field was counted */
						ss->need_destroy++; /* NOTE: cannot overflow: one value for each field at maximum */
					}
					continue; /* skip recursive field - check next field, recursive structures are will be processed later */
				}
				/* save vars into stack */
				switch ((*f)->f_power) {
					case F_ARRAY: arr_count++; break;
					case F_OPTIONAL: opt_count++; break;
					case F_REQUIRED: break;
					case F_POINTER: break;
					default:
						if (multiplier > ~0u/(*f)->f_power)
							err("structure '%s': too big packed size because of '%s'->'%s'\n", ss->s_name, s->s_name, (*f)->name);
						multiplier *= (*f)->f_power;
						break;
				}
				if (capacity <= depth)
					grow_stack();
				item = stack + (depth++);
				item->s = s;
				item->f = f;
				s = (*f)->user_type;
				goto process_structure;
process_structure_return:
				/* restore vars from stack */
				item = stack + (--depth);
				s = item->s;
				f = item->f;
				switch ((*f)->f_power) {
					case F_ARRAY: arr_count--; break;
					case F_OPTIONAL: opt_count--; break;
					case F_REQUIRED: break;
					case F_POINTER: break;
					default:
						multiplier /= (*f)->f_power;
						break;
				}
			}
			/* ok, now (*f)->user_type is processed */
			if ((*f)->user_type->convertable)
				ss->convertable = 1;
			switch ((*f)->f_power) {
				case F_ARRAY:
					/* check unknown_packed_bytes */
					if ((*f)->user_type->packed_bytes ||
						(*f)->user_type->unknown_packed_bytes ||
						(*f)->user_type->packed_optional_bytes)
						ss->unknown_packed_bytes = 1; /* invalidates ss->packed_optional_bytes */
					/* check unknown_packed_bits */
					if ((*f)->user_type->packed_bits ||
						(*f)->user_type->unknown_packed_bits ||
						(*f)->user_type->packed_optional_bits)
						ss->unknown_packed_bits = 1; /* invalidates ss->packed_optional_bits */
					break;
				case F_OPTIONAL:
					/* check unknown_packed_bits and packed_optional_bits */
					if ((*f)->user_type->unknown_packed_bits)
						ss->unknown_packed_bits = 1; /* invalidates ss->packed_optional_bits */
					else if ((*f)->user_type->packed_bits || (*f)->user_type->packed_optional_bits) {
						if (arr_count ||
							~0u - (*f)->user_type->packed_bits < (*f)->user_type->packed_optional_bits ||
							~0u/multiplier < (*f)->user_type->packed_bits + (*f)->user_type->packed_optional_bits ||
							~0u - ss->packed_optional_bits < multiplier*(
								(*f)->user_type->packed_bits + (*f)->user_type->packed_optional_bits))
						{
							ss->unknown_packed_bits = 1; /* invalidates ss->packed_optional_bits */
						}
						else {
							unsigned x = multiplier*((*f)->user_type->packed_bits + (*f)->user_type->packed_optional_bits);
							DPR("+%s: optional_bits(%u)+=%u (field: *%s)\n", ss->s_name, ss->packed_optional_bits, x, (*f)->name);
							ss->packed_optional_bits += x;
						}
					}
					/* check unknown_packed_bytes and packed_optional_bytes */
					if ((*f)->user_type->unknown_packed_bytes)
						ss->unknown_packed_bytes = 1; /* invalidates ss->packed_optional_bytes */
					else if ((*f)->user_type->packed_bytes || (*f)->user_type->packed_optional_bytes) {
						if (arr_count ||
							~0u - (*f)->user_type->packed_bytes < (*f)->user_type->packed_optional_bytes ||
							~0u/multiplier < (*f)->user_type->packed_bytes + (*f)->user_type->packed_optional_bytes ||
							~0u - ss->packed_optional_bytes < multiplier*(
								(*f)->user_type->packed_bytes + (*f)->user_type->packed_optional_bytes))
						{
							ss->unknown_packed_bytes = 1; /* invalidates ss->packed_optional_bytes */
						}
						else {
							unsigned x = multiplier*((*f)->user_type->packed_bytes + (*f)->user_type->packed_optional_bytes);
							DPR("+%s: optional_bytes(%u)+=%u (field: *%s)\n", ss->s_name, ss->packed_optional_bytes, x, (*f)->name);
							ss->packed_optional_bytes += x;
						}
					}
					break;
				case F_REQUIRED:
					if (!need_destroy_field && (*f)->user_type->need_destroy) {
						need_destroy_field = 1; /* mark that field was counted */
						ss->need_destroy++; /* NOTE: cannot overflow: one value for each field at maximum */
					}
					/* fall through */
				case F_POINTER:
				default: {
					unsigned pwr = field_pwr(*f);
					/* check unknown_packed_bits, packed_bits and packed_optional_bits */
					if ((*f)->user_type->packed_bits) {
						if (arr_count)
							ss->unknown_packed_bits = 1; /* invalidates ss->packed_optional_bits */
						else if (opt_count) {
							if (~0u/pwr < multiplier ||
								~0u/(multiplier*pwr) < (*f)->user_type->packed_bits ||
								~0u - ss->packed_optional_bits < (*f)->user_type->packed_bits*multiplier*pwr)
							{
								ss->unknown_packed_bits = 1; /* invalidates ss->packed_optional_bits */
							}
							else {
								unsigned x = (*f)->user_type->packed_bits*multiplier*pwr;
								DPR("+%s: optional_bits(%u)+=%u (field: *%s)\n", ss->s_name, ss->packed_optional_bits, x, (*f)->name);
								ss->packed_optional_bits += x;
							}
						}
						else if (~0u/pwr < multiplier ||
							~0u/(multiplier*pwr) < (*f)->user_type->packed_bits ||
							~0u - ss->packed_bits < (*f)->user_type->packed_bits*multiplier*pwr)
						{
							err("structure '%s': field '%s': too many packed bits\n", ss->s_name, (*f)->name);
						}
						else {
							unsigned x = (*f)->user_type->packed_bits*multiplier*pwr;
							DPR("%s: bits(%u)+=%u (field: %s)\n", ss->s_name, ss->packed_bits, x, (*f)->name);
							ss->packed_bits += x;
						}
					}
					if ((*f)->user_type->unknown_packed_bits)
						ss->unknown_packed_bits = 1; /* invalidates ss->packed_optional_bits */
					else if ((*f)->user_type->packed_optional_bits) {
						if (arr_count ||
							~0u/pwr < multiplier ||
							~0u/(multiplier*pwr) < (*f)->user_type->packed_optional_bits ||
							~0u - ss->packed_optional_bits < (*f)->user_type->packed_optional_bits*multiplier*pwr)
						{
							ss->unknown_packed_bits = 1; /* invalidates ss->packed_optional_bits */
						}
						else {
							unsigned x = (*f)->user_type->packed_optional_bits*multiplier*pwr;
							DPR("+%s: optional_bits(%u)+=%u (field: %s)\n", ss->s_name, ss->packed_optional_bits, x, (*f)->name);
							ss->packed_optional_bits += x;
						}
					}
					/* check unknown_packed_bytes, packed_bytes and packed_optional_bytes */
					if ((*f)->user_type->packed_bytes) {
						if (arr_count)
							ss->unknown_packed_bytes = 1; /* invalidates ss->packed_optional_bytes */
						else if (opt_count) {
							if (~0u/pwr < multiplier ||
								~0u/(multiplier*pwr) < (*f)->user_type->packed_bytes ||
								~0u - ss->packed_optional_bytes < (*f)->user_type->packed_bytes*multiplier*pwr)
							{
								ss->unknown_packed_bytes = 1; /* invalidates ss->packed_optional_bytes */
							}
							else {
								unsigned x = (*f)->user_type->packed_bytes*multiplier*pwr;
								DPR("+%s: optional_bytes(%u)+=%u (field: *%s)\n", ss->s_name, ss->packed_optional_bytes, x, (*f)->name);
								ss->packed_optional_bytes += x;
							}
						}
						else if (~0u/pwr < multiplier ||
							~0u/(multiplier*pwr) < (*f)->user_type->packed_bytes ||
							~0u - ss->packed_bytes < (*f)->user_type->packed_bytes*multiplier*pwr)
						{
							err("structure '%s': field '%s': too many packed bytes\n", ss->s_name, (*f)->name);
						}
						else {
							unsigned x = (*f)->user_type->packed_bytes*multiplier*pwr;
							DPR("%s: bytes(%u)+=%u (field: %s)\n", ss->s_name, ss->packed_bytes, x, (*f)->name);
							ss->packed_bytes += x;
						}
					}
					if ((*f)->user_type->unknown_packed_bytes)
						ss->unknown_packed_bytes = 1; /* invalidates ss->packed_optional_bytes */
					else if ((*f)->user_type->packed_optional_bytes) {
						if (arr_count ||
							~0u/pwr < multiplier ||
							~0u/(multiplier*pwr) < (*f)->user_type->packed_optional_bytes ||
							~0u - ss->packed_optional_bytes < (*f)->user_type->packed_optional_bytes*multiplier*pwr)
						{
							ss->unknown_packed_bytes = 1; /* invalidates ss->packed_optional_bytes */
						}
						else {
							unsigned x = (*f)->user_type->packed_optional_bytes*multiplier*pwr;
							DPR("+%s: optional_bytes(%u)+=%u (field: %s)\n", ss->s_name, ss->packed_optional_bytes, x, (*f)->name);
							ss->packed_optional_bytes += x;
						}
					}
					break;
				}
			}
		}
	}
	s->_marked = 0; /* unmark */
	if (depth)
		goto process_structure_return;
}

/* must be called after determine_convertable_need_destroy_packed_bits_bytes_simple_fields() */
static void determine_convertable_recursive_need_destroy_packed_bits_bytes_user_type_fields(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		determine_convertable_recursive_need_destroy_packed_bits_bytes_user_type_fields_inner(*s);
		(*s)->_processed = 0; /* clear mark set by unlimited_sized_struct_check() (inverse-mark structure as 'processed') */
	}
	/* NOTE: all structures are now marked as 'processed' with _processed bit _not_ set */
}

unsigned _counter_packed_size(unsigned x)
{
	if (x > 0x3FFF) {
		if (x > 0x1FFFFF) {
			if (x > 0xFFFFFFF)
				return 5;
			return 4;
		}
		return 3;
	}
	if (x > 0x7F)
		return 2;
	return 1;
}

/* set unknown_packed_bytes and unknown_packed_bits flags that were not set due to recursive fields */
static void determine_unknown_packed_bits_bytes_recursive_user_types_inner(struct struct_def *s)
{
	size_t depth = 0;
	struct stack_item *item;
	struct field_def *const *f;
process_structure:
	if (s->recursion_entry) {
		if (s->packed_bits || s->packed_optional_bits)
			s->unknown_packed_bits = 1;
		if (s->packed_bytes || s->packed_optional_bytes)
			s->unknown_packed_bytes = 1;
	}
	else for (f = s->fields; f < s->fields_end; f++) {
		if ((*f)->user_type) {
			/* recursive check of user type */
			if (!(*f)->user_type->_processed) {
				/* save vars into stack */
				if (capacity <= depth)
					grow_stack();
				item = stack + (depth++);
				item->s = s;
				item->f = f;
				s = (*f)->user_type;
				goto process_structure;
process_structure_return:
				/* restore vars from stack */
				item = stack + (--depth);
				s = item->s;
				f = item->f;
			}
			if ((*f)->user_type->unknown_packed_bits)
				s->unknown_packed_bits = 1;
			if ((*f)->user_type->unknown_packed_bytes)
				s->unknown_packed_bytes = 1;
			if (s->unknown_packed_bits && s->unknown_packed_bytes)
				break;
		}
	}
	s->_processed = 1;
	if (depth)
		goto process_structure_return;
}

/* must be called after determine_convertable_need_destroy_packed_bits_bytes_user_type_fields() */
static void determine_unknown_packed_bits_bytes_recursive_user_types_check_structs(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		if (!(*s)->_processed)
			determine_unknown_packed_bits_bytes_recursive_user_types_inner(*s);
		if ((*s)->packed_bytes > BRIDGE_MODEL_MAX_PACKED_SIZE)
			err("structure '%s': too big packed bytes count: >= %u\n", (*s)->s_name, (*s)->packed_bytes);
		if ((*s)->packed_bits > ((void)0,BRIDGE_MODEL_MAX_BIT_COUNT))
			err("structure '%s': too big packed bits count: >= %u\n", (*s)->s_name, (*s)->packed_bits);
		{
			unsigned bb = bytes_for_bits((*s)->packed_bits);
			if (need_pack_bits_counter(*s))
				bb += _counter_packed_size(bb);
			if (BRIDGE_MODEL_MAX_PACKED_SIZE - (*s)->packed_bytes < bb)
				err("structure '%s': too big packed size: >= %u + %u\n", (*s)->s_name, (*s)->packed_bytes, bb);
		}
	}
}

static void determine_tail_recursive_inner(struct struct_def *s)
{
	/* NOTE: prefer last fields as tail-recursive */
	size_t depth = 0;
	struct stack_item *item;
	struct field_def *const *f;
process_structure:
	/* prefer simple case: structure directly references itself */
	f = s->fields_end;
	do {
		f--;
		if (s == (*f)->user_type && F_OPTIONAL == (*f)->f_power) {
			s->tail_recursive = *f;
			s->recursion_entry = 1; /* mark entry point of tail-recursive circle */
			goto _return; /* recursion defined */
		}
	} while (f != s->fields);
	/* next try to find recursive path through complex fields */
	f = s->fields_end;
	do {
		f--;
		if ((*f)->user_type && !(*f)->user_type->tail_recursive &&
			(F_REQUIRED == (*f)->f_power || F_OPTIONAL == (*f)->f_power || F_POINTER == (*f)->f_power))
		{
			if ((*f)->user_type->_marked) {
				/* recusion found, now mark tail-recursive path while backtracing */
				int entry_point_set = 0;
				s->tail_recursive = *f;
				/* prefer optional field for entry point of recursive circle, some optional field must always be found */
				if (F_OPTIONAL == s->tail_recursive->f_power) {
					s->tail_recursive->user_type->recursion_entry = 1; /* mark entry point of tail-recursive circle */
					entry_point_set = 1; /* entry point may be defined only once */
				}
				else
					s->tail_recursive->user_type->recursion_entry = 0; /* not an entry point, reset flag */
				{
					const struct stack_item *i = stack + depth;
					do {
						i--;
						i->s->tail_recursive = *(i->f);
						if (!entry_point_set && F_OPTIONAL == i->s->tail_recursive->f_power) {
							i->s->tail_recursive->user_type->recursion_entry = 1; /* mark entry point of tail-recursive circle */
							entry_point_set = 1; /* entry point may be defined only once */
						}
						else
							i->s->tail_recursive->user_type->recursion_entry = 0; /* not an entry point, reset flag */
					} while (i->s != (*f)->user_type);
				}
				break; /* recursion defined */
			}
			/* recursive check of user type */
			s->_marked = 1;
			/* save vars into stack */
			if (capacity <= depth)
				grow_stack();
			item = stack + (depth++);
			item->s = s;
			item->f = f;
			s = (*f)->user_type;
			goto process_structure;
process_structure_return:
			/* restore vars from stack */
			item = stack + (--depth);
			s = item->s;
			f = item->f;
			s->_marked = 0;
			if (s->tail_recursive)
				break;
		}
	} while (f != s->fields);
_return:
	if (depth)
		goto process_structure_return;
}

/* must be called after determine_unknown_packed_bits_bytes_recursive_user_types_check_structs() */
static void determine_tail_recursive(void)
{
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		/* all structures are now marked as 'processed' with _processed bit set, just reset it */
		(*s)->_processed = 0;
		/* all possible recursive circles are marked with 'recursion_entry' flag */
		/* (*s)->tail_recursive may be set by previous determine_tail_recursive_inner() */
		if (!(*s)->tail_recursive && (*s)->recursion_entry) {
			(*s)->recursion_entry = 0; /* unmark current entry, will find and mark another one */
			determine_tail_recursive_inner(*s);
		}
	}
	/* NOTE: all structures are now marked as 'processed' with _processed bit _not_ set */
}

#define F_POWER_SWITCH(r,p,d,o,a) \
	F_REQUIRED == f->f_power ? r : \
	F_POINTER  == f->f_power ? p : \
	F_OPTIONAL == f->f_power ? o : \
	F_ARRAY    == f->f_power ? a : d

/* all structure fields are packed in order according to the field pack priority
  - try to pack fixed-sized fields first, to reduce limits checks while unpacking */
static int get_field_priority(const struct field_def *f)
{
	const struct struct_def *ut = f->user_type;
	/* -------------------------------------------------------(required,ponter,default,optional,array) */
	if (ut && ut->unknown_packed_bytes)  return F_POWER_SWITCH(     200,   201,    201,     202,  180);
	if (ut && ut->unknown_packed_bits)   return F_POWER_SWITCH(     190,   191,    191,     192,  170);
	if (ut && ut->packed_optional_bytes) return F_POWER_SWITCH(      78,    79,     79,      97,  168);
	if (ut && ut->packed_optional_bits)  return F_POWER_SWITCH(      70,    71,     71,      92,  160);
	if (ut && ut->packed_bits)           return F_POWER_SWITCH(      50,    51,     51,      91,  150);
	if (ut)                              return F_POWER_SWITCH(       6,     7,      7,      96,  114);
	if (field_type_long    == f->type)   return F_POWER_SWITCH(       0,    10,     10,      72,  103);
	if (field_type_double  == f->type)   return F_POWER_SWITCH(       1,    11,     11,      73,  104);
	if (field_type_int     == f->type)   return F_POWER_SWITCH(       2,    12,     12,      74,  105);
	if (field_type_float   == f->type)   return F_POWER_SWITCH(       3,    13,     13,      75,  106);
	if (field_type_short   == f->type)   return F_POWER_SWITCH(       4,    14,     14,      76,  107);
	if (field_type_byte    == f->type)   return F_POWER_SWITCH(       5,    15,     15,      77,  109);
	if (field_type_bit     == f->type)   return F_POWER_SWITCH(      45,    20,     20,      67,  108);
	if (field_type_plong   == f->type)   return F_POWER_SWITCH(      60,    63,     63,      81,  111);
	if (field_type_pplong  == f->type)   return F_POWER_SWITCH(      60,    63,     63,      81,  111);
	if (field_type_pint    == f->type)   return F_POWER_SWITCH(      61,    64,     64,      82,  112);
	if (field_type_ppint   == f->type)   return F_POWER_SWITCH(      61,    64,     64,      82,  112);
	if (field_type_pshort  == f->type)   return F_POWER_SWITCH(      62,    65,     65,      83,  113);
	if (field_type_ppshort == f->type)   return F_POWER_SWITCH(      62,    65,     65,      83,  113);
	if (field_type_str     == f->type)   return F_POWER_SWITCH(     115,   115,    115,     135,  125);
	if (field_type_pstr    == f->type)   return F_POWER_SWITCH(     120,   120,    120,     155,  126);
	return -1; /* unreachable */
}

static void priority_sort_fields(void)
{
	/* Shell's sorting */
	struct struct_def *const *s = structs;
	for (; s < structs_end; s++) {
		/* number of fields <= ~0 */
		unsigned d = (unsigned)((*s)->fields_end - (*s)->fields);
		while (d > 1) {
			d >>= 1;
			for (;;) {
				int sorted = 1;
				struct field_def **q = (*s)->fields + d;
				do {
					struct field_def **f = q - d;
					/* tail-recursive field should be the last one */
					if ((*s)->tail_recursive == *f || get_field_priority(*f) > get_field_priority(*q)) {
						struct field_def *w = *f;
						*f = *q;
						*q = w;
						sorted = 0;
					}
				} while (++q < (*s)->fields_end);
				if (sorted)
					break;
			}
		}
	}
}

/* required user-types must be declared before their users */
/* unlimited_sized_struct_check() had already checked that we will not loop forever */
static void sort_structs(void)
{
	for (;;) {
		int was_exchange = 0;
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			struct field_def *const *f = (*s)->fields;
			struct field_def *const *e = (*s)->fields_end;
			for (; f < e; f++) {
				if (F_REQUIRED == (*f)->f_power && (*f)->user_type) {
					struct struct_def **ss = s + 1;
					for (; ss < structs_end; ss++) {
						if (*ss == (*f)->user_type) {
							*ss = *s;
							*s = (*f)->user_type;
							s = ss;
							was_exchange = 1;
							break;
						}
					}
				}
			}
		}
		if (!was_exchange)
			break;
	}
}

unsigned s_packed_size_limit(const struct struct_def *s)
{
	if (!s->unknown_packed_bytes && !s->unknown_packed_bits) {
		unsigned bytes = s->packed_bytes;
		if (~0u - bytes >= s->packed_optional_bytes) {
			bytes += s->packed_optional_bytes;
			{
				unsigned bits = s->packed_bits;
				if (~0u - bits >= s->packed_optional_bits) {
					bits = bytes_for_bits(bits + s->packed_optional_bits);
					if (need_pack_bits_counter(s))
						bits += _counter_packed_size(bits);
					if (~0u - bytes >= bits)
						return bytes + bits; /* >0 */
				}
			}
		}
	}
	return 0u; /* unknown */
}

#define _STRINGIFY(n) #n
#define STRINGIFY(n) _STRINGIFY(n)

#define BIT_BRIDGE_MODEL_MAJOR_STR STRINGIFY(BIT_BRIDGE_MODEL_MAJOR)
#define BIT_BRIDGE_MODEL_MINOR_STR STRINGIFY(BIT_BRIDGE_MODEL_MINOR)
#define BIT_BRIDGE_MODEL_PATCH_STR STRINGIFY(BIT_BRIDGE_MODEL_PATCH)

FILE *print_model(FILE *file)
{
	fputs(
		"!----- BitBridge model version " BIT_BRIDGE_MODEL_MAJOR_STR "." BIT_BRIDGE_MODEL_MINOR_STR "." BIT_BRIDGE_MODEL_PATCH_STR
		"\nstructures properties legend:"
		"\nstruct <name> - structure name"
		"\nlayout <STATIC1|STATIC2|STATIC4|STATIC8|MONOLITH|DYNAMIC> - structure layout:"
		"\n\tSTATIC{1,2,4,8} - all fields of structure are of fixed size and all required,"
		"\n\t\tstructure address must be aligned on {1,2,4,8} bytes,"
		"\n\tMONOLITH - structure resides in one monolith memory block, some integer/bit fields are optional,"
		"\n\tDYNAMIC  - some structure fields allocated in different memory blocks."
		"\nbit_offset            <num>        - number of bytes needed to hold information"
		"\n\tabout the presence of optional fields (one bit for each optional field) and bit-fields,"
		"\n\texcept for user-types and strings, those presence is determined by NULL-checking field pointer"
		"\nneed_destroy          <num>        - number of destroyable fields:"
		"\n\tarrays (variable or fixed-sized), optional or destroyable user-types and strings"
		"\nequivalent            <type_name>  - name of optional simple non-bit type if structure may be"
		"\n\trepresented as fixed-sized array of this type"
		"\ntail_recursive        <field_name> - last-processed tail-recursive field"
		"\nrecursive             <true|false> - if <true> then marks entry point of tail-recursive circle"
		"\nequiv_many            <true|false> - if <true> then structure contains more than one field of equivalent or pointer type"
		"\nptr_arr               <true|false> - if <true> then structure may be destroyed as array of pointers"
		"\nconvertable           <true|false> - if <true> then some fields may require endian conversion while packing/unpacking"
		"\nunknown_packed_bits   <true|false> - if <true> then number of bits in packed form is not naturally limited"
		"\nunknown_packed_bytes  <true|false> - if <true> then number of bytes in packed form is not naturally limited"
		"\npacked_bits           <num>        - number of bits that will be present in packed form anyway"
		"\npacked_bytes          <num>        - number of bytes that will be present in packed form anyway"
		"\npacked_optional_bits  <num>        - maximum number of optional bits in packed form, valid if !unknown_packed_bits"
		"\npacked_optional_bytes <num>        - maximum number of optional bytes in packed form, valid if !unknown_packed_bytes"
		"\npacked_size_limit     <num>        - structure packed size limit, 0 if unknown"
		"\n!----------------------------------------------------"
		, file);
	{
		struct struct_def *const *s = structs;
		for (; s < structs_end; s++) {
			fputs("\n\nstruct ", file);
			fputs((*s)->s_name, file);
			fputs("\nlayout:                ", file);
			switch ((*s)->s_layout) {
				case S_STATIC1:  fputs("STATIC1",  file); break;
				case S_STATIC2:  fputs("STATIC2",  file); break;
				case S_STATIC4:  fputs("STATIC4",  file); break;
				case S_STATIC8:  fputs("STATIC8",  file); break;
				case S_MONOLITH: fputs("MONOLITH", file); break;
				case S_DYNAMIC:  fputs("DYNAMIC",  file); break;
				case S_INVALID1: break;
				case S_INVALID2: break;
				case S_INVALID3: break;
				case S_INVALID4: break;
				case S_INVALID5: break;
			}
			fprintf(file, "\nbit_offset:            %u", (*s)->bit_offset);
			fprintf(file, "\nneed_destroy:          %u", (*s)->need_destroy);
			fputs("\nequivalent:            ", file);
			fputs((*s)->equivalent ? (*s)->equivalent : "-none-", file);
			fputs("\ntail_recursive:        ", file);
			fputs((*s)->tail_recursive ? (*s)->tail_recursive->name : "-none-", file);
			fprintf(file, "\nrecursion_entry:       %s", (*s)->recursion_entry      ? "true" : "false");
			fprintf(file, "\nequiv_many:            %s", (*s)->equiv_many           ? "true" : "false");
			fprintf(file, "\nptr_arr:               %s", (*s)->ptr_arr              ? "true" : "false");
			fprintf(file, "\nconvertable:           %s", (*s)->convertable          ? "true" : "false");
			fprintf(file, "\nunknown_packed_bits:   %s", (*s)->unknown_packed_bits  ? "true" : "false");
			fprintf(file, "\nunknown_packed_bytes:  %s", (*s)->unknown_packed_bytes ? "true" : "false");
			fprintf(file, "\npacked_bits:           %u", (*s)->packed_bits);
			fprintf(file, "\npacked_bytes:          %u", (*s)->packed_bytes);
			fprintf(file, "\npacked_optional_bits:  %u", (*s)->unknown_packed_bits  ? ~0u : (*s)->packed_optional_bits);
			fprintf(file, "\npacked_optional_bytes: %u", (*s)->unknown_packed_bytes ? ~0u : (*s)->packed_optional_bytes);
			fprintf(file, "\npacked_size_limit:     %u", s_packed_size_limit(*s));
			fputs("\nfields:", file);
			{
				struct field_def *const *f = (*s)->fields;
				for (; f < (*s)->fields_end; f++) {
					fprintf(file, "\n\t%s: %s", (*f)->name, (*f)->type);
					if (F_OPTIONAL == (*f)->f_power)
						fputc('*', file);
					else if (F_ARRAY == (*f)->f_power)
						fputs("[]", file);
					else if (F_REQUIRED < (*f)->f_power)
						fprintf(file, "[%u]", (field_type_bit != (*f)->type && F_POINTER == (*f)->f_power) ? 1u : (*f)->f_power);
					if ((F_OPTIONAL == (*f)->f_power && !(*f)->user_type &&
							field_type_str != (*f)->type && field_type_pstr != (*f)->type) ||
						(F_REQUIRED == (*f)->f_power && field_type_bit == (*f)->type))
					{
						fprintf(file, ", bit = %u", (*f)->bit);
					}
				}
			}
		}
	}
	return file;
}
