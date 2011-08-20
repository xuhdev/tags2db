#include "global.h"
#include "utils.h"

    Record*
ctags_read_one_item(InputTagObject* ito)
{
#define MAX_LINE_SIZE   1024
    char        one_line[MAX_LINE_SIZE + 1];
    FILE*       tag_file = ito->input_tag_file;

    if(feof(tag_file))
        return NULL;

    while(!feof(tag_file))
    {
        fgets(one_line, MAX_LINE_SIZE, tag_file);

        /* before doing anything, trim the line */
        t2d_util_str_trim(one_line, NULL);

        /* if it's a comment line or empty line, skip it */
        if(one_line[0] == '\0' || one_line[0] == '!')
            continue;
    }
#undef MAX_LINE_SIZE
}
