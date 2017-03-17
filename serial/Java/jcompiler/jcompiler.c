/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* jcompiler.c */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "jcompiler.h"
#include "model.h"
#include "get_opts.inl"

/* compiler version: major.minor.patch */
#define BIT_BRIDGE_JAVA_COMPILER_MAJOR 1
#define BIT_BRIDGE_JAVA_COMPILER_MINOR 0
#define BIT_BRIDGE_JAVA_COMPILER_PATCH 0

#define BITBRIDGE_JAVA_COMPILER_VERSION "BitBridge Java-compiler version "

/* version: [model_ver.compiler_major.compiler_minor] */
char jcompiler_logo[] = BITBRIDGE_JAVA_COMPILER_VERSION "major.minor.patch";

static void make_logo(void)
{
	(void)sprintf(jcompiler_logo + sizeof(BITBRIDGE_JAVA_COMPILER_VERSION) - 1, "%u.%u.%u",
		(BIT_BRIDGE_MODEL_MAJOR + BIT_BRIDGE_JAVA_COMPILER_MAJOR) & 65535u,
		(BIT_BRIDGE_MODEL_MINOR + BIT_BRIDGE_JAVA_COMPILER_MINOR) & 65535u,
		(BIT_BRIDGE_MODEL_PATCH + BIT_BRIDGE_JAVA_COMPILER_PATCH) & 65535u);
}

static int usage(const char *program)
{
	size_t e = strlen(program);
	for (; e && program[e - 1] != '\\' && program[e - 1] != '/'; e--);
	program += e;
	fprintf(stderr,
		"%s"
		"\n Generates structures declarations and code of init/pack/unpack functions in Java."
		"\nusage:"
		"\n %s -s<input.sd> [-j<btypes.java>] [-p<pkgname>] [-m<bmodel.md>] [-n]"
		"\nwhere"
		"\n -s<input.sd>    - structures definitions file name"
		"\n -j<btypes.java> - generated Java-source file name"
		"\n -p<pkgname>     - java package name"
		"\n -m<bmodel.md>   - generated model file name, this file will contain meta information about structures"
		"\n -n              - don't add auto-prefix char (o,a,r) for optional, array and required array fields"
		"\n", jcompiler_logo, program);
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
		#define opts "sjpmn"
		#define in values[0]
		#define js values[1]
		#define pk values[2]
		#define md values[3]
		#define na values[4]
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
		if (js && !js[0]) {
			fprintf(stderr, "%s: please specify non-empty generated Java-source file name (via -j<btypes.java>)\n", program);
			return 1;
		}
		if (pk && !pk[0]) {
			fprintf(stderr, "%s: please specify non-empty Java-package name (via -p<pkgname>)\n", program);
			return 1;
		}
		if (md && !md[0]) {
			fprintf(stderr, "%s: please specify non-empty generated model file name (via -m<bmodel.md>)\n", program);
			return 1;
		}
		if (!js && !md) {
			fprintf(stderr, "%s: please specify generated Java-source or model file name (via -j<btypes.java> or -m<bmodel.md>)\n", program);
			return 1;
		}
		{
			size_t size;
			char *buf = read_file(in, &size/*out*/);
			if (parse(in, buf, size, /*autoprefix:*/!na)) {
				build_model();
				if (md)
					fclose(print_model(wopen_file(md)));
				if (js)
					fclose(generate_java(wopen_file(js), js, pk/*NULL?*/));
			}
			free(buf);
		}
	}
	return 0;
}
