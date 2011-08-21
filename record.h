#ifndef RECORD_H
#define RECORD_H

#include "global.h"

typedef struct stuRecord /* a record */
{
    int          number; /* number of fields */
    char**       fields_name; /* field names */
    Variant*     data;  /* data in a field in this record */
} Record;

void record_free(Record* r);

#endif /* RECORD_H */
