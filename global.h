#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stuVariant
{
    int             type;

    union
    {
        int     int_data;
        double  double_data;

    } data;

} Variant;

typedef struct stuRecord /* a record */
{
    char**       fields; /* field names */
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
