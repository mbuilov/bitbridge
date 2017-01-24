#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* compiler.h */

struct struct_def;
void generate_prototype_pack(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/);
void generate_prototype_unpack(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/);
int generate_prototype_variable_size(FILE *file, const struct struct_def *s);
int generate_prototype_append_packed(FILE *file, const struct struct_def *s);
void check_required_pointers(void);
void generate_code_pack(FILE *file, const struct struct_def *s, const char *decl/*NULL?*/);
int generate_prototype_init_unpacked(FILE *file, const struct struct_def *s);
void mark_strutures_for_unpack(void);
int generate_code_unpack(FILE *file, struct struct_def *s, const char *decl/*NULL?*/, int gotoz);
void print_equivalent_path_s(FILE *file, const struct struct_def *s);
FILE *generate_h(FILE *file, const char *fname, const char *decl/*NULL?*/);
FILE *generate_c(FILE *file, const char *fname, const char *header, const char *decl/*NULL?*/);

#ifdef __GNUC__
__attribute__ ((const))
#endif
const char *get_native_type(const char *eq);

#define print_equivalent_path(file, f) do { \
	const struct field_def *__f = f; \
	print_equivalent_path_s(file, __f->user_type); \
} while ((void)0,0)

extern char compiler_logo[];

#endif /* COMPILER_H_INCLUDED */
