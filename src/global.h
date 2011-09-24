/*
 * This file is part of tags2db.
 *
 * Tags2db is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tags2db is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tags2db.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif /* HAVE_INTTYPES_H */

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif /* HAVE_STDINT_H */

/* Dealing with boolean types */
#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# ifndef HAVE__BOOL
#  if defined(INT8_MAX) || defined(int8_t)
typedef signed int8_t   _Bool;
#  else
typedef signed int      _Bool;
#  endif /* defined(INT8_MAX) || defined(int8_t) */
# endif /* HAVE__BOOL_H */
# define bool   _Bool
# define true   1
# define false  0
#endif /* HAVE_STDBOOL_H */

#define TAGS_TYPE_INFO_COUNT   1
#define DB_TYPE_INFO_COUNT     1

#ifndef DEFINED_STRUCT_RECORD
# define DEFINED_STRUCT_RECORD
typedef struct stuRecord        Record;
#endif

typedef enum enumVariantType
{
    VARIANT_TYPE_UNKNOWN = 0,
    VARIANT_TYPE_INT,
    VARIANT_TYPE_DOUBLE,
    VARIANT_TYPE_CHAR,
    VARIANT_TYPE_STRING
} eVariantType;

typedef struct stuVariant
{
    eVariantType        type;

    union
    {
        int             int_data;
        double          double_data;
        char            char_data;
        char*           string_data;
    } data;

} Variant;

typedef struct stuInputTagObject
{
    FILE*           input_tag_file; /* the input tag file */
} InputTagObject;

typedef struct stuOutputDbObject
{
    const char*     connection_string;
    const char*     field_prefix;  /* prefix of field names */ 
} OutputDbObject;

/* tag types */
typedef struct stuTagsTypeInfo
{
    const char*     name;

    Record* (*func_read_one_record) (InputTagObject* ito);
} TagsTypeInfo;

/* db types */
typedef struct stuDbTypeInfo
{
    const char*     name;

    int (*func_initialize) (const OutputDbObject* odo);
    int (*func_write_one_record) (
            const OutputDbObject* odo, const Record* rec);
    int (*func_finalize) (const OutputDbObject* odo);
} DbTypeInfo;

typedef struct stuGlobal /* Global varibles, only one instance */
{
    InputTagObject input_tag_object;
    OutputDbObject output_db_object;
    TagsTypeInfo   tags_type_infos[TAGS_TYPE_INFO_COUNT];
    DbTypeInfo     db_type_infos[DB_TYPE_INFO_COUNT];
} Global;

extern Global           global;

#endif /* GLOBAL_H */
