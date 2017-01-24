#ifndef PTYPES_H_INCLUDED
#define PTYPES_H_INCLUDED

/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* ptypes.h */

/* predefined types */
extern const char field_type_bit[];     /* "bit"     */
extern const char field_type_byte[];    /* "byte"    */
extern const char field_type_short[];   /* "short"   */
extern const char field_type_pshort[];  /* "pshort"  */
extern const char field_type_ppshort[]; /* "ppshort" */
extern const char field_type_int[];     /* "int"     */
extern const char field_type_pint[];    /* "pint"    */
extern const char field_type_ppint[];   /* "ppint"   */
extern const char field_type_long[];    /* "long"    */
extern const char field_type_plong[];   /* "plong"   */
extern const char field_type_pplong[];  /* "pplong"  */
extern const char field_type_float[];   /* "float"   */
extern const char field_type_double[];  /* "double"  */
extern const char field_type_str[];     /* "str"     */
extern const char field_type_pstr[];    /* "pstr"    */

/* returns pointer to one of reserved type names field_type_...[]
  if checked name matches against any of reserved types, otherwise returns NULL */
const char *get_reserved_type(const char checked[], unsigned len)
#ifdef __GNUC__
__attribute__ ((pure))
#endif
;

/* check if given type_name points to one of field_type_...[] */
int is_reserved_type(const char *type_name)
#ifdef __GNUC__
__attribute__ ((const))
#endif
;

#endif /* PTYPES_H_INCLUDED */
