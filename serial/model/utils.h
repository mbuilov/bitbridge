#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* utils.h */

/* alloc memory, exit program on failure */
void *mem_alloc(size_t size);

/* realloc memory, exit program on failure */
void *mem_realloc(void *mem, size_t size);

/* reallocate array of pointers and append new pointer at end of array */
/* NOTE: exits program on failure */
void _add_ptr_to_array(void ***arr, void ***arr_end, void *ptr);

#define add_ptr_to_array(arr, arr_end, ptr) do { \
	void *__a = arr; \
	void *__e = arr_end; \
	/* check that arr is a pointer to pointer and is compatible with arr_end and ptr */ \
	(void)(sizeof(**(arr)) + sizeof((*(arr) - *(arr_end))) + sizeof(((ptr) - **(arr)))); \
	_add_ptr_to_array((void***)__a, (void***)__e, ptr); \
} while ((void)0,0)

/* show error and exit program */
#ifdef _MSC_VER
__declspec(noreturn)
#endif
void err(const char *format, ...)
#ifdef __GNUC__
__attribute__ ((__format__ (__printf__, 1, 2)))
__attribute__ ((noreturn))
#endif
;

/* open file for writing in binary mode */
/* NOTE: exits program on failure */
FILE *wopen_file(const char *output);

/* read file into buffer, returns non-empty dynamically allocated buffer */
/* NOTE: exits program on failure */
char *read_file(const char *input, size_t *size/*out*/);

/* print standard header for C header file: */
/* #ifndef "hname"_INCLUDED */
/* #define "hname"_INCLUDED */
void print_header_header_file_define(FILE *file, const char *hname);

#endif /* UTILS_H_INCLUDED */
