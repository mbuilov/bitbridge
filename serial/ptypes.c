/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* ptypes.c */

#include "ptypes.h"

#define type_len(type) (sizeof(type) - 1)

const char field_type_bit[]     = "bit";
const char field_type_byte[]    = "byte";
const char field_type_short[]   = "short";
const char field_type_pshort[]  = "pshort";
const char field_type_ppshort[] = "ppshort";
const char field_type_int[]     = "int";
const char field_type_pint[]    = "pint";
const char field_type_ppint[]   = "ppint";
const char field_type_long[]    = "long";
const char field_type_plong[]   = "plong";
const char field_type_pplong[]  = "pplong";
const char field_type_float[]   = "float";
const char field_type_double[]  = "double";
const char field_type_str[]     = "str";
const char field_type_pstr[]    = "pstr";

int is_reserved_type(const char *type_name)
{
	return
		field_type_str     == type_name ||
		field_type_pstr    == type_name ||
		field_type_bit     == type_name ||
		field_type_byte    == type_name ||
		field_type_int     == type_name ||
		field_type_pint    == type_name ||
		field_type_ppint   == type_name ||
		field_type_long    == type_name ||
		field_type_plong   == type_name ||
		field_type_pplong  == type_name ||
		field_type_short   == type_name ||
		field_type_pshort  == type_name ||
		field_type_ppshort == type_name ||
		field_type_float   == type_name ||
		field_type_double  == type_name;
}

const char *get_reserved_type(const char checked[], unsigned len)
{
	switch (len) {
		case type_len(field_type_bit):
		/*case type_len(field_type_int):*/
		/*case type_len(field_type_str):*/
			if ('b' == checked[0] && 'i' == checked[1] && 't' == checked[2])
				return field_type_bit;
			if ('i' == checked[0] && 'n' == checked[1] && 't' == checked[2])
				return field_type_int;
			if ('s' == checked[0] && 't' == checked[1] && 'r' == checked[2])
				return field_type_str;
			break;
		case type_len(field_type_byte):
		/*case type_len(field_type_pint):*/
		/*case type_len(field_type_long):*/
		/*case type_len(field_type_pstr):*/
			if ('b' == checked[0] && 'y' == checked[1] && 't' == checked[2] && 'e' == checked[3])
				return field_type_byte;
			if ('p' == checked[0] && 'i' == checked[1] && 'n' == checked[2] && 't' == checked[3])
				return field_type_pint;
			if ('l' == checked[0] && 'o' == checked[1] && 'n' == checked[2] && 'g' == checked[3])
				return field_type_long;
			if ('p' == checked[0] && 's' == checked[1] && 't' == checked[2] && 'r' == checked[3])
				return field_type_pstr;
			break;
		case type_len(field_type_short):
		/*case type_len(field_type_ppint):*/
		/*case type_len(field_type_plong):*/
		/*case type_len(field_type_float):*/
			if ('s' == checked[0] && 'h' == checked[1] && 'o' == checked[2] && 'r' == checked[3] && 't' == checked[4])
				return field_type_short;
			if ('p' == checked[0] && 'p' == checked[1] && 'i' == checked[2] && 'n' == checked[3] && 't' == checked[4])
				return field_type_ppint;
			if ('p' == checked[0] && 'l' == checked[1] && 'o' == checked[2] && 'n' == checked[3] && 'g' == checked[4])
				return field_type_plong;
			if ('f' == checked[0] && 'l' == checked[1] && 'o' == checked[2] && 'a' == checked[3] && 't' == checked[4])
				return field_type_float;
			break;
		case type_len(field_type_double):
		/*case type_len(field_type_pshort):*/
		/*case type_len(field_type_pplong):*/
			if ('d' == checked[0] && 'o' == checked[1] && 'u' == checked[2] &&
				'b' == checked[3] && 'l' == checked[4] && 'e' == checked[5])
				return field_type_double;
			if ('p' == checked[0] && 's' == checked[1] && 'h' == checked[2] &&
				'o' == checked[3] && 'r' == checked[4] && 't' == checked[5])
				return field_type_pshort;
			if ('p' == checked[0] && 'p' == checked[1] && 'l' == checked[2] &&
				'o' == checked[3] && 'n' == checked[4] && 'g' == checked[5])
				return field_type_pplong;
			break;
		case type_len(field_type_ppshort):
			if ('p' == checked[0] && 'p' == checked[1] && 's' == checked[2] && 'h' == checked[3] &&
				'o' == checked[4] && 'r' == checked[5] && 't' == checked[6])
				return field_type_ppshort;
			break;
	}
	return (const char*)0;
}
