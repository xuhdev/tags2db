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

#include "global.h"
#include "record.h"
#include "utils.h"

    Record*
tags_ctags_read_one_record(InputTagObject* ito)
{
    int         i;
#define MAX_LINE_SIZE   1024
    char        one_line[MAX_LINE_SIZE + 1];
    FILE*       tag_file = ito->input_tag_file;
    char*       tmpcharptr;
    char*       tmpcharptr0;
    Record*     ret;
    char*       first_three_fields[3];

    while(fgets(one_line, MAX_LINE_SIZE, tag_file))
    {
        bool     continue_flag = false;

        /* before doing anything, trim the line */
        t2d_util_str_trim(one_line, NULL);

        /* if it's a comment line or empty line, skip it */
        if(one_line[0] == '\0' || one_line[0] == '!')
            continue;

        /* check whether the line contains the standard 3 fields. if not, the
         * line is broken and we skip this line */
        tmpcharptr = one_line;
        for(i = 0; i < 2; ++ i)
        {
            tmpcharptr0 = tmpcharptr;
            tmpcharptr = strchr(tmpcharptr, '\t');

            if(!tmpcharptr)
            {
                /* this line is not right, continue to next line */
                continue_flag = true;
                break;
            }

            *tmpcharptr = '\0';
            first_three_fields[i] = strdup(tmpcharptr0);
            ++ tmpcharptr;
        }
        if(continue_flag)
            continue;

        ret = (Record*) malloc(sizeof(Record));
        /* search for ;"\t, if it is present, it's exctags, or it's
         * traditional ctags */
        tmpcharptr0 = tmpcharptr;
        tmpcharptr = strstr(tmpcharptr0, ";\"\t");
        if(tmpcharptr)
        {
            *tmpcharptr = '\0';
            ++ tmpcharptr;
            *tmpcharptr = '\0';
            ++ tmpcharptr;
            *tmpcharptr = '\0';
            ++ tmpcharptr;
        }
        first_three_fields[2] = strdup(tmpcharptr0);

        ret->number = 3;

        /* if we found ;"\t and there are more chars, then count the number of
         * extra fields */
        if(tmpcharptr && *tmpcharptr)
            ret->number += t2d_util_str_chr_count(tmpcharptr, '\t') + 1;

        ret->fields_name = (char**) malloc(sizeof(char*) * ret->number);
        ret->fields_name[0] = strdup("tagname");
        ret->fields_name[1] = strdup("tagfile");
        ret->fields_name[2] = strdup("tagaddress");

        ret->data = malloc(sizeof(Variant) * ret->number);
        for(i = 0; i < 3; ++ i)
        {
            ret->data[i].type = VARIANT_TYPE_STRING;
            ret->data[i].data.string_data = first_three_fields[i];
        }
        
        /* get extra fields if available */
        if(tmpcharptr)
        {
            for(tmpcharptr = strtok(tmpcharptr, "\t"), i = 3; tmpcharptr;
                    tmpcharptr = strtok(NULL, "\t"), ++ i)
            {
                char*       colon_pos;
                colon_pos = strchr(tmpcharptr, ':');

                /* if no colon is contained, then the field should be "kind"
                 * */
                if(!colon_pos)
                {
                    ret->fields_name[i] = strdup("kind");
                    ret->data[i].type = VARIANT_TYPE_CHAR;
                    ret->data[i].data.char_data = *tmpcharptr;
                    continue;
                }

                ret->fields_name[i] = (char*) malloc(sizeof(char) *
                        (colon_pos - tmpcharptr + 1));
                strncpy(ret->fields_name[i], tmpcharptr, colon_pos - tmpcharptr);
                ret->fields_name[i][colon_pos - tmpcharptr] = '\0';
                ret->data[i].type = VARIANT_TYPE_STRING;
                ret->data[i].data.string_data = strdup(colon_pos + 1);
            }
        }

        return ret;
    }

    return NULL;
#undef MAX_LINE_SIZE
}
