#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* model.h */

#include "ptypes.h"

/* model version: major.minor.patch */
#define BIT_BRIDGE_MODEL_MAJOR 1
#define BIT_BRIDGE_MODEL_MINOR 0
#define BIT_BRIDGE_MODEL_PATCH 0

/* workaround address sanitizer errors */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define FLEX_ARRAY_SIZE
#else
#define FLEX_ARRAY_SIZE 65536
#endif

/* packed structure size must not exceed this limit */
/* also this is maximum number of elements in an array (except bit arrays) */
#define BRIDGE_MODEL_MAX_PACKED_SIZE ((0x7FFFFFFF & ~0u) - 8u)

/* packed bit count must not exceed this limit */
/* also this is maximum number of elements in bit array */
#define BRIDGE_MODEL_MAX_BIT_COUNT (0xFFFFFFFF & ~0u)

/* maximum number of bytes needed to pack bridge counter */
#define BRIDGE_MAX_COUNTER_PACKED_SIZE 5

/* some predefined f_power values */
#define F_OPTIONAL  0u        /* field is optional */
#define F_REQUIRED  1u        /* field is required */
#define F_POINTER  (~0u - 1u) /* field is a fixed-size array of 1 element (except for fixed bits array) */
#define F_ARRAY     ~0u       /* field is a dynamic array */

/* fixed-size array field has f_power in range:
  for bit-field     [F_REQUIRED + 1, BRIDGE_MODEL_MAX_BIT_COUNT - 1] (BRIDGE_MODEL_MAX_BIT_COUNT may be == F_ARRAY, F_POINTER - ignored)
  for non-bit field [F_REQUIRED + 1, BRIDGE_MODEL_MAX_PACKED_SIZE] */

struct struct_def;

/* structure field info */
struct field_def {
	/* attributes initialized by the parser */
	const char *type;             /* field typename, points either to one of reserved types or to user-defined type name */
	struct field_def *next_f;     /* next parsed field, NULL if current field is the last one */
	/* attributes initialized while building the model */
	struct struct_def *user_type; /* if not NULL then field is of user-defined type */
	/* attributes initialized by the parser */
	unsigned f_power;             /* field power: F_OPTIONAL, F_REQUIRED ... F_POINTER, F_ARRAY */
	/* attributes initialized while building the model */
	unsigned bit;                 /* F_OPTIONAL-field's bit number in the generated owner structure bits array
	                                 (optional field is serialized only if this bit is set) */
	/* attributes initialized by the parser */
	char name[FLEX_ARRAY_SIZE];   /* '\0'-terminated field name */
};

enum struct_layout {
	S_STATIC1  = 0,             /* all fields of structure are of fixed size and all required, maximum simple field size is 1 byte */
	S_STATIC2  = 1,             /* all fields of structure are of fixed size and all required, maximum simple field size is 2 bytes */
	S_STATIC4  = 2,             /* all fields of structure are of fixed size and all required, maximum simple field size is 4 bytes */
	S_STATIC8  = 3,             /* all fields of structure are of fixed size and all required, maximum simple field size is 8 bytes */
	S_MONOLITH = 4,             /* structure resides in monolith memory block, some fields are optional */
	S_DYNAMIC  = 5,             /* some structure fields allocated in different memory blocks */
	S_INVALID1 = 8 + S_STATIC1, /* temporary value, used internally */
	S_INVALID2 = 8 + S_STATIC2, /* temporary value, used internally */
	S_INVALID3 = 8 + S_STATIC4, /* temporary value, used internally */
	S_INVALID4 = 8 + S_STATIC8, /* temporary value, used internally */
	S_INVALID5 = 8 + S_MONOLITH /* temporary value, used internally */
};

/* user type (structure) info */
struct struct_def {
	/* attributes initialized by the parser, modified while building the model */
	struct field_def **fields;         /* fields array, contains one or more fields <= ~0u */
	struct field_def **fields_end;     /* fields array end */
	/* attributes initialized by the parser */
	struct field_def *first_f;         /* fields array may be re-sorted, this is the first field as read from model definition */
	/* attributes initialized while building the model */
	struct field_def *tail_recursive;  /* last-processed tail-recursive field */
	const char *equivalent;            /* C: non-NULL if structure contains only required fields of this equivalent non-bit simple type */
	unsigned bit_offset;               /* number of bytes reserved for structure bits (for optional fields and bit-fields), <= ~0u/8 + 1 */
	unsigned need_destroy;             /* C: number of fields to destroy */
	enum struct_layout s_layout;       /* C: structure layout and alignment in memory */
	unsigned _marked              : 1; /* temporary flag, used internally, zero after model building */
	unsigned _processed           : 1; /* temporary flag, used internally, zero after model building */
	unsigned equiv_many           : 1; /* C: structure contains more than one field of equivalent type */
	unsigned recursion_entry      : 1; /* marks entry point of tail-recursive circle */
	unsigned convertable          : 1; /* structure has fields which are affected by big/little endian conversion */
	unsigned unknown_packed_bits  : 1; /* structure has unknown number of packed bits */
	unsigned unknown_packed_bytes : 1; /* structure has unknown number of packed bytes */
	unsigned ptr_arr              : 1; /* C: structure may be destroyed as array of pointers (one pointer if !equiv_many) */
	unsigned _aux                 : 8; /* auxiliary flags, initially zero */
	unsigned packed_bits;              /* number of required bits in packed structure, <= BRIDGE_MODEL_MAX_BIT_COUNT */
	unsigned packed_optional_bits;     /* number of optional bits in packed structure, valid if !unknown_packed_bits */
	unsigned packed_bytes;             /* number of required bytes in packed structure, <= BRIDGE_MODEL_MAX_PACKED_SIZE */
	unsigned packed_optional_bytes;    /* number of optional bytes in packed structure, valid if !unknown_packed_bytes */
	/* attributes initialized by the parser */
	char s_name[FLEX_ARRAY_SIZE];      /* '\0'-terminated type name */
};

/* is it needed to count bits to pack? (structure has variable packed bits count) */
static inline int v_bits(const struct struct_def *s)
{
	return s->packed_optional_bits || s->unknown_packed_bits;
}

/* is it needed to count bytes to pack? (structure has variable packed bytes count) */
static inline int v_bytes(const struct struct_def *s)
{
	return s->packed_optional_bytes || s->unknown_packed_bytes;
}

/* check if a structure has bits to pack */
static inline int s_has_bits(const struct struct_def *s)
{
	return s->packed_bits || v_bits(s);
}

/* check if a structure has bytes to pack */
static inline int s_has_bytes(const struct struct_def *s)
{
	return s->packed_bytes || v_bytes(s);
}

/* check if a structure packed size is fixed */
static inline int s_fixed(const struct struct_def *s)
{
	return !v_bytes(s) && !v_bits(s);
}

/* check if number of bytes needed to pack structure's bits is fixed */
/* NOTE: total bit count may overflow if s->packed_bits > BRIDGE_MODEL_MAX_BIT_COUNT - 7:
  ---------------------------------------------------------------------------------------------
  | s->packed_bits                 | s->packed_optional_bits |         total bit count        |
  |--------------------------------|-------------------------|--------------------------------|
  | BRIDGE_MODEL_MAX_BIT_COUNT - 6 |            7            | BRIDGE_MODEL_MAX_BIT_COUNT + 1 |
  | BRIDGE_MODEL_MAX_BIT_COUNT - 5 |            6            | BRIDGE_MODEL_MAX_BIT_COUNT + 1 |
  .............................................................................................
  | BRIDGE_MODEL_MAX_BIT_COUNT - 0 |            1            | BRIDGE_MODEL_MAX_BIT_COUNT + 1 |
  --------------------------------------------------------------------------------------------- */
static inline int bits_have_fixed_bytes(const struct struct_def *s)
{
	return
		!s->unknown_packed_bits &&
		s->packed_optional_bits <= (7u & (unsigned)-(int)s->packed_bits); /* note: s->packed_optional_bits <= 7 if returns non-zero */
}

/* check if number of packed bytes is variable and
  it's unknown how many bytes is needed to pack structure's bits */
static inline int need_pack_bits_counter(const struct struct_def *s)
{
	return v_bytes(s) && !bits_have_fixed_bytes(s);
}

/* returns non-zero for variable-packed interger type */
static inline int is_variabe_int_type(const char *type)
{
	return
		field_type_pshort  == type ||
		field_type_pint    == type ||
		field_type_plong   == type ||
		field_type_ppshort == type ||
		field_type_ppint   == type ||
		field_type_pplong  == type;
}

/* returns non-zero for string-type */
static inline int is_simple_str_type(const char *type)
{
	return
		field_type_str  == type ||
		field_type_pstr == type;
}

/* returns non-zero for variable-packed simple type */
static inline int is_variable_simple_type(const char *type)
{
	return
		is_variabe_int_type(type) ||
		is_simple_str_type(type);
}

/* C: returns non-zero if field is destroyable */
static inline int is_destroyable_field(const struct field_def *f)
{
	switch (f->f_power) {
		default: /* fixed-length array */
		case F_POINTER:
		case F_ARRAY:
			return 1;
		case F_OPTIONAL:
			if (f->user_type || is_simple_str_type(f->type))
				return 1;
			break;
		case F_REQUIRED:
			if ((f->user_type && S_DYNAMIC == f->user_type->s_layout) || is_simple_str_type(f->type))
				return 1;
			break;
	}
	return 0;
}

/* get field repeat count */
static inline unsigned field_pwr(const struct field_def *f)
{
	if (F_POINTER != f->f_power)
		return f->f_power;
	if (f->user_type || field_type_bit != f->type)
		return 1u;
	return F_POINTER;
}

/* returns number of bytes needed to encode given number of bits */
static inline unsigned bytes_for_bits(unsigned bits)
{
	return ((bits & 1) + (bits >> 1) + 3) >> 2;
}

/* counter packed size */
#ifdef __GNUC__
__attribute__ ((const))
#endif
unsigned _counter_packed_size(unsigned x);

/* structure packed size limit, may be > BRIDGE_MODEL_MAX_PACKED_SIZE, 0 if unknown */
#ifdef __GNUC__
__attribute__ ((pure))
#endif
unsigned s_packed_size_limit(const struct struct_def *s);

/* parsed model, initialized by the parse() before calling build_model() */
extern struct struct_def **structs;
extern struct struct_def **structs_end;

/* init model attributes not filled by the parse() */
void build_model(void);

/* parse model, returns number of parsed structures */
/* autoprefix - if non-zero, than add prefix to field names:
 'o' - for optional field,
 'a' - for arrays,
 'r' - for fixed-sized arrays */
size_t parse(const char *filename/*NULL?*/, const char input[], size_t size, int autoprefix);

FILE *print_model(FILE *file);

#endif /* MODEL_H_INCLUDED */
