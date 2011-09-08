#include "global.h"
#include "record.h"

/*
 * Release a record object
 */
    void
record_free(Record* r)
{
    int     i;

    for(i = 0; i < r->number; ++ i)
    {
        free(r->fields_name[i]);

        /* if the data type is string, release the string first */
        if(r->data[i].type == VARIANT_TYPE_STRING)
            free(r->data[i].data.string_data);
    }

    free(r->fields_name);
    free(r->data);

    free(r);
}
