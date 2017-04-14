/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* jpack_gen.c */

#include <stdlib.h>
#include <stdio.h>

#include "ptypes.h"
#include "jcompiler.h"
#include "model.h"

#define WHL while ((void)0,0)
#define FP(s)                                          do {if (file) fputs(s,file);} WHL
#define FR1(s,a1)                                      do {if (file) fprintf(file,s,a1);} WHL
#define FR2(s,a1,a2)                                   do {if (file) fprintf(file,s,a1,a2);} WHL
#define FR3(s,a1,a2,a3)                                do {if (file) fprintf(file,s,a1,a2,a3);} WHL
#define FR4(s,a1,a2,a3,a4)                             do {if (file) fprintf(file,s,a1,a2,a3,a4);} WHL
#define FR5(s,a1,a2,a3,a4,a5)                          do {if (file) fprintf(file,s,a1,a2,a3,a4,a5);} WHL
#define FR6(s,a1,a2,a3,a4,a5,a6)                       do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6);} WHL
#define FR7(s,a1,a2,a3,a4,a5,a6,a7)                    do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7);} WHL
#define FR9(s,a1,a2,a3,a4,a5,a6,a7,a8,a9)              do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9);} WHL
#define FR10(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1)          do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1);} WHL
#define FR12(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3)    do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3);} WHL

static void generate_prototype_variable_size(FILE *file, const struct struct_def *s)
{
	FR5("\n\tstatic %s _%s_var_size(%s s%s%s) throws BridgeException {",
		(v_bits(s) && v_bytes(s)) ? "long" : "int", s->s_name, s->s_name,
		v_bytes(s) ? ", int byte_count" : "", v_bits(s) ? ", int bit_count" : "");
}

enum CALL_FROM {
	CALL_FROM_ARRAY,
	CALL_FROM_FIXED_ARRAY,
	CALL_FROM_OPTIONAL,
	CALL_FROM_STATIC
};

static void print_call_from(FILE *file, const struct field_def *f, enum CALL_FROM from)
{
	switch (from) {
		case CALL_FROM_ARRAY:       FR1("s.%s[i], ", f->name); return;
		case CALL_FROM_FIXED_ARRAY: FR1("s.%s[i], ", f->name); return;
		case CALL_FROM_OPTIONAL:    FR1("s.%s, ", f->name);    return;
		case CALL_FROM_STATIC:      FR1("s.%s, ", f->name);    return;
	}
}

/* called from generate_code_var_size() */
static int call_var_size_user_type(FILE *file, const struct struct_def *s/*!=NULL for tail call*/,
	const struct field_def *f, enum CALL_FROM from, int need_block, const char *o)
{
	const char *offs = o;
	if (CALL_FROM_ARRAY == from || CALL_FROM_FIXED_ARRAY == from) {
		if (CALL_FROM_ARRAY == from && v_bits(f->user_type) && v_bytes(f->user_type))
			FP(" {");
		offs = o[0] ? o[1] ? "\t\t\t\t" : "\t\t\t" : "\t\t";
	}
	else if (CALL_FROM_OPTIONAL == from && need_block != 2)
		offs = o[0] ? o[1] ? "\t\t\t" : "\t\t" : "\t";
	if (!s || v_bytes(s) != v_bytes(f->user_type) || v_bits(s) != v_bits(f->user_type)) {
		if (v_bits(f->user_type)) {
			if (v_bytes(f->user_type)) {
				if (need_block) {
					if (CALL_FROM_OPTIONAL == from && need_block != 2)
						offs = o[0] ? o[1] ? "\t\t\t\t" : "\t\t\t" : "\t\t";
					else
						offs = o[0] ? o[1] ? "\t\t\t" : "\t\t" : "\t";
					FR1("\n%s\t{", offs);
				}
				FR1("\n%s\t\tlong bb", offs);
			}
			else
				FR1("\n%s\t\tbit_count", offs);
		}
		else
			FR1("\n%s\t\tbyte_count", offs);
		FR1(" = _%s_var_size(", f->type);
	}
	else
		FR2("\n%s\t\treturn _%s_var_size(", offs, f->type);
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
		if (v_bits(f->user_type) && v_bytes(f->user_type)) {
			FR2("\n%s\t\tbyte_count = _bridge_bits_bytes_get_bytes(bb);"
				"\n%s\t\tbit_count = _bridge_bits_bytes_get_bits(bb);", offs, offs);
			if (need_block || CALL_FROM_ARRAY == from)
				FR1("\n%s\t}", offs);
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

static int switch_var_size(FILE *file, const struct struct_def *s/*!=NULL for tail call*/, const struct field_def *f, const char *offs)
{
	int returned = 0;
	switch (f->f_power) {
		case F_ARRAY:
			FR3("\n%s\t\t/* %s: %s[] */", offs, f->name, f->type);
			if (f->user_type) {
				FR4("\n%s\t\tif (null != s.%s) {"
					"\n%s\t\t\tif (s.%s.length > BRIDGE_MAX", offs, f->name, offs, f->name);
				if (f->user_type->packed_bytes > 1)
					FR1("/%u", f->user_type->packed_bytes);
				if (f->user_type->packed_bits > 1)
					FR2(" || s.%s.length > (int)(BRIDGE_BIT_COUNT_MAX/0x%xL)", f->name, f->user_type->packed_bits/*>1*/);
				FR1(")"
					"\n%s\t\t\t\tthrow new BridgeTooManyElements();", offs);
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR2("\n%s\t\t\tbyte_count = _br_of_add_bytes(byte_count, bridge_counter_packed_size_minus_one(s.%s.length)", offs, f->name);
				if (f->user_type->packed_bytes) {
					/* NOTE: it's always safe to add without overflow counter packed size to value <= BRIDGE_MAX */
					FR1(" + s.%s.length", f->name);
					if (f->user_type->packed_bytes > 1)
						FR1("*%u", f->user_type->packed_bytes);
				}
				FP(");");
				if (f->user_type->packed_bits) {
					FR2("\n%s\t\t\tbit_count = _br_of_add_bits(bit_count, s.%s.length", offs, f->name);
					if (f->user_type->packed_bits > 1)
						FR1("*0x%x", f->user_type->packed_bits);
					FP(");");
				}
				if (!s_fixed(f->user_type)) {
					FR2("\n%s\t\t\tfor (int i = 0; i < s.%s.length; i++)", offs, f->name);
					(void)call_var_size_user_type(file, NULL, f, CALL_FROM_ARRAY, /*need_block:*/0, offs);
				}
				FR1("\n%s\t\t}", offs);
			}
			else if (field_type_bit == f->type) {
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR2("\n%s\t\tbyte_count = bridge_add_bit_array_packed_size_(byte_count, s.%s_bit_count);", offs, f->name);
			}
			else {
				/* array counter packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR3("\n%s\t\tbyte_count = bridge_add_%s_array_packed_size_(byte_count, s.%s);", offs, f->type, f->name);
			}
			break;
		case F_OPTIONAL:
			FR3("\n%s\t\t/* %s: %s* */", offs, f->name, f->type);
			if (f->user_type) {
				FR2("\n%s\t\tif (null != s.%s) {", offs, f->name);
				if (f->user_type->packed_bytes)
					FR2("\n%s\t\t\tbyte_count = _br_of_add_bytes(byte_count, %u);", offs, f->user_type->packed_bytes);
				if (1u == f->user_type->packed_bits)
					FR1("\n%s\t\t\tbit_count = _br_of_inc_bits(bit_count);", offs);
				else if (f->user_type->packed_bits)
					FR2("\n%s\t\t\tbit_count = _br_of_add_bits(bit_count, 0x%x);", offs, f->user_type->packed_bits);
				if (!s_fixed(f->user_type)) {
					(void)call_var_size_user_type(file, s, f, CALL_FROM_OPTIONAL,
						/*need_block:*/f->user_type->packed_bytes || f->user_type->packed_bits, offs);
				}
				FR1("\n%s\t\t}", offs);
			}
			else if (is_simple_str_type(f->type)) {
				FR5("\n%s\t\tif (null != s.%s)"
					"\n%s\t\t\tbyte_count = _br_of_add_bytes(byte_count, _bridge_%s_packed_size__(s.%s) + 1);",
					offs, f->name, offs, f->type, f->name);
			}
			else {
				FR3("\n%s\t\tif (0 != (s._bit_int%u & 0x%x))", offs, BIT_INT(f->bit), BIT_NUM(f->bit));
				if (field_type_bit == f->type)
					FR1("\n%s\t\t\tbit_count = _br_of_inc_bits(bit_count);", offs);
				else if (is_variabe_int_type(f->type))
					FR3("\n%s\t\t\tbyte_count = _br_of_add_bytes(byte_count, bridge_%s_packed_size(s.%s));", offs, f->type, f->name);
				else
					FR2("\n%s\t\t\tbyte_count = _br_of_add_bytes(byte_count, %u);", offs, simple_fixed_int_size(f->type));
			}
			break;
		case F_REQUIRED:
			if (f->user_type ? !s_fixed(f->user_type) : is_variable_simple_type(f->type))
				FR3("\n%s\t\t/* %s: %s */", offs, f->name, f->type);
			if (f->user_type) {
				if (!s_fixed(f->user_type))
					returned = call_var_size_user_type(file, s, f, CALL_FROM_STATIC, /*need_block:*/1, offs);
			}
			else if (is_simple_str_type(f->type)) {
				/* required string packed size >= 1, 1 byte is already counted in s->packed_bytes */
				FR3("\n%s\t\tbyte_count = _br_of_add_bytes(byte_count, _bridge_%s_packed_size_(s.%s));", offs, f->type, f->name);
			}
			else if (is_variabe_int_type(f->type)) {
				/* variable-packed integer size >= 1, 1 byte is already counted in s->packed_bytes */
				FR3("\n%s\t\tbyte_count = _br_of_add_bytes(byte_count, bridge_%s_packed_size(s.%s) - 1);", offs, f->type, f->name);
			}
			break;
		case F_POINTER:
		default:
			if (f->user_type ? !s_fixed(f->user_type) : is_variable_simple_type(f->type))
				FR4("\n%s\t\t/* %s: %s[%u] */", offs, f->name, f->type, (F_POINTER == f->f_power) ? 1u : f->f_power);
			if (f->user_type) {
				if (!s_fixed(f->user_type)) {
					/* required bytes and bits are already counted in s->packed_bytes and s->packed_bits */
					if (F_POINTER == f->f_power)
						returned = call_var_size_user_type(file, s, f, CALL_FROM_OPTIONAL, /*need_block:*/2, offs);
					else {
						FR3("\n%s\t\t{"
							"\n%s\t\t\tint i = 0;"
							"\n%s\t\t\tdo {", offs, offs, offs);
						(void)call_var_size_user_type(file, NULL, f, CALL_FROM_FIXED_ARRAY, /*need_block:*/0, offs);
						FR3("\n%s\t\t\t} while (++i < %u);"
							"\n%s\t\t}", offs, f->f_power, offs);
					}
				}
			}
			else if (is_variable_simple_type(f->type)) {
				if (F_POINTER != f->f_power) {
					/* array packed size >= elements count, elements count bytes are already counted in s->packed_bytes */
					FR5("\n%s\t\tbyte_count = _bridge_add_%s_array_packed_size(byte_count - %u, s.%s, %u);",
						offs, f->type, f->f_power, f->name, f->f_power);
				}
				else if (is_simple_str_type(f->type)) {
					/* required string packed size >= 1, 1 byte is already counted in s->packed_bytes */
					FR3("\n%s\t\tbyte_count = _br_of_add_bytes(byte_count, _bridge_%s_packed_size_(s.%s[0]));", offs, f->type, f->name);
				}
				else {
					/* required 1 byte of unfixed packed size is already counted in s->packed_bytes */
					FR3("\n%s\t\tbyte_count = _br_of_add_bytes(byte_count, bridge_%s_packed_size(s.%s[0]) - 1);", offs, f->type, f->name);
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
		const char *enum_type = ((size_t)(structs_end - structs) <= 0xFFFFFFFF) ? "int" : "long";
		const char *enum_suff = ((size_t)(structs_end - structs) <= 0xFFFFFFFF) ? "" : "L";
		{
			size_t i = 0;
			const struct struct_def *r = s;
			do {
				FR4("\n\tstatic final %s V_%s = 0x%llx%s;", enum_type, r->s_name, (unsigned long long)i++, enum_suff);
				r = r->tail_recursive->user_type;
			} while (r != s);
		}
		FR5("\n"
			"\n\t@SuppressWarnings(\"fallthrough\")"
			"\n\tstatic %s _%s_var(Object p, %s t%s%s) throws BridgeException {",
			(v_bits(s) && v_bytes(s)) ? "long" : "int", s->s_name, enum_type,
			v_bytes(s) ? ", int byte_count" : "", v_bits(s) ? ", int bit_count" : "");
	}
	else
		generate_prototype_variable_size(file, s);
	if (s->tail_recursive) {
		if (long_loop) {
			FP("\n\t\tf: for (;;) switch (t) {"
				"\n\tdefault:");
		}
		else
			FP("\n\t\tfor (;;) {");
	}
	{
		const char *offs = long_loop ? "\t\t" : s->tail_recursive ? "\t" : "";
		const struct struct_def *r = s;
		do {
			if (long_loop) {
				FR3("\n\tcase V_%s: {"
					"\n\t\t\t\t%s s = (%s)p;", r->s_name, r->s_name, r->s_name);
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
				FR4("\n%s\t\t/* %s: %s%s (tail recursive) */",
					offs, r->tail_recursive->name, r->tail_recursive->type,
					F_POINTER == r->tail_recursive->f_power ? "[1]" :
					F_OPTIONAL == r->tail_recursive->f_power ? "*" : "");
				{
					int break_else = 0;
					if (long_loop) {
						FR1("\n\t\t\t\tp = s.%s;"
							"\n\t\t\t}", r->tail_recursive->name);
						if (F_OPTIONAL == r->tail_recursive->f_power) {
							if (s != r->tail_recursive->user_type) {
								if (r->tail_recursive->user_type->packed_bytes || r->tail_recursive->user_type->packed_bits) {
									FP("\n\t\t\tif (null != p)");
									break_else = 1;
									if (r->tail_recursive->user_type->packed_bytes && r->tail_recursive->user_type->packed_bits) {
										FP(" {");
										break_else = 2;
									}
								}
								else {
									FP("\n\t\t\tif (null == p)"
										"\n\t\t\t\tbreak f;");
								}
							}
							else {
								FP("\n\t\t\tif (null != p)");
								break_else = 1;
								if (s->packed_bytes || s->packed_bits) {
									FP(" {");
									break_else = 2;
								}
								FR1("\n\t\t\t\tt = V_%s;", s->s_name);
							}
						}
					}
					else {
						FR1("\n\t\t\ts = s.%s;", s->tail_recursive->name);
						if (s->packed_bytes || s->packed_bits) {
							FP("\n\t\t\tif (null != s)");
							break_else = 1;
							if (s->packed_bytes && s->packed_bits) {
								FP(" {");
								break_else = 2;
							}
						}
						else {
							FP("\n\t\t\tif (null == s)"
								"\n\t\t\t\tbreak;");
						}
					}
					if (F_OPTIONAL == r->tail_recursive->f_power) {
						if (r->tail_recursive->user_type->packed_bytes)
							FR1("\n\t\t\t\tbyte_count = _br_of_add_bytes(byte_count, %u);", r->tail_recursive->user_type->packed_bytes);
						if (1u == r->tail_recursive->user_type->packed_bits)
							FP("\n\t\t\t\tbit_count = _br_of_inc_bits(bit_count);");
						else if (r->tail_recursive->user_type->packed_bits)
							FR1("\n\t\t\t\tbit_count = _br_of_add_bits(bit_count, 0x%x);", r->tail_recursive->user_type->packed_bits);
						if (break_else) {
							if (2 == break_else)
								FP("\n\t\t\t}");
							FR1("\n\t\t\telse"
								"\n\t\t\t\tbreak%s;", long_loop ? " f" : "");
						}
					}
				}
				r = r->tail_recursive->user_type;
			}
		} while (r != s);
	}
	if (s->tail_recursive)
		FP("\n\t\t}");
	if (!returned) {
		if (v_bits(s)) {
			if (v_bytes(s))
				FP("\n\t\treturn _bridge_bits_bytes_from_bits_and_bytes(bit_count, byte_count);");
			else
				FP("\n\t\treturn bit_count;");
		}
		else
			FP("\n\t\treturn byte_count;");
	}
	FP("\n\t}"
		"\n");
}

static const char *_get_packed_info(const struct struct_def *s)
{
	if (s->convertable) {
		if (s_has_bits(s))
			return "_bits_mem_conv_info";
		return "_mem_conv_info";
	}
	if (s_has_bytes(s)) {
		if (s_has_bits(s))
			return "_bits_mem_info";
		return "_mem_info";
	}
	return "_bits_info";
}

static void generate_prototype_append_packed(FILE *file, const struct struct_def *s)
{
	FR3("\n\tstatic void _%s_append_packed(%s s, _pk%s info) {", s->s_name, s->s_name, _get_packed_info(s));
}

static void switch_app_packed(FILE *file, const struct field_def *f, const char *offs)
{
	switch (f->f_power) {
		case F_ARRAY:
			FR3("\n%s\t\t/* %s: %s[] */", offs, f->name, f->type);
			if (f->user_type) {
				FR12("\n%s\t\tif (null != s.%s) {"
					"\n%s\t\t\tinfo.at = _bridge_pack_counter(info.mem, info.at, s.%s.length);"
					"\n%s\t\t\tfor (int i = 0; i < s.%s.length; i++)"
					"\n%s\t\t\t\t_%s_append_packed(s.%s[i], info);"
					"\n%s\t\t}"
					"\n%s\t\telse"
					"\n%s\t\t\tinfo.at++;",
					offs, f->name, offs, f->name, offs, f->name, offs, f->type, f->name, offs, offs, offs);
			}
			else if (field_type_bit == f->type)
				FR3("\n%s\t\tinfo.at = bridge_pack_bit_array(info.mem, info.at, s.%s, s.%s_bit_count);", offs, f->name, f->name);
			else if (is_variable_simple_type(f->type) || field_type_byte == f->type)
				FR3("\n%s\t\tinfo.at = bridge_pack_%s_array(info.mem, info.at, s.%s);", offs, f->type, f->name);
			else
				FR3("\n%s\t\tinfo.at = bridge_pack_%s_array(info.mem, info.at, s.%s, info.big);", offs, f->type, f->name);
			break;
		case F_POINTER:
		case F_OPTIONAL:
		case F_REQUIRED:
			if (F_POINTER != f->f_power || f->user_type || field_type_bit != f->type) {
				const char *of        = (F_OPTIONAL == f->f_power) ? offs[0] ? offs[1] ? "\t\t\t" : "\t\t" : "\t" : offs;
				const char *deref_ptr = (F_POINTER  == f->f_power) ? "[0]" : "";
				FR4("\n%s\t\t/* %s: %s%s */", offs, f->name, f->type,
					(F_POINTER == f->f_power) ? "[1]" : (F_OPTIONAL == f->f_power) ? "*" : "");
				if (f->user_type) {
					if (F_OPTIONAL == f->f_power) {
						FR4("\n%s\t\tif (null != s.%s) {"
							"\n%s\t\t\t_bridge_set_bit(info.mem, info.bits, info.bit_count%s);",
							offs, f->name, offs, s_has_bits(f->user_type) ? "++" : "");
					}
					FR3("\n%s\t\t_%s_append_packed(s.%s, info);", of, f->type, f->name);
				}
				else if (is_simple_str_type(f->type)) {
					if (F_OPTIONAL == f->f_power) {
						FR6("\n%s\t\tif (null != s.%s) {"
							"\n%s\t\t\t_bridge_set_bit(info.mem, info.bits, info.bit_count);"
							"\n%s\t\t\tinfo.at = _bridge_pack_%s(info.mem, info.at, s.%s);", offs, f->name, offs, offs, f->type, f->name);
					}
					else
						FR4("\n%s\t\tinfo.at = bridge_pack_%s(info.mem, info.at, s.%s%s);", offs, f->type, f->name, deref_ptr);
				}
				else {
					if (F_OPTIONAL == f->f_power) {
						FR5("\n%s\t\tif (0 != (s._bit_int%u & 0x%x)) {"
							"\n%s\t\t\t_bridge_set_bit(info.mem, info.bits, info.bit_count%s);",
							offs, BIT_INT(f->bit), BIT_NUM(f->bit), offs, (field_type_bit == f->type) ? "++" : "");
					}
					if (field_type_bit == f->type) {
						unsigned bit = f->bit + ((F_OPTIONAL == f->f_power) ? 1u : 0u);
						FR4("\n%s\t\tif (0 != (s._bit_int%u & 0x%x))"
							"\n%s\t\t\t_bridge_set_bit(info.mem, info.bits, info.bit_count);", of, BIT_INT(bit), BIT_NUM(bit), of);
					}
					else if (is_variabe_int_type(f->type))
						FR4("\n%s\t\tinfo.at = _bridge_pack_%s(info.mem, info.at, s.%s%s);", of, f->type, f->name, deref_ptr);
					else {
						FR5("\n%s\t\tinfo.at = _bridge_put_%s(info.mem, info.at, s.%s%s%s);",
							of, f->type, f->name, deref_ptr, field_type_byte == f->type ? "" : ", info.big");
					}
				}
				if (F_OPTIONAL == f->f_power) {
					FR2("\n%s\t\t}"
						"\n%s\t\t", offs, offs);
					if (f->user_type && s_has_bits(f->user_type)) {
						FR1("else"
							"\n%s\t\t\t", offs);
					}
				}
				if (F_OPTIONAL == f->f_power || (!f->user_type && field_type_bit == f->type)) {
					if (F_OPTIONAL != f->f_power)
						FR1("\n%s\t\t", offs);
					FP("info.bit_count++;");
				}
				break;
			}
			/* F_POINTER for field_type_bit, fall through */
		default:
			FR4("\n%s\t\t/* %s: %s[%u] */", offs, f->name, f->type, f->f_power);
			if (f->user_type) {
				FR9("\n%s\t\t{"
					"\n%s\t\t\tint i = 0;"
					"\n%s\t\t\tdo {"
					"\n%s\t\t\t\t_%s_append_packed(s.%s[i], info);"
					"\n%s\t\t\t} while (++i < %u);"
					"\n%s\t\t}", offs, offs, offs, offs, f->type, f->name, offs, f->f_power, offs);
			}
			else if (field_type_bit == f->type)
				FR3("\n%s\t\tinfo.at = _bridge_put_bytes(info.mem, info.at, s.%s, %u);", offs, f->name, bytes_for_bits(f->f_power));
			else if (is_variable_simple_type(f->type))
				FR4("\n%s\t\tinfo.at = _bridge_pack_%s_array(info.mem, info.at, s.%s, %u);", offs, f->type, f->name, f->f_power);
			else if (field_type_byte == f->type)
				FR3("\n%s\t\tinfo.at = _bridge_put_bytes(info.mem, info.at, s.%s, %u);", offs, f->name, f->f_power);
			else
				FR4("\n%s\t\tinfo.at = _bridge_put_%s_array(info.mem, info.at, s.%s, %u, info.big);", offs, f->type, f->name, f->f_power);
			break;
	}
}

static void generate_code_app_packed(FILE *file, const struct struct_def *const s)
{
	const int long_loop = (s->tail_recursive && s->tail_recursive->user_type != s);
	if (long_loop) {
		const char *enum_type = ((size_t)(structs_end - structs) <= 0xFFFFFFFF) ? "int" : "long";
		FR3("\n\t@SuppressWarnings(\"fallthrough\")"
			"\n\tstatic void _%s_app(Object p, %s t, _pk%s info) {",
			s->s_name, enum_type, _get_packed_info(s));
	}
	else
		generate_prototype_append_packed(file, s);
	if (s->tail_recursive) {
		if (long_loop) {
			FP("\n\t\tf: for (;;) switch (t) {"
				"\n\tdefault:");
		}
		else
			FP("\n\t\tfor (;;) {");
	}
	{
		const char *offs = long_loop ? "\t\t" : s->tail_recursive ? "\t" : "";
		const struct struct_def *r = s;
		/* NOTE: just pack: all sizes have already been checked while determining packed size */
		do {
			if (long_loop) {
				FR3("\n\tcase V_%s: {"
					"\n\t\t\t\t%s s = (%s)p;", r->s_name, r->s_name, r->s_name);
			}
			{
				struct field_def **f = r->fields;
				for (; f < r->fields_end; f++) {
					if (r->tail_recursive == *f)
						continue; /* tail-recursive field will be processed last */
					switch_app_packed(file, *f, offs); /* NOTE: use 's' (not 'r') to work with arguments of generated function */
				}
			}
			if (r->tail_recursive) {
				FR4("\n%s\t\t/* %s: %s%s (tail recursive) */",
					offs, r->tail_recursive->name, r->tail_recursive->type,
					F_POINTER == r->tail_recursive->f_power ? "[1]" :
					F_OPTIONAL == r->tail_recursive->f_power ? "*" : "");
				if (long_loop) {
					FR1("\n\t\t\t\tp = s.%s;"
						"\n\t\t\t}", r->tail_recursive->name);
					if (F_OPTIONAL == r->tail_recursive->f_power) {
						if (s != r->tail_recursive->user_type) {
							FP("\n\t\t\tif (null != p)"
								"\n\t\t\t\t_bridge_set_bit(info.mem, info.bits, info.bit_count++);");
						}
						else {
							FR1("\n\t\t\tif (null != p) {"
								"\n\t\t\t\t_bridge_set_bit(info.mem, info.bits, info.bit_count++);"
								"\n\t\t\t\tt = V_%s;"
								"\n\t\t\t}", s->s_name);
						}
						FP("\n\t\t\telse"
								"\n\t\t\t\tbreak f;");
					}
				}
				else {
					FR1("\n\t\t\ts = s.%s;"
						"\n\t\t\tif (null != s)"
						"\n\t\t\t\t_bridge_set_bit(info.mem, info.bits, info.bit_count++);"
						"\n\t\t\telse"
						"\n\t\t\t\tbreak;", r->tail_recursive->name);
				}
				r = r->tail_recursive->user_type;
			}
		} while (r != s);
	}
	if (s->tail_recursive) {
		FP("\n\t\t}"
			"\n\t\tinfo.bit_count++;");
	}
	FP("\n\t}"
		"\n");
}

/* check if generated _%s_var_size() function may throw NullPointerException or ArrayIndexOutOfBoundsException exceptions */
/* check if generated _%s_append_packed() function may throw NullPointerException or ArrayIndexOutOfBoundsException exceptions */
/* NOTE: generated code must not throw unchecked exceptions,
  just after new structure was allocated, user may forget to fill required fields or fill them incorrectly:
 1) set bit_count to non-zero, but forget to allocate corresponding bits array,
 2) allocate required array of size less than needed,
 3) allocate array of user-types containing nulls */
void check_unchecked_exceptions(void)
{
	/* NOTE: assume initially (*s)->_aux, (*s)->_marked and (*s)->_processed are zero */
	for (;;) {
		int _marked_set = 0;
		int _processed_set = 0;
		int _marked_unknown = 0;
		int _processed_unknown = 0;
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			/* check if structure is not yet finally defined */
			if (!(*s)->_aux) {
				int m_unknown = 0;
				int p_unknown = 0;
				struct field_def *const *f = (*s)->fields;
				for (; f < (*s)->fields_end; f++) {
					switch ((*f)->f_power) {
						case F_OPTIONAL:
							if ((*f)->user_type) {
								/* inherit flags */
								if (!(*s)->_marked && (*f)->user_type->_marked) {
									(*s)->_marked = 1; /* inherit flag */
									_marked_set = 1; /* (*s)->_marked was changed */
									if ((*s)->_processed)
										break;
								}
								if (!(*s)->_processed && (*f)->user_type->_processed) {
									(*s)->_processed = 1; /* inherit flag */
									_processed_set = 1; /* (*s)->_processed was changed */
									if ((*s)->_marked)
										break;
								}
								if (!(*f)->user_type->_aux) {
									if (!(*s)->_marked)
										m_unknown = 1; /* not known yet - will be known after (*f)->user_type get defined */
									if (!(*s)->_processed)
										p_unknown = 1; /* not known yet - will be known after (*f)->user_type get defined */
								}
							}
							continue;
						case F_ARRAY:
						case F_REQUIRED:
							if (!(*f)->user_type)
								continue;
							/* fall through */
						case F_POINTER:
							if ((*f)->user_type) {
								if (!(*s)->_marked) {
									/* NullPointerException may be thrown for a null in array or if required field is null */
									(*s)->_marked = 1;
									_marked_set = 1; /* (*s)->_marked was changed */
									if ((*s)->_processed)
										break;
								}
								/* ((*s)->_processed == 0, ((*s)->_marked == 1 */
								if ((*f)->user_type->_processed) {
									(*s)->_processed = 1; /* inherit flag */
									_processed_set = 1; /* (*s)->_processed was changed */
									break;
								}
								if (!(*f)->user_type->_aux)
									p_unknown = 1; /* not known yet - will be known after (*f)->user_type get defined */
								continue;
							}
							/* fall through */
						default:
							if (!(*s)->_marked) {
								/* NullPointerException may be thrown if required array is null */
								(*s)->_marked = 1;
								_marked_set = 1; /* (*s)->_marked was changed */
							}
							if (!(*s)->_processed) {
								/* ArrayIndexOutOfBoundsException may be thrown if required array is empty */
								(*s)->_processed = 1;
								_processed_set = 1; /* (*s)->_processed was changed */
							}
							break;
					}
					/* (*s) defined: (*s)->_processed == 1, (*s)->_marked == 1 */
					(*s)->_aux = 1; /* defined */
					break;
				}
				if (!(*s)->_aux) {
					if (!m_unknown && !p_unknown)
						(*s)->_aux = 1; /* defined: no, cannot throw */
					else {
						if (m_unknown)
							_marked_unknown = 1;
						if (p_unknown)
							_processed_unknown = 1;
					}
				}
			}
		}
		if (!(_marked_unknown && _marked_set) && !(_processed_unknown && _processed_set))
			break;
	}
}

/* must be called after check_unchecked_exceptions() */
void generate_code_pack(FILE *file, const struct struct_def *s)
{
	if (!s_fixed(s)) {
		if (!s->tail_recursive || s->recursion_entry)
			generate_code_var_size(file, s);
		if (s->tail_recursive && s->tail_recursive->user_type != s) {
			const struct struct_def *r = s;
			while (!r->recursion_entry)
				r = r->tail_recursive->user_type;
			generate_prototype_variable_size(file, s);
			FR4("\n\t\treturn _%s_var(s, V_%s%s%s);"
				"\n\t}"
				"\n", r->s_name, s->s_name, v_bytes(s) ? ", byte_count" : "", v_bits(s) ? ", bit_count" : "");
		}
	}
	if (!s->tail_recursive || s->recursion_entry)
		generate_code_app_packed(file, s);
	if (s->tail_recursive && s->tail_recursive->user_type != s) {
		const struct struct_def *r = s;
		while (!r->recursion_entry)
			r = r->tail_recursive->user_type;
		generate_prototype_append_packed(file, s);
		FR2("\n\t\t_%s_app(s, V_%s, info);"
			"\n\t}"
			"\n", r->s_name, s->s_name);
	}
	/* note: _pk_mem_info constructor may throw BridgeBigReserves */
	FR3("\n\tstatic byte[] %s_pack(%s s, int head, int tail%s) throws BridgeException {",
		s->s_name, s->s_name, s->convertable ? ", boolean big" : "");
	{
		const char *offs = (s->_marked || s->_processed) ? "\t" : "";
		unsigned szl = s_packed_size_limit(s); /* non-zero if structure packed size is limited by szl */
		unsigned initial_bytes = s->packed_bytes; /* initial_bytes <= BRIDGE_MODEL_MAX_PACKED_SIZE (checked in model.c) */
		if (bits_have_fixed_bytes(s))
			initial_bytes += bytes_for_bits(s->packed_bits); /* initial_bytes <= BRIDGE_MODEL_MAX_PACKED_SIZE after addition */
		if (s->_marked || s->_processed)
			FP("\n\t\ttry {");
		/* ignore computed value of bit_count if number of bytes for bits is known and is already accounted in initial_bytes;
		   NOTE: even in this case computed total number of bits may overflow (for const bit count 0xFFFFFFFF and 1 variable bit) */
		if (v_bytes(s) && v_bits(s)) {
			/* variable bytes & bits */
			FR5("\n%s\t\tlong bb = _%s_var_size(s, %u, 0x%x);"
				"\n%s\t\tint packed_size = _bridge_bits_bytes_get_bytes(bb);",
				offs, s->s_name, initial_bytes, s->packed_bits ^ 0x80000000, offs);
			if (!bits_have_fixed_bytes(s)) {
				FR2("\n%s\t\tint bits_size = bridge_bit_array_size%s(_bridge_bits_bytes_get_bits(bb) ^ 0x80000000);",
					offs, s->packed_bits ? "_nz" : "");
				if (szl && szl <= 0x7FFFFFFFu)
					FR1("\n%s\t\tpacked_size += bits_size + bridge_counter_packed_size(bits_size);", offs);
				else
					FR1("\n%s\t\tpacked_size = _br_of_add_bytes(packed_size, bits_size + bridge_counter_packed_size(bits_size));", offs);
			}
		}
		else if (!bits_have_fixed_bytes(s)) {
			/* variable bits, fixed bytes */
			/* NOTE: s->packed_bits is non-zero:
			  the only case when s->packed_bits may be zero - for an array of user-types which pack only bits,
			  but array counter packed size is variable, and here we have fixed number of packed bytes */
			FR5("\n%s\t\tint bits_size = bridge_bit_array_size%s(_%s_var_size(s, 0x%x) ^ 0x80000000);"
				"\n%s\t\tint packed_size = ", offs, s->packed_bits ? "_nz" : "", s->s_name, s->packed_bits ^ 0x80000000, offs);
			if (szl && szl <= 0x7FFFFFFFu)
				FR1("%u + bits_size;", s->packed_bytes);
			else
				FR1("_br_of_add_bytes(%u, bits_size);", s->packed_bytes);
		}
		else if (v_bytes(s)) {
			/* variable bytes, fixed bits */
			FR3("\n%s\t\tint packed_size = _%s_var_size(s, %u);", offs, s->s_name, initial_bytes);
		}
		else if (v_bits(s)) {
			/* bits are variable, but in fixed bytes - check for bits overflow (s->unknown_packed_bits == 0) */
			if (~0u - s->packed_bits < s->packed_optional_bits ||
				s->packed_bits + s->packed_optional_bits > 0xFFFFFFFFu)
			{
				FR3("\n%s\t\t_%s_var_size(s, 0x%x);", offs, s->s_name, s->packed_bits ^ 0x80000000);
			}
		}
		if (!v_bytes(s) && bits_have_fixed_bytes(s))
			FR2("\n%s\t\tint packed_size = %u;", offs, initial_bytes); /* <= BRIDGE_MODEL_MAX_PACKED_SIZE */
		else if (!szl || szl > 0x7FFFFFFFu - 8u) {
			FR2("\n%s\t\tif (packed_size > BRIDGE_MAX)"
				"\n%s\t\t\tthrow new BridgeTooBig();", offs, offs);
		}
		{
			const char *info_type = _get_packed_info(s);
			FR4("\n%s\t\t{"
				"\n%s\t\t\t_pk%s info = new _pk%s(packed_size, head, tail);", offs, offs, info_type, info_type);
		}
		if (!s_has_bits(s))
			FR1("\n%s\t\t\tinfo.at = head;", offs);
		else if (need_pack_bits_counter(s))
			FR1("\n%s\t\t\tinfo.bits = _bridge_pack_counter(info.mem, head, bits_size);", offs);
		else
			FR1("\n%s\t\t\tinfo.bits = head;", offs);
		if (s_has_bytes(s)) {
			if (!bits_have_fixed_bytes(s))
				FR1("\n%s\t\t\tinfo.at = info.bits + bits_size;", offs);
			else if (s_has_bits(s))
				FR2("\n%s\t\t\tinfo.at = info.bits + %u;", offs, bytes_for_bits(s->packed_bits));
		}
		if (s->convertable)
			FR1("\n%s\t\t\tinfo.big = big;", offs);
		FR4("\n%s\t\t\t_%s_append_packed(s, info);"
			"\n%s\t\t\treturn info.mem;"
			"\n%s\t\t}", offs, s->s_name, offs, offs);
	}
	if (s->_marked || s->_processed)
		FP("\n\t\t}");
	if (s->_marked) {
		FP("\n\t\tcatch (NullPointerException ex) {"
			"\n\t\t\tthrow new BridgeNullPointer(ex);"
			"\n\t\t}");
	}
	if (s->_processed) {
		FP("\n\t\tcatch (ArrayIndexOutOfBoundsException ex) {"
			"\n\t\t\tthrow new BridgeSmallArray(ex);"
			"\n\t\t}");
	}
	FP("\n\t}"
		"\n");
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

#define CHECKS_BYTES_CREDIT 1u
#define CHECKS_BITS_CREDIT  2u

static void _mark_structure(struct struct_def *s, unsigned bit)
{
	/* 01 -> 0101
	   10 -> 1010 */
	s->_aux |= (unsigned char)((bit & 3u)*5u);
}

static void _check_limits_for_user_type(FILE *file, struct struct_def *s, const struct struct_def *user_type,
	unsigned *bytes_credit, unsigned *bits_credit, const char *blim, const char *offs)
{
	/* user_type may assume that caller have already checked
	  for availability of some minimum of bytes or bits to unpack */
	if ((*bytes_credit) >= user_type->packed_bytes) {
		(*bytes_credit) -= user_type->packed_bytes;
		/* really caller may not check the minimums if user_type doesn't rely on this check */
		if (user_type->_aux & CHECKS_BYTES_CREDIT) {
			FR2("\n%s\t\t/* use byte_credit: %u */", offs, user_type->packed_bytes);
			_mark_structure(s, CHECKS_BYTES_CREDIT); /* caller must check bytes_credit */
		}
	}
	else {
		(*bytes_credit) = 0;
		if (user_type->_aux & CHECKS_BYTES_CREDIT)
			FR2("\n%s\t\tupk_check_mem_lim(info.at, info.lim, %u);", offs, user_type->packed_bytes);
	}
	if ((*bits_credit) >= user_type->packed_bits) {
		(*bits_credit) -= user_type->packed_bits;
		/* really caller may not check the minimums if user_type doesn't rely on this check */
		if (user_type->_aux & CHECKS_BITS_CREDIT) {
			FR2("\n%s\t\t/* use bit_credit: %u */", offs, user_type->packed_bits);
			_mark_structure(s, CHECKS_BITS_CREDIT); /* caller must check bits_credit */
		}
	}
	else {
		(*bits_credit) = 0;
		if (user_type->_aux & CHECKS_BITS_CREDIT)
			FR3("\n%s\t\tupk_check_bits_lim_nz(info.bits, info.%s, info.bit_count, 0x%x);", offs, blim, user_type->packed_bits);
	}
	/* user_type may consume all checked credit of available bytes or bits to unpack */
	if (user_type->unknown_packed_bytes || user_type->packed_optional_bytes > (*bytes_credit))
		(*bytes_credit) = 0;
	else
		(*bytes_credit) -= user_type->packed_optional_bytes;
	if (user_type->unknown_packed_bits || user_type->packed_optional_bits > (*bits_credit))
		(*bits_credit) = 0;
	else
		(*bits_credit) -= user_type->packed_optional_bits;
}

static void _unpack_simple_int_type(FILE *file, struct struct_def *s, struct field_def *f, unsigned *bytes_credit, const char *offs)
{
	int bytes_credit_anough = 1;
	unsigned max_needed_bytes = _simple_type_max_required_bytes(f->type);
	if ((*bytes_credit) >= max_needed_bytes) {
		(*bytes_credit) -= max_needed_bytes;
		_mark_structure(s, CHECKS_BYTES_CREDIT); /* caller must check bytes_credit */
	}
	else {
		(*bytes_credit) = 0;
		bytes_credit_anough = 0;
	}
	FR5("\n%s\t\ts.%s = bridge_unpack_%s%s(info%s);",
		offs, f->name, f->type,
		bytes_credit_anough ? "_no_lim" : "",
		(is_variabe_int_type(f->type) || field_type_byte == f->type) ? "" : ", info.big");
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

static void switch_init_unpacked(FILE *file/*NULL if determining marks*/, struct field_def *f, struct struct_def *r,
	unsigned *bytes_credit, unsigned *bits_credit, const char *blim, const char *offs)
{
	switch (f->f_power) {
		case F_ARRAY:
			FR3("\n%s\t\t/* %s: %s[]", offs, f->name, f->type);
			print_checked_credits(file, r, *bytes_credit, *bits_credit);
			{
				int bytes_credit_anough = 1;
				if ((*bytes_credit) >= BRIDGE_MAX_COUNTER_PACKED_SIZE) {
					_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
					(*bytes_credit) -= BRIDGE_MAX_COUNTER_PACKED_SIZE;
				}
				else {
					(*bytes_credit) = 0;
					bytes_credit_anough = 0;
				}
				FR3("\n%s\t\t{"
					"\n%s\t\t\tint count = _bridge_unpack_counter%s(info);", offs, offs, bytes_credit_anough ? "_no_lim" : "");
			}
			if (f->user_type) {
				if (s_has_bytes(f->user_type))
					(*bytes_credit) = 0;
				if (s_has_bits(f->user_type))
					(*bits_credit) = 0;
				FR1("\n%s\t\t\tif (count != 0) {", offs);
				/* if f->user_type assumes that caller have done some checks - try to do some of them outside the loop */
				if (f->user_type->packed_bytes && !v_bytes(f->user_type)) {
					if (1 == f->user_type->packed_bytes)
						FR1("\n%s\t\t\t\t_bridge_check_byte_array_counter(info.at, info.lim, count);", offs);
					else {
						FR2("\n%s\t\t\t\t_bridge_check_fixed_array_counter(info.at, info.lim, count, %u);",
							offs, f->user_type->packed_bytes/*>1*/);
					}
				}
				else {
					FR2("\n%s\t\t\t\t_bridge_check_array_counter(count, %u);",
						offs, f->user_type->packed_bytes ? f->user_type->packed_bytes : 1u);
				}
				if (f->user_type->packed_bits && !v_bits(f->user_type)) {
					FR3("\n%s\t\t\t\tupk_check_bits_lim_n(info.bits, info.%s, info.bit_count, 0x%x, count);",
						offs, blim, f->user_type->packed_bits);
				}
				FR6("\n%s\t\t\t\ts.%s = new %s[count];"
					"\n%s\t\t\t\t{"
					"\n%s\t\t\t\t\tint i = 0;"
					"\n%s\t\t\t\t\tdo {", offs, f->name, f->type, offs, offs, offs);
				{
					/* if f->user_type assumes that caller have done some checks - do remaining checks inside the loop */
					int check_bytes = v_bytes(f->user_type) && (f->user_type->_aux & CHECKS_BYTES_CREDIT);
					int check_bits  = v_bits(f->user_type) && (f->user_type->_aux & CHECKS_BITS_CREDIT);
					if (check_bytes)
						FR2("\n%s\t\t\t\t\t\tupk_check_mem_lim(info.at, info.lim, %u);", offs, f->user_type->packed_bytes);
					if (check_bits) {
						FR3("\n%s\t\t\t\t\t\tupk_check_bits_lim_nz(info.bits, info.%s, info.bit_count, 0x%x);",
							offs, blim, f->user_type->packed_bits/*!=0 because CHECKS_BITS_CREDIT is set*/);
					}
				}
				FR10("\n%s\t\t\t\t\t\ts.%s[i] = _%s_unpack(info);"
					"\n%s\t\t\t\t\t} while (++i < count);"
					"\n%s\t\t\t\t}"
					"\n%s\t\t\t}"
					"\n%s\t\t\telse"
					"\n%s\t\t\t\ts.%s = new %s[0];",
					offs, f->name, f->type, offs, offs, offs, offs, offs, f->name, f->type);
			}
			else if (field_type_bit == f->type) {
				(*bytes_credit) = 0;
				FR4("\n%s\t\t\ts.%s_bit_count = count;"
					"\n%s\t\t\ts.%s = bridge_unpack_bit_array(info, count);", offs, f->name, offs, f->name);
			}
			else {
				(*bytes_credit) = 0;
				FR4("\n%s\t\t\ts.%s = bridge_unpack_%s_array(info, count%s);",
					offs, f->name, f->type, (field_type_byte == f->type || is_variable_simple_type(f->type)) ? "" : ", info.big");
			}
			FR1("\n%s\t\t}", offs);
			break;
		case F_REQUIRED:
		case F_OPTIONAL:
		case F_POINTER:
			if (F_POINTER != f->f_power || f->user_type || field_type_bit != f->type) {
				const char *of = (F_OPTIONAL == f->f_power) ? offs[0] ? offs[1] ? "\t\t\t" : "\t\t" : "\t" : offs;
				FR4("\n%s\t\t/* %s: %s%s", offs, f->name, f->type,
					(F_OPTIONAL == f->f_power) ? "*" : (F_POINTER == f->f_power) ? "[1]" : "");
				print_checked_credits(file, r, *bytes_credit, *bits_credit);
				if (F_OPTIONAL == f->f_power) {
					if (*bits_credit) {
						(*bits_credit)--;
						_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
						FR1("\n%s\t\tif (_bridge_get_bit(info.mem, info.bits, info.bit_count++ ^ 0x80000000))", offs);
					}
					else
						FR2("\n%s\t\tif (_bridge_unpack_next_bit(info, info.%s))", offs, blim);
					if (f->user_type || !is_simple_str_type(f->type))
						FP(" {");
				}
				if (f->user_type) {
					_check_limits_for_user_type(file, r, f->user_type, bytes_credit, bits_credit, blim, of);
					FR3("\n%s\t\ts.%s = _%s_unpack(info);", of, f->name, f->type);
				}
				else if (F_POINTER != f->f_power) {
					if (is_simple_str_type(f->type)) {
						(*bytes_credit) = 0;
						FR3("\n%s\t\ts.%s = _bridge_unpack_%s(info);", of, f->name, f->type);
					}
					else {
						if (F_OPTIONAL == f->f_power)
							FR3("\n%s\t\ts._bit_int%u |= 0x%x;", of, BIT_INT(f->bit), BIT_NUM(f->bit));
						if (field_type_bit == f->type) {
							if ((*bits_credit)) {
								(*bits_credit)--; /* for F_OPTIONAL: r - already marked with CHECKS_BITS_CREDIT */
								if (F_REQUIRED == f->f_power)
									_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
								FR1("\n%s\t\tif (_bridge_get_bit(info.mem, info.bits, info.bit_count++ ^ 0x80000000))", of);
							}
							else
								FR2("\n%s\t\tif (_bridge_unpack_next_bit(info, info.%s))", of, blim);
							{
								unsigned bit = f->bit + ((F_OPTIONAL == f->f_power) ? 1u : 0u);
								FR3("\n%s\t\t\ts._bit_int%u |= 0x%x;", of, BIT_INT(bit), BIT_NUM(bit));
							}
						}
						else
							_unpack_simple_int_type(file, r, f, bytes_credit, of);
					}
				}
				else if (is_variable_simple_type(f->type))
					goto _unpack_variable_simple_type_ptr;
				else
					goto _unpack_fixed_simple_type_ptr;
				if (F_OPTIONAL == f->f_power && (f->user_type || !is_simple_str_type(f->type)))
					FR1("\n%s\t\t}", offs);
				break;
			}
			/* F_POINTER for field_type_bit, fall through */
		default:
			FR4("\n%s\t\t/* %s: %s[%u]", offs, f->name, f->type, f->f_power);
			print_checked_credits(file, r, *bytes_credit, *bits_credit);
			if (f->user_type) {
				/* if f->user_type assumes that caller have done some checks - try to do some of them outside the loop */
				unsigned credit_anough = CHECKS_BYTES_CREDIT | CHECKS_BITS_CREDIT;
				/* NOTE: f->user_type->packed_bytes*f->f_power - will not overflow - checked in model.c */
				if ((*bytes_credit) >= f->user_type->packed_bytes*f->f_power) {
					(*bytes_credit) -= f->user_type->packed_bytes*f->f_power;
					if (f->user_type->packed_bytes && !v_bytes(f->user_type))
						_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
				}
				else {
					(*bytes_credit) = 0;
					credit_anough &= ~CHECKS_BYTES_CREDIT;
					if (!v_bytes(f->user_type))
						FR2("\n%s\t\tupk_check_mem_lim(info.at, info.lim, %u);", offs, f->user_type->packed_bytes*f->f_power);
				}
				/* NOTE: f->user_type->packed_bits*f->f_power - will not overflow - checked in model.c */
				if ((*bits_credit) >= f->user_type->packed_bits*f->f_power) {
					(*bits_credit) -= f->user_type->packed_bits*f->f_power;
					if (f->user_type->packed_bits && !v_bits(f->user_type))
						_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
				}
				else {
					(*bits_credit) = 0;
					credit_anough &= ~CHECKS_BITS_CREDIT;
					if (!v_bits(f->user_type)) {
						FR3("\n%s\t\tupk_check_bits_lim_nz(info.bits, info.%s, info.bit_count, 0x%x);",
							offs, blim, f->user_type->packed_bits*f->f_power);
					}
				}
				if (f->user_type->unknown_packed_bytes || f->user_type->packed_optional_bytes > (*bytes_credit)/f->f_power) {
					(*bytes_credit) = 0;
					credit_anough &= ~CHECKS_BYTES_CREDIT;
				}
				else
					(*bytes_credit) -= f->user_type->packed_optional_bytes*f->f_power;
				if (f->user_type->unknown_packed_bits || f->user_type->packed_optional_bits > (*bits_credit)/f->f_power) {
					(*bits_credit) = 0;
					credit_anough &= ~CHECKS_BITS_CREDIT;
				}
				else
					(*bits_credit) -= f->user_type->packed_optional_bits*f->f_power;
				FR7("\n%s\t\ts.%s = new %s[%u];"
					"\n%s\t\t{"
					"\n%s\t\t\tint i = 0;"
					"\n%s\t\t\tdo {", offs, f->name, f->type, f->f_power, offs, offs, offs);
				{
					/* if f->user_type assumes that caller have done some checks - do remaining checks inside the loop */
					int check_bytes = v_bytes(f->user_type) && (f->user_type->_aux & CHECKS_BYTES_CREDIT);
					int check_bits  = v_bits(f->user_type) && (f->user_type->_aux & CHECKS_BITS_CREDIT);
					if (check_bytes && (CHECKS_BYTES_CREDIT & credit_anough)) {
						check_bytes = 0;
						_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
					}
					if (check_bits && (CHECKS_BITS_CREDIT & credit_anough)) {
						check_bits = 0;
						_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check (*bits_credit) */
					}
					if (check_bytes)
						FR2("\n%s\t\t\t\tupk_check_mem_lim(info.at, info.lim, %u);", offs, f->user_type->packed_bytes);
					if (check_bits) {
						FR3("\n%s\t\t\t\tupk_check_bits_lim_nz(info.bits, info.%s, info.bit_count, 0x%x);",
							offs, blim, f->user_type->packed_bits);
					}
				}
				FR6("\n%s\t\t\t\ts.%s[i] = _%s_unpack(info);"
					"\n%s\t\t\t} while (++i < %u);"
					"\n%s\t\t}", offs, f->name, f->type, offs, f->f_power, offs);
			}
			else if (field_type_bit == f->type) {
				unsigned bb = bytes_for_bits(f->f_power);
				if ((*bytes_credit) >= bb) {
					(*bytes_credit) -= bb;
					_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
					FR3("\n%s\t\ts.%s = bridge_unpack_bit_array_no_lim(info, %u);", offs, f->name, bb);
				}
				else {
					(*bytes_credit) = 0;
					FR3("\n%s\t\ts.%s = _bridge_unpack_bit_array(info, %u);", offs, f->name, bb);
				}
			}
			else if (is_variable_simple_type(f->type)) {
_unpack_variable_simple_type_ptr: /* F_POINTER */
				{
					unsigned pwr = (F_POINTER == f->f_power) ? 1u : f->f_power;
					if (is_simple_str_type(f->type)) {
						(*bytes_credit) = 0;
						FR4("\n%s\t\ts.%s = _bridge_unpack_%s_array(info, %u);", offs, f->name, f->type, pwr);
					}
					else {
						unsigned max_needed_bytes = _simple_type_max_required_bytes(f->type);
						if (max_needed_bytes > (*bytes_credit)/pwr) {
							(*bytes_credit) = 0;
							FR4("\n%s\t\ts.%s = _bridge_unpack_%s_array(info, %u);", offs, f->name, f->type, pwr);
						}
						else {
							(*bytes_credit) -= max_needed_bytes*pwr;
							_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
							FR4("\n%s\t\ts.%s = bridge_unpack_%s_array_no_lim(info, %u);", offs, f->name, f->type, pwr);
						}
					}
				}
			}
			else {
_unpack_fixed_simple_type_ptr: /* F_POINTER */
				{
					unsigned pwr = (F_POINTER == f->f_power) ? 1u : f->f_power;
					unsigned needed_bytes = _simple_type_max_required_bytes(f->type);
					if (needed_bytes <= (*bytes_credit)/pwr) {
						(*bytes_credit) -= needed_bytes*pwr;
						_mark_structure(r, CHECKS_BYTES_CREDIT); /* caller must check (*bytes_credit) */
						FR5("\n%s\t\ts.%s = bridge_unpack_%s_array_no_lim(info, %u%s);",
							offs, f->name, f->type, pwr, (field_type_byte == f->type) ? "" : ", info.big");
					}
					else {
						(*bytes_credit) = 0;
						FR5("\n%s\t\ts.%s = bridge_unpack_fixed_%s_array(info, %u%s);",
							offs, f->name, f->type, pwr, (field_type_byte == f->type) ? "" : ", info.big");
					}
				}
			}
			break;
	}
}

static void _generate_prototype_unpack(FILE *file, const struct struct_def *s)
{
	FR4("\n\tstatic %s _%s_unpack(_upk%s info)%s {",
		s->s_name, s->s_name, _get_packed_info(s), s_fixed(s) ? "" : " throws BridgeException");
}

/* caller have checked required limits, now may unpack the structure */
static void _generate_code_unpack(FILE *file/*NULL if determining marks*/, struct struct_def *s)
{
	const int long_loop = (s->tail_recursive && s->tail_recursive->user_type != s);
	if (long_loop) {
		const char *enum_type = ((size_t)(structs_end - structs) <= 0xFFFFFFFF) ? "int" : "long";
		FR3("\n\t@SuppressWarnings(\"fallthrough\")"
			"\n\tstatic Object _%s_unp(%s t, _upk%s info) throws BridgeException {",
			s->s_name, enum_type, _get_packed_info(s));
	}
	else
		_generate_prototype_unpack(file, s);
	if (s->tail_recursive) {
		if (long_loop) {
			FP("\n\t\tObject r = null, p = null;"
				"\n\t\tfor (;;) switch (t) {"
				"\n\tdefault:");
		}
		else {
			FR1("\n\t\t%s r = null, p = null;"
				"\n\t\tfor (;;) {", s->s_name);
		}
	}
	{
		const char *const blim = s_has_bytes(s) ? "at" : "lim";
		const char *const offs = long_loop ? "\t\t" : s->tail_recursive ? "\t" : "";
		struct struct_def *p = NULL; /* initialize to avoid bogus compiler warning */
		struct struct_def *r = s;
		if (long_loop) {
			for (;;) {
				p = r;
				r = r->tail_recursive->user_type;
				if (s == r)
					break;
			}
		}
		do {
			/* assume caller had already checked minimum number of bytes and bits required
			  to unpack this structure - we have a credit of checked bytes and bits */
			unsigned bytes_credit = r->packed_bytes;
			unsigned bits_credit = r->packed_bits;
			if (long_loop)
				FR1("\n\tcase V_%s: {", r->s_name);
			FR3("\n%s\t\t%s s = new %s();", offs, r->s_name, r->s_name);
			if (r->tail_recursive) {
				FR3("\n%s\t\tif (null == p)"
					"\n%s\t\t\tr = s;"
					"\n%s\t\telse", offs, offs, offs);
				if (long_loop)
					FR2("\n\t\t\t\t\t((%s)p).%s = s;", p->s_name, p->tail_recursive->name);
				else
					FR1("\n\t\t\t\tp.%s = s;", r->tail_recursive->name);
			}
			{
				struct field_def **f = r->fields;
				for (; f < r->fields_end; f++) {
					if (r->tail_recursive == *f)
						continue; /* tail-recursive field will be processed last */
					switch_init_unpacked(file, *f, r, &bytes_credit, &bits_credit, blim, offs);
				}
			}
			if (r->tail_recursive) {
				FR4("\n%s\t\t/* %s: %s%s (tail recursive) */",
					offs, r->tail_recursive->name, r->tail_recursive->type,
					F_POINTER == r->tail_recursive->f_power ? "[1]" :
					F_OPTIONAL == r->tail_recursive->f_power ? "*" : "");
				if (F_OPTIONAL == r->tail_recursive->f_power) {
					if (bits_credit) {
						bits_credit--;
						_mark_structure(r, CHECKS_BITS_CREDIT); /* caller must check bits_credit */
						FR1("\n%s\t\tif (_bridge_get_bit(info.mem, info.bits, info.bit_count++ ^ 0x80000000)) {", offs);
					}
					else
						FR2("\n%s\t\tif (_bridge_unpack_next_bit(info, info.%s)) {", offs, blim);
				}
				{
					/* bytes_credit & bits_credit are zero if r->tail_recursive is optional, else - may be non-zero */
					const char *of = (F_OPTIONAL == r->tail_recursive->f_power) ? long_loop ? "\t\t\t" : "\t\t" : long_loop ? "\t\t" : "\t";
					_check_limits_for_user_type(file, r, r->tail_recursive->user_type, &bytes_credit, &bits_credit, blim, of);
					FR1("\n%s\t\tp = s;", of);
					if (long_loop && r->tail_recursive->user_type == s)
						FR2("\n%s\t\tt = V_%s;", of, s->s_name);
				}
				if (F_OPTIONAL == r->tail_recursive->f_power) {
					FR3("\n%s\t\t}"
						"\n%s\t\telse"
						"\n%s\t\t\treturn r;", offs, offs, offs);
				}
				p = r;
				r = r->tail_recursive->user_type;
			}
			if (long_loop)
				FP("\n\t\t\t}");
		} while (r != s);
	}
	if (s->tail_recursive)
		FP("\n\t\t}");
	else
		FP("\n\t\treturn s;");
	FP("\n\t}"
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
					unsigned _aux = (*s)->_aux;
					_generate_code_unpack(/*file:*/NULL, *s);
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

void generate_code_unpack(FILE *file, struct struct_def *s)
{
	if (!s->tail_recursive || s->recursion_entry)
		_generate_code_unpack(file, s);
	if (s->tail_recursive && s->tail_recursive->user_type != s) {
		const struct struct_def *r = s;
		while (!r->recursion_entry)
			r = r->tail_recursive->user_type;
		_generate_prototype_unpack(file, s);
		FR3("\n\t\treturn (%s)_%s_unp(V_%s, info);"
			"\n\t}"
			"\n", s->s_name, r->s_name, s->s_name);
	}
	FR3("\n\tstatic %s %s_unpack(BridgeIterator it, byte[] mem, int limit%s) throws BridgeException {"
		"\n\t\tint at = it.at;", s->s_name, s->s_name, s->convertable ? ", boolean big" : "");
	{
		const char *info_type = _get_packed_info(s);
		FR2("\n\t\t_upk%s info = new _upk%s(mem, (limit - at > BRIDGE_MAX) ? at + BRIDGE_MAX : limit);", info_type, info_type);
	}
	if (need_pack_bits_counter(s)) {
		FP("\n\t\tinfo.at = at;"
			"\n\t\t{"
			"\n\t\t\tint bit_bytes = _bridge_unpack_and_check_byte_counter(info);"
			"\n\t\t\tinfo.bits = info.at;"
			"\n\t\t\tinfo.at = info.at + bit_bytes;"
			"\n\t\t}");
	}
	else if (s_has_bits(s) && s_has_bytes(s)) {
		if (v_bytes(s)) {
			/*bits_have_fixed_bytes(s) == 1*/
			FR2("\n\t\tupk_check_mem_lim(at, info.lim, %u);"
				"\n\t\tinfo.bits = at;"
				"\n\t\tinfo.at = at + %u;",
				bytes_for_bits(s->packed_bits/* + s->packed_optional_bits*/),
				bytes_for_bits(s->packed_bits/* + s->packed_optional_bits*/));
		}
		else {
			/* bits may be variable */
			FR2("\n\t\tupk_check_mem_lim(at, info.lim, %u)"
				"\n\t\tinfo.bits = at;"
				"\n\t\tinfo.at = info.lim - %uu;", s->packed_bytes, s->packed_bytes);
		}
	}
	else if (s_has_bits(s))
		FP("\n\t\tinfo.bits = at;");
	else
		FP("\n\t\tinfo.at = at;");
	if ((s->_aux & CHECKS_BYTES_CREDIT) && (!s_has_bits(s) || v_bytes(s)))
		FR1("\n\t\tupk_check_mem_lim(info.at, info.lim, %u);", s->packed_bytes);
	if ((s->_aux & CHECKS_BITS_CREDIT) && (need_pack_bits_counter(s) || !v_bytes(s)))
		FR2("\n\t\t_upk_check_bits_lim_nz(info.bits, info.%s, 0x%x);", s_has_bytes(s) ? "at" : "lim", s->packed_bits);
	if (s_has_bits(s))
		FP("\n\t\tinfo.bit_count = 0x80000000;");
	if (s->convertable)
		FP("\n\t\tinfo.big = big;");
	FR2("\n\t\t{"
		"\n\t\t\t%s s = _%s_unpack(info);", s->s_name, s->s_name);
	if (s_has_bytes(s))
		FP("\n\t\t\tit.at = info.at;");
	else if (bits_have_fixed_bytes(s))
		FR1("\n\t\t\tit.at = info.bits + %u;", bytes_for_bits(s->packed_bits/* + s->packed_optional_bits*/));
	else {
		/* no packed bytes - packed bits count must be non-zero */
		FP("\n\t\t\tit.at = info.bits + bridge_bit_array_size_nz(info.bit_count ^ 0x80000000);");
	}
	FP("\n\t\t\treturn s;"
		"\n\t\t}"
		"\n\t}"
		"\n");
}
