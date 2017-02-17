/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* utils.c */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

void err(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(-1);
}

void *mem_alloc(size_t size)
{
	void *r = malloc(size);
	if (!r)
		err("cannot allocate %lu bytes\n", (unsigned long)(size & ~0ul));
	return r;
}

void *mem_realloc(void *mem, size_t size)
{
	void *new_mem = realloc(mem, size);
	if (!new_mem)
		err("cannot reallocate %lu bytes\n", (unsigned long)(size & ~0ul));
	return new_mem;
}

FILE *wopen_file(const char *output)
{
	FILE *out = fopen(output, "wb");
	if (!out) {
		fprintf(stderr, "cannot open file '%s' for writing\n", output);
		exit(-2);
	}
	return out;
}

static int _read_file(FILE *in, const char *input, void **mem/*out*/, size_t *size/*out*/)
{
	if (fseek(in, 0, SEEK_END)) {
		fprintf(stderr, "failed to seek '%s'\n", input);
		return 0;
	}
	{
		long sz = ftell(in);
		if (!sz) {
			fprintf(stderr, "empty input file '%s'\n", input);
			return 0;
		}
		if (sz < 0 || (unsigned long)sz > (size_t)~(size_t)0) {
			fprintf(stderr, "bad length of input file '%s'\n", input);
			return 0;
		}
		*size = (size_t)sz;
	}
	if (fseek(in, 0, SEEK_SET)) {
		fprintf(stderr, "failed to seek '%s'\n", input);
		return 0;
	}
	*mem = malloc(*size);
	if (!*mem) {
		fprintf(stderr, "failed to allocate %lu bytes\n", (unsigned long)(*size & ~0lu));
		return 0;
	}
	if (1 != fread(*mem, *size, 1, in)) {
		free(*mem);
		fprintf(stderr, "failed to read %lu bytes from '%s'\n", (unsigned long)(*size & ~0lu), input);
		return 0;
	}
	return 1;
}

char *read_file(const char *input, size_t *size/*out*/)
{
	FILE *in = fopen(input, "rb");
	if (!in) {
		fprintf(stderr, "cannot open file '%s' for reading\n", input);
		exit(-3);
	}
	{
		void *mem;
		int r = _read_file(in, input, &mem, size);
		fclose(in);
		if (!r)
			exit(-3);
		return (char*)mem;
	}
}

void print_header_header_file_define(FILE *file, const char *hname)
{
	char buf[256];
	size_t len = strlen(hname);
	{
		size_t cp = len;
		for (; cp && hname[cp - 1] != '\\' && hname[cp - 1] != '/'; cp--);
		len -= cp;
		hname += cp;
	}
	if (len > sizeof(buf)/sizeof(buf[0]) - 1)
		len = sizeof(buf)/sizeof(buf[0]) - 1;
	buf[len] = '\0';
	while (len) {
		char c = hname[--len];
		buf[len] =
			('A' <= c && c <= 'Z') ? c :
			('a' <= c && c <= 'z') ? (char)(c - ('a' - 'A')) :
			'_';
	}
	fprintf(file, "#ifndef %s_INCLUDED\n", buf);
	fprintf(file, "#define %s_INCLUDED\n", buf);
	fprintf(file, "\n/* %s */\n", hname);
}

void _add_ptr_to_array(void ***arr, void ***arr_end, void *ptr)
{
	size_t cnt = (size_t)(*arr_end - *arr);
	if ((size_t)~(size_t)0 - cnt*sizeof(void*) < sizeof(void*)) {
		fprintf(stderr, "cannot add one more pointer to array of %lu elements\n", (unsigned long)(cnt & ~0lu));
		exit(-4);
	}
	{
		void **new_mem = (void**)mem_realloc(*arr, sizeof(void*)*(cnt + 1u));
		new_mem[cnt] = ptr;
		*arr = new_mem;
		*arr_end = new_mem + cnt + 1u;
	}
}
