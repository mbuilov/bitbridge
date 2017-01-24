#ifndef JCOMPILER_H_INCLUDED
#define JCOMPILER_H_INCLUDED

/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* jcompiler.h */

/* for int of 32 bits, determine int number and bit mask of given bit */
#define BIT_INT(b) ((b) >> 5u)
#define BIT_NUM(b) (0x80000000u >> (31u - ((b) & 31u)))

struct struct_def;
void check_unchecked_exceptions(void);
void mark_strutures_for_unpack(void);
void generate_code_pack(FILE *file, const struct struct_def *s);
void generate_code_unpack(FILE *file, struct struct_def *s);
FILE *generate_java(FILE *file, char *fname, const char *pkg/*NULL?*/);

extern char jcompiler_logo[];

#endif /* JCOMPILER_H_INCLUDED */
