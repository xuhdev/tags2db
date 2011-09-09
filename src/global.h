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
} OutputDbObject;

typedef struct stuGlobal /* Global varibles, only one instance */
{
    FILE*           input_tag_file;

    InputTagObject input_tag_object;
    OutputDbObject output_db_object;
} Global;

extern Global           global;

#endif /* GLOBAL_H */
