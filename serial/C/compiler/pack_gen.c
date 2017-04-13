/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* pack_gen.c */

#include <stdlib.h>
#include <stdio.h>

#include "ptypes.h"
#include "compiler.h"
#include "model.h"

/* don't tell alloc_cb() to copy bytes less than BR_MEMCPY_MAX_SIZE, we'll copy them inplace via _br_memcpy{1..8}() */
#define BR_MEMCPY_MAX_SIZE 8

#define WHL while ((void)0,0)
#define FP(s)                                          do {if (file) fputs(s,file);} WHL
#define FR1(s,a1)                                      do {if (file) fprintf(file,s,a1);} WHL
#define FR2(s,a1,a2)                                   do {if (file) fprintf(file,s,a1,a2);} WHL
#define FR3(s,a1,a2,a3)                                do {if (file) fprintf(file,s,a1,a2,a3);} WHL
#define FR4(s,a1,a2,a3,a4)                             do {if (file) fprintf(file,s,a1,a2,a3,a4);} WHL
#define FR5(s,a1,a2,a3,a4,a5)                          do {if (file) fprintf(file,s,a1,a2,a3,a4,a5);} WHL
#define FR6(s,a1,a2,a3,a4,a5,a6)                       do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6);} WHL
#define FR7(s,a1,a2,a3,a4,a5,a6,a7)                    do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7);} WHL
#define FR8(s,a1,a2,a3,a4,a5,a6,a7,a8)                 do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8);} WHL
#define FR9(s,a1,a2,a3,a4,a5,a6,a7,a8,a9)              do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9);} WHL
#define FR10(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1)          do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1);} WHL
#define FR11(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2)       do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2);} WHL
#define FR12(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3)    do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3);} WHL
#define FR13(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4) do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4);} WHL

void generate_prototype_pack(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/)
{
	FR5("\n%s%sA_Nonnull_all_args A_Check_return"
		"\nvoid *_%s_pack(A_In const struct %s *s, A_Inout union bridge_pack_%sinfo *in BRIDGE_DEBUG_ARGS_DECL)",
		decl ? decl : "", decl ? " " : "", s->s_name, s->s_name, s->convertable ? "conv_" : "");
}

void generate_prototype_unpack(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/)
{
	FR6("\n%s%sA_Nonnull_arg(2) A_Check_return A_Success(return != NULL) A_Ret_valid A_When(s != NULL, A_Post_satisfies(return == s))"
		"\nstruct %s *_%s_unpack_to(A_Out_opt struct %s *s, "
		"A_Pre_valid A_Post_valid union bridge_unpack_%sinfo *in BRIDGE_DEBUG_ARGS_DECL)",
		decl ? decl : "", decl ? " " : "", s->s_name, s->s_name, s->s_name, s->convertable ? "conv_" : "");
}

int generate_prototype_variable_size(FILE *file, const struct struct_def *s)
{
	if (!s->equivalent && !s_fixed(s)) {
		FR5("\nA_Pure_function A_Nonnull_all_args A_Check_return"
			"\nstatic %s _%s_var_size(A_In const struct %s *s%s%s)",
			v_bits(s) ? "_bridge_bits_bytes_t" : "unsigned", s->s_name, s->s_name,
			v_bytes(s) ? ", unsigned byte_count" : "",
			v_bits(s) ? ", unsigned bit_count" : "");
		return 1;
	}
	return 0;
}

enum CALL_FROM {
	CALL_FROM_ARRAY,
	CALL_FROM_ARRAY_INC,
	CALL_FROM_OPTIONAL,
	CALL_FROM_STATIC,
	CALL_FROM_FIXED_ARRAY,
	CALL_FROM_FIXED_ARRAY_INC
};

static void print_call_from(FILE *file, const struct field_def *f, enum CALL_FROM from)
{
	switch (from) {
		case CALL_FROM_ARRAY:           FP("i, ");                        return;
		case CALL_FROM_ARRAY_INC:       FP("i++, ");                      return;
		case CALL_FROM_OPTIONAL:        FR1("s->%s, ", f->name);          return;
		case CALL_FROM_STATIC:          FR1("&s->%s, ", f->name);         return;
		case CALL_FROM_FIXED_ARRAY:     FR1("&(*s->%s)[i], ", f->name);   return;
		case CALL_FROM_FIXED_ARRAY_INC: FR1("&(*s->%s)[i++], ", f->name); return;
	}
}

/* called from generate_code_var_size() */
static int call_var_size_user_type(FILE *file, const struct struct_def *s/*!=NULL for tail call*/,
	const struct field_def *f, enum CALL_FROM from, int need_block, const char *o)
{
	const char *offs = o;
	if (CALL_FROM_ARRAY == from || CALL_FROM_FIXED_ARRAY == from) {
		if (v_bits(f->user_type))
			FP(" {");
		offs = o[0] ? o[1] ? "\t\t\t\t" : "\t\t\t" : "\t\t";
	}
	else if (CALL_FROM_OPTIONAL == from && need_block != 2)
		offs = o[0] ? o[1] ? "\t\t\t" : "\t\t" : "\t";
	if (!s || v_bytes(s) != v_bytes(f->user_type) || v_bits(s) != v_bits(f->user_type)) {
		if (v_bits(f->user_type)) {
			if (need_block) {
				if (CALL_FROM_OPTIONAL == from && need_block != 2)
					offs = o[0] ? o[1] ? "\t\t\t\t" : "\t\t\t" : "\t\t";
				else
					offs = o[0] ? o[1] ? "\t\t\t" : "\t\t" : "\t";
				FR1("\n%s{", offs);
			}
			FR1("\n%s\t_bridge_bits_bytes_t bb", offs);
		}
		else
			FR1("\n%s\tbyte_count", offs);
		FR1(" = _%s_var_size(", f->type);
	}
	else
		FR2("\n%s\treturn _%s_var_size(", offs, f->type);
	print_call_from(file, f, from);
	if (v_bytes(f->user_type)) {
		FP("byte_count");
		if (v_bits(f->user_type))
			FP(", ");
	}
	if (v_bits(f->user_type))
		FP("bit_count");
	FP(");");
	if (!s || v_bytes(s) != v_bytes(f->user_type) || v_bits(s) != v_bits(f->user_type)) {
		if (!v_bytes(f->user_type)) {
			FR2("\n%s\tif (_br_unlikely(_bridge_bits_bytes_get_bytes(bb)))"
				"\n%s\t\treturn ~0u;", offs, offs);
		}
		else if (v_bits(f->user_type))
			FR1("\n%s\tbyte_count = _bridge_bits_bytes_get_bytes(bb);", offs);
		if (v_bits(f->user_type)) {
			FR1("\n%s\tbit_count = _bridge_bits_bytes_get_bits(bb);", offs);
			if (need_block || CALL_FROM_ARRAY == from || CALL_FROM_FIXED_ARRAY == from)
				FR1("\n%s}", offs);
		}
		return 0;
	}
	return 1; /* returned */
}

static unsigned simple_fixed_int_size(const char *type)
{
	return
		field_type_byte   == type ? 1u :
		field_type_short  == type ? 2u :
		field_type_int    == type ? 4u :
		field_type_long   == type ? 8u :
		field_type_float  == type ? 4u :
		field_type_double == type ? 8u : 0u;
}

const char *get_native_type(const char *eq)
{
	return
		field_type_byte    == eq ? "INT8_TYPE"  :
		field_type_str     == eq ? "char*"      :
		field_type_pstr    == eq ? "char*"      :
		field_type_short   == eq ? "INT16_TYPE" :
		field_type_pshort  == eq ? "INT16_TYPE" :
		field_type_ppshort == eq ? "INT16_TYPE" :
		field_type_int     == eq ? "INT32_TYPE" :
		field_type_pint    == eq ? "INT32_TYPE" :
		field_type_ppint   == eq ? "INT32_TYPE" :
		field_type_long    == eq ? "INT64_TYPE" :
		field_type_plong   == eq ? "INT64_TYPE" :
		field_type_pplong  == eq ? "INT64_TYPE" :
		field_type_float   == eq ? "float"      :
		field_type_double  == eq ? "double"     :
		"internal error";
}

void print_equivalent_path_s(FILE *file, const struct struct_def *s)
{
	for (;;) {
		const struct field_def *f = *s->fields;
		fputs(f->name, file);
		if (!f->user_type)
			return;
		fputc('.', file);
		s = f->user_type;
	}
}

static int switch_var_size(FILE *file, const struct struct_def *s/*!=NULL for tail call*/, const struct field_def *f, const char *offs)
{
	int returned = 0;
	switch (f->f_power) {
		case F_ARRAY:
			FR3("\n%s\t/* %s: %s[] */", offs, f->name, f->type);
			if (f->user_type) {
				FR5("\n%s\t{"
					"\n%s\t\tsize_t count = (size_t)(s->%s_end - s->%s);"
					"\n%s\t\tif (_br_unlikely(count > BRIDGE_MAX", offs, offs, f->name, f->name, offs);
				if (f->user_type->packed_bytes > 1)
					FR1("/%uu", f->user_type->packed_bytes);
				if (f->user_type->packed_bits > 1) {
					FR2(") ||"
						"\n%s\t\t\t_br_unlikely(count > BRIDGE_BIT_COUNT_MAX/%uu", offs, f->user_type->packed_bits);
				}
				FR1("))"
					"\n%s\t\t\treturn ~0u;", offs);
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR2("\n%s\t\tif (_br_unlikely(!_br_of_add(&byte_count,"
					"\n%s\t\t\tbridge_counter_packed_size((bridge_counter_t)count) - 1u", offs, offs);
				if (f->user_type->packed_bytes && (!f->user_type->equivalent || s_fixed(f->user_type))) {
					/* NOTE: it's always safe to add without overflow counter packed size to value <= BRIDGE_MAX */
					FP(" + (bridge_counter_t)count");
					if (f->user_type->packed_bytes > 1)
						FR1("*%uu", f->user_type->packed_bytes);
				}
				FR3(")))"
					"\n%s\t\t{"
					"\n%s\t\t\treturn ~0u;"
					"\n%s\t\t}", offs, offs, offs);
				if (f->user_type->packed_bits) {
					FR1("\n%s\t\tif (_br_unlikely(!_br_of_add(&bit_count, (bridge_counter_t)count", offs);
					if (f->user_type->packed_bits > 1)
						FR1("*%uu", f->user_type->packed_bits);
					FR1(")))"
						"\n%s\t\t\treturn ~0u;", offs);
				}
				FR1("\n%s\t}", offs);
				if (!s_fixed(f->user_type)) {
					if (!f->user_type->equivalent) {
						FR6("\n%s\t{"
							"\n%s\t\tconst struct %s *i = s->%s;"
							"\n%s\t\tfor (; i < s->%s_end; i++)", offs, offs, f->type, f->name, offs, f->name);
						(void)call_var_size_user_type(file, NULL, f, CALL_FROM_ARRAY, /*need_block:*/0, offs);
						FR1("\n%s\t}", offs);
					}
					else if (is_simple_str_type(f->user_type->equivalent)) {
						FR4("\n%s\tbyte_count = _bridge_add_%s_array_packed_size(byte_count,"
							"\n%s\t\t&s->%s->", offs, f->user_type->equivalent, offs, f->name);
						print_equivalent_path(file, f);
						FR1(", &s->%s_end->", f->name);
						print_equivalent_path(file, f);
						FP(");");
					}
					else {
						FR4("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count,"
							"\n%s\t\t_bridge_%s_array_packed_size(&s->%s->", offs, offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR1(", &s->%s_end->", f->name);
						print_equivalent_path(file, f);
						FR3("))))"
							"\n%s\t{"
							"\n%s\t\treturn ~0u;"
							"\n%s\t}", offs, offs, offs);
					}
				}
			}
			else if (field_type_bit == f->type) {
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR7("\n%s\tif (_br_unlikely(((void)0,BRIDGE_BIT_COUNT_MAX) < s->%s_bit_count) ||"
					"\n%s\t\t_br_unlikely(!_br_of_add(&byte_count, bridge_bit_array_packed_size(s->%s_bit_count) - 1u)))"
					"\n%s\t{"
					"\n%s\t\treturn ~0u;"
					"\n%s\t}", offs, f->name, offs, f->name, offs, offs, offs);
			}
			else if (is_simple_str_type(f->type)) {
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes, so byte_count >= 1 */
				FR4("\n%s\tbyte_count = bridge_add_%s_array_packed_size(byte_count - 1u, s->%s, s->%s_end);",
					offs, f->type, f->name, f->name);
			}
			else if (is_variabe_int_type(f->type)) {
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR5("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count, bridge_%s_array_packed_size(s->%s, s->%s_end) - 1u)))"
					"\n%s\t\treturn ~0u;", offs, f->type, f->name, f->name, offs);
			}
			else {
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR5("\n%s\tif (_br_unlikely(bridge_append_%s_array_packed_size(&byte_count, 1u, s->%s, s->%s_end)))"
					"\n%s\t\treturn ~0u;", offs, f->type, f->name, f->name, offs);
			}
			break;
		case F_OPTIONAL:
			FR3("\n%s\t/* %s: %s* */", offs, f->name, f->type);
			if (f->user_type) {
				if (!f->user_type->equivalent || s_fixed(f->user_type)) {
					FR2("\n%s\tif (s->%s) {", offs, f->name);
					if (f->user_type->packed_bytes) {
						FR3("\n%s\t\tif (_br_unlikely(!_br_of_add(&byte_count, %uu)))"
							"\n%s\t\t\treturn ~0u;", offs, f->user_type->packed_bytes, offs);
					}
					if (f->user_type->packed_bits) {
						FR3("\n%s\t\tif (_br_unlikely(!_br_of_add(&bit_count, %uu)))"
							"\n%s\t\t\treturn ~0u;", offs, f->user_type->packed_bits, offs);
					}
					if (!s_fixed(f->user_type)) {
						(void)call_var_size_user_type(file, s, f, CALL_FROM_OPTIONAL,
							/*need_block:*/f->user_type->packed_bytes || f->user_type->packed_bits, offs);
					}
					FR1("\n%s\t}", offs);
				}
				else if (f->user_type->equiv_many) {
					if (is_simple_str_type(f->user_type->equivalent)) {
						FR6("\n%s\tif (s->%s) {"
							"\n%s\t\tbyte_count = _bridge_add_%s_array_packed_size(byte_count,"
							"\n%s\t\t\t&s->%s->", offs, f->name, offs, f->user_type->equivalent, offs, f->name);
						print_equivalent_path(file, f);
						FR1(", &(s->%s + 1)->", f->name);
						print_equivalent_path(file, f);
						FR1(");"
							"\n%s\t}", offs);
					}
					else {
						FR5("\n%s\tif (s->%s && _br_unlikely(!_br_of_add(&byte_count,"
							"\n%s\t\t_bridge_%s_array_packed_size(&s->%s->", offs, f->name, offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR1(", &(s->%s + 1)->", f->name);
						print_equivalent_path(file, f);
						FR1("))))"
							"\n%s\t\treturn ~0u;", offs);
					}
				}
				else if (is_simple_str_type(f->user_type->equivalent)) {
					FR4("\n%s\tif (s->%s && _br_unlikely(_bridge_append_%s_packed_size(&byte_count, 0u, s->%s->",
						offs, f->name, f->user_type->equivalent, f->name);
					print_equivalent_path(file, f);
					FR1(")))"
						"\n%s\t\treturn ~0u;", offs);
				}
				else {
					FR4("\n%s\tif (s->%s && _br_unlikely(!_br_of_add(&byte_count, bridge_%s_packed_size(s->%s->",
						offs, f->name, f->user_type->equivalent, f->name);
					print_equivalent_path(file, f);
					FR1("))))"
						"\n%s\t\treturn ~0u;", offs);
				}
			}
			else if (is_simple_str_type(f->type)) {
				FR5("\n%s\tif (s->%s && _br_unlikely(_bridge_append_%s_packed_size(&byte_count, 0u, s->%s)))"
					"\n%s\t\treturn ~0u;", offs, f->name, f->type, f->name, offs);
			}
			else {
				FR2("\n%s\tif (s->has_%s && _br_unlikely(!_br_of_add(", offs, f->name);
				if (field_type_bit == f->type)
					FP("&bit_count, 1u");
				else if (is_variabe_int_type(f->type))
					FR2("&byte_count, bridge_%s_packed_size(s->%s)", f->type, f->name);
				else
					FR2("&byte_count, _br_sizeof(s->%s, %uu)", f->name, simple_fixed_int_size(f->type));
				FR1(")))"
					"\n%s\t\treturn ~0u;", offs);
			}
			break;
		case F_REQUIRED:
			if (f->user_type) {
				if (!s_fixed(f->user_type)) {
					FR3("\n%s\t/* %s: %s */", offs, f->name, f->type);
					if (!f->user_type->equivalent)
						returned = call_var_size_user_type(file, s, f, CALL_FROM_STATIC, /*need_block:*/1, offs);
					else if (f->user_type->equiv_many) {
						if (is_simple_str_type(f->user_type->equivalent)) {
							/* required n bytes of unfixed packed size are already counted in s->packed_bytes, so
							  byte_count >= sizeof(s->%s)/sizeof(s->%s.xxx) */
							/* NOTE: may cast 'sizeof(s->%s)/sizeof(s->%s.xxx)' to bridge_counter_t
							  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
							FR5("\n%s\tbyte_count = _bridge_add_%s_array_packed_size("
								"\n%s\t\tbyte_count - (bridge_counter_t)(sizeof(s->%s)/sizeof(s->%s.",
								offs, f->user_type->equivalent, offs, f->name, f->name);
							print_equivalent_path(file, f);
							FR2(")),"
								"\n%s\t\t&s->%s.", offs, f->name);
							print_equivalent_path(file, f);
							FR1(", &(&s->%s + 1)->", f->name);
							print_equivalent_path(file, f);
							FP(");");
						}
						else {
							/* required n bytes of unfixed packed size are already counted in s->packed_bytes */
							FR4("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count,"
								"\n%s\t\t_bridge_%s_array_packed_size(&s->%s.", offs, offs, f->user_type->equivalent, f->name);
							print_equivalent_path(file, f);
							FR1(", &(&s->%s + 1)->", f->name);
							print_equivalent_path(file, f);
							/* NOTE: may cast 'sizeof(s->%s)/sizeof(s->%s.xxx)' to bridge_counter_t
							  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
							FR3(") -"
								"\n%s\t\t(bridge_counter_t)(sizeof(s->%s)/sizeof(s->%s.", offs, f->name, f->name);
							print_equivalent_path(file, f);
							FR1(")))))"
								"\n%s\t\treturn ~0u;", offs);
						}
					}
					else if (is_simple_str_type(f->user_type->equivalent)) {
						/* required string packed size >= 1, 1 byte is already counted in s->packed_bytes */
						FR3("\n%s\tif (_br_unlikely(_bridge_append_%s_packed_size(&byte_count, 1u, s->%s.",
							offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR1(")))"
							"\n%s\t\treturn ~0u;", offs);
					}
					else {
						/* required 1 byte of unfixed packed size is already counted in s->packed_bytes */
						FR3("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count, bridge_%s_packed_size(s->%s.",
							offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR1(") - 1u)))"
							"\n%s\t\treturn ~0u;", offs);
					}
				}
			}
			else if (is_simple_str_type(f->type)) {
				/* required string packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR7("\n%s\t/* %s: %s */"
					"\n%s\tif (_br_unlikely(_bridge_append_%s_packed_size(&byte_count, 1u, s->%s)))"
					"\n%s\t\treturn ~0u;", offs, f->name, f->type, offs, f->type, f->name, offs);
			}
			else if (is_variabe_int_type(f->type)) {
				/* variable-packed integer size >= 1, 1 byte is already counted in s->packed_bytes */
				FR7("\n%s\t/* %s: %s */"
					"\n%s\tif (_br_unlikely(!_br_of_add(&byte_count, bridge_%s_packed_size(s->%s) - 1u)))"
					"\n%s\t\treturn ~0u;", offs, f->name, f->type, offs, f->type, f->name, offs);
			}
			break;
		case F_POINTER:
		default:
			if (f->user_type) {
				if (!s_fixed(f->user_type)) {
					FR7("\n%s\t/* %s: %s[%u] */"
						"\n%s\tif (_br_unlikely(!s->%s))"
						"\n%s\t\treturn ~0u;", offs, f->name, f->type, (F_POINTER == f->f_power) ? 1u : f->f_power, offs, f->name, offs);
					if (!f->user_type->equivalent) {
						/* required bytes and bits are already counted in s->packed_bytes and s->packed_bits */
						if (F_POINTER == f->f_power)
							returned = call_var_size_user_type(file, s, f, CALL_FROM_OPTIONAL, /*need_block:*/2, offs);
						else {
							FR5("\n%s\t{"
								"\n%s\t\tsize_t i = 0;"
								"\n%s\t\tfor (; i < sizeof(*s->%s)/sizeof((*s->%s)[0]); i++)", offs, offs, offs, f->name, f->name);
							(void)call_var_size_user_type(file, NULL, f, CALL_FROM_FIXED_ARRAY, /*need_block:*/0, offs);
							FR1("\n%s\t}", offs);
						}
					}
					else if (F_POINTER != f->f_power) {
						if (is_simple_str_type(f->user_type->equivalent)) {
							/* required n*m bytes of unfixed packed size are already counted in s->packed_bytes, so
							  byte_count >= sizeof(*s->%s)/sizeof((*s->%s)[0].xxx) */
							/* NOTE: may cast 'sizeof(*s->%s)/sizeof((*s->%s)[0].xxx)' to bridge_counter_t
							  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
							FR5("\n%s\tbyte_count = _bridge_add_%s_array_packed_size("
								"\n%s\t\tbyte_count - (bridge_counter_t)(sizeof(*s->%s)/sizeof((*s->%s)[0].",
								offs, f->user_type->equivalent, offs, f->name, f->name);
							print_equivalent_path(file, f);
							FR2(")),"
								"\n%s\t\t&(*s->%s)[0].", offs, f->name);
							print_equivalent_path(file, f);
							FR1(", &(*(s->%s + 1))[0].", f->name);
							print_equivalent_path(file, f);
							FP(");");
						}
						else {
							/* required n*m bytes of unfixed packed size are already counted in s->packed_bytes */
							FR4("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count,"
								"\n%s\t\t_bridge_%s_array_packed_size(&(*s->%s)[0].", offs, offs, f->user_type->equivalent, f->name);
							print_equivalent_path(file, f);
							FR1(", &(*(s->%s + 1))[0].", f->name);
							print_equivalent_path(file, f);
							/* NOTE: may cast 'sizeof(*s->%s)/sizeof((*s->%s)[0].xxx)' to bridge_counter_t
							  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
							FR3(") -"
								"\n%s\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof((*s->%s)[0].", offs, f->name, f->name);
							print_equivalent_path(file, f);
							FR1(")))))"
								"\n%s\t\treturn ~0u;", offs);
						}
					}
					else if (f->user_type->equiv_many) {
						if (is_simple_str_type(f->user_type->equivalent)) {
							/* required n*m bytes of unfixed packed size are already counted in s->packed_bytes, so
							  byte_count >= sizeof(*s->%s)/sizeof(s->%s->xxx) */
							/* NOTE: may cast 'sizeof(*s->%s)/sizeof(s->%s->xxx)' to bridge_counter_t
							  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
							FR5("\n%s\tbyte_count = _bridge_add_%s_array_packed_size("
								"\n%s\t\tbyte_count - (bridge_counter_t)(sizeof(*s->%s)/sizeof(s->%s->",
								offs, f->user_type->equivalent, offs, f->name, f->name);
							print_equivalent_path(file, f);
							FR2(")),"
								"\n%s\t\t&s->%s->", offs, f->name);
							print_equivalent_path(file, f);
							FR1(", &(s->%s + 1)->", f->name);
							print_equivalent_path(file, f);
							FP(");");
						}
						else {
							/* required n*m bytes of unfixed packed size are already counted in s->packed_bytes */
							FR4("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count,"
								"\n%s\t\t_bridge_%s_array_packed_size(&s->%s->", offs, offs, f->user_type->equivalent, f->name);
							print_equivalent_path(file, f);
							FR1(", &(s->%s + 1)->", f->name);
							print_equivalent_path(file, f);
							/* NOTE: may cast 'sizeof(*s->%s)/sizeof(s->%s->xxx)' to bridge_counter_t
							  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
							FR3(") -"
								"\n%s\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof(s->%s->", offs, f->name, f->name);
							print_equivalent_path(file, f);
							FR1(")))))"
								"\n%s\t\treturn ~0u;", offs);
						}
					}
					else if (is_simple_str_type(f->user_type->equivalent)) {
						/* required string packed size >= 1, 1 byte is already counted in s->packed_bytes */
						FR3("\n%s\tif (_br_unlikely(_bridge_append_%s_packed_size(&byte_count, 1u, s->%s->",
							offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR1(")))"
							"\n%s\t\treturn ~0u;", offs);
					}
					else {
						/* required 1 byte of unfixed packed size is already counted in s->packed_bytes */
						FR3("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count, bridge_%s_packed_size(s->%s->",
							offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR1(") - 1u)))"
							"\n%s\t\treturn ~0u;", offs);
					}
				}
			}
			else if (is_variable_simple_type(f->type)) {
				FR7("\n%s\t/* %s: %s[%u] */"
					"\n%s\tif (_br_unlikely(!s->%s))"
					"\n%s\t\treturn ~0u;", offs, f->name, f->type, (F_POINTER == f->f_power) ? 1u : f->f_power, offs, f->name, offs);
				if (F_POINTER != f->f_power) {
					if (is_simple_str_type(f->type)) {
						/* array packed size >= elements count, elements count bytes are already counted in s->packed_bytes */
						/* NOTE: may cast 'sizeof(*s->%s)/sizeof((*s->%s)[0])' to bridge_counter_t
						  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
						FR8("\n%s\tbyte_count = _bridge_add_%s_array_packed_size("
							"\n%s\t\tbyte_count - (bridge_counter_t)(sizeof(*s->%s)/sizeof((*s->%s)[0])),"
							"\n%s\t\t*s->%s, *(s->%s + 1));", offs, f->type, offs, f->name, f->name, offs, f->name, f->name);
					}
					else {
						/* array packed size >= elements count, elements count bytes are already counted in s->packed_bytes */
						/* NOTE: may cast 'sizeof(*s->%s)/sizeof((*s->%s)[0])' to bridge_counter_t
						  - structure 's' packed size was statically checked in code generated by generate_code_pack() */
						FR9("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count,"
							"\n%s\t\t_bridge_%s_array_packed_size(*s->%s, *(s->%s + 1)) -"
							"\n%s\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof((*s->%s)[0])))))"
							"\n%s\t\treturn ~0u;", offs, offs, f->type, f->name, f->name, offs, f->name, f->name, offs);
					}
				}
				else if (is_simple_str_type(f->type)) {
					/* required string packed size >= 1, 1 byte is already counted in s->packed_bytes */
					FR4("\n%s\tif (_br_unlikely(_bridge_append_%s_packed_size(&byte_count, 1u, *s->%s)))"
						"\n%s\t\treturn ~0u;", offs, f->type, f->name, offs);
				}
				else {
					/* required 1 byte of unfixed packed size is already counted in s->packed_bytes */
					FR4("\n%s\tif (_br_unlikely(!_br_of_add(&byte_count, bridge_%s_packed_size(*s->%s) - 1u)))"
						"\n%s\t\treturn ~0u;", offs, f->type, f->name, offs);
				}
			}
			break;
	}
	return returned;
}

static void generate_code_var_size(FILE *file, const struct struct_def *const s)
{
	int returned = 0;
	const int long_loop = (s->tail_recursive && s->tail_recursive->user_type != s);
	if (long_loop) {
		FR1("\nenum VAR_%s {", s->s_name);
		{
			const struct struct_def *r = s;
			do {
				const struct struct_def *n = r->tail_recursive->user_type;
				FR2("\n\tV_%s%s", r->s_name, n != s ? "," : "");
				r = n;
			} while (r != s);
		}
		FR5("\n};"
			"\n"
			"\nA_Pure_function A_Nonnull_all_args A_Check_return"
			"\nstatic %s _%s_var(A_In const void *p, enum VAR_%s t%s%s)",
			v_bits(s) ? "_bridge_bits_bytes_t" : "unsigned", s->s_name, s->s_name,
			v_bytes(s) ? ", unsigned byte_count" : "",
			v_bits(s) ? ", unsigned bit_count" : "");
	}
	else
		(void)generate_prototype_variable_size(file, s);
	FP("\n{");
	if (s->tail_recursive) {
		if (long_loop) {
			FP("\n\tswitch (t) {"
				"\ndefault:");
		}
		FP("\n\tfor (;;) {");
	}
	{
		const char *offs = long_loop ? "\t\t" : s->tail_recursive ? "\t" : "";
		const struct struct_def *r = s;
		do {
			if (long_loop) {
				FR3("\ncase V_%s: {"
					"\n\t\t\tconst struct %s *s = (const struct %s*)p;", r->s_name, r->s_name, r->s_name);
			}
			{
				struct field_def **f = r->fields;
				for (; f < r->fields_end; f++) {
					if (r->tail_recursive == *f)
						continue; /* tail-recursive field will be processed last */
					returned = switch_var_size(file, (!r->tail_recursive && f == r->fields_end - 1) ? r : NULL, *f, offs);
				}
			}
			if (r->tail_recursive) {
				FR4("\n%s\t/* %s: %s%s (tail recursive) */",
					offs, r->tail_recursive->name, r->tail_recursive->type,
					F_POINTER == r->tail_recursive->f_power ? "[1]" :
					F_OPTIONAL == r->tail_recursive->f_power ? "*" : "");
				if (long_loop) {
					FR2("\n\t\t\tp = %ss->%s;"
						"\n\t\t}", F_REQUIRED == r->tail_recursive->f_power ? "&" : "", r->tail_recursive->name);
					if (F_OPTIONAL == r->tail_recursive->f_power) {
						FP("\n\t\tif (!p)"
							"\n\t\t\tbreak;");
					}
					else if (F_POINTER == r->tail_recursive->f_power) {
						FP("\n\t\tif (_br_unlikely(!p))"
							"\n\t\t\treturn ~0u;");
					}
				}
				else {
					FR1("\n\t\ts = s->%s;"
						"\n\t\tif (!s)"
						"\n\t\t\tbreak;", r->tail_recursive->name);
				}
				if (F_OPTIONAL == r->tail_recursive->f_power) {
					if (r->tail_recursive->user_type->packed_bytes) {
						FR1("\n\t\tif (_br_unlikely(!_br_of_add(&byte_count, %uu)))"
							"\n\t\t\treturn ~0u;", r->tail_recursive->user_type->packed_bytes);
					}
					if (r->tail_recursive->user_type->packed_bits) {
						FR1("\n\t\tif (_br_unlikely(!_br_of_add(&bit_count, %uu)))"
							"\n\t\t\treturn ~0u;", r->tail_recursive->user_type->packed_bits);
					}
				}
				r = r->tail_recursive->user_type;
			}
		} while (r != s);
	}
	if (long_loop)
		FP("\n\t}}");
	else if (s->tail_recursive)
		FP("\n\t}");
	if (!returned) {
		if (v_bits(s))
			FR1("\n\treturn _bridge_bits_bytes_from_bits_and_bytes(bit_count, %s);", v_bytes(s) ? "byte_count" : "0");
		else
			FP("\n\treturn byte_count;");
	}
	FP("\n}"
		"\n");
}

static const char *_get_append_packed_info(const struct struct_def *s)
{
	if (s->convertable) {
		if (s_has_bits(s))
			return "_pk_bits_mem_conv_info";
		return "_pk_mem_conv_info";
	}
	if (s_has_bytes(s)) {
		if (s_has_bits(s))
			return "_pk_bits_mem_info";
		return "_pk_mem_info";
	}
	return "_pk_bits_info";
}

/* must be called after check_required_pointers() */
int generate_prototype_append_packed(FILE *file, const struct struct_def *s)
{
	if (!s->equivalent) {
		FR5("\nA_Nonnull_all_args%s"
			"\nstatic %s _%s_append_packed(A_In const struct %s *s, A_Inout union %s *info)",
			s->_processed ? " A_Check_return A_Ret_range(0,1)" : "",
			s->_processed ? "int" : "void",
			s->s_name, s->s_name, _get_append_packed_info(s));
		return 1;
	}
	return 0;
}

static void print_append_packed_call_args(FILE *file, const struct struct_def *s, const struct field_def *f)
{
	if (f->user_type->convertable) {
		if (s_has_bits(f->user_type) != s_has_bits(s))
			FP("&info->_pmc._mc)");
		else
			FP("info)");
	}
	else if (s_has_bytes(f->user_type)) {
		if (s_has_bits(f->user_type)) {
			if (s->convertable)
				FP("&info->_pbm._bm)");
			else
				FP("info)");
		}
		else if (s->convertable || s_has_bits(s))
			FP("&info->_pm._m)");
		else
			FP("info)");
	}
	else if (s->convertable || s_has_bytes(s))
		FP("&info->_pb._b)");
	else
		FP("info)");
}

static void print_append_packed_call(FILE *file, const struct struct_def *s,
	const struct field_def *f, enum CALL_FROM from, const char *offs, int after_else)
{
	if (f->user_type->_processed) {
		if (CALL_FROM_ARRAY == from || CALL_FROM_FIXED_ARRAY == from) {
			FR1(" {"
				"\n%s\t", offs);
		}
		else if (after_else)
			FP(" ");
		else
			FR1("\n%s\t", offs);
		FP("if (_br_unlikely(!");
	}
	else if (after_else)
		FR1("\n%s\t\t", offs);
	else
		FR1("\n%s\t", offs);
	FR1("_%s_append_packed(", f->type);
	print_call_from(file, f, from);
	print_append_packed_call_args(file, s, f);
	if (f->user_type->_processed) {
		FR1("))"
			"\n%s\t\treturn 0;", offs);
		if (CALL_FROM_ARRAY == from || CALL_FROM_FIXED_ARRAY == from)
			FR1("\n%s}", offs);
	}
	else
		FP(";");
}

static void switch_app_packed(FILE *file, const struct field_def *f, const struct struct_def *s, const char *offs)
{
	switch (f->f_power) {
		case F_ARRAY:
			FR3("\n%s\t/* %s: %s[] */", offs, f->name, f->type);
			if (f->user_type) {
				FR9("\n%s\tASSUME(s->%s_end - s->%s <= BRIDGE_MAX);"
					"\n%s\t{"
					"\n%s\t\tbridge_counter_t count = (bridge_counter_t)(s->%s_end - s->%s);"
					"\n%s\t\tinfo->p.mem = _bridge_pack_counter(info->p.mem, count);"
					"\n%s\t\t", offs, f->name, f->name, offs, offs, f->name, f->name, offs, offs);
				if (!f->user_type->equivalent) {
					if (f->user_type->s_layout < S_MONOLITH && !s_has_bits(f->user_type) && !v_bytes(f->user_type)) {
						/* if structure has padding bytes - then cannot just memcpy */
						FR2("if (((void)0,%uu) == sizeof(*s->%s)", f->user_type->packed_bytes, f->name);
						if (f->user_type->convertable)
							FP(" && !info->p.convert");
						FR9(") {"
							"\n%s\t\t\tchar *m = info->p.mem;"
							"\n%s\t\t\tinfo->p.mem = m + sizeof(*s->%s)*count;"
							"\n%s\t\t\tif (count)"
							"\n%s\t\t\t\tBRIDGE_MEMCPY(m, s->%s, sizeof(*s->%s)*count);"
							"\n%s\t\t}"
							"\n%s\t\telse ", offs, offs, f->name, offs, offs, f->name, f->name, offs, offs);
					}
					FR5("{"
						"\n%s\t\t\tconst struct %s *i = s->%s;"
						"\n%s\t\t\tfor (; i < s->%s_end; i++)", offs, f->type, f->name, offs, f->name);
					print_append_packed_call(file, s, f, CALL_FROM_ARRAY,
						offs[0] ? offs[1] ? "\t\t\t\t\t" : "\t\t\t\t" : "\t\t\t", /*after_else:*/0);
					FR1("\n%s\t\t}", offs);
				}
				else if (!s_fixed(f->user_type)) {
					FR2("info->p.mem = _bridge_pack_%s_array(info->p.mem, &s->%s->", f->user_type->equivalent, f->name);
					print_equivalent_path(file, f);
					FR1(", &s->%s_end->", f->name);
					print_equivalent_path(file, f);
					FP(");");
				}
				else {
					FR4("{"
						"\n%s\t\t\tchar *m = info->p.mem;"
						"\n%s\t\t\tinfo->p.mem = m + sizeof(*s->%s)*count;"
						"\n%s\t\t\t", offs, offs, f->name, offs);
					if (field_type_byte != f->user_type->equivalent) {
						FR3("if (info->p.convert)"
							"\n%s\t\t\t\tput_exchange_%s_array(&s->%s->", offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR2(", m, (sizeof(*s->%s)/sizeof(s->%s->", f->name, f->name);
						print_equivalent_path(file, f);
						FR1("))*count);"
							"\n%s\t\t\telse ", offs);
					}
					FR4("if (count)"
						"\n%s\t\t\t\tBRIDGE_MEMCPY(m, s->%s, sizeof(*s->%s)*count);"
						"\n%s\t\t}", offs, f->name, f->name, offs);
				}
				FR1("\n%s\t}", offs);
			}
			else if (field_type_bit == f->type)
				FR3("\n%s\tinfo->p.mem = bridge_pack_bit_array(info->p.mem, s->%s, s->%s_bit_count);", offs, f->name, f->name);
			else if (is_variable_simple_type(f->type))
				FR4("\n%s\tinfo->p.mem = bridge_pack_%s_array(info->p.mem, s->%s, s->%s_end);", offs, f->type, f->name, f->name);
			else {
				FR10("\n%s\t{"
					"\n%s\t\tbridge_counter_t count = (bridge_counter_t)(s->%s_end - s->%s);"
					"\n%s\t\tinfo->p.mem = _bridge_pack_counter(info->p.mem, count);"
					"\n%s\t\t{"
					"\n%s\t\t\tchar *m = info->p.mem;"
					"\n%s\t\t\tinfo->p.mem = m + sizeof(*s->%s)*count;"
					"\n%s\t\t\t", offs, offs, f->name, f->name, offs, offs, offs, offs, f->name, offs);
				if (field_type_byte != f->type) {
					FR4("if (info->p.convert)"
						"\n%s\t\t\t\tput_exchange_%s_array(s->%s, m, count);"
						"\n%s\t\t\telse ", offs, f->type, f->name, offs);
				}
				FR5("if (count)"
					"\n%s\t\t\t\tBRIDGE_MEMCPY(m, s->%s, sizeof(*s->%s)*count);"
					"\n%s\t\t}"
					"\n%s\t}", offs, f->name, f->name, offs, offs);
			}
			break;
		case F_POINTER:
		case F_OPTIONAL:
		case F_REQUIRED:
			if (F_POINTER != f->f_power || f->user_type || field_type_bit != f->type) {
				const char *of        = (F_OPTIONAL == f->f_power) ? offs[0] ? offs[1] ? "\t\t\t" : "\t\t" : "\t" : offs;
				const char *deref     = (F_REQUIRED == f->f_power) ? ""   : "*";
				const char *ref       = (F_REQUIRED == f->f_power) ? "&"  : "";
				const char *f_access  = (F_REQUIRED == f->f_power) ? "."  : "->";
				const char *deref_ptr = (F_POINTER  == f->f_power) ? "*"  : "";
				FR4("\n%s\t/* %s: %s%s */", offs, f->name, f->type, (F_POINTER == f->f_power) ? "[1]" : deref);
				if (F_POINTER == f->f_power &&
					(f->user_type ? s_fixed(f->user_type) : !is_variable_simple_type(f->type)))
				{
					FR3("\n%s\tif (_br_unlikely(!s->%s))"
						"\n%s\t\treturn 0;", offs, f->name, offs);
				}
				if (f->user_type) {
					if (F_OPTIONAL == f->f_power)
						FR2("\n%s\tif (bridge_set_bit(info->p.bits, info->p.bit_count++, s->%s ? 1 : 0))", offs, f->name);
					if (!f->user_type->equivalent) {
						int after_else = 0;
						if (f->user_type->s_layout < S_MONOLITH && !s_has_bits(f->user_type) && !v_bytes(f->user_type)) {
							if (F_OPTIONAL == f->f_power)
								FP(" {");
							else
								FR1("\n%s\t", offs);
							/* if structure has padding bytes - then cannot just memcpy */
							FR3("if (((void)0,%uu) == sizeof(%ss->%s)", f->user_type->packed_bytes, deref, f->name);
							if (f->user_type->convertable)
								FP(" && !info->p.convert");
							FR11(") {"
								"\n%s\t\tchar *m = info->p.mem;"
								"\n%s\t\tinfo->p.mem = m + sizeof(%ss->%s);"
								"\n%s\t\tBRIDGE_MEMCPY(m, %ss->%s, sizeof(%ss->%s));"
								"\n%s\t}"
								"\n%s\telse", of, of, deref, f->name, of, ref, f->name, deref, f->name, of, of);
							after_else = 1;
						}
						else if (f->user_type->_processed && F_OPTIONAL == f->f_power)
							FP(" {");
						print_append_packed_call(file, s, f,
							(F_REQUIRED == f->f_power) ? CALL_FROM_STATIC : CALL_FROM_OPTIONAL, of, after_else);
						if (F_OPTIONAL == f->f_power && (after_else || f->user_type->_processed))
							FR1("\n%s\t}", offs);
					}
					else if (f->user_type->equiv_many) {
						if (!s_fixed(f->user_type)) {
							FR4("\n%s\tinfo->p.mem = _bridge_pack_%s_array(info->p.mem, &s->%s%s",
								of, f->user_type->equivalent, f->name, f_access);
							print_equivalent_path(file, f);
							FR2(", &(%ss->%s + 1)->", ref, f->name);
							print_equivalent_path(file, f);
							FP(");");
						}
						else {
							if (F_OPTIONAL == f->f_power)
								FP(" {");
							else
								FR1("\n%s\t{", offs);
							FR5("\n%s\t\tchar *m = info->p.mem;"
								"\n%s\t\tinfo->p.mem = m + sizeof(%ss->%s);"
								"\n%s\t\t", offs, offs, deref, f->name, offs);
							if (field_type_byte != f->user_type->equivalent) {
								FR4("if (info->p.convert)"
									"\n%s\t\t\tput_exchange_%s_array(&s->%s%s", offs, f->user_type->equivalent, f->name, f_access);
								print_equivalent_path(file, f);
								FR4(", m, sizeof(%ss->%s)/sizeof(s->%s%s", deref, f->name, f->name, f_access);
								print_equivalent_path(file, f);
								FR2("));"
									"\n%s\t\telse"
									"\n%s\t\t\t", offs, offs);
							}
							FR5("BRIDGE_MEMCPY(m, %ss->%s, sizeof(%ss->%s));"
								"\n%s\t}", ref, f->name, deref, f->name, offs);
						}
					}
					else if (!s_fixed(f->user_type)) {
						FR4("\n%s\tinfo->p.mem = _bridge_pack_%s(info->p.mem, s->%s%s", of, f->user_type->equivalent, f->name, f_access);
						print_equivalent_path(file, f);
						FP(");");
					}
					else {
						FR5("\n%s\t_bridge_put_%s(&info->p.mem%s, s->%s%s",
							of, f->user_type->equivalent,
							field_type_byte == f->user_type->equivalent ? "" : ", info->p.convert",
							f->name, f_access);
						print_equivalent_path(file, f);
						FP(");");
					}
				}
				else if (is_simple_str_type(f->type)) {
					if (F_OPTIONAL == f->f_power)
						FR2("\n%s\tif (bridge_set_bit(info->p.bits, info->p.bit_count++, s->%s ? 1 : 0))", offs, f->name);
					FR4("\n%s\tinfo->p.mem = _bridge_pack_%s(info->p.mem, %ss->%s);", of, f->type, deref_ptr, f->name);
				}
				else {
					if (F_OPTIONAL == f->f_power)
						FR2("\n%s\tif (bridge_set_bit(info->p.bits, info->p.bit_count++, s->has_%s ? 1 : 0))", offs, f->name);
					if (field_type_bit == f->type)
						FR2("\n%s\tbridge_set_bit(info->p.bits, info->p.bit_count++, s->%s ? 1 : 0);", of, f->name);
					else if (is_variabe_int_type(f->type))
						FR4("\n%s\tinfo->p.mem = _bridge_pack_%s(info->p.mem, %ss->%s);", of, f->type, deref_ptr, f->name);
					else {
						FR5("\n%s\t_bridge_put_%s(&info->p.mem%s, %ss->%s);",
							of, f->type, field_type_byte == f->type ? "" : ", info->p.convert", deref_ptr, f->name);
					}
				}
				break;
			}
			/* F_POINTER for field_type_bit, fall through */
		default:
			FR4("\n%s\t/* %s: %s[%u] */", offs, f->name, f->type, f->f_power);
			if (f->user_type ? s_fixed(f->user_type) : !is_variable_simple_type(f->type)) {
				FR3("\n%s\tif (_br_unlikely(!s->%s))"
					"\n%s\t\treturn 0;", offs, f->name, offs);
			}
			if (f->user_type) {
				if (!f->user_type->equivalent) {
					if (f->user_type->s_layout < S_MONOLITH && !s_has_bits(f->user_type) && !v_bytes(f->user_type)) {
						/* if structure has padding bytes - then cannot just memcpy */
						FR3("\n%s\tif (((void)0,%uu) == sizeof(*s->%s)", offs, f->user_type->packed_bytes*f->f_power, f->name);
						if (f->user_type->convertable)
							FP(" && !info->p.convert");
						FR8(") {"
							"\n%s\t\tchar *m = info->p.mem;"
							"\n%s\t\tinfo->p.mem = m + sizeof(*s->%s);"
							"\n%s\t\tBRIDGE_MEMCPY(m, s->%s, sizeof(*s->%s));"
							"\n%s\t}"
							"\n%s\telse ", offs, offs, f->name, offs, f->name, f->name, offs, offs);
					}
					else
						FR1("\n%s\t", offs);
					FR4("{"
						"\n%s\t\tsize_t i = 0;"
						"\n%s\t\tfor (; i < sizeof(*s->%s)/sizeof((*s->%s)[0]); i++)", offs, offs, f->name, f->name);
					print_append_packed_call(file, s, f, CALL_FROM_FIXED_ARRAY,
						offs[0] ? offs[1] ? "\t\t\t\t" : "\t\t\t" : "\t\t", /*after_else:*/0);
					FR1("\n%s\t}", offs);
				}
				else if (!s_fixed(f->user_type)) {
					FR3("\n%s\tinfo->p.mem = _bridge_pack_%s_array(info->p.mem, &(*s->%s)[0].", offs, f->user_type->equivalent, f->name);
					print_equivalent_path(file, f);
					FR1(", &(*(s->%s + 1))[0].", f->name);
					print_equivalent_path(file, f);
					FP(");");
				}
				else {
					FR5("\n%s\t{"
						"\n%s\t\tchar *m = info->p.mem;"
						"\n%s\t\tinfo->p.mem = m + sizeof(*s->%s);"
						"\n%s\t\t", offs, offs, offs, f->name, offs);
					if (field_type_byte != f->user_type->equivalent) {
						FR3("if (info->p.convert)"
							"\n%s\t\t\tput_exchange_%s_array(&(*s->%s)[0].", offs, f->user_type->equivalent, f->name);
						print_equivalent_path(file, f);
						FR2(", m, sizeof(*s->%s)/sizeof((*s->%s)[0].", f->name, f->name);
						print_equivalent_path(file, f);
						FR2("));"
							"\n%s\t\telse"
							"\n%s\t\t\t", offs, offs);
					}
					FR3("BRIDGE_MEMCPY(m, s->%s, sizeof(*s->%s));"
						"\n%s\t}", f->name, f->name, offs);
				}
			}
			else if (field_type_bit == f->type) {
				if (f->f_power > 8) {
					FR8("\n%s\t{"
						"\n%s\t\tchar *m = info->p.mem;"
						"\n%s\t\tinfo->p.mem = m + sizeof(*s->%s);"
						"\n%s\t\tBRIDGE_MEMCPY(m, s->%s, sizeof(*s->%s));"
						"\n%s\t}", offs, offs, offs, f->name, offs, f->name, f->name, offs);
				}
				else {
					FR5("\n%s\t{"
						"\n%s\t\tchar *m = info->p.mem++;"
						"\n%s\t\t*(unsigned char*)m = (*s->%s)[0];"
						"\n%s\t}", offs, offs, offs, f->name, offs);
				}
			}
			else if (is_variable_simple_type(f->type))
				FR4("\n%s\tinfo->p.mem = _bridge_pack_%s_array(info->p.mem, *s->%s, *(s->%s + 1));", offs, f->type, f->name, f->name);
			else {
				FR5("\n%s\t{"
					"\n%s\t\tchar *m = info->p.mem;"
					"\n%s\t\tinfo->p.mem = m + sizeof(*s->%s);"
					"\n%s\t\t", offs, offs, offs, f->name, offs);
				if (field_type_byte != f->type) {
					FR7("if (info->p.convert)"
						"\n%s\t\t\tput_exchange_%s_array(*s->%s, m, sizeof(*s->%s)/sizeof((*s->%s)[0]));"
						"\n%s\t\telse"
						"\n%s\t\t\t", offs, f->type, f->name, f->name, f->name, offs, offs);
				}
				FR3("BRIDGE_MEMCPY(m, s->%s, sizeof(*s->%s));"
					"\n%s\t}", f->name, f->name, offs);
			}
			break;
	}
}

static void generate_code_app_packed(FILE *file, const struct struct_def *const s)
{
	const int long_loop = (s->tail_recursive && s->tail_recursive->user_type != s);
	if (long_loop) {
		FR1("\nenum APP_%s {", s->s_name);
		{
			const struct struct_def *r = s;
			do {
				const struct struct_def *n = r->tail_recursive->user_type;
				FR2("\n\tA_%s%s", r->s_name, n != s ? "," : "");
				r = n;
			} while (r != s);
		}
		FR5("\n};"
			"\n"
			"\nA_Nonnull_all_args%s"
			"\nstatic %s _%s_app(A_In const void *p, enum APP_%s t, A_Inout union %s *info)",
			s->_processed ? " A_Check_return A_Ret_range(0,1)" : "",
			s->_processed ? "int" : "void",
			s->s_name, s->s_name, _get_append_packed_info(s));
	}
	else
		(void)generate_prototype_append_packed(file, s);
	FP("\n{");
	if (s->tail_recursive) {
		if (long_loop) {
			FP("\n\tswitch (t) {"
				"\ndefault:");
		}
		FP("\n\tfor (;;) {");
	}
	{
		const char *offs = long_loop ? "\t\t" : s->tail_recursive ? "\t" : "";
		const struct struct_def *r = s;
		/* NOTE: just pack: all sizes have already been checked while determining packed size */
		do {
			if (long_loop) {
				FR3("\ncase A_%s: {"
					"\n\t\t\tconst struct %s *s = (const struct %s*)p;", r->s_name, r->s_name, r->s_name);
			}
			{
				struct field_def **f = r->fields;
				for (; f < r->fields_end; f++) {
					if (r->tail_recursive == *f)
						continue; /* tail-recursive field will be processed last */
					switch_app_packed(file, *f, s, offs); /* NOTE: use 's' (not 'r') to work with arguments of generated function */
				}
			}
			if (r->tail_recursive) {
				FR4("\n%s\t/* %s: %s%s (tail recursive) */",
					offs, r->tail_recursive->name, r->tail_recursive->type,
					F_POINTER == r->tail_recursive->f_power ? "[1]" :
					F_OPTIONAL == r->tail_recursive->f_power ? "*" : "");
				if (long_loop) {
					FR2("\n\t\t\tp = %ss->%s;"
						"\n\t\t}", F_REQUIRED == r->tail_recursive->f_power ? "&" : "", r->tail_recursive->name);
					if (F_OPTIONAL == r->tail_recursive->f_power) {
						FP("\n\t\tif (!bridge_set_bit(info->p.bits, info->p.bit_count++, p ? 1 : 0))"
							"\n\t\t\tbreak;");
					}
				}
				else {
					FR1("\n\t\ts = s->%s;"
						"\n\t\tif (!bridge_set_bit(info->p.bits, info->p.bit_count++, s ? 1 : 0))"
						"\n\t\t\tbreak;", r->tail_recursive->name);
				}
				r = r->tail_recursive->user_type;
			}
		} while (r != s);
	}
	if (long_loop)
		FP("\n\t}}");
	else if (s->tail_recursive)
		FP("\n\t}");
	if (s->_processed)
		FP("\n\treturn 1;");
	FP("\n}"
		"\n");
}

/* check if generated _%s_var_size() function may fail to determine packed size because of NULL'ed required fixed-sized array pointer */
/* check if generated _%s_append_packed() function may fail to pack a structure because of NULL'ed required fixed-sized array pointer */
/* NOTE: generated code must not crash if user have forget to fill some required fields just after a structure was allocated,
  but it _may_ crash if user fills fields incorrectly, e.g. sets bit_count to non-zero and does not allocate corresponding bits array */
void check_required_pointers(void)
{
	/* NOTE: assume initially (*s)->_aux, (*s)->_marked and (*s)->_processed are zero */
	/* check if generated _%s_var_size() function may fail to determine packed size because of NULL'ed required fixed-sized array pointer */
	for (;;) {
		int has_unknown = 0;
		int mark_set = 0;
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			/* check if structure is not yet finally defined */
			if (!(*s)->_aux) {
				int unknown = 0;
				struct field_def *const *f = (*s)->fields;
				for (; f < (*s)->fields_end; f++) {
					switch ((*f)->f_power) {
						case F_ARRAY:
						case F_OPTIONAL:
						case F_REQUIRED:
							if ((*f)->user_type &&
								/* check if user type has pointers */
								!(*f)->user_type->equivalent &&
								S_DYNAMIC == (*f)->user_type->s_layout &&
								/* and we will check its variable size */
								!s_fixed((*f)->user_type))
							{
								if ((*f)->user_type->_marked)
									break; /* inherit flag */
								if (!(*f)->user_type->_aux)
									unknown = 1; /* not known yet - will be known after (*f)->user_type get defined */
							}
							continue;
						case F_POINTER:
						default:
							if ((*f)->user_type) {
								if (!s_fixed((*f)->user_type))
									break;
							}
							else if (is_variable_simple_type((*f)->type))
								break;
							continue;
					}
					mark_set = 1; /* (*s)->_marked was changed */
					(*s)->_marked = 1; /* yes, may fail */
					(*s)->_aux = 1; /* defined */
					break;
				}
				if (!(*s)->_aux) {
					if (!unknown)
						(*s)->_aux = 1; /* defined: no, cannot fail */
					else
						has_unknown = 1;
				}
			}
		}
		if (!has_unknown || !mark_set)
			break;
	}
	/* NOTE: here all structures have _aux != 2 (_aux may be zero for recursive types) */
	/* check if generated _%s_append_packed() function may fail to pack a structure because of NULL'ed required fixed-sized array pointer */
	for (;;) {
		int has_unknown = 0;
		int mark_set = 0;
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			/* check if structure is not yet finally defined */
			if (2 != (*s)->_aux) {
				int unknown = 0;
				struct field_def *const *f = (*s)->fields;
				for (; f < (*s)->fields_end; f++) {
					switch ((*f)->f_power) {
						case F_ARRAY:
						case F_OPTIONAL:
						case F_REQUIRED:
							if ((*f)->user_type &&
								/* check if user type has pointers */
								!(*f)->user_type->equivalent &&
								S_DYNAMIC == (*f)->user_type->s_layout)
							{
								if ((*f)->user_type->_processed)
									break; /* inherit flag */
								if (2 != (*f)->user_type->_aux)
									unknown = 1; /* not known yet - will be known after (*f)->user_type get defined */
							}
							continue;
						case F_POINTER:
						default:
							if ((*f)->user_type) {
								if (!s_fixed((*f)->user_type))
									continue; /* required ptr must be checked by _%s_var_size() */
							}
							else if (is_variable_simple_type((*f)->type))
								continue; /* required ptr must be checked by _%s_var_size() */
							break;
					}
					mark_set = 1; /* (*s)->_processed was changed */
					(*s)->_processed = 1; /* yes, may fail */
					(*s)->_aux = 2; /* defined */
					break;
				}
				if (2 != (*s)->_aux) {
					if (!unknown)
						(*s)->_aux = 2; /* defined: no, cannot fail */
					else
						has_unknown = 1;
				}
			}
		}
		if (!has_unknown || !mark_set)
			break;
	}
}

/* must be called after check_required_pointers() */
void generate_code_pack(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/)
{
	if (!s->equivalent) {
		if (!s_fixed(s)) {
			if (!s->tail_recursive || s->recursion_entry)
				generate_code_var_size(file, s);
			if (s->tail_recursive && s->tail_recursive->user_type != s) {
				const struct struct_def *r = s;
				while (!r->recursion_entry)
					r = r->tail_recursive->user_type;
				(void)generate_prototype_variable_size(file, s);
				FR4("\n{"
					"\n\treturn _%s_var(s, V_%s%s%s);"
					"\n}"
					"\n", r->s_name, s->s_name, v_bytes(s) ? ", byte_count" : "", v_bits(s) ? ", bit_count" : "");
			}
		}
		if (!s->tail_recursive || s->recursion_entry)
			generate_code_app_packed(file, s);
		if (s->tail_recursive && s->tail_recursive->user_type != s) {
			const struct struct_def *r = s;
			while (!r->recursion_entry)
				r = r->tail_recursive->user_type;
			(void)generate_prototype_append_packed(file, s);
			FR3("\n{"
				"\n\t%s_%s_app(s, A_%s, info);"
				"\n}"
				"\n", s->_processed ? "return " : "", r->s_name, s->s_name);
		}
	}
	FR3("\nA_Nonnull_all_args A_Check_return"
		"\nstatic void *_%s_pack_(A_In const struct %s *s, A_Inout union bridge_pack_%sinfo *in BRIDGE_DEBUG_ARGS_DECL)"
		"\n{", s->s_name, s->s_name, s->convertable ? "conv_" : "");
	if (!s->equivalent)
		FR1("\n\tunion %s info;", _get_append_packed_info(s));
	{
		unsigned szl = s_packed_size_limit(s); /* non-zero if structure packed size is limited by szl */
		unsigned initial_bytes = s->packed_bytes; /* initial_bytes <= BRIDGE_MODEL_MAX_PACKED_SIZE (checked in model.c) */
		{
			unsigned _bb = bytes_for_bits(s->packed_bits);
			unsigned min_bytes = initial_bytes + _bb + (need_pack_bits_counter(s) ? _counter_packed_size(_bb) : 0);
			/* may assert if generator's BRIDGE_MODEL_MAX_PACKED_SIZE > target's BRIDGE_MAX */
			if (min_bytes > 1)
				FR1("\n\t(void)sizeof(int[1-2*(%uu > BRIDGE_MAX)]);", min_bytes);
			/* may assert if generator's BRIDGE_MODEL_MAX_BIT_COUNT > target's BRIDGE_BIT_COUNT_MAX */
			if (s->packed_bits > 1)
				FR1("\n\t(void)sizeof(int[1-2*(%uu > BRIDGE_BIT_COUNT_MAX)]);", s->packed_bits);
			if (bits_have_fixed_bytes(s))
				initial_bytes += _bb; /* initial_bytes <= BRIDGE_MODEL_MAX_PACKED_SIZE after addition */
		}
		/* ignore computed value of bit_count if number of bytes for bits is known and is already accounted in initial_bytes;
		   NOTE: even in this case computed total number of bits may overflow (for const bit count 0xFFFFFFFF and 1 variable bit) */
		if (v_bytes(s) && v_bits(s)) {
			/* variable bytes & bits */
			FR3("\n\t{"
				"\n\t\t_bridge_bits_bytes_t bb = _%s_var_size(s, %uu, %uu);"
				"\n\t\tunsigned packed_size = _bridge_bits_bytes_get_bytes(bb);"
				"\n\t\tunsigned packed_bits = _bridge_bits_bytes_get_bits(bb);"
				"\n\t\tif (", s->s_name, initial_bytes, s->packed_bits);
			if (!s->unknown_packed_bits && ~0u - s->packed_bits >= s->packed_optional_bits)
				FR1("((void)0,%uu) > BRIDGE_BIT_COUNT_MAX && ", s->packed_bits + s->packed_optional_bits);
			FP("_br_unlikely(packed_bits > ((void)0,BRIDGE_BIT_COUNT_MAX)))"
				"\n\t\t\treturn NULL;"
				"\n\t\t");
			/* NOTE: _bridge_bits_bytes_get_bits(bb) may return zero on error (may be because of overflow) */
			if (!bits_have_fixed_bytes(s)) {
				if (!s->_marked && s->packed_bits &&
					!s->unknown_packed_bits && ~0u - s->packed_bits >= s->packed_optional_bits &&
					!s->unknown_packed_bytes && ~0u - s->packed_bytes >= s->packed_optional_bytes)
				{
					FR3("if (((void)0,%uu) <= ~0u && ((void)0,%uu) <= ~0u) {"
						"\n\t\t\tBRIDGE_ASSERT(packed_bits >= %uu);"
						"\n\t\t\tinfo.p.bit_count = bridge_nonzero_bit_array_size(packed_bits);"
						"\n\t\t}"
						"\n\t\telse"
						"\n\t\t\t", s->packed_bytes + s->packed_optional_bytes, s->packed_bits + s->packed_optional_bits, s->packed_bits);
				}
				FP("info.p.bit_count = bridge_bit_array_size(packed_bits);"
					"\n\t\tif (_br_unlikely(!");
				/* on error (if s->_marked is non-zero) packed_size may be == ~0u > szl */
				if (!s->_marked && szl) {
					/* NOTE: szl != 0, so s->packed_bits + s->packed_optional_bits does not overflow */
					/* NOTE: on runtime, if computed bit count does overflow, packed_size will be == ~0u */
					FR2("_br_maybe_of_add(&packed_size,"
						"\n\t\t\tinfo.p.bit_count + bridge_counter_packed_size((bridge_counter_t)info.p.bit_count),"
						"\n\t\t\t((void)0,%uu) > ~0u || ((void)0,%uu) > ~0u)", szl, s->packed_bits + s->packed_optional_bits);
				}
				else {
					FP("_br_of_add(&packed_size,"
						"\n\t\t\tinfo.p.bit_count + bridge_counter_packed_size((bridge_counter_t)info.p.bit_count))");
				}
				FP("))"
					"\n\t\t{"
					"\n\t\t\treturn NULL;"
					"\n\t\t}"
					"\n\t\t");
			}
			FP("if (");
			if (!s->_marked && szl)
				FR2("(((void)0,%uu) > BRIDGE_MAX || ((void)0,%uu) > ~0u) && ", szl, s->packed_bits + s->packed_optional_bits);
			FP("_br_unlikely(packed_size > BRIDGE_MAX))"
				"\n\t\t\treturn NULL;"
				"\n\t\tin->pi.size = packed_size;"
				"\n\t}");
		}
		else if (!bits_have_fixed_bytes(s)) {
			/* variable bits, fixed bytes */
			/* NOTE: s->packed_bits is non-zero:
			  the only case when s->packed_bits may be zero - for an array of user-types which pack only bits,
			  but array counter packed size is variable, and here we have fixed number of packed bytes */
			FR3("\n\t(void)sizeof(int[1-2*!%uu]);"
				"\n\t{"
				"\n\t\t_bridge_bits_bytes_t bb = _%s_var_size(s, %uu);"
				"\n\t\tif (", s->packed_bits, s->s_name, s->packed_bits);
			if (!s->_marked && !s->unknown_packed_bits && ~0u - s->packed_bits >= s->packed_optional_bits)
				FR1("((void)0,%uu) > ~0u && ", s->packed_bits + s->packed_optional_bits);
			FP("_br_unlikely(_bridge_bits_bytes_get_bytes(bb)))"
				"\n\t\t\treturn NULL;"
				"\n\t\t{"
				"\n\t\t\tunsigned packed_bits = _bridge_bits_bytes_get_bits(bb);"
				"\n\t\t\tif (");
			if (!s->unknown_packed_bits && ~0u - s->packed_bits >= s->packed_optional_bits)
				FR1("((void)0,%uu) > BRIDGE_BIT_COUNT_MAX && ", s->packed_bits + s->packed_optional_bits);
			FP("_br_unlikely(packed_bits > ((void)0,BRIDGE_BIT_COUNT_MAX)))"
				"\n\t\t\t\treturn NULL;"
				"\n\t\t\t");
			/* NOTE: _bridge_bits_bytes_get_bits(bb) may return zero on error (may be because of overflow) */
			if (!s->_marked && !s->unknown_packed_bits && ~0u - s->packed_bits >= s->packed_optional_bits) {
				FR2("if (((void)0,%uu) <= ~0u) {"
					"\n\t\t\t\tBRIDGE_ASSERT(packed_bits >= %uu);"
					"\n\t\t\t\tinfo.p.bit_count = bridge_nonzero_bit_array_size(packed_bits);"
					"\n\t\t\t}"
					"\n\t\t\telse"
					"\n\t\t\t\t", s->packed_bits + s->packed_optional_bits, s->packed_bits/*!=0*/);
			}
			FR1("info.p.bit_count = bridge_bit_array_size(packed_bits);"
				"\n\t\t}"
				"\n\t}"
				"\n\t{"
				"\n\t\tunsigned packed_size = %uu;"
				"\n\t\tif (_br_unlikely(!", s->packed_bytes);
			if (!s->_marked && szl)
				FR1("_br_maybe_of_add(&packed_size, info.p.bit_count, ((void)0,%uu) > ~0u)", szl);
			else
				FP("_br_of_add(&packed_size, info.p.bit_count)");
			FP("))"
				"\n\t\t\treturn NULL;"
				"\n\t\tif (");
			if (!s->_marked && szl)
				FR1("((void)0,%uu) > BRIDGE_MAX && ", szl);
			FP("_br_unlikely(packed_size > BRIDGE_MAX))"
				"\n\t\t\treturn NULL;"
				"\n\t\tin->pi.size = packed_size;"
				"\n\t}");
		}
		else if (v_bytes(s)) {
			/* variable bytes, fixed bits */
			if (!s->equivalent) {
				FR2("\n\t{"
					"\n\t\tunsigned packed_size = _%s_var_size(s, %uu);"
					"\n\t\tif (", s->s_name, initial_bytes);
				if (!s->_marked && szl)
					FR1("((void)0,%uu) > BRIDGE_MAX && ", szl);
				FP("_br_unlikely(packed_size > BRIDGE_MAX))"
					"\n\t\t\treturn NULL;"
					"\n\t\tin->pi.size = packed_size;"
					"\n\t}");
			}
			else if (s->equiv_many) {
				FR1("\n\t{"
					"\n\t\tunsigned packed_size = _bridge_%s_array_packed_size(&s->", s->equivalent);
				print_equivalent_path_s(file, s);
				FP(", &(s + 1)->");
				print_equivalent_path_s(file, s);
				FP(");"
					"\n\t\tif (");
				if (szl)
					FR1("((void)0,%uu) > BRIDGE_MAX && ", szl);
				FP("_br_unlikely(packed_size > BRIDGE_MAX))"
					"\n\t\t\treturn NULL;"
					"\n\t\tin->pi.size = packed_size;"
					"\n\t}");
			}
			else if (is_simple_str_type(s->equivalent)) {
				FR1("\n\t{"
					"\n\t\tunsigned packed_size = 0;"
					"\n\t\tif (_br_unlikely(_bridge_append_%s_packed_size(&packed_size, 0u, s->", s->equivalent);
				print_equivalent_path_s(file, s);
				FP(")) ||"
					"\n\t\t\t_br_unlikely(packed_size > BRIDGE_MAX))"
					"\n\t\t{"
					"\n\t\t\treturn NULL;"
					"\n\t\t}"
					"\n\t\tin->pi.size = packed_size;"
					"\n\t}");
			}
			else {
				FR1("\n\tin->pi.size = bridge_%s_packed_size(s->", s->equivalent);
				print_equivalent_path_s(file, s);
				FP(");");
			}
		}
		else {
			/* fixed bytes, fixed packed size */
			if (v_bits(s)) {
				/* bits are variable, but in fixed bytes - check for bits overflow (s->unknown_packed_bits == 0) */
				if (!s->_marked && ~0u - s->packed_bits >= s->packed_optional_bits)
					FR1("\n\tif (((void)0,%uu) > BRIDGE_BIT_COUNT_MAX) ", s->packed_bits + s->packed_optional_bits);
				else
					FP("\n\t");
				FR2("{"
					"\n\t\t_bridge_bits_bytes_t bb = _%s_var_size(s, %uu);"
					"\n\t\tif (", s->s_name, s->packed_bits);
				if (!s->_marked && ~0u - s->packed_bits >= s->packed_optional_bits)
					FR1("((void)0,%uu) > ~0u && ", s->packed_bits + s->packed_optional_bits);
				FP("_br_unlikely(_bridge_bits_bytes_get_bytes(bb)))"
					"\n\t\t\treturn NULL;"
					"\n\t\t{"
					"\n\t\t\tunsigned packed_bits = _bridge_bits_bytes_get_bits(bb);"
					"\n\t\t\tif (");
				if (s->_marked && ~0u - s->packed_bits >= s->packed_optional_bits)
					FR1("((void)0,%uu) > BRIDGE_BIT_COUNT_MAX && ", s->packed_bits + s->packed_optional_bits);
				FP("_br_unlikely(packed_bits > ((void)0,BRIDGE_BIT_COUNT_MAX)))"
					"\n\t\t\t\treturn NULL;"
					"\n\t\t}"
					"\n\t}");
			}
			FR1("\n\tin->pi.size = %uu;", initial_bytes);
		}
		FP("\n\t{"
			"\n\t\tvoid *a = in->pi.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(in->pi.ac), NULL, in->pi.size, BRIDGE_ALLOC_PACK);"
			"\n\t\tif (_br_unlikely(!a))"
			"\n\t\t\treturn NULL;");
		if (!s->equivalent) {
			if (!s_has_bits(s))
				FP("\n\t\tinfo.p.mem = (char*)a;");
			else if (need_pack_bits_counter(s))
				FP("\n\t\tinfo.p.bits = (unsigned char*)_bridge_pack_counter((char*)a, (bridge_counter_t)info.p.bit_count);");
			else
				FP("\n\t\tinfo.p.bits = (unsigned char*)a;");
			if (s_has_bytes(s)) {
				if (!bits_have_fixed_bytes(s))
					FP("\n\t\tinfo.p.mem = (char*)info.p.bits + info.p.bit_count;");
				else if (s_has_bits(s))
					FR1("\n\t\tinfo.p.mem = (char*)info.p.bits + %uu;", bytes_for_bits(s->packed_bits));
			}
			if (s_has_bits(s))
				FP("\n\t\tinfo.p.bit_count = 0u;");
			if (s->convertable)
				FP("\n\t\tinfo.p.convert = in->pi.convert;");
			if (s->s_layout < S_MONOLITH && !s_has_bits(s) && !v_bytes(s)) {
				/* if structure has padding bytes - then cannot just memcpy */
				FR1("\n\t\tif (((void)0,%uu) == sizeof(*s)", s->packed_bytes);
				if (s->convertable)
					FP(" && !info.p.convert");
				FP(") {"
					"\n\t\t\tBRIDGE_MEMCPY(info.p.mem, s, sizeof(*s));"
					"\n\t\t\treturn info.p.mem;"
					"\n\t\t}");
			}
			FP("\n\t\t");
			if (s->_processed)
				FP("if (_br_unlikely(!");
			FR1("_%s_append_packed(s, &info)", s->s_name);
			if (s->_processed) {
				FP(")) {"
					"\n\t\t\tif (in->pi.ac->free_cb)"
					"\n\t\t\t\tin->pi.ac->free_cb(BRIDGE_PASS_ALLOCATOR(in->pi.ac), a);"
					"\n\t\t\treturn NULL;"
					"\n\t\t}"
					"\n\t\treturn a;");
			}
			else {
				FP(";");
				if (!s_has_bytes(s))
					FP("\n\t\treturn info.p.bits;");
				else if (!s_fixed(s))
					FP("\n\t\treturn info.p.mem - in->pi.size;");
				else
					FR1("\n\t\treturn info.p.mem - %uu;", initial_bytes);
			}
		}
		else if (s->equiv_many) {
			if (!s_fixed(s)) {
				FR1("\n\t\t{"
					"\n\t\t\tchar *m = _bridge_pack_%s_array((char*)a, &s->", s->equivalent);
				print_equivalent_path_s(file, s);
				FP(", &(s + 1)->");
				print_equivalent_path_s(file, s);
				FP(");"
					"\n\t\t\t(void)m;"
					"\n\t\t\treturn a;"
					"\n\t\t}");
			}
			else {
				FP("\n\t\t");
				if (field_type_byte != s->equivalent) {
					FR1("if (in->pi.convert)"
						"\n\t\t\tput_exchange_%s_array(&s->", s->equivalent);
					print_equivalent_path_s(file, s);
					FP(", a, sizeof(*s)/sizeof(s->");
					print_equivalent_path_s(file, s);
					FP("));"
						"\n\t\telse"
						"\n\t\t\t");
				}
				FP("BRIDGE_MEMCPY(a, s, sizeof(*s));"
					"\n\t\treturn a;");
			}
		}
		else if (!s_fixed(s)) {
			FR1("\n\t\t{"
				"\n\t\t\tchar *m = _bridge_pack_%s((char*)a, s->", s->equivalent);
			print_equivalent_path_s(file, s);
			FP(");"
				"\n\t\t\t(void)m;"
				"\n\t\t\treturn a;"
				"\n\t\t}");
		}
		else {
			FR2("\n\t\t_bridge_put_%s_at((char*)a%s, s->",
				s->equivalent, field_type_byte == s->equivalent ? "" : ", in->pi.convert");
			print_equivalent_path_s(file, s);
			FP(");"
				"\n\t\treturn a;");
		}
	}
	FP("\n\t}"
		"\n}"
		"\n");
	generate_prototype_pack(file, s, decl);
	FR1("\n{"
		"\n\tvoid *ret = _%s_pack_(s, in BRIDGE_DEBUG_ARGS_PASS);"
		"\n\tbridge_allocator_dec_level(in->pi.ac);"
		"\n\treturn ret;"
		"\n}"
		"\n", s->s_name);
}

static inline int need_check_init_unpacked_return(const struct struct_def *s)
{
	return !s_fixed(s) || S_DYNAMIC == s->s_layout;
}

static const char *_get_init_unpacked_info(const struct struct_def *s)
{
	if (!s->equivalent && S_DYNAMIC == s->s_layout) {
		if (s_has_bits(s))
			return "_upk_bits_mem_conv_allocator_info";
		return "_upk_mem_conv_allocator_info";
	}
	if (!s->equivalent && s->convertable) {
		if (s_has_bits(s))
			return "_upk_bits_mem_conv_info";
		return "_upk_mem_conv_info";
	}
	if (s_has_bytes(s)) {
		if (s_has_bits(s))
			return "_upk_bits_mem_info";
		return "_upk_mem_info";
	}
	return "_upk_bits_info";
}

int generate_prototype_init_unpacked(FILE *file, const struct struct_def *s)
{
	if (!s->equivalent) {
		int nc = need_check_init_unpacked_return(s);
		FR7("\nA_Nonnull_all_args%s%s"
			"\nstatic %s _%s_init_unpacked(A_Out struct %s *s, A_Inout union %s *info%s)",
			nc ? " A_Check_return A_Success(return == 0) A_Ret_range(-1,0)" : "",
			s_has_bytes(s) ? "" : " A_Post_satisfies(info->u.bit_count > 0)",
			nc ? "int" : "void", s->s_name, s->s_name, _get_init_unpacked_info(s),
			S_DYNAMIC == s->s_layout ? " BRIDGE_DEBUG_ARGS_DECL" : "");
		return 1;
	}
	return 0;
}

static void call_init_unpacked_struct(FILE *file, const struct struct_def *s, const struct field_def *f, enum CALL_FROM from)
{
	FR1("_%s_init_unpacked(", f->type);
	print_call_from(file, f, from);
	if (S_DYNAMIC == f->user_type->s_layout) {
		FP("BRIDGE_PASS_ALLOCATOR_CONTAINER(info->u.ac, ");
		if (s_has_bits(f->user_type) != s_has_bits(s))
			FP("&info->_umca._mca))");
		else
			FP("info))");
	}
	else if (f->user_type->convertable) {
		if (s_has_bits(f->user_type)) {
			if (S_DYNAMIC == s->s_layout)
				FP("&info->_ubmc._bmc)");
			else
				FP("info)");
		}
		else if (S_DYNAMIC == s->s_layout || s_has_bits(s))
			FP("&info->_umc._mc)");
		else
			FP("info)");
	}
	else if (s_has_bytes(f->user_type)) {
		if (s_has_bits(f->user_type)) {
			if (S_DYNAMIC == s->s_layout || s->convertable)
				FP("&info->_ubm._bm)");
			else
				FP("info)");
		}
		else if (S_DYNAMIC == s->s_layout || s->convertable || s_has_bits(s))
			FP("&info->_um._m)");
		else
			FP("info)");
	}
	else if (S_DYNAMIC == s->s_layout || s->convertable || s_has_bytes(s))
		FP("&info->_ub._b)");
	else
		FP("info)");
	if (!need_check_init_unpacked_return(f->user_type))
		FP(";");
}

/* copy:
  2 - append condition and copy if resulting condition is true
  1 - copy if condition is true, don't copy otherwise
  0 - don't copy
 -1 - don't copy */
static const char *get_alloc_type_for_layout(enum struct_layout layout, int copy)
{
	switch (layout) {
		case S_STATIC1: return
			copy > 1 ? "? BRIDGE_ALLOC_REF_COPY_ALIGNED1" :
			copy > 0 ? "BRIDGE_ALLOC_REF_COPY_ALIGNED1" :
			"BRIDGE_ALLOC_REF_ALIGNED1";
		case S_STATIC2: return
			copy > 1 ? "&& !info->u.convert ? BRIDGE_ALLOC_REF_COPY_ALIGNED2" :
			copy > 0 ? "info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED2 : BRIDGE_ALLOC_REF_COPY_ALIGNED2" :
			"BRIDGE_ALLOC_REF_ALIGNED2";
		case S_STATIC4: return
			copy > 1 ? "&& !info->u.convert ? BRIDGE_ALLOC_REF_COPY_ALIGNED4" :
			copy > 0 ? "info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED4 : BRIDGE_ALLOC_REF_COPY_ALIGNED4" :
			"BRIDGE_ALLOC_REF_ALIGNED4";
		case S_STATIC8: return
			copy > 1 ? "&& !info->u.convert ? BRIDGE_ALLOC_REF_COPY_ALIGNED8" :
			copy > 0 ? "info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED8 : BRIDGE_ALLOC_REF_COPY_ALIGNED8" :
			"BRIDGE_ALLOC_REF_ALIGNED8";
		case S_MONOLITH: return copy < 0 ? "BRIDGE_ALLOC_MEM" : "?";
		case S_DYNAMIC:  return copy < 0 ? "BRIDGE_ALLOC_MEM" : "?";
		case S_INVALID1:
		case S_INVALID2:
		case S_INVALID3:
		case S_INVALID4:
		case S_INVALID5:
			break;
	}
	return "?";
}

static const char *get_alloc_type_for_simple_type(const char *f_type, int copy)
{
	if (field_type_byte   == f_type) return copy ? "BRIDGE_ALLOC_REF_COPY_ALIGNED1" : "BRIDGE_ALLOC_REF_ALIGNED1";
	if (field_type_short  == f_type) return copy ?
		"info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED2 : BRIDGE_ALLOC_REF_COPY_ALIGNED2" : "BRIDGE_ALLOC_REF_ALIGNED2";
	if (field_type_int    == f_type) return copy ?
		"info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED4 : BRIDGE_ALLOC_REF_COPY_ALIGNED4" : "BRIDGE_ALLOC_REF_ALIGNED4";
	if (field_type_float  == f_type) return copy ?
		"info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED4 : BRIDGE_ALLOC_REF_COPY_ALIGNED4" : "BRIDGE_ALLOC_REF_ALIGNED4";
	if (field_type_long   == f_type) return copy ?
		"info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED8 : BRIDGE_ALLOC_REF_COPY_ALIGNED8" : "BRIDGE_ALLOC_REF_ALIGNED8";
	if (field_type_double == f_type) return copy ?
		"info->u.convert ? BRIDGE_ALLOC_REF_ALIGNED8 : BRIDGE_ALLOC_REF_COPY_ALIGNED8" : "BRIDGE_ALLOC_REF_ALIGNED8";
	return "?";
}

static unsigned _simple_type_max_required_bytes(const char *type)
{
	return
		field_type_byte    == type ? 1u :
		field_type_short   == type ? 2u :
		field_type_int     == type ? 4u :
		field_type_long    == type ? 8u :
		field_type_float   == type ? 4u :
		field_type_double  == type ? 8u :
		field_type_pshort  == type ? 3u :
		field_type_ppshort == type ? 3u :
		field_type_pint    == type ? 5u :
		field_type_ppint   == type ? 5u :
		field_type_plong   == type ? 9u :
		field_type_pplong  == type ? 9u : 0u;
}

static struct field_def **_get_destroyable_field(struct field_def **f, struct field_def **end)
{
	for (; f < end; f++) {
		if (is_destroyable_field(*f))
			return f;
	}
	return NULL;
}

static struct field_def **_next_destroyable_field(struct field_def **f, struct field_def **end)
{
	return _get_destroyable_field(++f, end);
}

static void _print_destroyable_path(FILE *file, const struct field_def *d)
{
	for (;;) {
		fputs(d->name, file);
		if (!d->user_type || F_REQUIRED != d->f_power)
			return;
		fputc('.', file);
		{
			struct field_def **f = d->user_type->fields;
			while (!is_destroyable_field(*f))
				f++;
			d = *f;
		}
	}
}

#define FZ2(s,a1,a2)                                   do {(*need_z) |= 1 + !!ze[0]; FR2(s,a1,a2);} WHL
#define FZ4(s,a1,a2,a3,a4)                             do {(*need_z) |= 1 + !!ze[0]; FR4(s,a1,a2,a3,a4);} WHL
#define FZ5(s,a1,a2,a3,a4,a5)                          do {(*need_z) |= 1 + !!ze[0]; FR5(s,a1,a2,a3,a4,a5);} WHL
#define FZ6(s,a1,a2,a3,a4,a5,a6)                       do {(*need_z) |= 1 + !!ze[0]; FR6(s,a1,a2,a3,a4,a5,a6);} WHL
#define FZ7(s,a1,a2,a3,a4,a5,a6,a7)                    do {(*need_z) |= 1 + !!ze[0]; FR7(s,a1,a2,a3,a4,a5,a6,a7);} WHL
#define FZ8(s,a1,a2,a3,a4,a5,a6,a7,a8)                 do {(*need_z) |= 1 + !!ze[0]; FR8(s,a1,a2,a3,a4,a5,a6,a7,a8);} WHL
#define FZ9(s,a1,a2,a3,a4,a5,a6,a7,a8,a9)              do {(*need_z) |= 1 + !!ze[0]; FR9(s,a1,a2,a3,a4,a5,a6,a7,a8,a9);} WHL
#define FZ12(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3)    do {(*need_z) |= 1 + !!ze[0]; FR12(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3);} WHL
#define FZ13(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4) do {(*need_z) |= 1 + !!ze[0]; FR13(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4);} WHL

static void _print_gotoz_or_return(FILE *file, struct field_def **df, int *need_z, const char *offs, const char *ze/*"E"or""*/)
{
	if (df) {
		FZ2("\n%s\t\tGOTOZ%s(&s->", offs, ze);
		if (file)
			_print_destroyable_path(file, *df);
		FP(");");
	}
	else
		FR1("\n%s\t\treturn -1;", offs);
}

#define CHECKS_BYTES_CREDIT 1u
#define CHECKS_BITS_CREDIT  2u

static void _mark_structure(struct struct_def *s, unsigned bit)
{
	/* 01 -> 0101
	   10 -> 1010 */
	s->_aux |= (unsigned char)((bit & 3u)*5u);
}

static unsigned _check_limits_for_user_type(FILE *file, struct struct_def *s, struct field_def **f,
	int *need_z, unsigned *bytes_credit, unsigned *bits_credit, const char *blim, const char *offs, const char *ze)
{
	unsigned credit_anough = CHECKS_BYTES_CREDIT | CHECKS_BITS_CREDIT;
	{
		int print_fail = 0;
		struct field_def **df = NULL; /* initialize only to avoid bogus compiler warning */
		/* (*f)->user_type may assume that caller have already checked
		  for availability of some minimum of bytes or bits to unpack */
		if ((*bytes_credit) >= (*f)->user_type->packed_bytes) {
			(*bytes_credit) -= (*f)->user_type->packed_bytes;
			/* really caller may not check the minimums if (*f)->user_type doesn't rely on this check */
			if ((*f)->user_type->_aux & CHECKS_BYTES_CREDIT)
				_mark_structure(s, CHECKS_BYTES_CREDIT); /* caller must check bytes_credit */
		}
		else {
			(*bytes_credit) = 0;
			credit_anough &= ~CHECKS_BYTES_CREDIT;
			if ((*f)->user_type->_aux & CHECKS_BYTES_CREDIT) {
				FR2("\n%s\tif (_br_unlikely(!upk_check_mem_lim(info->u.mem, info->u.lim, %uu)))", offs, (*f)->user_type->packed_bytes);
				print_fail++;
			}
		}
		if ((*bits_credit) >= (*f)->user_type->packed_bits) {
			(*bits_credit) -= (*f)->user_type->packed_bits;
			/* really caller may not check the minimums if (*f)->user_type doesn't rely on this check */
			if ((*f)->user_type->_aux & CHECKS_BITS_CREDIT)
				_mark_structure(s, CHECKS_BITS_CREDIT); /* caller must check bits_credit */
		}
		else {
			(*bits_credit) = 0;
			credit_anough &= ~CHECKS_BITS_CREDIT;
			if ((*f)->user_type->_aux & CHECKS_BITS_CREDIT) {
				if (print_fail) {
					df = _get_destroyable_field(f, s->fields_end);
					_print_gotoz_or_return(file, df, need_z, offs, ze);
				}
				FR3("\n%s\tif (_br_unlikely(!upk_check_bits_lim_nz(info->u.bits, info->u.%s, info->u.bit_count, %uu)))",
					offs, blim, (*f)->user_type->packed_bits);
				print_fail++;
			}
		}
		if (print_fail) {
			if (1 == print_fail)
				df = _get_destroyable_field(f, s->fields_end);
			_print_gotoz_or_return(file, df, need_z, offs, ze);
		}
	}
	/* (*f)->user_type may consume all checked credit of available bytes or bits to unpack */
	if ((*f)->user_type->unknown_packed_bytes || (*f)->user_type->packed_optional_bytes > (*bytes_credit)) {
		(*bytes_credit) = 0;
		credit_anough &= ~CHECKS_BYTES_CREDIT;
	}
	else
		(*bytes_credit) -= (*f)->user_type->packed_optional_bytes;
	if ((*f)->user_type->unknown_packed_bits || (*f)->user_type->packed_optional_bits > (*bits_credit)) {
		(*bits_credit) = 0;
		credit_anough &= ~CHECKS_BITS_CREDIT;
	}
	else
		(*bits_credit) -= (*f)->user_type->packed_optional_bits;
	return credit_anough;
}

static void _unpack_simple_int_type(FILE *file, struct struct_def *s, struct field_def **f,
	int *need_z, unsigned *bytes_credit, const char *offs, const char *ze)
{
	int bytes_credit_anough = 1;
	unsigned max_needed_bytes = _simple_type_max_required_bytes((*f)->type);
	if ((*bytes_credit) >= max_needed_bytes) {
		(*bytes_credit) -= max_needed_bytes;
		_mark_structure(s, CHECKS_BYTES_CREDIT); /* caller must check bytes_credit */
	}
	else {
		(*bytes_credit) = 0;
		bytes_credit_anough = 0;
	}
	if (is_variabe_int_type((*f)->type)) {
		FR5("\n%s\tinfo->u.mem = _bridge_unpack_%s%s(info->u.mem%s, &s->%s);",
			offs, (*f)->type, bytes_credit_anough ? "_no_lim" : "", bytes_credit_anough ? "" : ", info->u.lim", (*f)->name);
		if (!bytes_credit_anough)
			FR1("\n%s\tif (_br_unlikely(!info->u.mem))", offs);
	}
	else if (!bytes_credit_anough) {
		FR3("\n%s\tif (_br_unlikely(!upk_check_mem_lim(info->u.mem, info->u.lim, _br_sizeof(s->%s, %uu))))",
			offs, (*f)->name, simple_fixed_int_size((*f)->type));
	}
	if (!bytes_credit_anough)
		_print_gotoz_or_return(file, _next_destroyable_field(f, s->fields_end), need_z, offs, ze);
	if (!is_variabe_int_type((*f)->type)) {
		FR4("\n%s\ts->%s = _bridge_get_%s_no_lim(&info->u.mem%s);",
			offs, (*f)->name, (*f)->type, field_type_byte == (*f)->type ? "" : ", info->u.convert");
	}
}

static void print_checked_credits(FILE *file/*NULL?*/, const struct struct_def *s, unsigned bytes_credit, unsigned bits_credit)
{
	if (!(s->_aux & CHECKS_BYTES_CREDIT))
		bytes_credit = 0;
	if (!(s->_aux & CHECKS_BITS_CREDIT))
		bits_credit = 0;
	if (bytes_credit && bits_credit)
		FR2(" (byte_credit: %u, bit_credit: %u) */", bytes_credit, bits_credit);
	else if (bytes_credit)
		FR1(" (byte_credit: %u) */", bytes_credit);
	else if (bits_credit)
		FR1(" (bits_credit: %u) */", bits_credit);
	else
		FP(" */");
}

static void switch_init_unpacked(FILE *file/*NULL if determining marks*/, struct field_def **f, struct struct_def *r,
	int *need_z, unsigned *bytes_credit, unsigned *bits_credit, const char *blim, const char *offs, const char *ze)
{
	switch ((*f)->f_power) {
		case F_ARRAY:
			FR3("\n%s\t/* %s: %s[]", offs, (*f)->name, (*f)->type);
			print_checked_credits(file, r, *bytes_credit, *bits_credit);
			if ((*f)->user_type) {
				FR2("\n%s\t{"
					"\n%s\t\tINT32_TYPE count;", offs, offs);
				if ((*bytes_credit) >= BRIDGE_MAX_COUNTER_PACKED_SIZE) {
					_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
					FR1("\n%s\t\tinfo->u.mem = _bridge_unpack_counter_no_lim(info->u.mem, &count);", offs);
					if (s_has_bytes((*f)->user_type))
						(*bytes_credit) = 0;
					else
						(*bytes_credit) -= BRIDGE_MAX_COUNTER_PACKED_SIZE;
				}
				else {
					FZ5("\n%s\t\tinfo->u.mem = _bridge_unpack_counter(info->u.mem, info->u.lim, &count);"
						"\n%s\t\tif (_br_unlikely(!info->u.mem))"
						"\n%s\t\t\tGOTOZ%s(&s->%s);", offs, offs, offs, ze, (*f)->name);
					(*bytes_credit) = 0;
				}
				if (s_has_bits((*f)->user_type))
					(*bits_credit) = 0;
				FR1("\n%s\t\tif (count) {", offs);
				/* if (*f)->user_type assumes that caller have done some checks - try to do some of them outside the loop */
				if ((*f)->user_type->packed_bytes && !v_bytes((*f)->user_type)) {
					FZ6("\n%s\t\t\tif (_br_unlikely(!_bridge_check_fixed_array_counter(info->u.mem, info->u.lim,"
						" sizeof(*s->%s), count, %uu)))"
						"\n%s\t\t\t\tGOTOZ%s(&s->%s);", offs, (*f)->name, (*f)->user_type->packed_bytes/*!=0*/, offs, ze, (*f)->name);
				}
				else {
					FZ6("\n%s\t\t\tif (_br_unlikely(!_bridge_check_array_counter(sizeof(*s->%s), count, %uu)))"
						"\n%s\t\t\t\tGOTOZ%s(&s->%s);",
						offs, (*f)->name, (*f)->user_type->packed_bytes ? (*f)->user_type->packed_bytes : 1u, offs, ze, (*f)->name);
				}
				if ((*f)->user_type->packed_bits && !v_bits((*f)->user_type)) {
					FZ9("\n%s\t\t\tif (_br_unlikely(!upk_check_bits_lim_n(info->u.bits, info->u.%s,"
						"\n%s\t\t\t\tinfo->u.bit_count, %uu, (bridge_counter_t)(unsigned INT32_TYPE)count)))"
						"\n%s\t\t\t{"
						"\n%s\t\t\t\tGOTOZ%s(&s->%s);"
						"\n%s\t\t\t}", offs, blim, offs, (*f)->user_type->packed_bits, offs, offs, ze, (*f)->name, offs);
				}
				FR2("\n%s\t\t\t{"
					"\n%s\t\t\t\tbridge_counter_t c = (bridge_counter_t)(unsigned INT32_TYPE)count;", offs, offs);
				if (!(*f)->user_type->equivalent) {
					if ((*f)->user_type->s_layout < S_MONOLITH && !s_has_bits((*f)->user_type) && !v_bytes((*f)->user_type)) {
						/* if structure has padding bytes - then cannot just memcpy */
						FR10("\n%s\t\t\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\t\t\t((void)0,%uu) == sizeof(*s->%s) ? info->u.mem : NULL, sizeof(*s->%s)*c,"
							"\n%s\t\t\t\t\t((void)0,%uu) == sizeof(*s->%s) %s : %s);",
							offs, offs, (*f)->user_type->packed_bytes, (*f)->name, (*f)->name,
							offs, (*f)->user_type->packed_bytes, (*f)->name,
							get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/2),
							get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/0));
					}
					else {
						FR4("\n%s\t\t\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\t\t\tNULL, sizeof(*s->%s)*c, %s);",
							offs, offs, (*f)->name, get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/-1));
					}
					FZ9("\n%s\t\t\t\tif (_br_unlikely(!a))"
						"\n%s\t\t\t\t\tGOTOZ%s(&s->%s);"
						"\n%s\t\t\t\ts->%s_end = (s->%s = (struct %s*)a) + c;"
						"\n%s\t\t\t\t", offs, offs, ze, (*f)->name, offs, (*f)->name, (*f)->name, (*f)->type, offs);
					if ((*f)->user_type->s_layout < S_MONOLITH && !s_has_bits((*f)->user_type) && !v_bytes((*f)->user_type)) {
						FR2("if (((void)0,%uu) == sizeof(*s->%s)", (*f)->user_type->packed_bytes, (*f)->name);
						if ((*f)->user_type->convertable)
							FP(" && !info->u.convert");
						FR3(")"
							"\n%s\t\t\t\t\tinfo->u.mem += sizeof(*s->%s)*c;"
							"\n%s\t\t\t\telse ", offs, (*f)->name, offs);
					}
					FR5("{"
						"\n%s\t\t\t\t\tstruct %s *i = s->%s;"
						"\n%s\t\t\t\t\tdo {"
						"\n%s\t\t\t\t\t\t", offs, (*f)->type, (*f)->name, offs, offs);
					{
						/* if (*f)->user_type assumes that caller have done some checks - do remaining checks inside the loop */
						int check_bytes  = v_bytes((*f)->user_type) && ((*f)->user_type->_aux & CHECKS_BYTES_CREDIT);
						int check_bits   = v_bits((*f)->user_type) && ((*f)->user_type->_aux & CHECKS_BITS_CREDIT);
						int check_return = need_check_init_unpacked_return((*f)->user_type);
						if (check_bytes || check_bits || check_return)
							FP("if (_br_unlikely(");
						if (check_bytes) {
							FR1("!upk_check_mem_lim(info->u.mem, info->u.lim, %uu)", (*f)->user_type->packed_bytes);
							if (check_bits || check_return) {
								FR1(") ||"
									"\n%s\t\t\t\t\t\t\t_br_unlikely(", offs);
							}
						}
    					if (check_bits) {
							FR2("!upk_check_bits_lim_nz(info->u.bits, info->u.%s, info->u.bit_count, %uu)",
								blim, (*f)->user_type->packed_bits/*!=0 because CHECKS_BITS_CREDIT is set*/);
							if (check_return) {
								FR1(") ||"
									"\n%s\t\t\t\t\t\t\t_br_unlikely(", offs);
							}
						}
						if (check_return)
							call_init_unpacked_struct(file, r, *f, CALL_FROM_ARRAY_INC);
						if (check_bytes || check_bits || check_return) {
							int b_opened = 0;
							if (S_DYNAMIC == (*f)->user_type->s_layout) {
								b_opened = 1;
								if (check_bytes + check_bits + check_return > 1) {
									FR1("))"
										"\n%s\t\t\t\t\t\t{", offs);
								}
								else
									FP(")) {");
								FR2("\n%s\t\t\t\t\t\t\ts->%s_end = i;", offs, (*f)->name);
							}
							else
								FP("))");
							_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end),
								need_z, offs[0] ? offs[1] ? "\t\t\t\t\t\t\t" : "\t\t\t\t\t\t" : "\t\t\t\t\t", ze);
							if (b_opened)
								FR1("\n%s\t\t\t\t\t\t}", offs);
						}
						if (!check_return)
							call_init_unpacked_struct(file, r, *f, CALL_FROM_ARRAY_INC);
					}
					FR3("\n%s\t\t\t\t\t} while (i < s->%s_end);"
						"\n%s\t\t\t\t}", offs, (*f)->name, offs);
				}
				else if (is_simple_str_type((*f)->user_type->equivalent)) {
					/* NOTE: may cast 'sizeof(*s->%s)/sizeof(char*)*c' to bridge_counter_t
					  - _bridge_check_array_counter() checked that */
					FR10("\n%s\t\t\t\tvoid *a = &s->%s, *e = &s->%s_end;"
						"\n%s\t\t\t\tinfo->u.mem = _bridge_unpack_%s_array_domains(info->u.mem, info->u.lim,"
						"\n%s\t\t\t\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof(char*)*c), (char***)a, (char***)e,"
						"\n%s\t\t\t\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof(char*)), BRIDGE_PASS_ALLOCATOR(info->u.ac));"
						"\n%s\t\t\t\tif (_br_unlikely(!info->u.mem))",
						offs, (*f)->name, (*f)->name, offs, (*f)->user_type->equivalent, offs, (*f)->name, offs, (*f)->name, offs);
					_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end),
						need_z, offs[0] ? offs[1] ? "\t\t\t\t\t" : "\t\t\t\t" : "\t\t\t", ze);
				}
				else if (!s_fixed((*f)->user_type)) {
					const char *native_type = get_native_type((*f)->user_type->equivalent);
					/* NOTE: may cast 'sizeof(*s->%s)/sizeof(%s)*c' to bridge_counter_t
					  - _bridge_check_array_counter() checked that */
					FR10("\n%s\t\t\t\tvoid *a = &s->%s;"
						"\n%s\t\t\t\tinfo->u.mem = _bridge_unpack_%s_array(info->u.mem, info->u.lim,"
						"\n%s\t\t\t\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof(%s)*c),"
						"\n%s\t\t\t\t\t(%s**)a, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
						"\n%s\t\t\t\tif (_br_unlikely(!info->u.mem))",
						offs, (*f)->name, offs, (*f)->user_type->equivalent, offs, (*f)->name, native_type, offs, native_type, offs);
					_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end),
						need_z, offs[0] ? offs[1] ? "\t\t\t\t\t" : "\t\t\t\t" : "\t\t\t", ze);
					FR3("\n%s\t\t\t\ts->%s_end = s->%s + c;", offs, (*f)->name, (*f)->name);
				}
				else {
					FZ13("\n%s\t\t\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
						"\n%s\t\t\t\t\tinfo->u.mem, sizeof(*s->%s)*c,%s%s);"
						"\n%s\t\t\t\tif (_br_unlikely(!a))"
						"\n%s\t\t\t\t\tGOTOZ%s(&s->%s);"
						"\n%s\t\t\t\ts->%s_end = (s->%s = (struct %s*)a) + c;",
						offs, offs, (*f)->name, S_STATIC1 == (*f)->user_type->s_layout ? " " : "\n\t\t\t\t\t",
						get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/1),
						offs, offs, ze, (*f)->name, offs, (*f)->name, (*f)->name, (*f)->type);
					if (field_type_byte == (*f)->user_type->equivalent)
						FR2("\n%s\t\t\t\tinfo->u.mem += sizeof(*s->%s)*c;", offs, (*f)->name);
					else {
						FR8("\n%s\t\t\t\t{"
							"\n%s\t\t\t\t\tconst char *m = info->u.mem;"
							"\n%s\t\t\t\t\tinfo->u.mem = m + sizeof(*s->%s)*c;"
							"\n%s\t\t\t\t\tif (info->u.convert)"
							"\n%s\t\t\t\t\t\tget_exchange_%s_array(m, &s->%s->",
							offs, offs, offs, (*f)->name, offs, offs, (*f)->user_type->equivalent, (*f)->name);
						if (file)
							print_equivalent_path(file, *f);
						FR2(", sizeof(*s->%s)/sizeof(s->%s->", (*f)->name, (*f)->name);
						if (file)
							print_equivalent_path(file, *f);
						FR1(")*c);"
							"\n%s\t\t\t\t}", offs);
					}
				}
				FR7("\n%s\t\t\t}"
					"\n%s\t\t}"
					"\n%s\t\telse"
					"\n%s\t\t\ts->%s = s->%s_end = NULL;"
					"\n%s\t}", offs, offs, offs, offs, (*f)->name, (*f)->name, offs);
			}
			else if (field_type_bit == (*f)->type) {
				(*bytes_credit) = 0;
				FZ8("\n%s\tinfo->u.mem = bridge_unpack_bit_array(info->u.mem, info->u.lim,"
					"\n%s\t\t&s->%s_bit_count, &s->%s, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
					"\n%s\tif (_br_unlikely(!info->u.mem))"
					"\n%s\t\tGOTOZ%s(&s->%s);", offs, offs, (*f)->name, (*f)->name, offs, offs, ze, (*f)->name);
			}
			else if (is_variable_simple_type((*f)->type)) {
				(*bytes_credit) = 0;
				FR5("\n%s\tinfo->u.mem = bridge_unpack_%s_array(info->u.mem, info->u.lim,"
					" &s->%s, &s->%s_end, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
					"\n%s\tif (_br_unlikely(!info->u.mem))", offs, (*f)->type, (*f)->name, (*f)->name, offs);
				_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end), need_z, offs, ze);
			}
			else {
				FR2("\n%s\t{"
					"\n%s\t\tINT32_TYPE count;", offs, offs);
				if ((*bytes_credit) >= BRIDGE_MAX_COUNTER_PACKED_SIZE) {
					_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
					FR1("\n%s\t\tinfo->u.mem = _bridge_unpack_counter_no_lim(info->u.mem, &count);", offs);
				}
				else {
					FZ5("\n%s\t\tinfo->u.mem = _bridge_unpack_counter(info->u.mem, info->u.lim, &count);"
						"\n%s\t\tif (_br_unlikely(!info->u.mem))"
						"\n%s\t\t\tGOTOZ%s(&s->%s);", offs, offs, offs, ze, (*f)->name);
				}
				(*bytes_credit) = 0;
				FZ9("\n%s\t\tif (count) {"
					"\n%s\t\t\tif (_br_unlikely(!upk_check_mem_lim_n(info->u.mem, info->u.lim, _br_sizeof(*s->%s, %uu), count)))"
					"\n%s\t\t\t\tGOTOZ%s(&s->%s);"
					"\n%s\t\t\t{"
					"\n%s\t\t\t\tbridge_counter_t c = (bridge_counter_t)(unsigned INT32_TYPE)count;",
					offs, offs, (*f)->name, simple_fixed_int_size((*f)->type), offs, ze, (*f)->name, offs, offs);
				FZ12("\n%s\t\t\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
					"\n%s\t\t\t\t\tinfo->u.mem, sizeof(*s->%s)*c, %s);"
					"\n%s\t\t\t\tif (_br_unlikely(!a))"
					"\n%s\t\t\t\t\tGOTOZ%s(&s->%s);"
					"\n%s\t\t\t\ts->%s_end = (s->%s = (%s*)a) + c;",
					offs, offs, (*f)->name, get_alloc_type_for_simple_type((*f)->type, /*copy:*/1),
					offs, offs, ze, (*f)->name, offs, (*f)->name, (*f)->name, get_native_type((*f)->type));
				if (field_type_byte == (*f)->type)
					FR2("\n%s\t\t\t\tinfo->u.mem += sizeof(*s->%s)*c;", offs, (*f)->name);
				else {
					FR9("\n%s\t\t\t\t{"
						"\n%s\t\t\t\t\tconst char *m = info->u.mem;"
						"\n%s\t\t\t\t\tinfo->u.mem = m + sizeof(*s->%s)*c;"
						"\n%s\t\t\t\t\tif (info->u.convert)"
						"\n%s\t\t\t\t\t\tget_exchange_%s_array(m, s->%s, c);"
						"\n%s\t\t\t\t}", offs, offs, offs, (*f)->name, offs, offs, (*f)->type, (*f)->name, offs);
				}
				FR7("\n%s\t\t\t}"
					"\n%s\t\t}"
					"\n%s\t\telse"
					"\n%s\t\t\ts->%s = s->%s_end = NULL;"
					"\n%s\t}", offs, offs, offs, offs, (*f)->name, (*f)->name, offs);
			}
			break;
		case F_REQUIRED:
			FR3("\n%s\t/* %s: %s", offs, (*f)->name, (*f)->type);
			print_checked_credits(file, r, *bytes_credit, *bits_credit);
			if ((*f)->user_type) {
				unsigned credit_anough = _check_limits_for_user_type(file, r, f, need_z, bytes_credit, bits_credit, blim, offs, ze);
				if (!(*f)->user_type->equivalent) {
					if ((*f)->user_type->s_layout < S_MONOLITH && !s_has_bits((*f)->user_type) && !v_bytes((*f)->user_type)) {
						/* if structure has padding bytes - then cannot just memcpy */
						FR3("\n%s\tif (((void)0,%uu) == sizeof(s->%s)", offs, (*f)->user_type->packed_bytes, (*f)->name);
						if ((*f)->user_type->convertable)
							FP(" && !info->u.convert");
						FR9(") {"
							"\n%s\t\tconst char *m = info->u.mem;"
							"\n%s\t\tinfo->u.mem = m + sizeof(s->%s);"
							"\n%s\t\tBRIDGE_MEMCPY(&s->%s, m, sizeof(s->%s));"
							"\n%s\t}"
							"\n%s\telse"
							"\n%s\t\t", offs, offs, (*f)->name, offs, (*f)->name, (*f)->name, offs, offs, offs);
					}
					else
						FR1("\n%s\t", offs);
					{
						int check_return = need_check_init_unpacked_return((*f)->user_type);
						if (check_return) {
							if ((CHECKS_BYTES_CREDIT | CHECKS_BITS_CREDIT) == credit_anough &&
								S_DYNAMIC != (*f)->user_type->s_layout)
							{
								check_return = 2; /* assume return is always successfull */
								/* ensure that (*f)->user_type will unpack successfully,
								  even if (*f)->user_type also checks the limits */
								if (s_has_bytes((*f)->user_type))
									_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
								if (s_has_bits((*f)->user_type))
									_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
								FP("_br_assume(");
							}
							else
								FP("if (_br_unlikely(");
						}
						call_init_unpacked_struct(file, r, *f, CALL_FROM_STATIC);
						if (2 == check_return)
							FP(");");
						else if (check_return) {
							FP("))");
							_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end), need_z, offs, ze);
						}
					}
				}
				else if ((*f)->user_type->equiv_many && is_simple_str_type((*f)->user_type->equivalent)) {
					/* NOTE: may cast 'sizeof(s->%s)/sizeof(char*)' to bridge_counter_t
					  - structure 's' packed size was statically checked in code generated by generate_code_unpack() */
					FR7("\n%s\t{"
						"\n%s\t\tchar **ae;"
						"\n%s\t\tinfo->u.mem = _bridge_unpack_%s_array_(info->u.mem, info->u.lim,"
						"\n%s\t\t\t(bridge_counter_t)(sizeof(s->%s)/sizeof(s->%s.",
						offs, offs, offs, (*f)->user_type->equivalent, offs, (*f)->name, (*f)->name);
					if (file)
						print_equivalent_path(file, *f);
					FR1(")), &s->%s.", (*f)->name);
					if (file)
						print_equivalent_path(file, *f);
					FZ4(", &ae, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
						"\n%s\t\tif (_br_unlikely(!info->u.mem))"
						"\n%s\t\t\tGOTOZ%s(ae);"
						"\n%s\t}", offs, offs, ze, offs);
				}
				else if ((*f)->user_type->equiv_many) {
					if (!s_fixed((*f)->user_type)) {
						/* NOTE: may cast 'sizeof(s->%s)/sizeof(%s)' to bridge_counter_t
						  - structure 's' packed size was statically checked in code generated by generate_code_unpack() */
						FR7("\n%s\tinfo->u.mem = _bridge_unpack_%s_array_%s(info->u.mem%s,"
							"\n%s\t\t(bridge_counter_t)(sizeof(s->%s)/sizeof(s->%s.",
							offs, (*f)->user_type->equivalent,
							(CHECKS_BYTES_CREDIT & credit_anough) ? "no_lim" : "",
							(CHECKS_BYTES_CREDIT & credit_anough) ? "" : ", info->u.lim",
							offs, (*f)->name, (*f)->name);
						if (file)
							print_equivalent_path(file, *f);
						FR1(")), &s->%s.", (*f)->name);
						if (file)
							print_equivalent_path(file, *f);
						FP(");");
						if (CHECKS_BYTES_CREDIT & credit_anough)
							_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
						else {
							FR1("\n%s\tif (_br_unlikely(!info->u.mem))", offs);
							_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end), need_z, offs, ze);
						}
					}
					else {
						FR5("\n%s\t{"
							"\n%s\t\tconst char *m = info->u.mem;"
							"\n%s\t\tinfo->u.mem = m + sizeof(s->%s);"
							"\n%s\t\t", offs, offs, offs, (*f)->name, offs);
						if (field_type_byte != (*f)->user_type->equivalent) {
							FR3("if (info->u.convert)"
								"\n%s\t\t\tget_exchange_%s_array(m, &s->%s.", offs, (*f)->user_type->equivalent, (*f)->name);
							if (file)
								print_equivalent_path(file, *f);
							FR2(", sizeof(s->%s)/sizeof(s->%s.", (*f)->name, (*f)->name);
							if (file)
								print_equivalent_path(file, *f);
							FR2("));"
								"\n%s\t\telse"
								"\n%s\t\t\t", offs, offs);
						}
						FR3("BRIDGE_MEMCPY(&s->%s, m, sizeof(s->%s));"
							"\n%s\t}", (*f)->name, (*f)->name, offs);
					}
				}
				else if (is_simple_str_type((*f)->user_type->equivalent)) {
					FR3("\n%s\tinfo->u.mem = _bridge_unpack_%s_(info->u.mem, info->u.lim, &s->%s.",
						offs, (*f)->user_type->equivalent, (*f)->name);
					if (file)
						print_equivalent_path(file, *f);
					FZ4(", BRIDGE_PASS_ALLOCATOR(info->u.ac));"
						"\n%s\tif (_br_unlikely(!info->u.mem))"
						"\n%s\t\tGOTOZ%s(&s->%s);", offs, offs, ze, (*f)->name);
				}
				else if (!s_fixed((*f)->user_type)) {
					FR5("\n%s\tinfo->u.mem = _bridge_unpack_%s%s(info->u.mem%s, &s->%s.",
						offs, (*f)->user_type->equivalent,
						(CHECKS_BYTES_CREDIT & credit_anough) ? "_no_lim" : "",
						(CHECKS_BYTES_CREDIT & credit_anough) ? "" : ", info->u.lim",
						(*f)->name);
					if (file)
						print_equivalent_path(file, *f);
					FP(");");
					if (CHECKS_BYTES_CREDIT & credit_anough)
						_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
					else {
						FR1("\n%s\tif (_br_unlikely(!info->u.mem))", offs);
						_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end), need_z, offs, ze);
					}
				}
				else {
					FR2("\n%s\ts->%s.", offs, (*f)->name);
					if (file)
						print_equivalent_path(file, *f);
					FR2(" = _bridge_get_%s_no_lim(&info->u.mem%s);",
						(*f)->user_type->equivalent, field_type_byte == (*f)->user_type->equivalent ? "" : ", info->u.convert");
				}
			}
			else
				goto _unpack_simple_type;
			break;
		case F_OPTIONAL:
		case F_POINTER:
			if (F_POINTER != (*f)->f_power || (*f)->user_type || field_type_bit != (*f)->type) {
				FR4("\n%s\t/* %s: %s%s", offs, (*f)->name, (*f)->type, (F_OPTIONAL == (*f)->f_power) ? "*" : "[1]");
				print_checked_credits(file, r, *bytes_credit, *bits_credit);
				if (F_OPTIONAL == (*f)->f_power) {
					if (*bits_credit) {
						(*bits_credit)--;
						_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
					}
					else {
						FR2("\n%s\tif (_br_unlikely(!upk_check_bits_lim_nz(info->u.bits, info->u.%s, info->u.bit_count, 1u)))", offs, blim);
    					_print_gotoz_or_return(file, _get_destroyable_field(f, r->fields_end), need_z, offs, ze);
					}
					FR1("\n%s\tif (bridge_get_bit(info->u.bits, info->u.bit_count++)) {", offs);
				}
				if ((*f)->user_type) {
					const char *of = (F_OPTIONAL == (*f)->f_power) ? offs[0] ? offs[1] ? "\t\t\t" : "\t\t" : "\t" : offs;
					unsigned credit_anough = _check_limits_for_user_type(file, r, f, need_z, bytes_credit, bits_credit, blim, of, ze);
					if (!(*f)->user_type->equivalent) {
						if ((*f)->user_type->s_layout < S_MONOLITH && !s_has_bits((*f)->user_type) && !v_bytes((*f)->user_type)) {
							/* if structure has padding bytes - then cannot just memcpy */
							FR6("\n%s\t{"
								"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
								"\n%s\t\t\t((void)0,%uu) == sizeof(*s->%s) ? info->u.mem : NULL, sizeof(*s->%s),",
								of, of, of, (*f)->user_type->packed_bytes, (*f)->name, (*f)->name);
							if ((*f)->user_type->packed_bytes > BR_MEMCPY_MAX_SIZE) {
								FR4("\n%s\t\t\t((void)0,%uu) == sizeof(*s->%s) %s :",
									of, (*f)->user_type->packed_bytes, (*f)->name,
									get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/2));
							}
							FR1(" %s);", get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/0));
						}
						else {
							FR5("\n%s\t{"
								"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
								"\n%s\t\t\tNULL, sizeof(*s->%s), %s);",
								of, of, of, (*f)->name, get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/-1));
						}
						if (_next_destroyable_field(f, r->fields_end)) {
							FZ7("\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\tGOTOZ%s(&s->%s);"
								"\n%s\t\ts->%s = (struct %s*)a;", of, of, ze, (*f)->name, of, (*f)->name, (*f)->type);
						}
						else {
							FR5("\n%s\t\ts->%s = (struct %s*)a;"
								"\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\treturn -1;", of, (*f)->name, (*f)->type, of, of);
						}
						if ((*f)->user_type->s_layout < S_MONOLITH && !s_has_bits((*f)->user_type) && !v_bytes((*f)->user_type)) {
							FR3("\n%s\t\tif (((void)0,%uu) == sizeof(*s->%s)", of, (*f)->user_type->packed_bytes, (*f)->name);
							if ((*f)->user_type->convertable)
								FP(" && !info->u.convert");
							if ((*f)->user_type->packed_bytes > BR_MEMCPY_MAX_SIZE) {
								FR4(")"
									"\n%s\t\t\tinfo->u.mem += sizeof(*s->%s);"
									"\n%s\t\telse"
									"\n%s\t\t\t", of, (*f)->name, of, of);
							}
							else {
								FR9(") {"
									"\n%s\t\t\tconst char *m = info->u.mem;"
									"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
									"\n%s\t\t\tif (m != (const char*)a)"
									"\n%s\t\t\t\t_br_memcpy%u(a, m);"
									"\n%s\t\t}"
									"\n%s\t\telse"
									"\n%s\t\t\t", of, of, (*f)->name, of, of, (*f)->user_type->packed_bytes, of, of, of);
							}
						}
						else
							FR1("\n%s\t\t", of);
						{
							int check_return = need_check_init_unpacked_return((*f)->user_type);
							if (check_return) {
								if ((CHECKS_BYTES_CREDIT | CHECKS_BITS_CREDIT) == credit_anough &&
									S_DYNAMIC != (*f)->user_type->s_layout)
								{
									check_return = 2; /* assume return is always successfull */
									/* ensure that (*f)->user_type will unpack successfully,
									  even if (*f)->user_type also checks the limits */
									if (s_has_bytes((*f)->user_type))
										_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
									if (s_has_bits((*f)->user_type))
										_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
									FP("_br_assume(");
								}
								else
									FP("if (_br_unlikely(");
							}
							call_init_unpacked_struct(file, r, *f, CALL_FROM_OPTIONAL);
							if (2 == check_return)
								FP(");");
							else if (check_return) {
								FP("))");
								_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end),
									need_z, of[0] ? of[1] ? "\t\t\t" : "\t\t" : "\t", ze);
							}
						}
						FR1("\n%s\t}", of);
					}
					else if ((*f)->user_type->equiv_many) {
						if (!s_fixed((*f)->user_type)) {
							const char *native_type = get_native_type((*f)->user_type->equivalent);
							/* NOTE: may cast 'sizeof(*s->%s)/sizeof(%s)' to bridge_counter_t
							  - structure (*f)->type packed size was statically checked
							  in code generated by generate_code_unpack() */
							FR12("\n%s\t{"
								"\n%s\t\tvoid *p = &s->%s;"
								"\n%s\t\tinfo->u.mem = _bridge_unpack_%s_array(info->u.mem, info->u.lim,"
								"\n%s\t\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof(%s)),"
								"\n%s\t\t\t(%s**)p, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
								"\n%s\t}"
								"\n%s\tif (_br_unlikely(!info->u.mem))",
								of, of, (*f)->name, of, (*f)->user_type->equivalent, of, (*f)->name, native_type, of, native_type, of, of);
							_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end), need_z, of, ze);
						}
						else {
							FR4("\n%s\t{"
								"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
								"\n%s\t\t\tinfo->u.mem, sizeof(*s->%s),", of, of, of, (*f)->name);
							if ((*f)->user_type->packed_bytes > BR_MEMCPY_MAX_SIZE) {
								if (S_STATIC1 == (*f)->user_type->s_layout)
									FP(" ");
								else
									FR1("\n%s\t\t\t", of);
								FR1("%s);", get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/1));
							}
							else
								FR1(" %s);", get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/0));
							if (_next_destroyable_field(f, r->fields_end)) {
								FZ7("\n%s\t\tif (_br_unlikely(!a))"
									"\n%s\t\t\tGOTOZ%s(&s->%s);"
									"\n%s\t\ts->%s = (struct %s*)a;", of, of, ze, (*f)->name, of, (*f)->name, (*f)->type);
							}
							else {
								FR5("\n%s\t\ts->%s = (struct %s*)a;"
									"\n%s\t\tif (_br_unlikely(!a))"
									"\n%s\t\t\treturn -1;", of, (*f)->name, (*f)->type, of, of);
							}
							if ((*f)->user_type->packed_bytes > BR_MEMCPY_MAX_SIZE) {
								if (field_type_byte == (*f)->user_type->equivalent)
									FR2("\n%s\t\tinfo->u.mem += sizeof(*s->%s);", of, (*f)->name);
								else {
									FR8("\n%s\t\t{"
										"\n%s\t\t\tconst char *m = info->u.mem;"
										"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
										"\n%s\t\t\tif (info->u.convert)"
										"\n%s\t\t\t\tget_exchange_%s_array(m, &s->%s->",
										of, of, of, (*f)->name, of, of, (*f)->user_type->equivalent, (*f)->name);
									if (file)
										print_equivalent_path(file, *f);
									FR2(", sizeof(*s->%s)/sizeof(s->%s->", (*f)->name, (*f)->name);
									if (file)
										print_equivalent_path(file, *f);
									FR1("));"
										"\n%s\t\t}", of);
								}
							}
							else {
								FR5("\n%s\t\t{"
									"\n%s\t\t\tconst char *m = info->u.mem;"
									"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
									"\n%s\t\t\t", of, of, of, (*f)->name, of);
								if (field_type_byte != (*f)->user_type->equivalent) {
									FR3("if (info->u.convert)"
										"\n%s\t\t\t\tget_exchange_%s_array(m, &s->%s->", of, (*f)->user_type->equivalent, (*f)->name);
									if (file)
										print_equivalent_path(file, *f);
									FR2(", sizeof(*s->%s)/sizeof(s->%s->", (*f)->name, (*f)->name);
									if (file)
										print_equivalent_path(file, *f);
									FR1("));"
										"\n%s\t\t\telse ", of);
								}
								FR3("if (m != (const char*)a)"
									"\n%s\t\t\t\t_br_memcpy%u(a, m);"
									"\n%s\t\t}", of, (*f)->user_type->packed_bytes, of);
							}
							FR1("\n%s\t}", of);
						}
					}
					else {
						FR6("\n%s\t{"
							"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\t%s, sizeof(*s->%s), %s);",
							of, of, of, s_fixed((*f)->user_type) ? "info->u.mem" : "NULL", (*f)->name,
							is_simple_str_type((*f)->user_type->equivalent) ? "BRIDGE_ALLOC_MEM" :
								get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/0));
						if (_next_destroyable_field(f, r->fields_end)) {
							FZ7("\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\tGOTOZ%s(&s->%s);"
								"\n%s\t\ts->%s = (struct %s*)a;", of, of, ze, (*f)->name, of, (*f)->name, (*f)->type);
						}
						else {
							FR5("\n%s\t\ts->%s = (struct %s*)a;"
								"\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\treturn -1;", of, (*f)->name, (*f)->type, of, of);
						}
						if (!s_fixed((*f)->user_type)) {
							FR5("\n%s\t\tinfo->u.mem = _bridge_unpack_%s%s(info->u.mem%s, &s->%s->",
								of, (*f)->user_type->equivalent,
								(CHECKS_BYTES_CREDIT & credit_anough) ? "_no_lim" : "",
								(CHECKS_BYTES_CREDIT & credit_anough) ? "" : ", info->u.lim",
								(*f)->name);
							if (file)
								print_equivalent_path(file, *f);
							if (is_simple_str_type((*f)->user_type->equivalent))
								FP(", BRIDGE_PASS_ALLOCATOR(info->u.ac)");
							FP(");");
							if (CHECKS_BYTES_CREDIT & credit_anough)
								_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
							else {
								FR1("\n%s\t\tif (_br_unlikely(!info->u.mem))", of);
								_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end),
									need_z, of[0] ? of[1] ? "\t\t\t" : "\t\t" : "\t", ze);
							}
						}
						else {
							FR4("\n%s\t\tif (%sinfo->u.mem != (const char*)a)"
								"\n%s\t\t\ts->%s->",
								of, field_type_byte == (*f)->user_type->equivalent ? "" : "info->u.convert ||", of, (*f)->name);
							if (file)
								print_equivalent_path(file, *f);
							FR5(" = _bridge_get_%s_no_lim(&info->u.mem%s);"
								"\n%s\t\telse"
								"\n%s\t\t\tinfo->u.mem = (const char*)a + sizeof(*s->%s);",
								(*f)->user_type->equivalent,
								field_type_byte == (*f)->user_type->equivalent ? "" : ", info->u.convert", of, of, (*f)->name);
						}
						FR1("\n%s\t}", of);
					}
				}
				else if (F_OPTIONAL == (*f)->f_power) {
_unpack_simple_type: /* F_REQUIRED */
					{
						const char *of = (F_OPTIONAL == (*f)->f_power) ? offs[0] ? offs[1] ? "\t\t\t" : "\t\t" : "\t" : offs;
						if (is_simple_str_type((*f)->type)) {
							(*bytes_credit) = 0;
							FZ7("\n%s\tinfo->u.mem = _bridge_unpack_%s_(info->u.mem, info->u.lim, "
								"&s->%s, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
								"\n%s\tif (_br_unlikely(!info->u.mem))"
								"\n%s\t\tGOTOZ%s(&s->%s);", of, (*f)->type, (*f)->name, of, of, ze, (*f)->name);
						}
						else {
							if (F_OPTIONAL == (*f)->f_power)
								FR2("\n%s\ts->has_%s = 1u;", of, (*f)->name);
							if (field_type_bit == (*f)->type) {
								if ((*bits_credit)) {
									(*bits_credit)--; /* for F_OPTIONAL: r - already marked with CHECKS_BITS_CREDIT */
									if (F_REQUIRED == (*f)->f_power)
										_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
								}
								else {
									FR2("\n%s\tif (_br_unlikely(!upk_check_bits_lim_nz(info->u.bits, "
										"info->u.%s, info->u.bit_count, 1u)))", of, blim);
									_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end), need_z, of, ze);
								}
								FR2("\n%s\ts->%s = bridge_get_bit(info->u.bits, info->u.bit_count++) ? 1u : 0u;", of, (*f)->name);
							}
							else
								_unpack_simple_int_type(file, r, f, need_z, bytes_credit, of, ze);
						}
					}
				}
				else if (is_variable_simple_type((*f)->type))
					goto _unpack_variable_simple_type_ptr;
				else
					goto _unpack_fixed_simple_type_ptr;
				if (F_OPTIONAL == (*f)->f_power) {
					if ((*f)->user_type || is_simple_str_type((*f)->type)) {
						FR4("\n%s\t}"
							"\n%s\telse"
							"\n%s\t\ts->%s = NULL;", offs, offs, offs, (*f)->name);
					}
					else {
						FR4("\n%s\t}"
							"\n%s\telse"
							"\n%s\t\ts->has_%s = 0u;", offs, offs, offs, (*f)->name);
					}
				}
				break;
			}
			/* F_POINTER for field_type_bit, fall through */
		default:
			FR4("\n%s\t/* %s: %s[%u]", offs, (*f)->name, (*f)->type, (*f)->f_power);
			print_checked_credits(file, r, *bytes_credit, *bits_credit);
			if ((*f)->user_type) {
				/* if (*f)->user_type assumes that caller have done some checks - try to do some of them outside the loop */
				unsigned credit_anough = CHECKS_BYTES_CREDIT | CHECKS_BITS_CREDIT;
				/* NOTE: (*f)->user_type->packed_bytes*(*f)->f_power - will not overflow - checked in model.c */
				if ((*bytes_credit) >= (*f)->user_type->packed_bytes*(*f)->f_power) {
					(*bytes_credit) -= (*f)->user_type->packed_bytes*(*f)->f_power;
					if ((*f)->user_type->packed_bytes && !v_bytes((*f)->user_type))
						_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
				}
				else {
					(*bytes_credit) = 0;
					credit_anough &= ~CHECKS_BYTES_CREDIT;
					if (!v_bytes((*f)->user_type)) {
						FZ5("\n%s\tif (_br_unlikely(!upk_check_mem_lim(info->u.mem, info->u.lim, %uu)))"
							"\n%s\t\tGOTOZ%s(&s->%s);", offs, (*f)->user_type->packed_bytes*(*f)->f_power, offs, ze, (*f)->name);
					}
				}
				/* NOTE: (*f)->user_type->packed_bits*(*f)->f_power - will not overflow - checked in model.c */
				if ((*bits_credit) >= (*f)->user_type->packed_bits*(*f)->f_power) {
					(*bits_credit) -= (*f)->user_type->packed_bits*(*f)->f_power;
					if ((*f)->user_type->packed_bits && !v_bits((*f)->user_type))
						_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
				}
				else {
					(*bits_credit) = 0;
					credit_anough &= ~CHECKS_BITS_CREDIT;
					if (!v_bits((*f)->user_type)) {
						FZ6("\n%s\tif (_br_unlikely(!upk_check_bits_lim_nz(info->u.bits, info->u.%s, info->u.bit_count, %uu)))"
							"\n%s\t\tGOTOZ%s(&s->%s);", offs, blim, (*f)->user_type->packed_bits*(*f)->f_power, offs, ze, (*f)->name);
					}
				}
				if ((*f)->user_type->unknown_packed_bytes || (*f)->user_type->packed_optional_bytes > (*bytes_credit)/(*f)->f_power) {
					(*bytes_credit) = 0;
					credit_anough &= ~CHECKS_BYTES_CREDIT;
				}
				else
					(*bytes_credit) -= (*f)->user_type->packed_optional_bytes*(*f)->f_power;
				if ((*f)->user_type->unknown_packed_bits || (*f)->user_type->packed_optional_bits > (*bits_credit)/(*f)->f_power) {
					(*bits_credit) = 0;
					credit_anough &= ~CHECKS_BITS_CREDIT;
				}
				else
					(*bits_credit) -= (*f)->user_type->packed_optional_bits*(*f)->f_power;
				if (!(*f)->user_type->equivalent) {
					if ((*f)->user_type->s_layout < S_MONOLITH && !s_has_bits((*f)->user_type) && !v_bytes((*f)->user_type)) {
						/* if structure has padding bytes - then cannot just memcpy */
						FR6("\n%s\t{"
							"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\t((void)0,%uu) == sizeof(*s->%s) ? info->u.mem : NULL, sizeof(*s->%s),",
							offs, offs, offs, (*f)->user_type->packed_bytes*(*f)->f_power, (*f)->name, (*f)->name);
						if ((*f)->user_type->packed_bytes*(*f)->f_power > BR_MEMCPY_MAX_SIZE) {
							FR4("\n%s\t\t\t((void)0,%uu) == sizeof(*s->%s) %s :",
								offs, (*f)->user_type->packed_bytes*(*f)->f_power, (*f)->name,
								get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/2));
						}
						FR1(" %s);", get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/0));
					}
					else {
						FR5("\n%s\t{"
							"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\tNULL, sizeof(*s->%s), %s);",
							offs, offs, offs, (*f)->name, get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/-1));
					}
					if (_next_destroyable_field(f, r->fields_end)) {
						FZ9("\n%s\t\tif (_br_unlikely(!a))"
							"\n%s\t\t\tGOTOZ%s(&s->%s);"
							"\n%s\t\ts->%s = (struct %s(*)[sizeof(*s->%s)/sizeof((*s->%s)[0])])a;",
							offs, offs, ze, (*f)->name, offs, (*f)->name, (*f)->type, (*f)->name, (*f)->name);
					}
					else {
						FR7("\n%s\t\ts->%s = (struct %s(*)[sizeof(*s->%s)/sizeof((*s->%s)[0])])a;"
							"\n%s\t\tif (_br_unlikely(!a))"
							"\n%s\t\t\treturn -1;", offs, (*f)->name, (*f)->type, (*f)->name, (*f)->name, offs, offs);
					}
					if ((*f)->user_type->s_layout < S_MONOLITH && !s_has_bits((*f)->user_type) && !v_bytes((*f)->user_type)) {
						FR3("\n%s\t\tif (((void)0,%uu) == sizeof(*s->%s)", offs, (*f)->user_type->packed_bytes*(*f)->f_power, (*f)->name);
						if ((*f)->user_type->convertable)
							FP(" && !info->u.convert");
						if ((*f)->user_type->packed_bytes*(*f)->f_power > BR_MEMCPY_MAX_SIZE) {
							FR3(")"
								"\n%s\t\t\tinfo->u.mem += sizeof(*s->%s);"
								"\n%s\t\telse {", offs, (*f)->name, offs);
						}
						else {
							FR8(") {"
								"\n%s\t\t\tconst char *m = info->u.mem;"
								"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
								"\n%s\t\t\tif (m != (const char*)a)"
								"\n%s\t\t\t\t_br_memcpy%u(a, m);"
								"\n%s\t\t}"
								"\n%s\t\telse {",
								offs, offs, (*f)->name, offs, offs, (*f)->user_type->packed_bytes*(*f)->f_power, offs, offs);
						}
					}
					else
						FR1("\n%s\t\t{", offs);
					FR3("\n%s\t\t\tsize_t i = 0;"
						"\n%s\t\t\tdo {"
						"\n%s\t\t\t\t", offs, offs, offs);
					{
						/* if (*f)->user_type assumes that caller have done some checks - do remaining checks inside the loop */
						int check_bytes  = v_bytes((*f)->user_type) && ((*f)->user_type->_aux & CHECKS_BYTES_CREDIT);
						int check_bits   = v_bits((*f)->user_type) && ((*f)->user_type->_aux & CHECKS_BITS_CREDIT);
						int check_return = need_check_init_unpacked_return((*f)->user_type);
						if (check_bytes && (CHECKS_BYTES_CREDIT & credit_anough)) {
							check_bytes = 0;
							_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
						}
						if (check_bits && (CHECKS_BITS_CREDIT & credit_anough)) {
							check_bits = 0;
							_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
						}
						if (check_return &&
							(CHECKS_BYTES_CREDIT | CHECKS_BITS_CREDIT) == credit_anough &&
							S_DYNAMIC != (*f)->user_type->s_layout)
						{
							check_return = 2; /* assume return is always successfull */
							/* ensure that (*f)->user_type will unpack successfully,
							  even if (*f)->user_type also checks the limits */
							if (s_has_bytes((*f)->user_type))
								_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
							if (s_has_bits((*f)->user_type))
								_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
						}
						if (check_bytes || check_bits || 1 == check_return)
							FP("if (_br_unlikely(");
						if (check_bytes) {
							FR1("!upk_check_mem_lim(info->u.mem, info->u.lim, %uu)", (*f)->user_type->packed_bytes);
							if (check_bits || 1 == check_return) {
								FR1(") ||"
									"\n%s\t\t\t\t\t_br_unlikely(", offs);
							}
						}
						if (check_bits) {
							FR2("!upk_check_bits_lim_nz(info->u.bits, info->u.%s, info->u.bit_count, %uu)",
								blim, (*f)->user_type->packed_bits/*!=0 because CHECKS_BITS_CREDIT is set*/);
							if (1 == check_return) {
								FR1(") ||"
									"\n%s\t\t\t\t\t_br_unlikely(", offs);
							}
						}
						if (1 == check_return)
							call_init_unpacked_struct(file, r, *f, CALL_FROM_FIXED_ARRAY_INC);
						if (check_bytes || check_bits || 1 == check_return) {
							int b_opened = 0;
							if (S_DYNAMIC == (*f)->user_type->s_layout) {
								b_opened = 1;
								if (check_bytes + check_bits + (1 == check_return) > 1) {
									FR1("))"
										"\n%s\t\t\t\t{", offs);
								}
								else
									FP(")) {");
								FR4("\n%s\t\t\t\t\tBRIDGE_MEMSET(&(*s->%s)[i], 0, sizeof(*s->%s) - i*sizeof((*s->%s)[0]));",
									offs, (*f)->name, (*f)->name, (*f)->name);
							}
							else
								FP("))");
							_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end),
								need_z, offs[0] ? offs[1] ? "\t\t\t\t\t" : "\t\t\t\t" : "\t\t\t", ze);
							if (b_opened)
								FR1("\n%s\t\t\t\t}", offs);
						}
						if (2 == check_return)
							FP("_br_assume(");
						if (1 != check_return)
							call_init_unpacked_struct(file, r, *f, CALL_FROM_FIXED_ARRAY_INC);
						if (2 == check_return)
							FP(");");
					}
					FR5("\n%s\t\t\t} while (i < sizeof(*s->%s)/sizeof((*s->%s)[0]));"
						"\n%s\t\t}"
						"\n%s\t}", offs, (*f)->name, (*f)->name, offs, offs);
				}
				else {
					FR1("\n%s\t{", offs);
					if (!s_fixed((*f)->user_type)) {
						const char *native_type = get_native_type((*f)->user_type->equivalent);
						/* NOTE: may cast 'sizeof(*s->%s)/sizeof(%s)' to bridge_counter_t
						  - structure 's' packed size was statically checked in code generated by generate_code_unpack() */
						FR10("\n%s\t\tvoid *p = &s->%s;"
							"\n%s\t\tinfo->u.mem = _bridge_unpack_%s_array(info->u.mem, info->u.lim,"
							"\n%s\t\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof(%s)),"
							"\n%s\t\t\t(%s**)p, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
							"\n%s\t\tif (_br_unlikely(!info->u.mem))",
							offs, (*f)->name, offs, (*f)->user_type->equivalent, offs, (*f)->name, native_type, offs, native_type, offs);
						_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end),
							need_z, offs[0] ? offs[1] ? "\t\t\t" : "\t\t" : "\t", ze);
					}
					else {
						FR3("\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\tinfo->u.mem, sizeof(*s->%s),", offs, offs, (*f)->name);
						if ((*f)->user_type->packed_bytes*(*f)->f_power > BR_MEMCPY_MAX_SIZE) {
							if (S_STATIC1 == (*f)->user_type->s_layout)
								FP(" ");
							else
								FR1("\n%s\t\t\t", offs);
							FR1("%s);", get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/1));
						}
						else
							FR1(" %s);", get_alloc_type_for_layout((*f)->user_type->s_layout, /*copy:*/0));
						if (_next_destroyable_field(f, r->fields_end)) {
							FZ9("\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\tGOTOZ%s(&s->%s);"
								"\n%s\t\ts->%s = (struct %s(*)[sizeof(*s->%s)/sizeof((*s->%s)[0])])a;",
								offs, offs, ze, (*f)->name, offs, (*f)->name, (*f)->type, (*f)->name, (*f)->name);
						}
						else {
							FR7("\n%s\t\ts->%s = (struct %s(*)[sizeof(*s->%s)/sizeof((*s->%s)[0])])a;"
								"\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\treturn -1;", offs, (*f)->name, (*f)->type, (*f)->name, (*f)->name, offs, offs);
						}
						if ((*f)->user_type->packed_bytes*(*f)->f_power > BR_MEMCPY_MAX_SIZE) {
							if (field_type_byte == (*f)->user_type->equivalent)
								FR2("\n%s\t\tinfo->u.mem += sizeof(*s->%s);", offs, (*f)->name);
							else {
								FR8("\n%s\t\t{"
									"\n%s\t\t\tconst char *m = info->u.mem;"
									"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
									"\n%s\t\t\tif (info->u.convert)"
									"\n%s\t\t\t\tget_exchange_%s_array(m, &(*s->%s)[0].",
									offs, offs, offs, (*f)->name, offs, offs, (*f)->user_type->equivalent, (*f)->name);
								if (file)
									print_equivalent_path(file, *f);
								FR2(", sizeof(*s->%s)/sizeof((*s->%s)[0].", (*f)->name, (*f)->name);
								if (file)
									print_equivalent_path(file, *f);
								FR1("));"
									"\n%s\t\t}", offs);
							}
						}
						else {
							FR5("\n%s\t\t{"
								"\n%s\t\t\tconst char *m = info->u.mem;"
								"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
								"\n%s\t\t\t", offs, offs, offs, (*f)->name, offs);
							if (field_type_byte != (*f)->user_type->equivalent) {
								FR3("if (info->u.convert)"
									"\n%s\t\t\t\tget_exchange_%s_array(m, &(*s->%s)[0].", offs, (*f)->user_type->equivalent, (*f)->name);
								if (file)
									print_equivalent_path(file, *f);
								FR2(", sizeof(*s->%s)/sizeof((*s->%s)[0].", (*f)->name, (*f)->name);
								if (file)
									print_equivalent_path(file, *f);
								FR1("));"
									"\n%s\t\t\telse ", offs);
							}
							FR3("if (m != (const char*)a)"
								"\n%s\t\t\t\t_br_memcpy%u(a, m);"
								"\n%s\t\t}", offs, (*f)->user_type->packed_bytes*(*f)->f_power, offs);
						}
					}
					FR1("\n%s\t}", offs);
				}
			}
			else if (field_type_bit == (*f)->type) {
				unsigned bb = bytes_for_bits((*f)->f_power);
				if ((*bytes_credit) >= bb) {
					(*bytes_credit) -= bb;
					_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
				}
				else {
					(*bytes_credit) = 0;
					FZ6("\n%s\tif (_br_unlikely(!upk_check_mem_lim(info->u.mem, info->u.lim, _br_sizeof(*s->%s, %uu))))"
						"\n%s\t\tGOTOZ%s(&s->%s);", offs, (*f)->name, bb, offs, ze, (*f)->name);
				}
				FZ12("\n%s\t{"
					"\n%s\t\tvoid *a = &s->%s;"
					"\n%s\t\tinfo->u.mem = _bridge_unpack_bit_array(info->u.mem, _br_sizeof(*s->%s, %uu),"
					"\n%s\t\t\t(unsigned char**)a, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
					"\n%s\t}"
					"\n%s\tif (_br_unlikely(!info->u.mem))"
					"\n%s\t\tGOTOZ%s(&s->%s);", offs, offs, (*f)->name, offs, (*f)->name, bb, offs, offs, offs, offs, ze, (*f)->name);
			}
			else if (is_variable_simple_type((*f)->type)) {
_unpack_variable_simple_type_ptr: /* F_POINTER */
				if (is_simple_str_type((*f)->type))
					(*bytes_credit) = 0;
				else {
					unsigned pwr = (F_POINTER == (*f)->f_power) ? 1u : (*f)->f_power;
					unsigned max_needed_bytes = _simple_type_max_required_bytes((*f)->type);
					if (max_needed_bytes > (*bytes_credit)/pwr)
						(*bytes_credit) = 0;
					else {
						(*bytes_credit) -= max_needed_bytes*pwr;
						/* NOTE: will check limits anyway while unpacking an array */
					}
				}
				if (F_POINTER == (*f)->f_power) {
					FR3("\n%s\tinfo->u.mem = _bridge_unpack_%s_array(info->u.mem, info->u.lim, "
						"1u, &s->%s, BRIDGE_PASS_ALLOCATOR(info->u.ac));", offs, (*f)->type, (*f)->name);
				}
				else {
					const char *native_type = get_native_type((*f)->type);
					/* NOTE: may cast 'sizeof(*s->%s)/sizeof(%s)' to bridge_counter_t
					  - structure 's' packed size was statically checked in code generated by generate_code_unpack() */
					FR11("\n%s\t{"
						"\n%s\t\tvoid *a = &s->%s;"
						"\n%s\t\tinfo->u.mem = _bridge_unpack_%s_array(info->u.mem, info->u.lim,"
						"\n%s\t\t\t(bridge_counter_t)(sizeof(*s->%s)/sizeof(%s)),"
						"\n%s\t\t\t(%s**)a, BRIDGE_PASS_ALLOCATOR(info->u.ac));"
						"\n%s\t}", offs, offs, (*f)->name, offs, (*f)->type, offs, (*f)->name, native_type, offs, native_type, offs);
				}
				FR1("\n%s\tif (_br_unlikely(!info->u.mem))", offs);
				_print_gotoz_or_return(file, _next_destroyable_field(f, r->fields_end), need_z, offs, ze);
			}
			else {
_unpack_fixed_simple_type_ptr: /* F_POINTER */
				{
					unsigned pwr = (F_POINTER == (*f)->f_power) ? 1u : (*f)->f_power;
					unsigned needed_bytes = _simple_type_max_required_bytes((*f)->type);
					if (needed_bytes <= (*bytes_credit)/pwr) {
						(*bytes_credit) -= needed_bytes*pwr;
						_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
					}
					else {
						(*bytes_credit) = 0;
						FZ6("\n%s\tif (_br_unlikely(!upk_check_mem_lim(info->u.mem, info->u.lim, _br_sizeof(*s->%s, %uu))))"
							"\n%s\t\tGOTOZ%s(&s->%s);", offs, (*f)->name, needed_bytes, offs, ze, (*f)->name);
					}
					if (F_POINTER == (*f)->f_power) {
						FR5("\n%s\t{"
							"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\tinfo->u.mem, sizeof(*s->%s), %s);",
							offs, offs, offs, (*f)->name, get_alloc_type_for_simple_type((*f)->type, /*copy:*/0));
					}
					else {
						FR4("\n%s\t{"
							"\n%s\t\tvoid *a = info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
							"\n%s\t\t\tinfo->u.mem, sizeof(*s->%s),", offs, offs, offs, (*f)->name);
						if (needed_bytes*pwr > BR_MEMCPY_MAX_SIZE) {
							if (field_type_byte == (*f)->type)
								FP(" ");
							else
								FR1("\n%s\t\t\t", offs);
							FR1("%s);", get_alloc_type_for_simple_type((*f)->type, /*copy:*/1));
						}
						else
							FR1(" %s);", get_alloc_type_for_simple_type((*f)->type, /*copy:*/0));
					}
					{
						const char *native_type = get_native_type((*f)->type);
						int no_next_destroyable = !_next_destroyable_field(f, r->fields_end);
						if (!no_next_destroyable) {
							FZ4("\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\tGOTOZ%s(&s->%s);", offs, offs, ze, (*f)->name);
						}
						if (F_POINTER == (*f)->f_power)
							FR3("\n%s\t\ts->%s = (%s*)a;", offs, (*f)->name, native_type);
						else {
							FR5("\n%s\t\ts->%s = (%s(*)[sizeof(*s->%s)/sizeof((*s->%s)[0])])a;",
								offs, (*f)->name, native_type, (*f)->name, (*f)->name);
						}
						if (no_next_destroyable) {
							FR2("\n%s\t\tif (_br_unlikely(!a))"
								"\n%s\t\t\treturn -1;", offs, offs);
						}
					}
					if (F_POINTER == (*f)->f_power) {
						FR9("\n%s\t\tif (%sinfo->u.mem != (const char*)a)"
							"\n%s\t\t\t*s->%s = _bridge_get_%s_no_lim(&info->u.mem%s);"
							"\n%s\t\telse"
							"\n%s\t\t\tinfo->u.mem = (const char*)a + sizeof(*s->%s);",
							offs, field_type_byte == (*f)->type ? "" : "info->u.convert || ",
							offs, (*f)->name, (*f)->type, field_type_byte == (*f)->type ? "" : ", info->u.convert",
							offs, offs, (*f)->name);
					}
					else if (needed_bytes*pwr > BR_MEMCPY_MAX_SIZE) {
						if (field_type_byte == (*f)->type)
							FR2("\n%s\t\tinfo->u.mem += sizeof(*s->%s);", offs, (*f)->name);
						else {
							FR11("\n%s\t\t{"
								"\n%s\t\t\tconst char *m = info->u.mem;"
								"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
								"\n%s\t\t\tif (info->u.convert)"
								"\n%s\t\t\t\tget_exchange_%s_array(m, *s->%s, sizeof(*s->%s)/sizeof((*s->%s)[0]));"
								"\n%s\t\t}",
								offs, offs, offs, (*f)->name, offs, offs, (*f)->type, (*f)->name, (*f)->name, (*f)->name, offs);
						}
					}
					else {
						FR5("\n%s\t\t{"
							"\n%s\t\t\tconst char *m = info->u.mem;"
							"\n%s\t\t\tinfo->u.mem = m + sizeof(*s->%s);"
							"\n%s\t\t\t", offs, offs, offs, (*f)->name, offs);
						if (field_type_byte != (*f)->type) {
							FR6("if (info->u.convert)"
								"\n%s\t\t\t\tget_exchange_%s_array(m, *s->%s, sizeof(*s->%s)/sizeof((*s->%s)[0]));"
								"\n%s\t\t\telse ", offs, (*f)->type, (*f)->name, (*f)->name, (*f)->name, offs);
						}
						FR3("if (m != (const char*)a)"
							"\n%s\t\t\t\t_br_memcpy%u(a, m);"
							"\n%s\t\t}", offs, needed_bytes*pwr, offs);
					}
					FP("\n\t}");
				}
			}
			break;
	}
}

/* caller have checked required limits, now may unpack the structure */
static void generate_code_init_unpacked(FILE *file/*NULL if determining marks*/, struct struct_def *const s, int *need_z)
{
	const int long_loop = (s->tail_recursive && s->tail_recursive->user_type != s);
	if (long_loop) {
		FR1("\nenum UNP_%s {", s->s_name);
		{
			const struct struct_def *r = s;
			do {
				const struct struct_def *n = r->tail_recursive->user_type;
				FR2("\n\tU_%s%s", r->s_name, n != s ? "," : "");
				r = n;
			} while (r != s);
		}
		FR4("\n};"
			"\n"
			"\nA_Nonnull_all_args A_Check_return A_Success(return == 0) A_Ret_range(-1,0)%s"
			"\nstatic int _%s_unp(A_Out void *p, enum UNP_%s t, A_Inout union %s *info BRIDGE_DEBUG_ARGS_DECL)",
			s_has_bytes(s) ? "" : " A_Post_satisfies(info->u.bit_count > 0)", s->s_name, s->s_name, _get_init_unpacked_info(s));
	}
	else if (S_DYNAMIC == s->s_layout) {
		FR4("\nA_Nonnull_all_args A_Check_return A_Success(return == 0) A_Ret_range(-1,0)%s"
			"\nstatic int _%s_init_unpacked_(A_Out struct %s *s, A_Inout union %s *info BRIDGE_DEBUG_ARGS_DECL)",
			s_has_bytes(s) ? "" : " A_Post_satisfies(info->u.bit_count > 0)", s->s_name, s->s_name, _get_init_unpacked_info(s));
	}
	else
		(void)generate_prototype_init_unpacked(file, s);
	FP("\n{");
	if (*need_z) {
		FP("\n\tvoid *z;");
		if (long_loop)
			FP("\n\tvoid *ze;");
	}
	if (s->tail_recursive) {
		if (long_loop) {
			FP("\n\tswitch (t) {"
				"\ndefault:");
		}
		FP("\n\tfor (;;) {");
	}
	{
		const char *const ze = long_loop ? "E" : "";
		const char *const blim = s_has_bytes(s) ? "mem" : "lim";
		const char *const offs = long_loop ? "\t\t" : s->tail_recursive ? "\t" : "";
		struct struct_def *r = s;
		do {
			/* assume caller had already checked minimum number of bytes and bits required
			  to unpack this structure - we have a credit of checked bytes and bits */
			unsigned bytes_credit = r->packed_bytes;
			unsigned bits_credit = r->packed_bits;
			struct field_def **t = NULL;
			if (long_loop) {
				FR3("\ncase U_%s: {"
					"\n\t\t\tstruct %s *s = (struct %s*)p;", r->s_name, r->s_name, r->s_name);
			}
			{
				struct field_def **f = r->fields;
				for (; f < r->fields_end; f++) {
					if (r->tail_recursive == *f) {
						t = f;
						continue; /* tail-recursive field will be processed last */
					}
					switch_init_unpacked(file, f, r, need_z, &bytes_credit, &bits_credit, blim, offs, ze);
				}
			}
			if (t && r->tail_recursive) {
				FR4("\n%s\t/* %s: %s%s (tail recursive) */",
					offs, r->tail_recursive->name, r->tail_recursive->type,
					F_POINTER == r->tail_recursive->f_power ? "[1]" :
					F_OPTIONAL == r->tail_recursive->f_power ? "*" : "");
				if (F_OPTIONAL == r->tail_recursive->f_power) {
					if (bits_credit) {
						bits_credit--;
						_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check bits_credit */
					}
					else {
						FR6("\n%s\tif (_br_unlikely(!upk_check_bits_lim_nz(info->u.bits, info->u.%s, info->u.bit_count, 1u))) {"
							"\n%s\t\ts->%s = NULL;"
							"\n%s\t\treturn -1;"
							"\n%s\t}", offs, blim, offs, r->tail_recursive->name, offs, offs);
					}
					FR5("\n%s\tif (!bridge_get_bit(info->u.bits, info->u.bit_count++)) {"
						"\n%s\t\ts->%s = NULL;"
						"\n%s\t\tbreak;"
						"\n%s\t}", offs, offs, r->tail_recursive->name, offs, offs);
				}
				{
					/* bytes_credit & bits_credit are zero if r->tail_recursive is optional, else - may be non-zero */
					unsigned credit_anough = _check_limits_for_user_type(file, r, t, need_z, &bytes_credit, &bits_credit, blim, offs, ze);
					(void)credit_anough;
				}
				if (F_REQUIRED != r->tail_recursive->f_power) {
					FR8("\n%s\ts->%s = (struct %s*)info->u.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(info->u.ac),"
						"\n%s\t\tNULL, sizeof(*s->%s), BRIDGE_ALLOC_MEM);"
						"\n%s\tif (_br_unlikely(!s->%s))"
						"\n%s\t\treturn -1;",
						offs, r->tail_recursive->name, r->tail_recursive->type,
						offs, r->tail_recursive->name, offs, r->tail_recursive->name, offs);
				}
				if (long_loop) {
					FR2("\n\t\t\tp = %ss->%s;"
						"\n\t\t}", F_REQUIRED == r->tail_recursive->f_power ? "&" : "", r->tail_recursive->name);
				}
				else
					FR1("\n\t\ts = s->%s;", r->tail_recursive->name);
				r = r->tail_recursive->user_type;
			}
		} while (r != s);
	}
	if (long_loop)
		FP("\n\t}}");
	else if (s->tail_recursive)
		FP("\n\t}");
	if (need_check_init_unpacked_return(s))
		FP("\n\treturn 0;");
	if ((*need_z) & 1) {
		FP("\n_z:"
			"\n\tBRIDGE_MEMSET(z, 0, (size_t)((const char*)(s + 1) - (const char*)z));"
			"\n\treturn -1;");
	}
	if ((*need_z) & 2) {
		FP("\n_ze:"
			"\n\tBRIDGE_MEMSET(z, 0, (size_t)((const char*)ze - (const char*)z));"
			"\n\treturn -1;");
	}
	FP("\n}"
		"\n");
}

static void _determine_dependent_marks(void)
{
	for (;;) {
		int repeat = 0;
		for (;;) {
			struct struct_def **s = structs;
			for (; s < structs_end; s++) {
				if (!(*s)->tail_recursive || (*s)->recursion_entry) {
					int need_z = 0;
					unsigned _aux = (*s)->_aux;
					generate_code_init_unpacked(/*file:*/NULL, *s, &need_z);
					if (_aux != (*s)->_aux)
						repeat = 1; /* re-check all over again, some structures that depend on (*s) may change their marks */
				}
			}
			if (s == structs_end)
				break;
		}
		if (!repeat)
			break;
	}
}

void mark_strutures_for_unpack(void)
{
	{
		/* first pass: set obvious flags - reset (*s)->_aux value */
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			unsigned _aux = 0;
			if ((*s)->packed_bytes && !v_bytes(*s))
				_aux |= CHECKS_BYTES_CREDIT; /* caller must check bytes_credit */
			if ((*s)->packed_bits && !v_bits(*s))
				_aux |= CHECKS_BITS_CREDIT; /* caller must check bits_credit */
			(*s)->_aux = (unsigned char)_aux;
		}
	}
	/* next pass: determine dependent marks */
	/* NOTE: circle-dependencies may not be resolved */
	_determine_dependent_marks();
	{
		/* store & clear marks */
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			/* 0000yyxx -> 00xx00xx */
			(*s)->_aux = (unsigned char)(((*s)->_aux & 3u)*17u);
		}
	}
	{
		/* resolve circle-dependencies */
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			unsigned ax = 0;
			if ((*s)->packed_bytes)
				ax |= CHECKS_BYTES_CREDIT;
			if ((*s)->packed_bits)
				ax |= CHECKS_BITS_CREDIT;
			/* if flags are not already set */
			if (ax != ((*s)->_aux & ax)) {
				/* try to set flags, then see what happens */
				(*s)->_aux = (unsigned char)((*s)->_aux | ax);
				_determine_dependent_marks();
				ax = (*s)->_aux;
				if (!(ax & (CHECKS_BYTES_CREDIT << 4)) && !(ax & (CHECKS_BYTES_CREDIT << 2)))
					ax &= ~CHECKS_BYTES_CREDIT; /* flag was set incorrectly, reset it */
				if (!(ax & (CHECKS_BITS_CREDIT << 4)) && !(ax & (CHECKS_BITS_CREDIT << 2)))
					ax &= ~CHECKS_BITS_CREDIT; /* flag was set incorrectly, reset it */
				(*s)->_aux = (unsigned char)((ax & 3u) << 4u);
				/* reset temporary flags */
				{
					struct struct_def **ss = structs;
					for (; ss < structs_end; ss++) {
						/* 00xxtttt -> 00xx00xx */
						(*ss)->_aux = (unsigned char)(((unsigned)(*ss)->_aux >> 4u)*17u);
					}
				}
			}
		}
	}
}

int generate_code_unpack(FILE *file, struct struct_def *s, const char *decl/*NULL?*/, int gotoz)
{
	if (!s->equivalent) {
		if (!s->tail_recursive || s->recursion_entry) {
			int need_z = 0;
			generate_code_init_unpacked(/*file:*/NULL, s, &need_z); /* determine need_z value */
			if (!(gotoz & 1)) {
				gotoz |= 1;
				FP("\n#define GOTOZ(f) do {z = f; goto _z;} while ((void)0,0)\n");
			}
			if (!(gotoz & 2)) {
				gotoz |= 2;
				FP("\n#define GOTOZE(f) do {ze = s + 1; z = f; goto _ze;} while ((void)0,0)\n");
			}
			generate_code_init_unpacked(file, s, &need_z);
		}
		if (S_DYNAMIC == s->s_layout) {
			(void)generate_prototype_init_unpacked(file, s);
			if (s->tail_recursive && s->tail_recursive->user_type != s) {
				const struct struct_def *r = s;
				while (!r->recursion_entry)
					r = r->tail_recursive->user_type;
				FR2("\n{"
					"\n\tint ret = _%s_unp(s, U_%s, info BRIDGE_DEBUG_ARGS_PASS);"
					"\n\tbridge_allocator_dec_level(info->u.ac);"
					"\n\treturn ret;"
					"\n}"
					"\n", r->s_name, s->s_name);
			}
			else {
				FR1("\n{"
					"\n\tint ret = _%s_init_unpacked_(s, info BRIDGE_DEBUG_ARGS_PASS);"
					"\n\tbridge_allocator_dec_level(info->u.ac);"
					"\n\treturn ret;"
					"\n}"
					"\n", s->s_name);
			}
		}
	}
	FR5("\nA_Nonnull_arg(2) A_Check_return A_Ret_valid A_Success(return != NULL) A_When(s != NULL, A_Post_satisfies(return == s))"
		"\nstatic struct %s *_%s_unpack_to_(A_Out_opt struct %s *s, A_Inout union bridge_unpack_%sinfo *in BRIDGE_DEBUG_ARGS_DECL)"
		"\n{"
		"\n\tunion %s info;", s->s_name, s->s_name, s->s_name, s->convertable ? "conv_" : "", _get_init_unpacked_info(s));
	if (need_check_init_unpacked_return(s))
		FR1("\n\tstruct %s *const s1 = s;", s->s_name);
	/* may assert if generator's BRIDGE_MODEL_MAX_PACKED_SIZE > BRIDGE_MAX */
	if (s->packed_bytes > 1)
		FR1("\n\t(void)sizeof(int[1-2*(%uu > BRIDGE_MAX)]);", s->packed_bytes);
	/* may assert if generator's BRIDGE_MODEL_MAX_BIT_COUNT > target's BRIDGE_BIT_COUNT_MAX */
	if (s->packed_bits > 1)
		FR1("\n\t(void)sizeof(int[1-2*(%uu > BRIDGE_BIT_COUNT_MAX)]);", s->packed_bits);
	FP("\n\tinfo.u.lim = ((size_t)((const char*)in->ui.us.limit - (const char*)in->ui.us.from) > BRIDGE_MAX) ?"
		"\n\t\t(const char*)in->ui.us.from + BRIDGE_MAX : (const char*)in->ui.us.limit;");
	if (s_has_bits(s))
		FP("\n\tinfo.u.bits = (const unsigned char*)in->ui.us.from;");
	else
		FP("\n\tinfo.u.mem = (const char*)in->ui.us.from;");
	if (need_pack_bits_counter(s)) {
		FP("\n\t{"
			"\n\t\tINT32_TYPE bit_bytes;"
			"\n\t\tinfo.u.bits = (const unsigned char*)_bridge_unpack_counter((const char*)info.u.bits, info.u.lim, &bit_bytes);"
			"\n\t\tif (_br_unlikely(!info.u.bits))"
			"\n\t\t\treturn NULL;"
			"\n\t\tif (_br_unlikely(!upk_check_mem_lim_n((const char*)info.u.bits, info.u.lim, 1u, bit_bytes)))"
			"\n\t\t\treturn NULL;"
			"\n\t\tinfo.u.mem = (const char*)info.u.bits + (bridge_counter_t)(unsigned INT32_TYPE)bit_bytes;"
			"\n\t}");
	}
	else if (s_has_bits(s) && s_has_bytes(s)) {
		if (v_bytes(s)) {
			/*bits_have_fixed_bytes(s) == 1*/
			FR2("\n\tif (_br_unlikely(!upk_check_mem_lim((const char*)info.u.bits, info.u.lim, %uu)))"
				"\n\t\treturn NULL;"
				"\n\tinfo.u.mem = (const char*)info.u.bits + %uu;",
				bytes_for_bits(s->packed_bits/* + s->packed_optional_bits*/),
				bytes_for_bits(s->packed_bits/* + s->packed_optional_bits*/));
		}
		else {
			/* bits may be variable */
			FR2("\n\tif (_br_unlikely(!upk_check_mem_lim((const char*)info.u.bits, info.u.lim, %uu)))"
				"\n\t\treturn NULL;"
				"\n\tinfo.u.mem = info.u.lim - %uu;", s->packed_bytes, s->packed_bytes);
		}
	}
	if ((s->_aux & CHECKS_BYTES_CREDIT) && (!s_has_bits(s) || v_bytes(s))) {
		FR1("\n\tif (_br_unlikely(!upk_check_mem_lim(info.u.mem, info.u.lim, %uu)))"
			"\n\t\treturn NULL;", s->packed_bytes);
	}
	if ((s->_aux & CHECKS_BITS_CREDIT) && (need_pack_bits_counter(s) || !v_bytes(s))) {
		FR2("\n\tif (_br_unlikely(!upk_check_bits_lim_nz(info.u.bits, info.u.%s, 0u, %uu)))"
			"\n\t\treturn NULL;", s_has_bytes(s) ? "mem" : "lim", s->packed_bits);
	}
	if (!s_fixed(s) || s_has_bits(s) || s->s_layout >= S_MONOLITH) {
		FR2("\n\tif (!s) {"
			"\n\t\ts = (struct %s*)in->ui.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac),"
			"\n\t\t\tNULL, sizeof(*s), %s);"
			"\n\t\tif (_br_unlikely(!s))"
			"\n\t\t\treturn NULL;"
			"\n\t}", s->s_name, get_alloc_type_for_layout(s->s_layout, /*copy:*/-1));
	}
	if (s_has_bits(s))
		FP("\n\tinfo.u.bit_count = 0u;");
	if (!s->equivalent) {
		if (S_DYNAMIC == s->s_layout)
			FP("\n\tinfo.u.ac = in->ui.ac;");
		if (s->s_layout < S_MONOLITH && !s_has_bits(s) && !v_bytes(s)) {
			/* if structure has padding bytes - then cannot just memcpy */
			FR3("\n\tif (!s) {"
				"\n\t\ts = (struct %s*)in->ui.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac),"
				"\n\t\t\t((void)0,%uu) == sizeof(*s) ? info.u.mem : NULL, sizeof(*s), %s);"
				"\n\t\tif (_br_unlikely(!s))"
				"\n\t\t\treturn NULL;"
				"\n\t}", s->s_name, s->packed_bytes, get_alloc_type_for_layout(s->s_layout, /*copy:*/0));
			if (s->convertable)
				FP("\n\tinfo.u.convert = in->ui.convert;");
			FR1("\n\tif (((void)0,%uu) == sizeof(*s)", s->packed_bytes);
			if (s->convertable)
				FP(" && !info.u.convert");
			FP(") {"
				"\n\t\tconst char *m = info.u.mem;"
				"\n\t\tinfo.u.mem = m + sizeof(*s);"
				"\n\t\tif (m != (const char*)s)");
			if (s->packed_bytes > BR_MEMCPY_MAX_SIZE)
				FP("\n\t\t\tBRIDGE_MEMCPY(s, m, sizeof(*s));");
			else
				FR1("\n\t\t\t_br_memcpy%u(s, m);", s->packed_bytes);
			FP("\n\t}"
				"\n\telse"
				"\n\t\t");
		}
		else {
			if (s->convertable)
				FP("\n\tinfo.u.convert = in->ui.convert;");
			FP("\n#ifdef _DEBUG"
				"\n\tBRIDGE_MEMSET(s, 0xdc, sizeof(*s));"
				"\n#endif"
				"\n\t");
		}
		{
			int check_return = need_check_init_unpacked_return(s);
			if (check_return)
				FP("if (_br_unlikely(");
			FR1("_%s_init_unpacked(s, ", s->s_name);
			if (S_DYNAMIC == s->s_layout)
				FP("BRIDGE_PASS_ALLOCATOR_CONTAINER(info.u.ac, ");
			FP("&info");
			if (check_return) {
				if (S_DYNAMIC == s->s_layout)
					FP(")");
				FP("))) {");
				if (S_DYNAMIC == s->s_layout) {
					FR1("\n\t\tif (in->ui.ac->free_cb) {"
						"\n\t\t\t_%s_ac_destroy(s, BRIDGE_PASS_ALLOCATOR(in->ui.ac));"
						"\n\t\t\tif (!s1)"
						"\n\t\t\t\tin->ui.ac->free_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), s);"
						"\n\t\t}", s->s_name);
				}
				else {
					FP("\n\t\tif (!s1 && in->ui.ac->free_cb)"
						"\n\t\t\tin->ui.ac->free_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), s);");
				}
				FP("\n\t\treturn NULL;"
					"\n\t}");
			}
			else
				FP(");");
		}
	}
	else if (s->equiv_many && is_simple_str_type(s->equivalent)) {
		/* NOTE: may cast 'sizeof(*s)/sizeof(s->xxx)' to bridge_counter_t
		  - structure 's' packed size was statically checked above */
		FR1("\n\t{"
			"\n\t\tchar **ae;"
			"\n\t\tinfo.u.mem = _bridge_unpack_%s_array_(info.u.mem, info.u.lim,"
			"\n\t\t\t(bridge_counter_t)(sizeof(*s)/sizeof(s->", s->equivalent);
		print_equivalent_path_s(file, s);
		FP(")), &s->");
		print_equivalent_path_s(file, s);
		FP(", &ae, BRIDGE_PASS_ALLOCATOR(in->ui.ac));"
			"\n\t\tif (_br_unlikely(!info.u.mem)) {"
			"\n\t\t\tif (in->ui.ac->free_cb) {"
			"\n\t\t\t\t_bridge_destroy_str_array(&s->");
		print_equivalent_path_s(file, s);
		FP(", ae, BRIDGE_PASS_ALLOCATOR(in->ui.ac));"
			"\n\t\t\t\tif (!s1)"
			"\n\t\t\t\t\tin->ui.ac->free_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), s);"
			"\n\t\t\t}"
			"\n\t\t\treturn NULL;"
			"\n\t\t}"
			"\n\t}");
	}
	else if (s->equiv_many) {
		if (!s_fixed(s)) {
			/* NOTE: may cast 'sizeof(*s)/sizeof(s->xxx)' to bridge_counter_t
			  - structure 's' packed size was statically checked above */
			FR1("\n\tinfo.u.mem = _bridge_unpack_%s_array_(info.u.mem, info.u.lim,"
				"\n\t\t(bridge_counter_t)(sizeof(*s)/sizeof(s->", s->equivalent);
			print_equivalent_path_s(file, s);
			FP(")), &s->");
			print_equivalent_path_s(file, s);
			FP(");"
				"\n\tif (_br_unlikely(!info.u.mem)) {"
				"\n\t\tif (!s1 && in->ui.ac->free_cb)"
				"\n\t\t\tin->ui.ac->free_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), s);"
				"\n\t\treturn NULL;"
				"\n\t}");
		}
		else {
			FR2("\n\tif (!s) {"
				"\n\t\ts = (struct %s*)in->ui.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), info.u.mem, sizeof(*s), %s);"
				"\n\t\tif (_br_unlikely(!s))"
				"\n\t\t\treturn NULL;"
				"\n\t}"
				"\n\t{"
				"\n\t\tconst char *m = info.u.mem;"
				"\n\t\tinfo.u.mem = m + sizeof(*s);"
				"\n\t\t", s->s_name, get_alloc_type_for_layout(s->s_layout, /*copy:*/0));
			if (field_type_byte != s->equivalent) {
				FR1("if (in->ui.convert)"
					"\n\t\t\tget_exchange_%s_array(m, &s->", s->equivalent);
				print_equivalent_path_s(file, s);
				FP(", sizeof(*s)/sizeof(s->");
				print_equivalent_path_s(file, s);
				FP("));"
					"\n\t\telse ");
			}
			FP("if (m != (const char*)s)");
			if (s->packed_bytes > BR_MEMCPY_MAX_SIZE)
				FP("\n\t\t\tBRIDGE_MEMCPY(s, m, sizeof(*s));");
			else
				FR1("\n\t\t\t_br_memcpy%u(s, m);", s->packed_bytes);
			FP("\n\t}");
		}
	}
	else if (is_simple_str_type(s->equivalent)) {
		FR1("\n\tinfo.u.mem = _bridge_unpack_%s_(info.u.mem, info.u.lim, &s->", s->equivalent);
		print_equivalent_path_s(file, s);
		FP(", BRIDGE_PASS_ALLOCATOR(in->ui.ac));"
			"\n\tif (_br_unlikely(!info.u.mem)) {"
			"\n\t\tif (!s1 && in->ui.ac->free_cb)"
			"\n\t\t\tin->ui.ac->free_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), s);"
			"\n\t\treturn NULL;"
			"\n\t}");
	}
	else if (!s_fixed(s)) {
		FR1("\n\tinfo.u.mem = _bridge_unpack_%s(info.u.mem, info.u.lim, &s->", s->equivalent);
		print_equivalent_path_s(file, s);
		FP(");"
			"\n\tif (_br_unlikely(!info.u.mem)) {"
			"\n\t\tif (!s1 && in->ui.ac->free_cb)"
			"\n\t\t\tin->ui.ac->free_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), s);"
			"\n\t\treturn NULL;"
			"\n\t}");
	}
	else {
		FR2("\n\tif (!s) {"
			"\n\t\ts = (struct %s*)in->ui.ac->alloc_cb(BRIDGE_PASS_ALLOCATOR(in->ui.ac), info.u.mem, sizeof(*s), %s);"
			"\n\t\tif (_br_unlikely(!s))"
			"\n\t\t\treturn NULL;"
			"\n\t}", s->s_name, get_alloc_type_for_layout(s->s_layout, /*copy:*/0));
		FR1("\n\tif (%sinfo.u.mem != (const char*)s)"
			"\n\t\ts->", field_type_byte == s->equivalent ? "" : "in->ui.convert || ");
		print_equivalent_path_s(file, s);
		FR2(" = _bridge_get_%s_no_lim(&info.u.mem%s);"
			"\n\telse"
			"\n\t\tinfo.u.mem = (const char*)(s + 1);",
			s->equivalent, field_type_byte == s->equivalent ? "" : ", in->ui.convert");
	}
	if (s_has_bytes(s))
		FP("\n\tin->ui.us.from = info.u.mem;");
	else if (bits_have_fixed_bytes(s))
		FR1("\n\tin->ui.us.from = info.u.bits + %uu;", bytes_for_bits(s->packed_bits/* + s->packed_optional_bits*/));
	else {
		/* no packed bytes - packed bits count must be non-zero */
		FP("\n\tBRIDGE_ASSERT(info.u.bit_count);"
			"\n\tin->ui.us.from = info.u.bits + bridge_nonzero_bit_array_size(info.u.bit_count);");
	}
	FP("\n\treturn s;"
		"\n}"
		"\n");
	generate_prototype_unpack(file, s, decl);
	FR2("\n{"
		"\n\tstruct %s *ret = _%s_unpack_to_(s, in BRIDGE_DEBUG_ARGS_PASS);"
		"\n\tbridge_allocator_dec_level(in->ui.ac);"
		"\n\treturn ret;"
		"\n}"
		"\n", s->s_name, s->s_name);
	return gotoz;
}
