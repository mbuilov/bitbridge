/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* compiler.c */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "compiler.h"
#include "model.h"
#include "get_opts.inl"

/* compiler version: major.minor.patch */
#define BIT_BRIDGE_C_COMPILER_MAJOR 1
#define BIT_BRIDGE_C_COMPILER_MINOR 0
#define BIT_BRIDGE_C_COMPILER_PATCH 0

#define BITBRIDGE_C_COMPILER_VERSION "BitBridge C-compiler version "

/* version: [model_ver.compiler_major.compiler_minor] */
char compiler_logo[] = BITBRIDGE_C_COMPILER_VERSION "major.minor.patch";

static void make_logo(void)
{
	(void)sprintf(compiler_logo + sizeof(BITBRIDGE_C_COMPILER_VERSION) - 1, "%u.%u.%u",
		(BIT_BRIDGE_MODEL_MAJOR + BIT_BRIDGE_C_COMPILER_MAJOR) & 65535u,
		(BIT_BRIDGE_MODEL_MINOR + BIT_BRIDGE_C_COMPILER_MINOR) & 65535u,
		(BIT_BRIDGE_MODEL_PATCH + BIT_BRIDGE_C_COMPILER_PATCH) & 65535u);
}

static int usage(const char *program)
{
	size_t e = strlen(program);
	for (; e && program[e - 1] != '\\' && program[e - 1] != '/'; e--);
	program += e;
	fprintf(stderr,
		"%s"
		"\n Generates structures declarations and code of init/pack/unpack/delete functions in C."
		"\nusage:"
		"\n %s -s<input.sd> [-c<btypes.c>] [-h<btypes.h>] [-m<bmodel.md>] [-d<decl>] [-n]"
		"\nwhere"
		"\n -s<input.sd>  - structures definitions file name"
		"\n -c<btypes.c>  - generated C-source file name"
		"\n -h<btypes.h>  - generated C-header file name; if not specified then equals to source file name with replaced extension to '.h'"
		"\n -m<bmodel.md> - generated model file name, this file will contain meta information about structures"
		"\n -d<decl>      - declaration inserted before each generated non-static function"
		"\n -n            - don't add auto-prefix char (o,a,r) for optional, array and required array fields"
		"\n", compiler_logo, program);
	return 1;
}

int main(int argc, char *argv[])
{
	const char *program = argv[0];
	make_logo();
	/* <input.sd> must be specified */
	if (argc < 2)
		return usage(program);
	{
		char *al = NULL;
		#define opts "schmdn"
		#define in values[0]
		#define cs values[1]
		#define ch values[2]
		#define md values[3]
		#define dc values[4]
		#define na values[5]
		char *values[sizeof(opts) - 1];
		if ((unsigned)argc > 1/*program name*/ + sizeof(values)/sizeof(values[0])) {
			fprintf(stderr, "%s: too many arguments\n", program);
			return 1;
		}
		{
			int i = get_opts(argc, argv, opts, values);
			if (i > 0) {
				fprintf(stderr, "%s: expecting an option argument started with '-': %s\n", program, argv[i]);
				return 1;
			}
			if (i < 0) {
				fprintf(stderr, "%s: unknown option: %s\n", program, argv[-i]);
				return 1;
			}
		}
		if (na && na[0]) {
			fprintf(stderr, "%s: bad option: -n%s\n", program, na);
			return 1;
		}
		if (!in || !in[0]) {
			fprintf(stderr, "%s: please specify structure definitions file name (via -s<input.sd>)\n", program);
			return 1;
		}
		if (cs && !cs[0]) {
			fprintf(stderr, "%s: please specify non-empty generated C-source file name (via -c<btypes.c>)\n", program);
			return 1;
		}
		if (ch && !ch[0]) {
			fprintf(stderr, "%s: please specify non-empty generated C-header file name (via -h<btypes.h>)\n", program);
			return 1;
		}
		if (md && !md[0]) {
			fprintf(stderr, "%s: please specify non-empty generated model file name (via -m<bmodel.md>)\n", program);
			return 1;
		}
		if (!cs && !md) {
			fprintf(stderr, "%s: please specify generated C-source or model file name (via -c<btypes.c> or -m<bmodel.md>)\n", program);
			return 1;
		}
		if (dc && !dc[0])
			dc = NULL;
		if (cs && !ch) {
			size_t len = strlen(cs); /* >0 */
			{
				size_t c = len;
				while ('.' != cs[--c] && c);
				if (c)
					len = c;
			}
			if ((size_t)~(size_t)0 - sizeof(".h") < len) {
				fprintf(stderr, "%s: too long generated C-source file name: '%s'\n", program, cs);
				return 1;
			}
			al = (char*)mem_alloc(len + sizeof(".h"));
			memcpy(al, cs, len);
			memcpy(al + len, ".h", sizeof(".h"));
			ch = al;
		}
		{
			size_t size;
			char *buf = read_file(in, &size/*out*/);
			if (parse(in, buf, size, /*autoprefix:*/!na)) {
				build_model();
				if (md)
					fclose(print_model(wopen_file(md)));
				if (cs) {
					fclose(generate_h(wopen_file(ch), ch, dc/*NULL?*/));
					fclose(generate_c(wopen_file(cs), cs, ch, dc/*NULL?*/));
				}
			}
			free(buf);
		}
		free(al);
	}
	return 0;
}
