/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* parser.c */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "utils.h"
#include "model.h"
#include "gtparser/name_parser.h"
#include "gtparser/int_parser.h"
#include "gtparser/parser_err.h"

#define STRUCTS_GROW_COUNT 10
#define FILENAME_RESERVE 512u

#define MAX_STRUCT_NAME_LEN (~0u - offsetof(struct struct_def, s_name) - 1u/*'\0'*/)
#define MAX_FIELD_NAME_LEN  (~0u - offsetof(struct field_def, name) - 2u/*auto-prefix+'\0'*/)

#ifdef __GNUC__
__attribute__ ((noreturn))
#endif
static void err_at(const char *filename/*NULL?*/, const struct src_iter *it, const char *err_msg)
{
	char err_buf[2048];
	struct src_pos pos;
	src_iter_get_pos(it, &pos/*out*/);
	err("%s\n", parser_err_prepend_at(err_buf, sizeof(err_buf), filename ? FILENAME_RESERVE : 0u,
		filename, err_msg, pos.line, pos.column));
}

/* NOTE: detect end of structure definition (and start of another structure)
  by column position of next item, though column number may overflow...
  - anyway, structure name must end with ':' and field name must not,
  so we will detect bad input even if will ignore column number */

/* examples:

  struct1: int field11 int field12
  struct2: int field21 int field22

or

  struct1: int field11
    int field12
  struct2: int field21
    int field22

or
  struct1:
    int field11
    int field12
  struct2:
    int field21
    int field22
*/

static struct field_def *read_field(const char *filename/*NULL?*/,
	struct src_iter *it, struct struct_def *s, unsigned struct_column, int autoprefix)
{
	char c = read_non_space_skip_comments(it, '#');
	if (!c)
		return NULL; /* ok, eof */
	if (src_iter_get_column(it) <= struct_column)
		return NULL; /* ok, end of struct */
	if (!is_first_name(c))
		err_at(filename, it, "expecting type name");
	{
		const char *type_name = read_name(it);
		size_t type_name_len = (size_t)(it->current - type_name);
		if (type_name_len > MAX_STRUCT_NAME_LEN) {
			it->current = type_name;
			err_at(filename, it, "too long type name");
		}
		{
			const char *reserved_type = get_reserved_type(type_name, (unsigned)type_name_len);
			unsigned power;
			c = read_non_space_stop_eol(it);
			if ('[' == c) {
				src_iter_step(it);
				c = read_non_space_stop_eol(it);
				if (']' == c)
					power = F_ARRAY;
				else {
					if (!is_digit(c))
						err_at(filename, it, "expecting array size decimal number or ]");
					{
						const char *r = it->current;
						if (!read_uint(it, &power/*out*/) ||
							power > (field_type_bit == reserved_type ?
								BRIDGE_MODEL_MAX_BIT_COUNT - 1u : BRIDGE_MODEL_MAX_PACKED_SIZE))
						{
							it->current = r;
							err_at(filename, it, "too big array size number");
						}
						if (field_type_bit == reserved_type) {
							if (power < 2) {
								it->current = r;
								err_at(filename, it, "fixed bit array size must be > 1");
							}
						}
						else if (power < 1) {
							it->current = r;
							err_at(filename, it, "fixed array size must be > 0");
						}
						else if (1 == power)
							power = F_POINTER;
					}
					if (']' != read_non_space_stop_eol(it))
						err_at(filename, it, "expecting ]");
				}
				src_iter_step(it);
			}
			else if ('*' == c) {
				src_iter_step(it);
				power = F_OPTIONAL;
			}
			else
				power = F_REQUIRED;
			if (power != F_REQUIRED)
				c = read_non_space_stop_eol(it);
			if (!is_first_name(c))
				err_at(filename, it, "expecting field name");
			{
				const char *field_name = read_name(it);
				size_t field_name_len = (size_t)(it->current - field_name);
				if (field_name_len > MAX_FIELD_NAME_LEN) {
					it->current = field_name;
					err_at(filename, it, "too long field name");
				}
				if (get_reserved_type(field_name, (unsigned)field_name_len)) {
					it->current = field_name;
					err_at(filename, it, "using predefined type name as field name");
				}
				{
					struct field_def **i = s->fields;
					for (; i < s->fields_end; i++) {
						struct field_def *f = *i;
						unsigned f_prefix = autoprefix ? (unsigned)(F_REQUIRED != f->f_power) : 0u;
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
						if (!strncmp(field_name, f->name + f_prefix, field_name_len) && '\0' == f->name[f_prefix + field_name_len]) {
#ifdef __clang__
#pragma clang diagnostic pop
#endif
							it->current = field_name;
							err_at(filename, it, "duplicate field name");
						}
					}
				}
				{
					unsigned prefix = autoprefix ? (unsigned)(F_REQUIRED != power) : 0u;
					struct field_def *f = (struct field_def*)mem_alloc(
						offsetof(struct field_def, name) + prefix + field_name_len + 1u/*'\0'*/);
					if (prefix) {
						if (F_OPTIONAL == power)
							f->name[0] = 'o';
						else if (F_ARRAY == power)
							f->name[0] = 'a';
						else if (F_REQUIRED != power)
							f->name[0] = 'r';
					}
					memcpy(f->name + prefix, field_name, field_name_len);
					field_name_len += prefix;
					f->name[field_name_len] = '\0';
					f->f_power = power;
					if (reserved_type)
						f->type = reserved_type;
					else {
						char *ft = (char*)mem_alloc(type_name_len + 1u/*'\0'*/);
						memcpy(ft, type_name, type_name_len);
						ft[type_name_len] = '\0';
						f->type = ft;
					}
					return f;
				}
			}
		}
	}
}

static struct struct_def *parse_struct(const char *filename/*NULL?*/, struct src_iter *it, int autoprefix)
{
	char c = read_non_space_skip_comments(it, '#');
	if (!c)
		return NULL; /* ok, eof */
	if (!is_first_name(c))
		err_at(filename, it, "expecting structure name");
	{
		unsigned struct_column = src_iter_get_column(it);
		const char *struct_name = read_name(it);
		if (':' != src_iter_current_eof(it))
			err_at(filename, it, "expecting ':' at end of structure name");
		{
			size_t struct_name_len = (size_t)(it->current - struct_name);
			if (struct_name_len > MAX_STRUCT_NAME_LEN) {
				it->current = struct_name;
				err_at(filename, it, "too long structure name");
			}
			if (get_reserved_type(struct_name, (unsigned)struct_name_len)) {
				it->current = struct_name;
				err_at(filename, it, "using predefined type name as structure name");
			}
			{
				struct struct_def **i = structs;
				for (; i < structs_end; i++) {
					const struct struct_def *s = *i;
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
					if (!strncmp(struct_name, s->s_name, struct_name_len) && '\0' == s->s_name[struct_name_len]) {
#ifdef __clang__
#pragma clang diagnostic pop
#endif
						it->current = struct_name;
						err_at(filename, it, "duplicate structure name");
					}
				}
			}
			{
				unsigned line = it->line;
				unsigned back_column = it->back_column;
				struct struct_def *s = (struct struct_def*)mem_alloc(
					offsetof(struct struct_def, s_name) + struct_name_len + 1/*'\0'*/);
				memcpy(s->s_name, struct_name, struct_name_len);
				s->s_name[struct_name_len] = '\0';
				s->fields = NULL;
				s->fields_end = NULL;
				src_iter_step(it); /* skip ':' */
				{
					struct field_def **ff = &s->first_f;
					for (;;) {
						struct field_def *f = read_field(filename, it, s, struct_column, autoprefix);
						if (!f)
							break;
						if ((size_t)(s->fields_end - s->fields) == ~0u)
							err_at(filename, it, "too many fields!");
						add_ptr_to_array(&s->fields, &s->fields_end, f);
						*ff = f;
						ff = &f->next_f;
					}
					*ff = NULL;
				}
				if (!s->fields) {
					it->line = line;
					it->back_column = back_column;
					it->current = struct_name;
					err_at(filename, it, "structure must have fields!");
				}
				return s;
			}
		}
	}
}

/* returns number of parsed structures */
size_t parse(const char *filename/*NULL?*/, const char input[], size_t size, int autoprefix)
{
	struct src_iter it;
	src_iter_init(&it, input, size);
	{
		size_t cap = 0, cnt = 0;
		for (;;) {
			struct struct_def *s = parse_struct(filename, &it, autoprefix);
			if (!s)
				break;
			if (cnt == cap) {
				if ((size_t)~(size_t)0 - cap*sizeof(*structs) < STRUCTS_GROW_COUNT*sizeof(*structs))
					err_at(filename, &it, "too many structures");
				cap += STRUCTS_GROW_COUNT;
				structs = (struct struct_def**)mem_realloc(structs, sizeof(*structs)*cap);
			}
			structs[cnt++] = s;
		}
		structs_end = structs + cnt;
		return cnt;
	}
}
