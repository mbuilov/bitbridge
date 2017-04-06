/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* jstruct_gen.c */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ptypes.h"
#include "jcompiler.h"
#include "model.h"

#define MAX_FIXED_ARRAY_INITIALIZERS 20

#define WHL while ((void)0,0)
#define FP(s)                                             do {if (file) fputs(s,file);} WHL
#define FR1(s,a1)                                         do {if (file) fprintf(file,s,a1);} WHL
#define FR2(s,a1,a2)                                      do {if (file) fprintf(file,s,a1,a2);} WHL
#define FR3(s,a1,a2,a3)                                   do {if (file) fprintf(file,s,a1,a2,a3);} WHL
#define FR4(s,a1,a2,a3,a4)                                do {if (file) fprintf(file,s,a1,a2,a3,a4);} WHL
#define FR5(s,a1,a2,a3,a4,a5)                             do {if (file) fprintf(file,s,a1,a2,a3,a4,a5);} WHL
#define FR6(s,a1,a2,a3,a4,a5,a6)                          do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6);} WHL
#define FR7(s,a1,a2,a3,a4,a5,a6,a7)                       do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7);} WHL
#define FR8(s,a1,a2,a3,a4,a5,a6,a7,a8)                    do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8);} WHL
#define FR9(s,a1,a2,a3,a4,a5,a6,a7,a8,a9)                 do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9);} WHL
#define FR10(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1)             do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1);} WHL
#define FR11(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2)          do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2);} WHL
#define FR12(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3)       do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3);} WHL
#define FR14(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5) do {if (file) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5);} WHL

static const char *get_java_type(const char *type)
{
	return
		field_type_byte    == type ? "byte"   :
		field_type_str     == type ? "String" :
		field_type_pstr    == type ? "String" :
		field_type_short   == type ? "short"  :
		field_type_pshort  == type ? "short"  :
		field_type_ppshort == type ? "short"  :
		field_type_int     == type ? "int"    :
		field_type_pint    == type ? "int"    :
		field_type_ppint   == type ? "int"    :
		field_type_long    == type ? "long"   :
		field_type_plong   == type ? "long"   :
		field_type_pplong  == type ? "long"   :
		field_type_float   == type ? "float"  :
		field_type_double  == type ? "double" :
		"internal error";
}

static void print_structure_fields(const struct struct_def *s, FILE *file)
{
	/* print ints to encode bit-fields and presence marks of optional field */
	if (s->bit_offset) {
		unsigned i = 0, c = (s->bit_offset - 1)/4u + 1; /* there are 4 bytes in each int */
		for (; i < c; i++)
			FR1("\n\t\tint _bit_int%u;", i);
	}
	/* print all fields except bit-fields */
	{
		struct field_def **f = s->fields;
		for (; f < s->fields_end; f++) {
			switch ((*f)->f_power) {
				case F_ARRAY:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						FR2("\n\t\t/** may be null to pack, != null after unpack */"
							"\n\t\tpublic %s[] %s;", (*f)->user_type ? (*f)->type : get_java_type((*f)->type), (*f)->name);
					}
					else {
						FR4("\n\t\t/** array of {@link #%s_bit_count} bits, may be null to pack, != null after unpack */"
							"\n\t\tpublic byte[] %s;"
							"\n\t\t/** number of filled bits in bits array {@link #%s} */"
							"\n\t\tpublic int %s_bit_count;", (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
					break;
				case F_OPTIONAL:
				case F_REQUIRED:
					if ((*f)->user_type) {
						FR3("\n\t\t/** %s */"
							"\n\t\tpublic %s %s;",
							(F_OPTIONAL == (*f)->f_power) ? "optional, may be null" : "required, must be != null", (*f)->type, (*f)->name);
					}
					else if (is_simple_str_type((*f)->type)) {
						FR2("\n\t\t/** %s */"
							"\n\t\tpublic String %s;",
							(F_OPTIONAL == (*f)->f_power) ? "optional, may be null" :
								"required, may be null to pack, != null after unpack", (*f)->name);
					}
					else if (field_type_bit != (*f)->type) {
						FR3("\n\t\t/** %s field */"
							"\n\t\tpublic %s %s;",
							(F_REQUIRED == (*f)->f_power) ? "required" : "optional", get_java_type((*f)->type), (*f)->name);
					}
					break;
				case F_POINTER:
					if ((*f)->user_type) {
						FR2("\n\t\t/** required, must be != null */"
							"\n\t\tpublic %s %s;", (*f)->type, (*f)->name);
						break;
					}
					/* fall through */
				default:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						unsigned pwr = field_pwr(*f);
						const char *field_type = (*f)->user_type ? (*f)->type : get_java_type((*f)->type);
						FR7("\n\t\t/** length of fixed-sized array {@link #%s} */"
							"\n\t\tpublic static final int %s_length_ = %u;"
							"\n\t\t/** fixed-sized array of {@link #%s_length_} elements, must be != null */"
							"\n\t\tpublic %s[/*%u*/] %s;", (*f)->name, (*f)->name, pwr, (*f)->name, field_type, pwr, (*f)->name);
					}
					else {
						unsigned bb = bytes_for_bits((*f)->f_power);
						FR11("\n\t\t/** number of bits in fixed-sized bits array {@link #%s} */"
							"\n\t\tpublic static final int %s_bit_count_ = 0x%x; /* == %u */"
							"\n\t\t/** length of fixed-sized bits array {@link #%s} in bytes */"
							"\n\t\tpublic static final int %s_length_ = %u;"
							"\n\t\t/** fixed-sized array of {@link #%s_bit_count_} bits ({@link #%s_length_} bytes), must be != null */"
							"\n\t\tpublic byte[/*%u*/] %s;",
							(*f)->name, (*f)->name, (*f)->f_power, (*f)->f_power,
							(*f)->name, (*f)->name, bb, (*f)->name, (*f)->name, bb, (*f)->name);
					}
					break;
			}
		}
	}
	/* print get_...() and set_...() methods for all fields, has_...() and unset_...() methods for non-pointer optional fields,
	  new_...() for arrays and pointers */
	{
		struct field_def **f = s->fields;
		for (; f < s->fields_end; f++) {
			switch ((*f)->f_power) {
				case F_ARRAY:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						const char *field_type = (*f)->user_type ? (*f)->type : get_java_type((*f)->type);
						FR12("\n\t\t/** @return array {@link #%s}, != null after unpack */"
							"\n\t\tpublic %s[] get_%s() {"
							"\n\t\t\treturn %s;"
							"\n\t\t}"
							"\n\t\t/** @param %s_ new value of array {@link #%s}"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s set_%s(%s[] %s_) {"
							"\n\t\t\t%s = %s_;"
							"\n\t\t\treturn this;"
							"\n\t\t}",
							(*f)->name, field_type, (*f)->name, (*f)->name, (*f)->name, (*f)->name,
							s->s_name, (*f)->name, field_type, (*f)->name, (*f)->name, (*f)->name);
						FR7("\n\t\t/** allocate and set new value of array {@link #%s}"
							"\n\t\t * @param length_ length of allocated array"
							"\n\t\t * @return new value of array {@link #%s} which should be initialized by the caller */"
							"\n\t\tpublic %s[] new_%s(int length_) {"
							"\n\t\t\t%s = new %s[length_];"
							"\n\t\t\treturn %s;"
							"\n\t\t}", (*f)->name, (*f)->name, field_type, (*f)->name, (*f)->name, field_type, (*f)->name);
						FR5("\n\t\t/** @param idx_ index in array {@link #%s}"
							"\n\t\t * @return element of array {@link #%s} at idx_ */"
							"\n\t\tpublic %s %s_get(int idx_) {"
							"\n\t\t\treturn %s[idx_];"
							"\n\t\t}", (*f)->name, (*f)->name, field_type, (*f)->name, (*f)->name);
						FR6("\n\t\t/** @param idx_ index in array {@link #%s}"
							"\n\t\t * @param v_ new value in array {@link #%s} at index idx_"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s %s_set(int idx_, %s v_) {"
							"\n\t\t\t%s[idx_] = v_;"
							"\n\t\t\treturn this;"
							"\n\t\t}", (*f)->name, (*f)->name, s->s_name, (*f)->name, field_type, (*f)->name);
					}
					else {
						FR7("\n\t\t/** @return bits array {@link #%s}, != null after unpack */"
							"\n\t\tpublic byte[] get_%s() {"
							"\n\t\t\treturn %s;"
							"\n\t\t}"
							"\n\t\t/** @return {@link #%s_bit_count} - number of filled bits in bits array {@link #%s} */"
							"\n\t\tpublic int get_%s_bit_count() {"
							"\n\t\t\treturn %s_bit_count;"
							"\n\t\t}",
							(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
						FR12("\n\t\t/** @param %s_ new value of bits array {@link #%s}"
							"\n\t\t * @param %s_bit_count_ number of filled bits in bits array %s_"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s set_%s(byte[] %s_, int %s_bit_count_) {"
							"\n\t\t\t%s = %s_;"
							"\n\t\t\t%s_bit_count = %s_bit_count_;"
							"\n\t\t\treturn this;"
							"\n\t\t}",
							(*f)->name, (*f)->name, (*f)->name, (*f)->name,
							s->s_name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
						FR11("\n\t\t/** allocate and set new values of bits array {@link #%s} and {@link #%s_bit_count}"
							"\n\t\t * @param %s_bit_count_ bits capacity of allocated bits array"
							"\n\t\t * @return new value of bits array {@link #%s} which should be initialized by the caller */"
							"\n\t\tpublic byte[] new_%s(int %s_bit_count_) {"
							"\n\t\t\t%s = new byte[bridge_bit_array_size(%s_bit_count_)];"
							"\n\t\t\t%s_bit_count = %s_bit_count_;"
							"\n\t\t\treturn %s;"
							"\n\t\t}",
							(*f)->name, (*f)->name, (*f)->name, (*f)->name,
							(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
						FR3("\n\t\t/** @param bit_number bit to check in bits array{@link #%s}"
							"\n\t\t * @return true if bit is set, false - otherwise */"
							"\n\t\tpublic boolean %s_get_bit(int bit_number) {"
							"\n\t\t\treturn bridge_get_bit(%s, bit_number);"
							"\n\t\t}", (*f)->name, (*f)->name, (*f)->name);
						FR4("\n\t\t/** @param bit_number bit to set in bits array {@link #%s}"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s %s_set_bit(int bit_number) {"
							"\n\t\t\tbridge_set_bit(%s, bit_number);"
							"\n\t\t\treturn this;"
							"\n\t\t}", (*f)->name, s->s_name, (*f)->name, (*f)->name);
						FR4("\n\t\t/** @param bit_number bit to clear in bits array {@link #%s}"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s %s_clear_bit(int bit_number) {"
							"\n\t\t\tbridge_clear_bit(%s, bit_number);"
							"\n\t\t\treturn this;"
							"\n\t\t}", (*f)->name, s->s_name, (*f)->name, (*f)->name);
						FR4("\n\t\t/** @param bit_number bit to change in bits array {@link #%s}"
							"\n\t\t * @param set true to set bit, false - to clear"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s %s_set_bit(int bit_number, boolean set) {"
							"\n\t\t\tbridge_set_bit(%s, bit_number, set);"
							"\n\t\t\treturn this;"
							"\n\t\t}", (*f)->name, s->s_name, (*f)->name, (*f)->name);
					}
					break;
				case F_OPTIONAL:
				case F_REQUIRED:
					if ((*f)->user_type) {
_required_user_type:
						{
							FR5("\n\t\t/** @return reference {@link #%s} - %s */"
								"\n\t\tpublic %s get_%s() {"
								"\n\t\t\treturn %s;"
								"\n\t\t}",
								(*f)->name, (F_OPTIONAL == (*f)->f_power) ? "optional, may be null" : "required, must be != null",
								(*f)->type, (*f)->name, (*f)->name);
							FR10("\n\t\t/** @param %s_ new %s value of reference {@link #%s}"
								"\n\t\t * @return this */"
								"\n\t\tpublic %s set_%s(%s %s_/*%s*/) {"
								"\n\t\t\t%s = %s_;"
								"\n\t\t\treturn this;"
								"\n\t\t}",
								(*f)->name, (F_OPTIONAL == (*f)->f_power) ? "optional (may be null)" : "required (must be != null)",
								(*f)->name, s->s_name, (*f)->name, (*f)->type, (*f)->name,
								(F_OPTIONAL == (*f)->f_power) ? "null?" : "!=null", (*f)->name, (*f)->name);
							FR9("\n\t\t/** allocate and set new value of %s reference {@link #%s}"
								"\n\t\t * @return new value of %s reference {@link #%s} which should be initialized by the caller */"
								"\n\t\tpublic %s new_%s() {"
								"\n\t\t\t%s = new %s();"
								"\n\t\t\treturn %s;"
								"\n\t\t}",
								(F_OPTIONAL == (*f)->f_power) ? "optional" : "required", (*f)->name,
								(F_OPTIONAL == (*f)->f_power) ? "optional" : "required", (*f)->name,
								(*f)->type, (*f)->name, (*f)->name, (*f)->type, (*f)->name);
						}
						break;
					}
					else if (is_simple_str_type((*f)->type)) {
						FR4("\n\t\t/** @return string {@link #%s} - %s */"
							"\n\t\tpublic String get_%s() {"
							"\n\t\t\treturn %s;"
							"\n\t\t}",
							(*f)->name, (F_OPTIONAL == (*f)->f_power) ? "optional, may be null" :
								"required, may be null to pack, != null after unpack", (*f)->name, (*f)->name);
						FR8("\n\t\t/** @param %s_ new %s value of string {@link #%s}"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s set_%s(String %s_) {"
							"\n\t\t\t%s = %s_;"
							"\n\t\t\treturn this;"
							"\n\t\t}",
							(*f)->name, (F_OPTIONAL == (*f)->f_power) ? "optional (may be null)" : "required (but may be null to pack)",
							(*f)->name, s->s_name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
					else {
						if (F_OPTIONAL == (*f)->f_power) {
							if (field_type_bit == (*f)->type)
								FR1("\n\t\t/** @return true if optional bit %s has a value */", (*f)->name);
							else
								FR1("\n\t\t/** @return true if optional field {@link #%s} has a value */", (*f)->name);
							FR3("\n\t\tpublic boolean has_%s() {"
								"\n\t\t\treturn 0 != (_bit_int%u & 0x%x);"
								"\n\t\t}", (*f)->name, BIT_INT((*f)->bit), BIT_NUM((*f)->bit));
							if (field_type_bit == (*f)->type)
								FR1("\n\t\t/** unset value of optional bit %s", (*f)->name);
							else
								FR1("\n\t\t/** unset value of optional field {@link #%s}", (*f)->name);
							FR4("\n\t\t * @return this */"
								"\n\t\tpublic %s unset_%s() {"
								"\n\t\t\t_bit_int%u &= ~0x%x;"
								"\n\t\t\treturn this;"
								"\n\t\t}", s->s_name, (*f)->name, BIT_INT((*f)->bit), BIT_NUM((*f)->bit));
						}
						if (field_type_bit == (*f)->type) {
							unsigned bit_bit = (*f)->bit + ((F_OPTIONAL == (*f)->f_power) ? 1u : 0u);
							const char *bit_kind = (F_OPTIONAL == (*f)->f_power) ? "optional" : "required";
							FR5("\n\t\t/** @return true if %s bit %s is set, false - otherwise */"
								"\n\t\tpublic boolean get_%s() {"
								"\n\t\t\treturn 0 != (_bit_int%u & 0x%x);"
								"\n\t\t}",
								bit_kind, (*f)->name, (*f)->name, BIT_INT(bit_bit), BIT_NUM(bit_bit));
							FR3("\n\t\t/** @param %s_ true to set %s bit %s, false - to clear it"
								"\n\t\t * @return this */", (*f)->name, bit_kind, (*f)->name);
							if (F_REQUIRED == (*f)->f_power) {
								FR8("\n\t\tpublic %s set_%s(boolean %s_) {"
									"\n\t\t\t_bit_int%u = (_bit_int%u & ~0x%x) | (%s_ ? 0x%x : 0);"
									"\n\t\t\treturn this;"
									"\n\t\t}",
									s->s_name, (*f)->name, (*f)->name,
									BIT_INT((*f)->bit), BIT_INT((*f)->bit), BIT_NUM((*f)->bit), (*f)->name, BIT_NUM((*f)->bit));
							}
							else if (BIT_INT((*f)->bit + 1) == BIT_INT((*f)->bit)) {
								FR9("\n\t\tpublic %s set_%s(boolean %s_) {"
									"\n\t\t\t_bit_int%u = (_bit_int%u & ~0x%x) | (%s_ ? 0x%x : 0x%x);"
									"\n\t\t\treturn this;"
									"\n\t\t}",
									s->s_name, (*f)->name, (*f)->name, BIT_INT((*f)->bit), BIT_INT((*f)->bit), BIT_NUM((*f)->bit + 1),
									(*f)->name, BIT_NUM((*f)->bit) | BIT_NUM((*f)->bit + 1), BIT_NUM((*f)->bit));
							}
							else {
								FR10("\n\t\tpublic %s set_%s(boolean %s_) {"
									"\n\t\t\t_bit_int%u |= 0x%x;"
									"\n\t\t\t_bit_int%u = (_bit_int%u & ~0x%x) | (%s_ ? 0x%x : 0);"
									"\n\t\t\treturn this;"
									"\n\t\t}",
									s->s_name, (*f)->name, (*f)->name, BIT_INT((*f)->bit), BIT_NUM((*f)->bit),
									BIT_INT((*f)->bit + 1), BIT_INT((*f)->bit + 1), BIT_NUM((*f)->bit + 1),
									(*f)->name, BIT_NUM((*f)->bit + 1));
							}
						}
						else {
							const char *field_type = get_java_type((*f)->type);
							FR5("\n\t\t/** @return %s field {@link #%s} */"
								"\n\t\tpublic %s get_%s() {"
								"\n\t\t\treturn %s;"
								"\n\t\t}",
								(F_REQUIRED == (*f)->f_power) ? "required" : "optional",
								(*f)->name, field_type, (*f)->name, (*f)->name);
							if (F_REQUIRED == (*f)->f_power) {
								FR8("\n\t\t/** @param %s_ new value of required field {@link #%s}"
									"\n\t\t * @return this */"
									"\n\t\tpublic %s set_%s(%s %s_) {"
									"\n\t\t\t%s = %s_;"
									"\n\t\t\treturn this;"
									"\n\t\t}",
									(*f)->name, (*f)->name, s->s_name, (*f)->name, field_type, (*f)->name, (*f)->name, (*f)->name);
							}
							else {
								FR10("\n\t\t/** @param %s_ new value of optional field {@link #%s}"
									"\n\t\t * @return this */"
									"\n\t\tpublic %s set_%s(%s %s_) {"
									"\n\t\t\t_bit_int%u |= 0x%x;"
									"\n\t\t\t%s = %s_;"
									"\n\t\t\treturn this;"
									"\n\t\t}",
									(*f)->name, (*f)->name, s->s_name, (*f)->name, field_type, (*f)->name,
									BIT_INT((*f)->bit), BIT_NUM((*f)->bit), (*f)->name, (*f)->name);
							}
						}
					}
					break;
				case F_POINTER:
					if ((*f)->user_type)
						goto _required_user_type;
					/* fall through */
				default:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						unsigned pwr = field_pwr(*f);
						const char *field_type = (*f)->user_type ? (*f)->type : get_java_type((*f)->type);
						FR6("\n\t\t/** @return {@link #%s} != null fixed-sized array of {@link #%s_length_} elements */"
							"\n\t\tpublic %s[/*%u*/] get_%s() {"
							"\n\t\t\treturn %s;"
							"\n\t\t}",
							(*f)->name, (*f)->name, field_type, pwr, (*f)->name, (*f)->name);
						FR10("\n\t\t/** @param %s_ new != null value of {@link #%s} - fixed-sized array of {@link #%s_length_} elements"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s set_%s(%s[/*%u*/] %s_/*!=null*/) {"
							"\n\t\t\t%s = %s_;"
							"\n\t\t\treturn this;"
							"\n\t\t}",
							(*f)->name, (*f)->name, (*f)->name, s->s_name, (*f)->name, field_type, pwr, (*f)->name, (*f)->name, (*f)->name);
						FR9("\n\t\t/** allocate and set new value of {@link #%s} - fixed-sized array of {@link #%s_length_} elements"
							"\n\t\t * @return new value of {@link #%s} which should be initialized by the caller */"
							"\n\t\tpublic %s[] new_%s() {"
							"\n\t\t\t%s = new %s[%u];"
							"\n\t\t\treturn %s;"
							"\n\t\t}", (*f)->name, (*f)->name, (*f)->name, field_type, (*f)->name, (*f)->name, field_type, pwr, (*f)->name);
						if (pwr > 1) {
							if (pwr <= MAX_FIXED_ARRAY_INITIALIZERS) {
								FR2("\n\t\t/** allocate, initialize and set new value of {@link #%s} "
									"- fixed-sized array of {@link #%s_length_} elements", (*f)->name, (*f)->name);
								{
									unsigned i = 0;
									for (; i < pwr; i++)
										FR2("\n\t\t * @param v%u element to put into newly allocated array at %u position", i, i);
								}
								FR2("\n\t\t * @return this */"
									"\n\t\tpublic %s set_%s", s->s_name, (*f)->name);
								{
									unsigned i = 0;
									for (; i < pwr; i++)
										FR3("%c\n\t\t\t%s v%u", 0 == i ? '(' : ',', field_type, i);
								}
								FR2("\n\t\t) {"
									"\n\t\t\t%s = new %s[] ", (*f)->name, field_type);
								{
									unsigned i = 0;
									for (; i < pwr; i++)
										FR2("%cv%u", 0 == i ? '{' : ',', i);
								}
								FP("};"
									"\n\t\t\treturn this;"
									"\n\t\t}");
							}
							FR7("\n\t\t/** @param idx_ index in fixed-sized array {@link #%s}, must be in range [0..{@link #%s_length_})"
								"\n\t\t * @return element of {@link #%s} array at idx_ */"
								"\n\t\tpublic %s %s_get(int idx_/*[0..%u)*/) {"
								"\n\t\t\treturn %s[idx_];"
								"\n\t\t}", (*f)->name, (*f)->name, (*f)->name, field_type, (*f)->name, pwr, (*f)->name);
							FR8("\n\t\t/** @param idx_ index in fixed-sized array {@link #%s}, must be in range [0..{@link #%s_length_})"
								"\n\t\t * @param v_ new value in array {@link #%s} at index idx_"
								"\n\t\t * @return this */"
								"\n\t\tpublic %s %s_set(int idx_/*[0..%u)*/, %s v_) {"
								"\n\t\t\t%s[idx_] = v_;"
								"\n\t\t\treturn this;"
								"\n\t\t}", (*f)->name, (*f)->name, (*f)->name, s->s_name, (*f)->name, pwr, field_type, (*f)->name);
						}
						else {
							FR7("\n\t\t/** allocate, initiaize and set new value of {@link #%s} "
								"- fixed-sized array of {@link #%s_length_} elements"
								"\n\t\t * @param v_ element to put into newly allocated one-sized array"
								"\n\t\t * @return this */"
								"\n\t\tpublic %s set_%s(%s v_) {"
								"\n\t\t\t%s = new %s[] {v_};"
								"\n\t\t\treturn this;"
								"\n\t\t}", (*f)->name, (*f)->name, s->s_name, (*f)->name, field_type, (*f)->name, field_type);
							FR4("\n\t\t/** @return element of one-sized array {@link #%s} */"
								"\n\t\tpublic %s %s_get() {"
								"\n\t\t\treturn %s[0];"
								"\n\t\t}", (*f)->name, field_type, (*f)->name, (*f)->name);
							FR5("\n\t\t/** @param v_ new element of one-sized array {@link #%s}"
								"\n\t\t * @return this */"
								"\n\t\tpublic %s %s_set(%s v_) {"
								"\n\t\t\t%s[0] = v_;"
								"\n\t\t\treturn this;"
								"\n\t\t}", (*f)->name, s->s_name, (*f)->name, field_type, (*f)->name);
						}
					}
					else {
						unsigned bb = bytes_for_bits((*f)->f_power);
						FR6("\n\t\t/** @return {@link #%s} != null "
							"fixed-sized array of {@link #%s_bit_count_} bits ({@link #%s_length_} bytes) */"
							"\n\t\tpublic byte[/*%u*/] get_%s() {"
							"\n\t\t\treturn %s;"
							"\n\t\t}",
							(*f)->name, (*f)->name, (*f)->name, bb, (*f)->name, (*f)->name);
						FR10("\n\t\t/** @param %s_ new != null value of {@link #%s} - "
							"fixed-sized array of {@link #%s_bit_count_} bits ({@link #%s_length_} bytes)"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s set_%s(byte[/*%u*/] %s_/*!=null*/) {"
							"\n\t\t\t%s = %s_;"
							"\n\t\t\treturn this;"
							"\n\t\t}",
							(*f)->name, (*f)->name, (*f)->name, (*f)->name, s->s_name, (*f)->name, bb, (*f)->name, (*f)->name, (*f)->name);
						FR8("\n\t\t/** allocate and set new value of {@link #%s} - "
							"fixed-sized array of {@link #%s_bit_count_} bits ({@link #%s_length_} bytes)"
							"\n\t\t * @return new value of {@link #%s} which should be initialized by the caller */"
							"\n\t\tpublic byte[] new_%s() {"
							"\n\t\t\t%s = new byte[%u];"
							"\n\t\t\treturn %s;"
							"\n\t\t}", (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, bb, (*f)->name);
						FR5("\n\t\t/** @param bit_number bit to check in fixed-sized bits array {@link #%s}, "
							"must be in range [0..{@link #%s_bit_count_})"
							"\n\t\t * @return true if bit is set, false - otherwise */"
							"\n\t\tpublic boolean %s_get_bit(int bit_number/*[0..%u)*/) {"
							"\n\t\t\treturn bridge_get_bit(%s, bit_number);"
							"\n\t\t}", (*f)->name, (*f)->name, (*f)->name, (*f)->f_power, (*f)->name);
						FR6("\n\t\t/** @param bit_number bit to set in fixed-sized bits array {@link #%s}, "
							"must be in range [0..{@link #%s_bit_count_})"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s %s_set_bit(int bit_number/*[0..%u)*/) {"
							"\n\t\t\tbridge_set_bit(%s, bit_number);"
							"\n\t\t\treturn this;"
							"\n\t\t}", (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->f_power, (*f)->name);
						FR6("\n\t\t/** @param bit_number bit to clear in fixed-sized bits array {@link #%s}, "
							"must be in range [0..{@link #%s_bit_count_})"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s %s_clear_bit(int bit_number/*[0..%u)*/) {"
							"\n\t\t\tbridge_clear_bit(%s, bit_number);"
							"\n\t\t\treturn this;"
							"\n\t\t}", (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->f_power, (*f)->name);
						FR6("\n\t\t/** @param bit_number bit to change in fixed-sized bits array {@link #%s}, "
							"must be in range [0..{@link #%s_bit_count_})"
							"\n\t\t * @param set true to set bit, false - to clear"
							"\n\t\t * @return this */"
							"\n\t\tpublic %s %s_set_bit(int bit_number/*[0..%u)*/, boolean set) {"
							"\n\t\t\tbridge_set_bit(%s, bit_number, set);"
							"\n\t\t\treturn this;"
							"\n\t\t}", (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->f_power, (*f)->name);
					}
					break;
			}
		}
	}
}

static void generate_methods(FILE *file, const struct struct_def *s)
{
	const char *big_decl = s->convertable ? ", boolean big" : "";
	const char *big_arg = s->convertable ? ", big" : "";
	FR1("\n\t\t/** creates empty object, required fields must be properly filled before packing */"
		"\n\t\tpublic %s() {"
		"\n\t\t}", s->s_name);
	FR5("\n\t\t/** create new object and fill it from byte array"
		"\n\t\t * @param it position in mem array from where to read"
		"\n\t\t * @param mem input source of bytes"
		"\n\t\t * @param limit position in mem array after the last source byte%s"
		"\n\t\t * @return new unpacked object"
		"\n\t\t * @throws BridgeException if failed to unpack */"
		"\n\t\tpublic static %s unpack(BridgeIterator it, byte[] mem, int limit%s) throws BridgeException {"
		"\n\t\t\treturn %s_unpack(it, mem, limit%s);"
		"\n\t\t}",
		s->convertable ? "\n\t\t * @param big true to unpack integers from big-endian form, else - from little-endian" : "",
		s->s_name, big_decl, s->s_name, big_arg);
	FR4("\n\t\t/** pack object to byte array"
		"\n\t\t * @param head number of bytes to reserve at head of allocated array"
		"\n\t\t * @param tail number of bytes to reserve at tail of allocated array%s"
		"\n\t\t * @return new byte array with packed object in it"
		"\n\t\t * @throws BridgeException if failed to pack */"
		"\n\t\tpublic byte[] pack(int head, int tail%s) throws BridgeException {"
		"\n\t\t\treturn %s_pack(this, head, tail%s);"
		"\n\t\t}",
		s->convertable ? "\n\t\t * @param big true to pack integers in big-endian form, else - in little-endian" : "",
		big_decl, s->s_name, big_arg);
}

FILE *generate_java(FILE *file, char *fname, const char *pkg/*NULL?*/)
{
	{
		size_t js = strlen(fname);
		size_t dot = 0;
		for (; js && fname[js - 1] != '\\' && fname[js - 1] != '/'; js--) {
			if ('.' == fname[js - 1])
				dot = js;
		}
		FR2("/* %s */"
			"\n"
			"\n/* Generated by %s. Do not edit! */"
			"\n", fname + js, jcompiler_logo);
		if (pkg) {
			FR1("\npackage %s;"
				"\n", pkg);
		}
		if (dot)
			fname[dot - 1] = '\0';
		FR1("\nimport static bitbridge.Bridge.*;"
			"\nimport static bitbridge.BridgePriv.*;"
			"\n"
			"\npublic class %s {"
			"\n", fname + js);
		if (dot)
			fname[dot - 1] = '.';
	}
	{
		const char *enum_type = ((size_t)(structs_end - structs) <= 0xFFFFFFFF) ? "int" : "long";
		const char *enum_suff = ((size_t)(structs_end - structs) <= 0xFFFFFFFF) ? "" : "L";
		{
			struct struct_def **s = structs;
			for (; s < structs_end; s++) {
				FR4("\n\t/** bridge generated structure */"
					"\n\tpublic static final class %s {"
					"\n\t\t/** type index for global bridge pack()/unpack() functions */"
					"\n\t\tpublic static final %s _type_idx_ = 0x%llx%s;",
					(*s)->s_name, enum_type, (unsigned long long)(s - structs), enum_suff);
				print_structure_fields(*s, file);
				generate_methods(file, *s);
				FP("\n\t}"
					"\n");
			}
		}
		FR1("\n\t/** pack bridge generated structure to byte array"
			"\n\t * @param bridge_object bridge object to pack"
			"\n\t * @param type_idx bridge generated structure type index, "
			"if S - bridge generated structure then type index will be S._type_idx_"
			"\n\t * @param head number of bytes to reserve at head of allocated array"
			"\n\t * @param tail number of bytes to reserve at tail of allocated array"
			"\n\t * @param big true to pack integers in big-endian form, else - in little-endian"
			"\n\t * @return new byte array with packed object in it"
			"\n\t * @throws BridgeException if failed to pack */"
			"\n\tpublic static byte[] pack(Object bridge_object, %s type_idx, int head, int tail, boolean big) throws BridgeException {"
			"\n\t\tswitch (type_idx) {", enum_type);
		{
			struct struct_def **s = structs;
			for (; s < structs_end; s++) {
				FR4("\n\t\t\tcase %s._type_idx_: return %s_pack((%s)bridge_object, head, tail%s);",
					(*s)->s_name, (*s)->s_name, (*s)->s_name, (*s)->convertable ? ", big" : "");
			}
		}
		FR1("\n\t\t\tdefault: return null;"
			"\n\t\t}"
			"\n\t}"
			"\n"
			"\n\t/** unpack bridge generated structure from byte array"
			"\n\t * @param type_idx bridge generated structure type index, "
			"if S - bridge generated structure then type index will be S._type_idx_"
			"\n\t * @param it position in mem array from where to read"
			"\n\t * @param mem input source of bytes"
			"\n\t * @param limit position in mem array after the last source byte"
			"\n\t * @param big true to unpack integers from big-endian form, else - from little-endian"
			"\n\t * @return new unpacked object"
			"\n\t * @throws BridgeException if failed to unpack */"
			"\n\tpublic static Object unpack(%s type_idx, BridgeIterator it, byte[] mem, int limit, boolean big) throws BridgeException {"
			"\n\t\tswitch (type_idx) {", enum_type);
		{
			struct struct_def **s = structs;
			for (; s < structs_end; s++) {
				FR3("\n\t\t\tcase %s._type_idx_: return %s_unpack(it, mem, limit%s);",
					(*s)->s_name, (*s)->s_name, (*s)->convertable ? ", big" : "");
			}
		}
		FP("\n\t\t\tdefault: return null;"
			"\n\t\t}"
			"\n\t}"
			"\n"
			"\n\t/* private */"
			"\n");
	}
	check_unchecked_exceptions();
	mark_strutures_for_unpack();
	{
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			generate_code_pack(file, *s);
			generate_code_unpack(file, *s);
		}
	}
	FP("}"
		"\n");
	return file;
}
