/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* struct_gen.c */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h" /* for print_header_header_file_define() */
#include "ptypes.h"
#include "compiler.h"
#include "model.h"

#define FP(s) fputs(s,file)
#define FR1(s,a1)                                               fprintf(file,s,a1)
#define FR2(s,a1,a2)                                            fprintf(file,s,a1,a2)
#define FR3(s,a1,a2,a3)                                         fprintf(file,s,a1,a2,a3)
#define FR4(s,a1,a2,a3,a4)                                      fprintf(file,s,a1,a2,a3,a4)
#define FR5(s,a1,a2,a3,a4,a5)                                   fprintf(file,s,a1,a2,a3,a4,a5)
#define FR6(s,a1,a2,a3,a4,a5,a6)                                fprintf(file,s,a1,a2,a3,a4,a5,a6)
#define FR7(s,a1,a2,a3,a4,a5,a6,a7)                             fprintf(file,s,a1,a2,a3,a4,a5,a6,a7)
#define FR8(s,a1,a2,a3,a4,a5,a6,a7,a8)                          fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8)
#define FR9(s,a1,a2,a3,a4,a5,a6,a7,a8,a9)                       fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9)
#define FR10(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1)                   fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1)
#define FR11(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2)                fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2)
#define FR12(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3)             fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3)
#define FR13(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4)          fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4)
#define FR14(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5)       fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5)
#define FR15(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5,b6)    fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5,b6)
#define FR16(s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5,b6,b7) fprintf(file,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5,b6,b7)

static void generate_prototypes(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/, int w)
{
	if (w & 4)
		FP("\n");
	if (w & 1) {
		FR6("\n"
			"\n/* cleanup fields of structure %s before filling it,"
			"\n  otherwise %s_ac_destroy() called for partially filled structure %s may crash */"
			"\n#define %s_init(s)           do {struct %s *__s = s; %s;} while ((void)0,0)",
			s->s_name, s->s_name, s->s_name, s->s_name, s->s_name,
			s->s_layout >= S_MONOLITH ? "_bridge_memzero(__s, sizeof(*__s))" : "_bridge_nonnull(__s)");
	}
	if (w & 2) {
		FR5("\n"
			"\n/* allocate new structure %s, then cleanup its fields */"
			"\n#define %s_ac_new(ac)        ((struct %s*)_br_alloc%s_mem(sizeof(struct %s), BRIDGE_WRAP_ALLOCATOR(ac)))",
			s->s_name, s->s_name, s->s_name, s->s_layout >= S_MONOLITH ? "z" : "", s->s_name);
	}
	if (s->need_destroy) {
		if (w & 4)
			FR2("\n#define _%s_ac_destroy       BRIDGE_EXPORT_SUFFIX(_%s_ac_destroy)", s->s_name, s->s_name);
		if (w & 4)
			FR2("\n#define _%s_ac_delete        BRIDGE_EXPORT_SUFFIX(_%s_ac_delete)", s->s_name, s->s_name);
		if (w & 4) {
			FR4("\n%s%sA_Nonnull_all_args"
				"\nvoid _%s_ac_destroy(A_Pre_valid A_Post_invalid struct %s *s, BRIDGE_ALLOCATOR_ARG(ac));",
				decl ? decl : "", decl ? " " : "", s->s_name, s->s_name);
		}
		if (w & 4) {
			FR4("\n%s%sA_Nonnull_arg(2)"
				"\nvoid _%s_ac_delete(A_Pre_opt_valid A_Post_ptr_invalid struct %s *s, BRIDGE_ALLOCATOR_ARG(ac));",
				decl ? decl : "", decl ? " " : "", s->s_name, s->s_name);
		}
	}
	if (w & 2) {
		FR2("\n"
			"\n/* destroy structure %s: free its fields */"
			"\n#define %s_ac_destroy(s, ac) ", s->s_name, s->s_name);
		if (s->need_destroy)
			FR1("_%s_ac_destroy(s, BRIDGE_WRAP_ALLOCATOR(ac))", s->s_name);
		else
			FR1("do {struct %s *__s = s; _bridge_ac_nonnull(__s, ac);} while ((void)0,0)", s->s_name);
	}
	if (w & 2) {
		FR3("\n"
			"\n/* destroy structure %s via %s_ac_destroy(), then deallocate it"
			"\n  NOTE: does nothing if s is NULL */"
			"\n#define %s_ac_delete(s, ac)  ", s->s_name, s->s_name, s->s_name);
		if (s->need_destroy)
			FR1("_%s_ac_delete(s, BRIDGE_WRAP_ALLOCATOR(ac))", s->s_name);
		else
			FR1("do {struct %s *__s = s; _bridge_free_mem(__s, BRIDGE_WRAP_ALLOCATOR(ac));} while ((void)0,0)", s->s_name);
	}
	if (w & 1) {
		FR3("\n"
			"\n/* allocate new structure %s via BRIDGE_DEFAULT_ALLOCATOR, then cleanup its fields"
			"\n  returns NULL if failed to allocate */"
			"\n#define %s_new()             %s_ac_new(BRIDGE_DEFAULT_ALLOCATOR)", s->s_name, s->s_name, s->s_name);
	}
	if (w & 1) {
		FR3("\n"
			"\n/* destroy structure %s: free its fields via BRIDGE_DEFAULT_ALLOCATOR */"
			"\n#define %s_destroy(s)        %s_ac_destroy(s, BRIDGE_DEFAULT_ALLOCATOR)", s->s_name, s->s_name, s->s_name);
	}
	if (w & 1) {
		FR4("\n"
			"\n/* destroy structure %s via %s_destroy(s), then deallocate it via BRIDGE_DEFAULT_ALLOCATOR"
			"\n  NOTE: does nothing if s is NULL */"
			"\n#define %s_delete(s)         %s_ac_delete(s, BRIDGE_DEFAULT_ALLOCATOR)", s->s_name, s->s_name, s->s_name, s->s_name);
	}
	if (w & 4)
		FR2("\n#define _%s_pack             BRIDGE_EXPORT_SUFFIX(_%s_pack)", s->s_name, s->s_name);
	if (w & 4)
		FR2("\n#define _%s_unpack_to        BRIDGE_EXPORT_SUFFIX(_%s_unpack_to)", s->s_name, s->s_name);
	if (w & 1) {
		FR4("\n"
			"\n/* pack fields of structure %s to buffer"
			"\n  returns NULL if failed to pack"
			"\n  else returns newly allocated buffer containing packed fields of the structure"
			"\n  NOTE: on success, returned buffer size is stored in in->pi.size */"
			"\n#define %s_pack(s, in)       _%s_pack(s, BRIDGE_WRAP_PACK_%sINFO(in))",
			s->s_name, s->s_name, s->s_name, s->convertable ? "CONV_" : "");
	}
	if (w & 2) {
		FR4("\n"
			"\n/* unpack fields of structure %s from buffer"
			"\n  if s is NULL, then new structure is allocated and filled"
			"\n  else - s is used as destination structure (its fields values are not checked and just overwritten)"
			"\n  returns NULL if failed to unpack (if s != NULL, then its fields contain garbage)"
			"\n  else returns s if it's not NULL"
			"\n  else returns newly allocated and filled structure"
			"\n  NOTE: on success in->ui.us.from will point to next byte after unpacked fields data */"
			"\n#define %s_unpack_to(s, in)  _%s_unpack_to(s, BRIDGE_WRAP_UNPACK_%sINFO(in))",
			s->s_name, s->s_name, s->s_name, s->convertable ? "CONV_" : "");
	}
	if (w & 1) {
		FR3("\n"
			"\n/* allocate new structure %s and fill its fields from input buffer"
			"\n  returns NULL if failed to allocate or unpack"
			"\n  else returns newly allocated and filled structure"
			"\n  NOTE: on success in->ui.us.from will point to next byte after unpacked fields data */"
			"\n#define %s_unpack(in)        %s_unpack_to(NULL, in)", s->s_name, s->s_name, s->s_name);
	}
	if (w & 4) {
		generate_prototype_pack(file, s, decl);
		FP(";");
	}
	if (w & 4) {
		generate_prototype_unpack(file, s, decl);
		FP(";");
	}
	{
		struct field_def **f = s->fields;
		for (; f < s->fields_end; f++) {
			switch ((*f)->f_power) {
				case F_REQUIRED:
					break;
				case F_OPTIONAL:
					if ((*f)->user_type) {
						if (w & 4) {
							FR9("\nA_Nonnull_all_args A_Check_return"
								"\nstatic inline struct %s *_%s_ac_new_%s(A_Inout struct %s *s, BRIDGE_ALLOCATOR_ARG(ac)) {"
								"\n\tstruct %s *a = (struct %s*)_br_alloc%s_mem(sizeof(*s->%s), BRIDGE_PASS_ALLOCATOR(ac));"
								"\n\tif (a) s->%s = a;"
								"\n\tbridge_allocator_dec_level(ac);"
								"\n\treturn a;"
								"\n}",
								(*f)->type, s->s_name, (*f)->name, s->s_name, (*f)->type, (*f)->type,
								(*f)->user_type->s_layout >= S_MONOLITH ? "z" : "", (*f)->name, (*f)->name);
						}
						{
							int mask = 1;
							for (;;) {
								if (w & mask) {
									FR4("\n"
										"\n/* allocate and set new value of optional pointer %s%s"
										"\n  returns NULL if failed to allocate"
										"\n  else returns newly allocated value"
										"\n  NOTE: old value is not checked and just overwritten"
										"\n  NOTE: new value is properly cleaned up for %s%s_destroy(), "
										"optional fields are marked as unset */",
										(*f)->name, 1 == mask ? " via BRIDGE_DEFAULT_ALLOCATOR" : "", s->s_name, 1 == mask ? "" : "_ac");
									if (1 == mask) {
										FR4("\n#define %s_new_%s(s)        %s_ac_new_%s(s, BRIDGE_DEFAULT_ALLOCATOR)",
											s->s_name, (*f)->name, s->s_name, (*f)->name);
									}
									else {
										FR4("\n#define %s_ac_new_%s(s, ac) _%s_ac_new_%s(s, BRIDGE_WRAP_ALLOCATOR(ac))",
											s->s_name, (*f)->name, s->s_name, (*f)->name);
									}
								}
								if (2 == mask)
									break;
								mask = 2;
							}
						}
					}
					break;
				case F_ARRAY:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						const char *ss = (*f)->user_type ? "struct " : "";
						const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
						int zmem = (*f)->user_type ? (*f)->user_type->s_layout >= S_MONOLITH : is_simple_str_type((*f)->type);
						if (w & 4) {
							FR16("\nA_Nonnull_all_args A_Check_return"
								"\nstatic inline %s%s *_%s_ac_new_%s("
								"\n\tA_Inout struct %s *s, A_In_range(>,0) size_t %s_count_, BRIDGE_ALLOCATOR_ARG(ac)) {"
								"\n\t%s%s *a = (%s%s*)_br_alloc%s_mem(%s_count_*sizeof(*s->%s), BRIDGE_PASS_ALLOCATOR(ac));"
								"\n\tif (a) s->%s_end = (s->%s = a) + %s_count_;"
								"\n\tbridge_allocator_dec_level(ac);"
								"\n\treturn a;"
								"\n}",
								ss, native, s->s_name, (*f)->name, s->s_name, (*f)->name, ss, native, ss, native,
								zmem ? "z" : "", (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
						}
						{
							int mask = 1;
							for (;;) {
								if (w & mask) {
									FR3("\n"
										"\n/* allocate and set new value of array %s%s"
										"\n  %s_count_ - number of elements in new array - must be non-zero"
										"\n  returns NULL if failed to allocate"
										"\n  else returns newly allocated array"
										"\n  NOTE: old value is not checked and just overwritten",
										(*f)->name, 1 == mask ? " via BRIDGE_DEFAULT_ALLOCATOR" : "", (*f)->name);
									if ((*f)->user_type || is_simple_str_type((*f)->type)) {
										FR2("\n  NOTE: new array items are properly cleaned up for %s%s_destroy()",
											s->s_name, 1 == mask ? "" : "_ac");
									}
									if ((*f)->user_type)
										FP(", optional fields are marked as unset");
									FP(" */");
									if (1 == mask) {
										FR6("\n#define %s_new_%s(s, %s_count_)        "
											"%s_ac_new_%s(s, %s_count_, BRIDGE_DEFAULT_ALLOCATOR)",
											s->s_name, (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->name);
									}
									else {
										FR6("\n#define %s_ac_new_%s(s, %s_count_, ac) "
											"_%s_ac_new_%s(s, %s_count_, BRIDGE_WRAP_ALLOCATOR(ac))",
											s->s_name, (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->name);
									}
								}
								if (2 == mask)
									break;
								mask = 2;
							}
						}
					}
					else {
						if (w & 4) {
							/* NOTE: zero-initialize bit arrays */
							FR8("\nA_Nonnull_all_args A_Check_return"
								"\nstatic inline unsigned char *_%s_ac_new_%s("
								"\n\tA_Inout struct %s *s, A_In_range(>,0) unsigned %s_bit_count_, BRIDGE_ALLOCATOR_ARG(ac)) {"
								"\n\tunsigned char *a = (unsigned char*)_br_allocz_mem("
								"bridge_bytes_for_nonzero_bits(%s_bit_count_), BRIDGE_PASS_ALLOCATOR(ac));"
								"\n\tif (a) {s->%s = a; s->%s_bit_count = %s_bit_count_;}"
								"\n\tbridge_allocator_dec_level(ac);"
								"\n\treturn a;"
								"\n}", s->s_name, (*f)->name, s->s_name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
						}
						{
							int mask = 1;
							for (;;) {
								if (w & 1) {
									FR3("\n"
										"\n/* allocate and set new value of bits array %s%s"
										"\n  %s_bit_count_ - number of bits in new array - must be non-zero"
										"\n  returns NULL if failed to allocate"
										"\n  else returns newly allocated bits array"
										"\n  NOTE: old value is not checked and just overwritten"
										"\n  NOTE: all bits in new array are zero */",
										(*f)->name, 1 == mask ? " via BRIDGE_DEFAULT_ALLOCATOR" : "", (*f)->name);
									if (1 == mask) {
										FR6("\n#define %s_new_%s(s, %s_bit_count_)        "
											"%s_ac_new_%s(s, %s_bit_count_, BRIDGE_DEFAULT_ALLOCATOR)",
											s->s_name, (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->name);
									}
									else {
										FR6("\n#define %s_ac_new_%s(s, %s_bit_count_, ac) "
											"_%s_ac_new_%s(s, %s_bit_count_, BRIDGE_WRAP_ALLOCATOR(ac))",
											s->s_name, (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->name);
									}
								}
								if (2 == mask)
									break;
								mask = 2;
							}
						}
					}
					break;
				case F_POINTER:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						const char *ss = (*f)->user_type ? "struct " : "";
						const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
						int zmem = (*f)->user_type ? (*f)->user_type->s_layout >= S_MONOLITH : is_simple_str_type((*f)->type);
						if (w & 4) {
							FR12("\nA_Nonnull_all_args A_Check_return"
								"\nstatic inline %s%s *_%s_ac_new_%s(A_Inout struct %s *s, BRIDGE_ALLOCATOR_ARG(ac)) {"
								"\n\t%s%s *a = (%s%s*)_br_alloc%s_mem(sizeof(*s->%s), BRIDGE_PASS_ALLOCATOR(ac));"
								"\n\tif (a) s->%s = a;"
								"\n\tbridge_allocator_dec_level(ac);"
								"\n\treturn a;"
								"\n}",
								ss, native, s->s_name, (*f)->name, s->s_name, ss, native, ss, native,
								zmem ? "z" : "", (*f)->name, (*f)->name);
						}
						{
							int mask = 1;
							for (;;) {
								if (w & 1) {
									FR2("\n"
										"\n/* allocate and set new value of required pointer %s%s"
										"\n  returns NULL if failed to allocate"
										"\n  else returns newly allocated value"
										"\n  NOTE: old value is not checked and just overwritten",
										(*f)->name, 1 == mask ? " via BRIDGE_DEFAULT_ALLOCATOR" : "");
									if ((*f)->user_type || is_simple_str_type((*f)->type)) {
										FR2("\n  NOTE: new value is properly cleaned up for %s%s_destroy()",
											s->s_name, 1 == mask ? "" : "_ac");
									}
									if ((*f)->user_type)
										FP(", optional fields are marked as unset");
									FP(" */");
									if (1 == mask) {
										FR4("\n#define %s_new_%s(s)        %s_ac_new_%s(s, BRIDGE_DEFAULT_ALLOCATOR)",
											s->s_name, (*f)->name, s->s_name, (*f)->name);
									}
									else {
										FR4("\n#define %s_ac_new_%s(s, ac) _%s_ac_new_%s(s, BRIDGE_WRAP_ALLOCATOR(ac))",
											s->s_name, (*f)->name, s->s_name, (*f)->name);
									}
								}
								if (2 == mask)
									break;
								mask = 2;
							}
						}
						break;
					}
					/* fall through */
				default: {
					const char *ss = (*f)->user_type ? "struct " : "";
					const char *native = (*f)->user_type ? (*f)->type :
						field_type_bit == (*f)->type ? "unsigned char" :
						get_native_type((*f)->type);
					/* NOTE: zero-initialize bit arrays */
					int zmem = (*f)->user_type ? (*f)->user_type->s_layout >= S_MONOLITH :
						(field_type_bit == (*f)->type || is_simple_str_type((*f)->type));
					unsigned pwr = ((*f)->user_type || field_type_bit != (*f)->type) ? (*f)->f_power : bytes_for_bits((*f)->f_power);
					if (w & 4) {
						FR15("\nA_Nonnull_all_args A_Check_return"
							"\nstatic inline %s%s (*_%s_ac_new_%s(A_Inout struct %s *s, BRIDGE_ALLOCATOR_ARG(ac)))[%uu] {"
							"\n\t%s%s (*a)[%uu] = (%s%s(*)[%uu])_br_alloc%s_mem(sizeof(*s->%s), BRIDGE_PASS_ALLOCATOR(ac));"
							"\n\tif (a) s->%s = a;"
							"\n\tbridge_allocator_dec_level(ac);"
							"\n\treturn a;"
							"\n}",
							ss, native, s->s_name, (*f)->name, s->s_name, pwr, ss, native, pwr, ss, native, pwr,
							zmem ? "z" : "", (*f)->name, (*f)->name);
					}
					{
						int mask = 1;
						const char *bits = ((*f)->user_type && field_type_bit == (*f)->type) ? "bits " : "";
						for (;;) {
							if (w & 1) {
								FR4("\n"
									"\n/* allocate and set new value of fixed-sized %sarray %s%s"
									"\n  returns NULL if failed to allocate"
									"\n  else returns newly allocated %sarray"
									"\n  NOTE: old value is not checked and just overwritten",
									bits, (*f)->name, 1 == mask ? " via BRIDGE_DEFAULT_ALLOCATOR" : "", bits);
								if ((*f)->user_type || is_simple_str_type((*f)->type)) {
									FR2("\n  NOTE: new array items are properly cleaned up for %s%s_destroy()",
										s->s_name, 1 == mask ? "" : "_ac");
								}
								else if (field_type_bit == (*f)->type)
									FP("\n  NOTE: all bits in new array are zero");
								if ((*f)->user_type)
									FP(", optional fields are marked as unset");
								FP(" */");
								if (1 == mask) {
									FR4("\n#define %s_new_%s(s)        %s_ac_new_%s(s, BRIDGE_DEFAULT_ALLOCATOR)",
										s->s_name, (*f)->name, s->s_name, (*f)->name);
								}
								else {
									FR4("\n#define %s_ac_new_%s(s, ac) _%s_ac_new_%s(s, BRIDGE_WRAP_ALLOCATOR(ac))",
										s->s_name, (*f)->name, s->s_name, (*f)->name);
								}
							}
							if (2 == mask)
								break;
							mask = 2;
						}
					}
					break;
				}
			}
		}
	}
}

static void print_structure(FILE *file, const struct struct_def *s)
{
	struct field_def **f;
	unsigned bit = 0;
	FR1("\n"
		"\nstruct %s {", s->s_name);
	do {
		for (f = s->fields; f < s->fields_end; f++) {
			if (bit == (*f)->bit && !(*f)->user_type) {
				if ((F_OPTIONAL == (*f)->f_power && !is_simple_str_type((*f)->type)) ||
					(F_REQUIRED == (*f)->f_power && field_type_bit == (*f)->type))
				{
					if (F_OPTIONAL == (*f)->f_power) {
						FR3("\n\tunsigned has_%s: 1; /* non-zero if value of optional %s %s was set, else - zero */",
							(*f)->name, field_type_bit == (*f)->type ? "bit" : "field", (*f)->name);
						bit++;
					}
					if (field_type_bit == (*f)->type) {
						FR3("\n\tunsigned %s: 1; /* value of %s bit %s */",
							(*f)->name, F_OPTIONAL == (*f)->f_power ? "optional" : "required", (*f)->name);
						bit++;
					}
					break;
				}
			}
		}
	} while (f < s->fields_end);
	for (f = s->fields; f < s->fields_end; f++) {
		switch ((*f)->f_power) {
			case F_ARRAY:
				if ((*f)->user_type || field_type_bit != (*f)->type) {
					const char *ss = (*f)->user_type ? "struct " : "";
					const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
					FR7("\n\t%s%s *%s;     /* points to the first array element, may be NULL if %s_end is also NULL */"
						"\n\t%s%s *%s_end; /* points next to the last array element */",
						ss, native, (*f)->name, (*f)->name, ss, native, (*f)->name);
				}
				else {
					FR4("\n\tunsigned char *%s;     /* array of bits, may be NULL if %s_bit_count is zero */"
						"\n\tunsigned %s_bit_count; /* number of filed bits in bits array %s */",
						(*f)->name, (*f)->name, (*f)->name, (*f)->name);
				}
				break;
			case F_OPTIONAL:
			case F_REQUIRED:
				if ((*f)->user_type) {
					FR4("\n\tstruct %s %s%s; /* %s */",
						(*f)->type, F_OPTIONAL == (*f)->f_power ? "*" : "", (*f)->name,
						F_OPTIONAL == (*f)->f_power ? "optional, may be NULL if not set" : "required");
				}
				else if (field_type_bit != (*f)->type) {
					FR2("\n\t%s %s;", get_native_type((*f)->type), (*f)->name);
					if (is_simple_str_type((*f)->type)) {
						if (F_OPTIONAL == (*f)->f_power)
							FP(" /* optional, may be NULL if not set */");
						else
							FP(" /* required, NULL value packed as empty string, != NULL after unpack() */");
					}
					else if (F_OPTIONAL == (*f)->f_power)
						FR1(" /* optional, has_%s bit is non-zero if field is set and zero otherwise */", (*f)->name);
					else
						FP(" /* required */");
				}
				break;
			case F_POINTER:
				if ((*f)->user_type || field_type_bit != (*f)->type) {
					const char *ss = (*f)->user_type ? "struct " : "";
					const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
					FR3("\n\t%s%s *%s; /* required, must be != NULL */", ss, native, (*f)->name);
					break;
				}
				/* fall through */
			default:
				if ((*f)->user_type)
					FR3("\n\tstruct %s (*%s)[%uu]; /* required, must be != NULL */", (*f)->type, (*f)->name, (*f)->f_power);
				else if (field_type_bit == (*f)->type) {
					FR6("\n#define %s_%s_bit_count %uu /* number of bits in fixed-sized bits array %s */"
						"\n\tunsigned char (*%s)[%uu];",
						s->s_name, (*f)->name, (*f)->f_power, (*f)->name, (*f)->name, bytes_for_bits((*f)->f_power));
				}
				else
					FR3("\n\t%s (*%s)[%uu]; /* required, must be != NULL */", get_native_type((*f)->type), (*f)->name, (*f)->f_power);
				break;
		}
	}
	{
		const char *conv = s->convertable ? "conv_" : "";
		FR6("\n#ifdef __cplusplus"
			"\n\t/* cleanup fields before filling the structure"
			"\n\t  so ac_destroy() called for partially filled structure will not crash */"
			"\n\tinline void init();"
			"\n\t/* destroy the structure: free its fields */"
			"\n\tinline void ac_destroy(A_Inout struct bridge_allocator &ac);"
			"\n\t/* destroy the structure via ac_destroy(), then deallocate it */"
			"\n\tinline void ac_delete(A_Inout struct bridge_allocator &ac);"
			"\n\t/* allocate new structure, then cleanup its fields"
			"\n\t  returns NULL if failed to allocate */"
			"\n\tA_Check_return"
			"\n\tstatic inline %s *ac_new(A_Inout struct bridge_allocator &ac);"
			"\n\t/* allocate new structure, then unpack its fields from buffer"
			"\n\t  returns NULL if failed to allocate or unpack"
			"\n\t  else returns newly allocated and filled structure"
			"\n\t  NOTE: on success in->ui.us.from will point to next byte after unpacked fields data */"
			"\n\tA_Check_return"
			"\n\tstatic inline %s *unpack_new(A_Inout union bridge_unpack_%sinfo &in);"
			"\n\t/* unpack structure fields from buffer, old fields values are not checked and just overwritten"
			"\n\t  returns NULL if failed to unpack, structure fields contain garbage"
			"\n\t  else returns pointer to this structure"
			"\n\t  NOTE: on success in->ui.us.from will point to next byte after unpacked fields data */"
			"\n\tA_Check_return"
			"\n\tinline %s *unpack(A_Inout union bridge_unpack_%sinfo &in);"
			"\n\t/* pack structure fields to buffer"
			"\n\t  returns NULL if failed to pack"
			"\n\t  else returns newly allocated buffer containing packed fields of the structure"
			"\n\t  NOTE: on success, returned buffer size is stored in in->pi.size */"
			"\n\tA_Check_return"
			"\n\tinline void *pack(A_Inout union bridge_pack_%sinfo &in) const;",
			s->s_name, s->s_name, conv, s->s_name, conv, conv);
	}
	/* print get_...() and set_...() methods for all fields, unset_...() methods for non-pointer optional fields,
	  new_...() for arrays and pointers */
	for (f = s->fields; f < s->fields_end; f++) {
		switch ((*f)->f_power) {
			case F_ARRAY:
				if ((*f)->user_type || field_type_bit != (*f)->type) {
					const char *ss = (*f)->user_type ? "struct " : "";
					const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
					FR14("\n\t/* get pointer to first element of array %s"
						"\n\t  may return NULL if array is empty (get_%s_end() will also return NULL) */"
						"\n\tA_Check_return"
						"\n\t%s%s *get_%s() const {"
						"\n\t\treturn %s;"
						"\n\t}"
						"\n\t/* get pointer to one past the last element of array %s */"
						"\n\tA_Check_return"
						"\n\t%s%s *get_%s_end() const {"
						"\n\t\treturn %s_end;"
						"\n\t}"
						"\n\t/* get number of elements in array %s */"
						"\n\tA_Check_return"
						"\n\t%ssize_t get_%s_count() const",
						(*f)->name, (*f)->name, ss, native, (*f)->name, (*f)->name, (*f)->name,
						ss, native, (*f)->name, (*f)->name, (*f)->name, (*f)->user_type ? "inline " : "", (*f)->name);
					if ((*f)->user_type)
						FP(";");
					else {
						FR2(" {"
							"\n\t\treturn (size_t)(%s_end - %s);"
							"\n\t}", (*f)->name, (*f)->name);
					}
					FR15("\n\t/* set new value of array %s"
						"\n\t  returns set value"
						"\n\t  NOTE: %s_ may be NULL if %s_size_ is zero"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: this structure takes control over set array - it will be deleted in ac_destroy()"
						"\n\t  NOTE: ac_destroy() will ignore set array if %s_ is NULL, even if %s_size_ is non-zero */"
						"\n\tA_Post_satisfies(return == %s_)"
						"\n\t%s%s%s *set_%s("
						"\n\t\tA_When(A_Curr != NULL, A_Readable_elements(%s_size_)) %s%s *%s_,"
						"\n\t\tsize_t %s_size_)",
						(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name,
						(*f)->user_type ? "inline " : "", ss, native, (*f)->name, (*f)->name, ss, native, (*f)->name, (*f)->name);
					if ((*f)->user_type)
						FP(";");
					else {
						FR5("\n\t{"
							"\n\t\t%s_end = (%s = %s_) + %s_size_;"
							"\n\t\treturn %s_;"
							"\n\t}", (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
				}
				else {
					FR10("\n\t/* get pointer to first byte of bits array %s"
						"\n\t  may return NULL if array is empty (get_%s_bit_count() will return 0) */"
						"\n\tA_Check_return"
						"\n\tunsigned char *get_%s() const {"
						"\n\t\treturn %s;"
						"\n\t}"
						"\n\t/* get number of filled bits in bits array %s */"
						"\n\tA_Check_return"
						"\n\tunsigned get_%s_bit_count() const {"
						"\n\t\treturn %s_bit_count;"
						"\n\t}"
						"\n\t/* get number of filled bytes in bits array %s */"
						"\n\tA_Check_return"
						"\n\tunsigned get_%s_size() const {"
						"\n\t\treturn bridge_bytes_for_bits(%s_bit_count);"
						"\n\t}",
						(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name,
						(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					FR15("\n\t/* set new value of bits array %s"
						"\n\t  returns set value"
						"\n\t  NOTE: %s_ may be NULL if %s_bit_count_ is zero"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: this structure takes control over set bits array - it will be deleted in ac_destroy()"
						"\n\t  NOTE: ac_destroy() will ignore set bits array if %s_ is NULL, even if %s_bit_count_ is non-zero */"
						"\n\tA_Post_satisfies(return == %s_)"
						"\n\tunsigned char *set_%s("
						"\n\t\tA_When(A_Curr != NULL, A_Readable_elements(_bridge_bytes_for_bits(%s_bit_count_))) unsigned char *%s_,"
						"\n\t\tunsigned %s_bit_count_)"
						"\n\t{"
						"\n\t\t%s = %s_;"
						"\n\t\t%s_bit_count = %s_bit_count_;"
						"\n\t\treturn %s_;"
						"\n\t}",
						(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name,
						(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
				}
				if ((*f)->user_type || field_type_bit != (*f)->type) {
					const char *ss = (*f)->user_type ? "struct " : "";
					const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
					FR2("\n\t/* allocate and set new value of array %s"
						"\n\t  %s_count_ - number of elements in new array - must be non-zero"
						"\n\t  returns NULL if failed to allocate"
						"\n\t  else returns newly allocated array"
						"\n\t  NOTE: old value is not checked and just overwritten", (*f)->name, (*f)->name);
					if ((*f)->user_type || is_simple_str_type((*f)->type))
						FP("\n\t  NOTE: new array items are properly cleaned up for ac_destroy()");
					if ((*f)->user_type)
						FP(", optional fields are marked as unset");
					FR4(" */"
						"\n\tA_Check_return"
						"\n\tinline %s%s *new_%s("
						"\n\t\tA_In_range(>,0) size_t %s_count_,"
						"\n\t\tA_Inout struct bridge_allocator &ac);", ss, native, (*f)->name, (*f)->name);
					if ((*f)->user_type) {
						FR6("\n\t/* get constant reference to element of array %s at given index */"
							"\n\tA_Check_return"
							"\n\tinline const struct %s &%s_get(size_t idx_) const;"
							"\n\t/* get reference to element of array %s at given index */"
							"\n\tA_Check_return"
							"\n\tinline struct %s &%s_get(size_t idx_);",
							(*f)->name, (*f)->type, (*f)->name, (*f)->name, (*f)->type, (*f)->name);
					}
					else if (is_simple_str_type((*f)->type)) {
						FR7("\n\t/* get element of strings array %s at given index"
							"\n\t  returns non-NULL string after unpack()"
							"\n\t  may return NULL if string at given index is not set yet */"
							"\n\tA_Check_return A_Ret_maybenull_z"
							"\n\tchar *%s_get(size_t idx_) const {"
							"\n\t\treturn %s[idx_];"
							"\n\t}"
							"\n\t/* set element of strings array %s at given index"
							"\n\t  returns set string"
							"\n\t  NOTE: new value may be NULL - it will be packed as empty string"
							"\n\t  NOTE: old string at given index is not checked and just overwritten"
							"\n\t  NOTE: array %s takes control over set string - it will be deleted in ac_destroy() */"
							"\n\tA_Ret_maybenull_z A_Post_satisfies(return == v_)"
							"\n\tchar *%s_set(size_t idx_, A_In_opt_z char *v_) {"
							"\n\t\treturn %s[idx_] = v_;"
							"\n\t}", (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
					else {
						FR9("\n\t/* get element of array %s at given index */"
							"\n\tA_Check_return"
							"\n\t%s %s_get(size_t idx_) const {"
							"\n\t\treturn %s[idx_];"
							"\n\t}"
							"\n\t/* set element of array %s at given index"
							"\n\t  returns reference to this structure */"
							"\n\tstruct %s &%s_set(size_t idx_, %s v_) {"
							"\n\t\t%s[idx_] = v_;"
							"\n\t\treturn *this;"
							"\n\t}", (*f)->name, native, (*f)->name, (*f)->name, (*f)->name, s->s_name, (*f)->name, native, (*f)->name);
					}
				}
				else {
					FR4("\n\t/* allocate and set new value of bits array %s"
						"\n\t  %s_bit_count_ - number of bits in new array - must be non-zero"
						"\n\t  returns NULL if failed to allocate"
						"\n\t  else returns newly allocated bits array"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: all bits in new array are zero */"
						"\n\tA_Check_return"
						"\n\tinline unsigned char *new_%s("
						"\n\t\tA_In_range(>,0) unsigned %s_bit_count_,"
						"\n\t\tA_Inout struct bridge_allocator &ac);", (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					FR3("\n\t/* check bit in bits array %s at given index"
						"\n\t  returns true if bit is set, false - otherwise */"
						"\n\tA_Check_return"
						"\n\tbool %s_get_bit(unsigned bit_number) {"
						"\n\t\treturn bridge_get_bit(%s, bit_number) ? true : false;"
						"\n\t}", (*f)->name, (*f)->name, (*f)->name);
					FR4("\n\t/* clear bit in bits array %s at given index"
						"\n\t  returns reference to this structure */"
						"\n\tstruct %s &%s_clear_bit(unsigned bit_number) {"
						"\n\t\tbridge_set_bit(%s, bit_number, 0);"
						"\n\t\treturn *this;"
						"\n\t}", (*f)->name, s->s_name, (*f)->name, (*f)->name);
					FR4("\n\t/* change bit in bits array %s at given index"
						"\n\t  if set is true then set bit, if false - clear it"
						"\n\t  returns reference to this structure */"
						"\n\tstruct %s &%s_set_bit(unsigned bit_number, bool set = true) {"
						"\n\t\tbridge_set_bit(%s, bit_number, set ? 1 : 0);"
						"\n\t\treturn *this;"
						"\n\t}", (*f)->name, s->s_name, (*f)->name, (*f)->name);
				}
				break;
			case F_OPTIONAL:
				if ((*f)->user_type) {
					FR12("\n\t/* get value of optional pointer %s"
						"\n\t  returns NULL if field value is not set */"
						"\n\tA_Check_return"
						"\n\tstruct %s *get_%s() const {"
						"\n\t\treturn %s;"
						"\n\t}"
						"\n\t/* set new value of optional pointer %s, NULL to unset"
						"\n\t  returns set value"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: this structure takes control over set pointer - it will be deleted in ac_destroy() */"
						"\n\tA_Post_satisfies(return == %s_)"
						"\n\tstruct %s *set_%s(struct %s *%s_) {"
						"\n\t\treturn %s = %s_;"
						"\n\t}",
						(*f)->name, (*f)->type, (*f)->name, (*f)->name, (*f)->name, (*f)->name,
						(*f)->type, (*f)->name, (*f)->type, (*f)->name, (*f)->name, (*f)->name);
					FR3("\n\t/* allocate and set new value of optional pointer %s"
						"\n\t  returns NULL if failed to allocate"
						"\n\t  else returns newly allocated value"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: new value is properly cleaned up for ac_destroy(), optional fields are marked as unset */"
						"\n\tA_Check_return"
						"\n\tinline struct %s *new_%s(A_Inout struct bridge_allocator &ac);", (*f)->name, (*f)->type, (*f)->name);
				}
				else if (!is_simple_str_type((*f)->type)) {
					FR5("\n\t/* unset value of optional %s %s"
						"\n\t  returns reference to this structure */"
						"\n\tstruct %s &unset_%s() {"
						"\n\t\thas_%s = 0u;"
						"\n\t\treturn *this;"
						"\n\t}", field_type_bit == (*f)->type ? "bit" : "field", (*f)->name, s->s_name, (*f)->name, (*f)->name);
				}
				/* fall through */
			case F_REQUIRED:
				if ((*f)->user_type) {
					if (F_OPTIONAL != (*f)->f_power) {
						FR8("\n\t/* get constant reference to required field %s */"
							"\n\tA_Check_return"
							"\n\tconst struct %s &get_%s() const {"
							"\n\t\treturn %s;"
							"\n\t}"
							"\n\t/* get reference to required field %s */"
							"\n\tA_Check_return"
							"\n\tstruct %s &get_%s() {"
							"\n\t\treturn %s;"
							"\n\t}", (*f)->name, (*f)->type, (*f)->name, (*f)->name, (*f)->name, (*f)->type, (*f)->name, (*f)->name);
					}
				}
				else if (is_simple_str_type((*f)->type)) {
					if (F_OPTIONAL == (*f)->f_power) {
						FR9("\n\t/* get value of optional string %s"
							"\n\t  returns NULL if field value is not set */"
							"\n\tA_Check_return A_Ret_maybenull_z"
							"\n\tchar *get_%s() const {"
							"\n\t\treturn %s;"
							"\n\t}"
							"\n\t/* set new value of optional string %s, NULL to unset"
							"\n\t  returns set value"
							"\n\t  NOTE: old value is not checked and just overwritten"
							"\n\t  NOTE: this structure takes control over set string - it will be deleted in ac_destroy() */"
							"\n\tA_Ret_maybenull_z A_Post_satisfies(return == %s_)"
							"\n\tchar *set_%s(A_In_opt_z char *%s_) {"
							"\n\t\treturn %s = %s_;"
							"\n\t}",
							(*f)->name, (*f)->name, (*f)->name, (*f)->name,
							(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
					else {
						FR9("\n\t/* get value of required string %s"
							"\n\t  returns non-NULL value after unpack()"
							"\n\t  NOTE: may return NULL if value is not set yet */"
							"\n\tA_Check_return A_Ret_maybenull_z"
							"\n\tchar *get_%s() const {"
							"\n\t\treturn %s;"
							"\n\t}"
							"\n\t/* set new value of required string %s"
							"\n\t  returns set value"
							"\n\t  NOTE: old value is not checked and just overwritten"
							"\n\t  NOTE: new value may be NULL - it will be packed as empty string"
							"\n\t  NOTE: this structure takes control over set string - it will be deleted in ac_destroy() */"
							"\n\tA_Ret_maybenull_z A_Post_satisfies(return == %s_)"
							"\n\tchar *set_%s(A_In_opt_z char *%s_) {"
							"\n\t\treturn %s = %s_;"
							"\n\t}",
							(*f)->name, (*f)->name, (*f)->name, (*f)->name,
							(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
				}
				else {
					const char *what = (field_type_bit == (*f)->type) ? "bit" : "field";
					const char *native_type = (field_type_bit == (*f)->type) ? "bool" : get_native_type((*f)->type);
					const char *get_expr = (field_type_bit == (*f)->type) ? " ? true : false" : "";
					const char *set_expr = (field_type_bit == (*f)->type) ? " ? 1u : 0u" : "";
					const char *kind = (F_OPTIONAL == (*f)->f_power) ? "optional" : "required";
					FR3("\n\t/* get value of %s %s %s", kind, what, (*f)->name);
					if (F_OPTIONAL == (*f)->f_power)
						FR1("\n\t  NOTE: may return garbage if field value is not set - has_%s is zero", (*f)->name);
					FR4(" */"
						"\n\tA_Check_return"
						"\n\t%s get_%s() const {"
						"\n\t\treturn %s%s;"
						"\n\t}", native_type, (*f)->name, (*f)->name, get_expr);
					FR3("\n\t/* set new value of %s %s %s"
						"\n\t  returns reference to this structure", kind, what, (*f)->name);
					if (F_OPTIONAL == (*f)->f_power)
						FP("\n\t  NOTE: field is automatically marked as set");
					FR4(" */"
						"\n\tstruct %s &set_%s(%s %s_) {", s->s_name, (*f)->name, native_type, (*f)->name);
					if (F_OPTIONAL == (*f)->f_power)
						FR1("\n\t\thas_%s = 1u;", (*f)->name);
					FR3("\n\t\t%s = %s_%s;"
						"\n\t\treturn *this;"
						"\n\t}", (*f)->name, (*f)->name, set_expr);
				}
				break;
			case F_POINTER:
				if ((*f)->user_type || field_type_bit != (*f)->type) {
					const char *ss = (*f)->user_type ? "struct " : "";
					const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
					FR15("\n\t/* get value of required pointer %s"
						"\n\t  returns non-NULL value after unpack()"
						"\n\t  NOTE: may return NULL if value is not set yet */"
						"\n\tA_Check_return"
						"\n\t%s%s *get_%s() const {"
						"\n\t\treturn %s;"
						"\n\t}"
						"\n\t/* set new value of required pointer %s"
						"\n\t  returns set value"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: new value should be not NULL - packing will fail if it's NULL"
						"\n\t  NOTE: this structure takes control over set pointer - it will be deleted in ac_destroy() */"
						"\n\tA_Post_satisfies(return == %s_)"
						"\n\t%s%s *set_%s(%s%s *%s_) {"
						"\n\t\treturn %s = %s_;"
						"\n\t}",
						(*f)->name, ss, native, (*f)->name, (*f)->name, (*f)->name, (*f)->name,
						ss, native, (*f)->name, ss, native, (*f)->name, (*f)->name, (*f)->name);
					FR1("\n\t/* allocate and set new value of required pointer %s"
						"\n\t  returns NULL if failed to allocate"
						"\n\t  else returns newly allocated value"
						"\n\t  NOTE: old value is not checked and just overwritten", (*f)->name);
					if ((*f)->user_type || is_simple_str_type((*f)->type))
						FP("\n\t  NOTE: new value is properly cleaned up for ac_destroy()");
					if ((*f)->user_type)
						FP(", optional fields are marked as unset");
					FR3(" */"
						"\n\tA_Check_return"
						"\n\tinline %s%s *new_%s(A_Inout struct bridge_allocator &ac);", ss, native, (*f)->name);
					if ((*f)->user_type) {
						FR4("\n\t/* get reference to pointed value of required pointer %s */"
							"\n\tA_Check_return"
							"\n\tstruct %s &%s_get() const {"
							"\n\t\treturn *%s;"
							"\n\t}", (*f)->name, (*f)->type, (*f)->name, (*f)->name);
					}
					else if (is_simple_str_type((*f)->type)) {
						FR7("\n\t/* get pointed value of required string pointer %s"
							"\n\t  returns non-NULL value after unpack()"
							"\n\t  NOTE: may return NULL if value is not set yet */"
							"\n\tA_Check_return A_Ret_maybenull_z"
							"\n\tchar *%s_get() const {"
							"\n\t\treturn *%s;"
							"\n\t}"
							"\n\t/* set pointed value of required string pointer %s"
							"\n\t  returns set value"
							"\n\t  NOTE: old value is not checked and just overwritten"
							"\n\t  NOTE: new value may be NULL - it will be packed as empty string"
							"\n\t  NOTE: pointer %s takes control over set string - it will be deleted in ac_destroy() */"
							"\n\tA_Ret_maybenull_z A_Post_satisfies(return == v_)"
							"\n\tchar *%s_set(A_In_opt_z char *v_) {"
							"\n\t\treturn *%s = v_;"
							"\n\t}", (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
					else {
						FR9("\n\t/* get pointed value of required pointer %s */"
							"\n\tA_Check_return"
							"\n\t%s %s_get() const {"
							"\n\t\treturn *%s;"
							"\n\t}"
							"\n\t/* set pointed value of required pointer %s"
							"\n\t  returns reference to this structure */"
							"\n\tstruct %s &%s_set(%s v_) {"
							"\n\t\t*%s = v_;"
							"\n\t\treturn *this;"
							"\n\t}", (*f)->name, native, (*f)->name, (*f)->name, (*f)->name, s->s_name, (*f)->name, native, (*f)->name);
					}
					break;
				}
				/* fall through for field_type_bit */
			default:
				if ((*f)->user_type || field_type_bit != (*f)->type) {
					const char *ss = (*f)->user_type ? "struct " : "";
					const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
					FR6("\n\t/* get value of fixed-sized array %s"
						"\n\t  returns non-NULL value after unpack()"
						"\n\t  NOTE: may return NULL if value is not set yet */"
						"\n\tA_Check_return"
						"\n\t%s%s (*get_%s() const)[%uu] {"
						"\n\t\treturn %s;"
						"\n\t}", (*f)->name, ss, native, (*f)->name, (*f)->f_power, (*f)->name);
					FR12("\n\t/* set new non-NULL value of fixed-sized array %s"
						"\n\t  returns set value"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: new value should be not NULL - packing will fail if it's NULL"
						"\n\t  NOTE: this structure takes control over set array - it will be deleted in ac_destroy() */"
						"\n\tA_Post_satisfies(return == %s_)"
						"\n\t%s%s (*set_%s(%s%s (*%s_)[%uu]))[%uu] {"
						"\n\t\treturn %s = %s_;"
						"\n\t}",
						(*f)->name, (*f)->name, ss, native, (*f)->name, ss, native,
						(*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name, (*f)->name);
					FR1("\n\t/* allocate and set new value of fixed-sized array %s"
						"\n\t  returns NULL if failed to allocate"
						"\n\t  else returns newly allocated array"
						"\n\t  NOTE: old value is not checked and just overwritten", (*f)->name);
					if ((*f)->user_type || is_simple_str_type((*f)->type))
						FP("\n\t  NOTE: new array items are properly cleaned up for ac_destroy()");
					if ((*f)->user_type)
						FP(", optional fields are marked as unset");
					FR4(" */"
						"\n\tA_Check_return"
						"\n\tinline %s%s (*new_%s(A_Inout struct bridge_allocator &ac))[%uu];", ss, native, (*f)->name, (*f)->f_power);
					if ((*f)->user_type) {
						FR6("\n\t/* get constant reference to element of fixed-sized array %s at given index */"
							"\n\tA_Check_return"
							"\n\tconst struct %s &%s_get(A_In_range(<,%uu) size_t idx_/*<%uu*/) const {"
							"\n\t\treturn (*%s)[idx_];"
							"\n\t}", (*f)->name, (*f)->type, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
						FR6("\n\t/* get reference to element of fixed-sized array %s at given index */"
							"\n\tA_Check_return"
							"\n\tstruct %s &%s_get(A_In_range(<,%uu) size_t idx_/*<%uu*/) {"
							"\n\t\treturn (*%s)[idx_];"
							"\n\t}", (*f)->name, (*f)->type, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
					}
					else if (is_simple_str_type((*f)->type)) {
						FR5("\n\t/* get element of fixed-sized strings array %s at given index"
							"\n\t  returns non-NULL string after unpack()"
							"\n\t  may return NULL if string at given index is not set yet */"
							"\n\tA_Check_return A_Ret_maybenull_z"
							"\n\tchar *%s_get(A_In_range(<,%uu) size_t idx_/*<%uu*/) const {"
							"\n\t\treturn (*%s)[idx_];"
							"\n\t}", (*f)->name, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
						FR6("\n\t/* set element of fixed-sized strings array %s at given index"
							"\n\t  returns set string"
							"\n\t  NOTE: new value may be NULL - it will be packed as empty string"
							"\n\t  NOTE: old string at given index is not checked and just overwritten"
							"\n\t  NOTE: fixed-sized array %s takes control over set string - it will be deleted in ac_destroy() */"
							"\n\tA_Ret_maybenull_z A_Post_satisfies(return == v_)"
							"\n\tchar *%s_set(A_In_range(<,%uu) size_t idx_/*<%uu*/, A_In_opt_z char *v_) {"
							"\n\t\treturn (*%s)[idx_] = v_;"
							"\n\t}", (*f)->name, (*f)->name, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
					}
					else {
						FR6("\n\t/* get element of fixed-sized array %s at given index */"
							"\n\tA_Check_return"
							"\n\t%s %s_get(A_In_range(<,%uu) size_t idx_/*<%uu*/) const {"
							"\n\t\treturn (*%s)[idx_];"
							"\n\t}", (*f)->name, native, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
						FR7("\n\t/* set element of fixed-sized array %s at given index"
							"\n\t  returns reference to this structure */"
							"\n\tstruct %s &%s_set(A_In_range(<,%uu) size_t idx_/*<%uu*/, %s v_) {"
							"\n\t\t(*%s)[idx_] = v_;"
							"\n\t\treturn *this;"
							"\n\t}", (*f)->name, s->s_name, (*f)->name, (*f)->f_power, (*f)->f_power, native, (*f)->name);
					}
				}
				else {
					unsigned bb = bytes_for_bits((*f)->f_power);
					FR12("\n\t/* get value of fixed-sized bits array %s"
						"\n\t  returns non-NULL value after unpack()"
						"\n\t  NOTE: may return NULL if value is not set yet */"
						"\n\tA_Check_return"
						"\n\tunsigned char (*get_%s() const)[%uu] {"
						"\n\t\treturn %s;"
						"\n\t}"
						"\n\t/* set new non-NULL value of fixed-sized bits array %s"
						"\n\t  returns set value"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: new value should be not NULL - packing will fail if it's NULL"
						"\n\t  NOTE: this structure takes control over set bits array - it will be deleted in ac_destroy() */"
						"\n\tA_Post_satisfies(return == %s_)"
						"\n\tunsigned char (*set_%s(unsigned char (*%s_)[%uu]))[%uu] {"
						"\n\t\treturn %s = %s_;"
						"\n\t}",
						(*f)->name, (*f)->name, bb, (*f)->name,
						(*f)->name, (*f)->name, (*f)->name, (*f)->name, bb, bb, (*f)->name, (*f)->name);
					FR3("\n\t/* allocate and set new value of fixed-sized bits array %s"
						"\n\t  returns NULL if failed to allocate"
						"\n\t  else returns newly allocated bits array"
						"\n\t  NOTE: old value is not checked and just overwritten"
						"\n\t  NOTE: all bits in new array are zero */"
						"\n\tA_Check_return"
						"\n\tinline unsigned char (*new_%s(A_Inout struct bridge_allocator &ac))[%uu];", (*f)->name, (*f)->name, bb);
					FR5("\n\t/* check bit in fixed-sized bits array %s at given index"
						"\n\t  returns true if bit is set, false - otherwise */"
						"\n\tA_Check_return"
						"\n\tbool %s_get_bit(A_In_range(<,%uu) unsigned bit_number/*<%uu*/) {"
						"\n\t\treturn bridge_get_bit(*%s, bit_number) ? true : false;"
						"\n\t}", (*f)->name, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
					FR6("\n\t/* clear bit in fixed-sized bits array %s at given index"
						"\n\t  returns reference to this structure */"
						"\n\tstruct %s &%s_clear_bit(A_In_range(<,%uu) unsigned bit_number/*<%u*/) {"
						"\n\t\tbridge_set_bit(*%s, bit_number, 0);"
						"\n\t\treturn *this;"
						"\n\t}", (*f)->name, s->s_name, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
					FR6("\n\t/* change bit in fixed-sized bits array %s at given index"
						"\n\t  if set is true then set bit, if false - clear it"
						"\n\t  returns reference to this structure */"
						"\n\tstruct %s &%s_set_bit(A_In_range(<,%uu) unsigned bit_number/*<%u*/, bool set = true) {"
						"\n\t\tbridge_set_bit(*%s, bit_number, set ? 1 : 0);"
						"\n\t\treturn *this;"
						"\n\t}", (*f)->name, s->s_name, (*f)->name, (*f)->f_power, (*f)->f_power, (*f)->name);
				}
				break;
		}
	}
	FP("\n#endif"
		"\n};");
}

static void print_cplusplus_impl(FILE *file, const struct struct_def *s)
{
	{
		const char *conv = s->convertable ? "conv_" : "";
		FR7("\nA_Check_return"
			"\ninline struct %s *%s::ac_new(A_Inout struct bridge_allocator &ac) {"
			"\n\treturn %s_ac_new(&ac);"
			"\n}"
			"\ninline void %s::ac_destroy(A_Inout struct bridge_allocator &ac) {"
			"\n\t%s_ac_destroy(this, &ac);"
			"\n}"
			"\ninline void %s::ac_delete(A_Inout struct bridge_allocator &ac) {"
			"\n\t%s_ac_delete(this, &ac);"
			"\n}", s->s_name, s->s_name, s->s_name, s->s_name, s->s_name, s->s_name, s->s_name);
		FR11("\nA_Check_return"
			"\ninline %s *%s::unpack_new(A_Inout union bridge_unpack_%sinfo &in) {"
			"\n\treturn %s_unpack_to(NULL, &in);"
			"\n}"
			"\nA_Check_return"
			"\ninline %s *%s::unpack(A_Inout union bridge_unpack_%sinfo &in) {"
			"\n\treturn %s_unpack_to(this, &in);"
			"\n}"
			"\nA_Check_return"
			"\ninline void *%s::pack(A_Inout union bridge_pack_%sinfo &in) const {"
			"\n\treturn %s_pack(this, &in);"
			"\n}", s->s_name, s->s_name, conv, s->s_name, s->s_name, s->s_name, conv, s->s_name, s->s_name, conv, s->s_name);
	}
	{
		struct field_def **f = s->fields;
		for (; f < s->fields_end; f++) {
			switch ((*f)->f_power) {
				case F_REQUIRED:
					break;
				case F_ARRAY:
					if ((*f)->user_type) {
						FR4("\nA_Check_return"
							"\ninline size_t %s::get_%s_count() const {"
							"\n\treturn (size_t)(%s_end - %s);"
							"\n}", s->s_name, (*f)->name, (*f)->name, (*f)->name);
						FR13("\nA_Post_satisfies(return == %s_)"
							"\ninline struct %s *%s::set_%s("
							"\n\tA_When(A_Curr != NULL, A_Readable_elements(%s_size_)) struct %s *%s_,"
							"\n\tsize_t %s_size_)"
							"\n{"
							"\n\t%s_end = (%s = %s_) + %s_size_;"
							"\n\treturn %s_;"
							"\n}",
							(*f)->name, (*f)->type, s->s_name, (*f)->name, (*f)->name, (*f)->type, (*f)->name,
							(*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name, (*f)->name);
					}
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						const char *ss = (*f)->user_type ? "struct " : "";
						const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
						FR8("\nA_Check_return"
							"\ninline %s%s *%s::new_%s("
							"\n\tA_In_range(>,0) size_t %s_count_,"
							"\n\tA_Inout struct bridge_allocator &ac)"
							"\n{"
							"\n\treturn %s_ac_new_%s(this, %s_count_, &ac);"
							"\n}", ss, native, s->s_name, (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->name);
						if ((*f)->user_type) {
							FR8("\nA_Check_return"
								"\ninline const struct %s &%s::%s_get(size_t idx_) const {"
								"\n\treturn %s[idx_];"
								"\n}"
								"\nA_Check_return"
								"\ninline struct %s &%s::%s_get(size_t idx_) {"
								"\n\treturn %s[idx_];"
								"\n}", (*f)->type, s->s_name, (*f)->name, (*f)->name, (*f)->type, s->s_name, (*f)->name, (*f)->name);
						}
					}
					else {
						FR6("\nA_Check_return"
							"\ninline unsigned char *%s::new_%s("
							"\n\tA_In_range(>,0) unsigned %s_bit_count_,"
							"\n\tA_Inout struct bridge_allocator &ac)"
							"\n{"
							"\n\treturn %s_ac_new_%s(this, %s_bit_count_, &ac);"
							"\n}", s->s_name, (*f)->name, (*f)->name, s->s_name, (*f)->name, (*f)->name);
					}
					break;
				case F_OPTIONAL:
					if (!(*f)->user_type)
						break;
					/* fall through */
				case F_POINTER:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						const char *ss = (*f)->user_type ? "struct " : "";
						const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
						FR6("\nA_Check_return"
							"\ninline %s%s *%s::new_%s(A_Inout struct bridge_allocator &ac) {"
							"\n\treturn %s_ac_new_%s(this, &ac);"
							"\n}", ss, native, s->s_name, (*f)->name, s->s_name, (*f)->name);
						break;
					}
					/* fall through */
				default:
					if ((*f)->user_type || field_type_bit != (*f)->type) {
						const char *ss = (*f)->user_type ? "struct " : "";
						const char *native = (*f)->user_type ? (*f)->type : get_native_type((*f)->type);
						FR7("\nA_Check_return"
							"\ninline %s%s (*%s::new_%s(A_Inout struct bridge_allocator &ac))[%uu] {"
							"\n\treturn %s_ac_new_%s(this, &ac);"
							"\n}", ss, native, s->s_name, (*f)->name, (*f)->f_power, s->s_name, (*f)->name);
					}
					else {
						unsigned bb = bytes_for_bits((*f)->f_power);
						FR5("\nA_Check_return"
							"\ninline unsigned char (*%s::new_%s(A_Inout struct bridge_allocator &ac))[%uu] {"
							"\n\treturn %s_ac_new_%s(this, &ac);"
							"\n}", s->s_name, (*f)->name, bb, s->s_name, (*f)->name);
					}
					break;
			}
		}
	}
}

FILE *generate_h(FILE *file, const char *hname, const char *decl/*NULL?*/)
{
	print_header_header_file_define(file, hname);
	FR1("\n/* Generated by %s. Do not edit! */"
		"\n"
		"\n#include \"bitbridge/bitbridge_comn.h\""
		"\n"
		"\n#ifdef __cplusplus"
		"\nextern \"C\" {"
		"\n#endif", compiler_logo);
	{
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			print_structure(file, *s);
			generate_prototypes(file, *s, decl, 1);
		}
	}
	{
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			FR1("\n"
				"\n/* %s */", (*s)->s_name);
			generate_prototypes(file, *s, decl, 2);
			generate_prototypes(file, *s, decl, 4);
		}
	}
	FP("\n"
		"\n#ifdef __cplusplus");
	{
		struct struct_def **s = structs;
		for (; s < structs_end; s++)
			print_cplusplus_impl(file, *s);
	}
	FP("\n} /* extern \"C\" */"
		"\n#endif"
		"\n"
		"\n#endif"
		"\n");
	return file;
}

static void print_source_includes(FILE *file, const char *cname, const char *hname, unsigned bridge_need_runtime)
{
	FR4("/* %s */"
		"\n"
		"\n/* generated by %s */"
		"\n"
		"\n#include \"%s\""
		"\n#define BRIDGE_NEED_RUNTIME_LIB %u"
		"\n#include \"bitbridge/bitbridge.h\""
		"\n", cname, compiler_logo, hname, bridge_need_runtime);
}

/* NOTE: for a structure with non-zero ptr_arr flag */
static void print_ptr_path_s(FILE *file, const struct struct_def *s)
{
	for (;;) {
		const struct field_def *f = *s->fields;
		fputs(f->name, file);
		if (!f->user_type || F_REQUIRED != f->f_power)
			return;
		fputc('.', file);
		s = f->user_type;
	}
}

/* NOTE: for a f->user_type with non-zero ptr_arr flag */
#define print_ptr_path(file, f) do { \
	const struct field_def *__f = f; \
	print_ptr_path_s(file, __f->user_type); \
} while ((void)0,0)

/* NOTE: destroyed structure may be partially initialized, pointer- and fixed-sized array fields may be NULL */
static void generate_code_del_struct(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/)
{
	int long_loop = 0;
	if (s->tail_recursive && s->tail_recursive->user_type != s) {
		long_loop = 1;
		FR1("\nenum DEL_%s {", s->s_name);
		{
			const struct struct_def *r = s;
			do {
				const struct struct_def *n = r->tail_recursive->user_type;
				FR2("\n\tD_%s%s", r->s_name, n != s ? "," : "");
				r = n;
			} while (r != s);
		}
		FR2("\n};"
			"\n"
			"\nA_Nonnull_arg(1) A_Nonnull_arg(4)"
			"\nstatic void _%s_del(A_Pre_valid A_Post_invalid void *p,"
			"\n\tA_Pre_opt_valid A_Post_ptr_invalid void *p1, enum DEL_%s d, BRIDGE_ALLOCATOR_ARG(ac))"
			"\n{", s->s_name, s->s_name);
	}
	else if (s->tail_recursive) {
		FR3("\nA_Nonnull_arg(1) A_Nonnull_arg(3)"
			"\nstatic void _%s_del(A_Pre_valid A_Post_invalid struct %s *s,"
			"\n\tA_Pre_opt_valid A_Post_ptr_invalid struct %s *s1, BRIDGE_ALLOCATOR_ARG(ac))"
			"\n{", s->s_name, s->s_name, s->s_name);
	}
	else {
		FR4("\n%s%sA_Nonnull_all_args"
			"\nvoid _%s_ac_destroy(A_Pre_valid A_Post_invalid struct %s *s, BRIDGE_ALLOCATOR_ARG(ac))"
			"\n{", decl ? decl : "", decl ? " " : "", s->s_name, s->s_name);
	}
	if (s->equivalent && s->equiv_many) {
		FP("\n\t_bridge_destroy_str_array(&s->");
		print_equivalent_path_s(file, s);
		FP(", &(s + 1)->");
		print_equivalent_path_s(file, s);
		FP(", BRIDGE_PASS_ALLOCATOR(ac));");
	}
	else if (s->ptr_arr && s->equiv_many) {
		FP("\n\t_bridge_destroy_ptr_array_(&s->");
		print_ptr_path_s(file, s);
		FP(", &(s + 1)->");
		print_ptr_path_s(file, s);
		FP(", BRIDGE_PASS_ALLOCATOR(ac));");
	}
	else {
		const char *offs = long_loop ? "\t\t" : s->tail_recursive ? "\t" : "";
		const struct struct_def *r = s;
		if (s->tail_recursive) {
			if (long_loop) {
				FP("\n\tswitch (d) {"
					"\ndefault:"
					"\n\tfor (;;) {");
			}
			else
				FP("\n\tdo {");
		}
		do {
			int mark_valid = 0;
			struct field_def **f = r->fields;
			if (long_loop) {
				FR3("\ncase D_%s: {"
					"\n\t\t\tstruct %s *s = (struct %s*)p;", r->s_name, r->s_name, r->s_name);
			}
			for (; f < r->fields_end; f++) {
				if (r->tail_recursive == *f)
					continue; /* tail-recursive field will be processed last */
				switch ((*f)->f_power) {
					case F_ARRAY:
						if (f == r->fields)
							mark_valid = 1; /* to avoid false analyzer warnings */
						else if (mark_valid) {
							mark_valid = 0;
							FR1("\n%s\tA_Mark_ptr_valid(s);", offs);
						}
						FR3("\n%s\t/* %s: %s[] */", offs, (*f)->name, (*f)->type);
						if ((*f)->user_type) {
							if (S_DYNAMIC == (*f)->user_type->s_layout) {
								if ((*f)->user_type->equivalent) {
									FR2("\n%s\t_bridge_delete_str_array(&s->%s->", offs, (*f)->name);
									print_equivalent_path(file, *f);
									FR1(", &s->%s_end->", (*f)->name);
									print_equivalent_path(file, *f);
									FP(", BRIDGE_PASS_ALLOCATOR(ac));");
									break;
								}
								if ((*f)->user_type->ptr_arr) {
									FR2("\n%s\t_bridge_delete_ptr_array_(&s->%s->", offs, (*f)->name);
									print_ptr_path(file, *f);
									FR1(", &s->%s_end->", (*f)->name);
									print_ptr_path(file, *f);
									FP(", BRIDGE_PASS_ALLOCATOR(ac));");
									break;
								}
								FR9("\n%s\t{"
									"\n%s\t\tstruct %s *a = s->%s, *const e = s->%s_end;"
									"\n%s\t\tfor (; a < e; a++)"
									"\n%s\t\t\t_%s_ac_destroy(a, BRIDGE_PASS_ALLOCATOR(ac));"
									"\n%s\t}", offs, offs, (*f)->type, (*f)->name, (*f)->name, offs, offs, (*f)->type, offs);
							}
						}
						else if (is_simple_str_type((*f)->type)) {
							FR3("\n%s\t_bridge_delete_str_array(s->%s, s->%s_end, BRIDGE_PASS_ALLOCATOR(ac));",
								offs, (*f)->name, (*f)->name);
							break;
						}
						FR2("\n%s\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), s->%s);", offs, (*f)->name);
						break;
					case F_POINTER:
						if ((*f)->user_type || field_type_bit != (*f)->type) {
							if (f == r->fields)
								mark_valid = 1; /* to avoid false analyzer warnings */
							else if (mark_valid) {
								mark_valid = 0;
								FR1("\n%s\tA_Mark_ptr_valid(s);", offs);
							}
							FR3("\n%s\t/* %s: %s[1] */", offs, (*f)->name, (*f)->type);
							if ((*f)->user_type) {
_delete_user_type_pointer:
								if (S_DYNAMIC == (*f)->user_type->s_layout) {
									if ((*f)->user_type->equivalent) {
										if ((*f)->user_type->equiv_many) {
											FR4("\n%s\tif (s->%s)"
												"\n%s\t\t_bridge_delete_str_array(&s->%s->", offs, (*f)->name, offs, (*f)->name);
											print_equivalent_path(file, *f);
											FR1(", &(s->%s + 1)->", (*f)->name);
											print_equivalent_path(file, *f);
											FP(", BRIDGE_PASS_ALLOCATOR(ac));");
										}
										else {
											FR2("\n%s\t_bridge_delete_str_cont(&s->%s->", offs, (*f)->name);
											print_equivalent_path(file, *f);
											FP(", BRIDGE_PASS_ALLOCATOR(ac));");
										}
										break;
									}
									if ((*f)->user_type->ptr_arr) {
										if ((*f)->user_type->equiv_many) {
											FR4("\n%s\tif (s->%s)"
												"\n%s\t\t_bridge_delete_ptr_array_(&s->%s->",
												offs, (*f)->name, offs, (*f)->name);
											print_ptr_path(file, *f);
											FR1(", &(s->%s + 1)->", (*f)->name);
											print_ptr_path(file, *f);
											FP(", BRIDGE_PASS_ALLOCATOR(ac));");
										}
										else {
											FR2("\n%s\t_bridge_delete_ptr_cont_(&s->%s->", offs, (*f)->name);
											print_ptr_path(file, *f);
											FP(", BRIDGE_PASS_ALLOCATOR(ac));");
										}
										break;
									}
									FR3("\n%s\t_%s_ac_delete(s->%s, BRIDGE_PASS_ALLOCATOR(ac));", offs, (*f)->type, (*f)->name);
									break;
								}
							}
							else if (is_simple_str_type((*f)->type)) {
								FR2("\n%s\t_bridge_delete_str_cont(s->%s, BRIDGE_PASS_ALLOCATOR(ac));", offs, (*f)->name);
								break;
							}
							FR2("\n%s\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), s->%s);", offs, (*f)->name);
							break;
						}
						/* fall through */
					default:
						if (f == r->fields)
							mark_valid = 1; /* to avoid false analyzer warnings */
						else if (mark_valid) {
							mark_valid = 0;
							FR1("\n%s\tA_Mark_ptr_valid(s);", offs);
						}
						FR4("\n%s\t/* %s: %s[%uu] */", offs, (*f)->name, (*f)->type, (*f)->f_power);
						if ((*f)->user_type) {
							if (S_DYNAMIC == (*f)->user_type->s_layout) {
								FR2("\n%s\tif (s->%s)", offs, (*f)->name);
								if ((*f)->user_type->equivalent) {
									FR2("\n%s\t\t_bridge_delete_str_array(&(*s->%s)[0].", offs, (*f)->name);
									print_equivalent_path(file, *f);
									FR1(", &(*(s->%s + 1))[0].", (*f)->name);
									print_equivalent_path(file, *f);
									FP(", BRIDGE_PASS_ALLOCATOR(ac));");
									break;
								}
								if ((*f)->user_type->ptr_arr) {
									FR2("\n%s\t\t_bridge_delete_ptr_array_(&(*s->%s)[0].", offs, (*f)->name);
									print_ptr_path(file, *f);
									FR1(", &(*(s->%s + 1))[0].", (*f)->name);
									print_ptr_path(file, *f);
									FP(", BRIDGE_PASS_ALLOCATOR(ac));");
									break;
								}
								FR8(" {"
									"\n%s\t\tsize_t n = 0;"
									"\n%s\t\tfor (; n < sizeof(*s->%s)/sizeof((*s->%s)[0]); n++)"
									"\n%s\t\t\t_%s_ac_destroy(&(*s->%s)[n], BRIDGE_PASS_ALLOCATOR(ac));"
									"\n%s\t}", offs, offs, (*f)->name, (*f)->name, offs, (*f)->type, (*f)->name, offs);
							}
						}
						else if (is_simple_str_type((*f)->type)) {
							FR5("\n%s\tif (s->%s)"
								"\n%s\t\t_bridge_delete_str_array(*s->%s, *(s->%s + 1), BRIDGE_PASS_ALLOCATOR(ac));",
								offs, (*f)->name, offs, (*f)->name, (*f)->name);
							break;
						}
						FR2("\n%s\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), s->%s);", offs, (*f)->name);
						break;
					case F_OPTIONAL:
						if ((*f)->user_type || is_simple_str_type((*f)->type)) {
							if (f == r->fields)
								mark_valid = 1; /* to avoid false analyzer warnings */
							else if (mark_valid) {
								mark_valid = 0;
								FR1("\n%s\tA_Mark_ptr_valid(s);", offs);
							}
							FR3("\n%s\t/* %s: %s* */", offs, (*f)->name, (*f)->type);
						}
						if ((*f)->user_type)
							goto _delete_user_type_pointer;
						if (is_simple_str_type((*f)->type))
							FR2("\n%s\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), s->%s);", offs, (*f)->name);
						break;
					case F_REQUIRED:
						if ((*f)->user_type ? S_DYNAMIC == (*f)->user_type->s_layout : is_simple_str_type((*f)->type)) {
							if (f == r->fields)
								mark_valid = 1; /* to avoid false analyzer warnings */
							else if (mark_valid) {
								mark_valid = 0;
								FR1("\n%s\tA_Mark_ptr_valid(s);", offs);
							}
							FR3("\n%s\t/* %s: %s */", offs, (*f)->name, (*f)->type);
						}
						if ((*f)->user_type) {
							if (S_DYNAMIC == (*f)->user_type->s_layout)
								FR3("\n%s\t_%s_ac_destroy(&s->%s, BRIDGE_PASS_ALLOCATOR(ac));", offs, (*f)->type, (*f)->name);
						}
						else if (is_simple_str_type((*f)->type))
							FR2("\n%s\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), s->%s);", offs, (*f)->name);
						break;
				}
			}
			if (r->tail_recursive) {
				if (mark_valid)
					FR1("\n%s\tA_Mark_ptr_valid(s);", offs);
				FR4("\n%s\t/* %s: %s%s (tail recursive) */",
					offs, r->tail_recursive->name, r->tail_recursive->type,
					F_POINTER == r->tail_recursive->f_power ? "[1]" :
					F_OPTIONAL == r->tail_recursive->f_power ? "*" : "");
				if (long_loop) {
					FR2("\n\t\t\tp = %ss->%s;"
						"\n\t\t}", F_REQUIRED == r->tail_recursive->f_power ? "&" : "", r->tail_recursive->name);
					if (F_REQUIRED != r->tail_recursive->f_power) {
						FP("\n\t\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), p1);"
							"\n\t\tif (!p)"
							"\n\t\t\tbreak;"
							"\n\t\tp1 = p;");
					}
				}
				else {
					FR1("\n\t\ts = s->%s;"
						"\n\t\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), s1);"
						"\n\t\ts1 = s;"
						"\n\t} while (s);", r->tail_recursive->name);
				}
				r = r->tail_recursive->user_type;
			}
		} while (r != s);
		if (long_loop)
			FP("\n\t}}");
	}
	FP("\n\tbridge_allocator_dec_level(ac);"
		"\n}"
		"\n");
	if (!s->tail_recursive) {
		FR5("\n%s%sA_Nonnull_arg(2)"
			"\nvoid _%s_ac_delete(A_Pre_opt_valid A_Post_ptr_invalid struct %s *s, BRIDGE_ALLOCATOR_ARG(ac))"
			"\n{"
			"\n\tif (s) {"
			"\n\t\t_%s_ac_destroy(s, BRIDGE_PASS_ALLOCATOR(ac));"
			"\n\t\tac->free_cb(BRIDGE_PASS_ALLOCATOR(ac), s);"
			"\n\t}"
			"\n\tbridge_allocator_dec_level(ac);"
			"\n}"
			"\n", decl ? decl : "", decl ? " " : "", s->s_name, s->s_name, s->s_name);
	}
}

static void generate_code_tail_destructors(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/)
{
	const char *entry_name = s->s_name;
	int long_loop = 0;
	if (s->tail_recursive->user_type != s) {
		long_loop = 1;
		{
			const struct struct_def *r = s;
			while (!r->recursion_entry)
				r = r->tail_recursive->user_type;
			entry_name = r->s_name;
		}
	}
	FR7("\n%s%sA_Nonnull_all_args"
		"\nvoid _%s_ac_destroy(A_Pre_valid A_Post_invalid struct %s *s, BRIDGE_ALLOCATOR_ARG(ac))"
		"\n{"
		"\n\t_%s_del(s, NULL%s%s, BRIDGE_PASS_ALLOCATOR(ac));"
		"\n\tbridge_allocator_dec_level(ac);"
		"\n}"
		"\n", decl ? decl : "", decl ? " " : "", s->s_name, s->s_name, entry_name, long_loop ? ", D_" : "", long_loop ? s->s_name : "");
	FR7("\n%s%sA_Nonnull_arg(2)"
		"\nvoid _%s_ac_delete(A_Pre_opt_valid A_Post_ptr_invalid struct %s *s, BRIDGE_ALLOCATOR_ARG(ac))"
		"\n{"
		"\n\tif (s)"
		"\n\t\t_%s_del(s, s%s%s, BRIDGE_PASS_ALLOCATOR(ac));"
		"\n\tbridge_allocator_dec_level(ac);"
		"\n}"
		"\n", decl ? decl : "", decl ? " " : "", s->s_name, s->s_name, entry_name, long_loop ? ", D_" : "", long_loop ? s->s_name : "");
}

FILE *generate_c(FILE *file, const char *cname, const char *hname, unsigned bridge_need_runtime, const char *decl/*NULL?*/)
{
	{
		size_t ch = strlen(hname), cs = strlen(cname);
		for (; ch && hname[ch - 1] != '\\' && hname[ch - 1] != '/'; ch--);
		for (; cs && cname[cs - 1] != '\\' && cname[cs - 1] != '/'; cs--);
		print_source_includes(file, cname + cs, hname + ch, bridge_need_runtime);
	}
	check_required_pointers();
	{
		int need_newline = 0;
		struct struct_def **s = structs;
		for (; s < structs_end; s++) {
			if (need_newline) {
				fputc('\n', file);
				need_newline = 0;
			}
			if (generate_prototype_variable_size(file, *s)) {
				FP(";");
				need_newline = 1;
			}
			if (generate_prototype_append_packed(file, *s)) {
				FP(";");
				need_newline = 1;
			}
			if (generate_prototype_init_unpacked(file, *s)) {
				FP(";");
				need_newline = 1;
			}
		}
		if (need_newline)
			fputc('\n', file);
	}
	mark_strutures_for_unpack();
	{
		int gotoz = 0;
		struct struct_def **s = structs;
		int process_tail = 0;
		for (; s < structs_end; s++) {
			if (!(*s)->tail_recursive || (*s)->recursion_entry) {
				if ((*s)->need_destroy)
					generate_code_del_struct(file, *s, decl);
				generate_code_pack(file, *s, decl);
				gotoz = generate_code_unpack(file, *s, decl, gotoz);
			}
			if ((*s)->tail_recursive)
				process_tail = 1;
		}
		if (process_tail) {
			for (s = structs; s < structs_end; s++) {
				if ((*s)->tail_recursive) {
					generate_code_tail_destructors(file, *s, decl);
					if (!(*s)->recursion_entry) {
						generate_code_pack(file, *s, decl);
						gotoz = generate_code_unpack(file, *s, decl, gotoz);
					}
				}
			}
		}
	}
	return file;
}
