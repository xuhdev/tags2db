#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct stuRecord /* a record */
{
    int          number; /* number of fields */
    char**       fields_name; /* field names */
    Variant*     data;  /* data in a field in this record */
} Record;

typedef struct stuInputTagObject
{
    FILE*           input_tag_file; /* the input tag file */
} InputTagObject;

typedef struct stuGlobal /* Global varibles, only one instance */
{
    FILE*           input_tag_file;
    const char*     out_path;

    InputTagObject input_tag_object;
} Global;

extern Global           global;

#endif /* GLOBAL_H */
