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
#include "tags_ctags.h"
#include "db_sqlite3.h"
#include "record.h"

Global global;

    int
main(int argc, const char *argv[])
{
    int             i;
    Record*         rec;
    int             code_return;

    /* zero global */
    memset(&global, 0, sizeof(global));

    /* parse argument */
    {
        bool     output_connection_string_flag = false;

        for(i = 1; i < argc; ++i)
        {
            if(output_connection_string_flag)
            {
                global.output_db_object.connection_string = strdup(argv[i]);
            }
            else
            {
                if(!strcmp(argv[i], "-"))
                    global.input_tag_file = stdin;
                else
                {
                    global.input_tag_file = fopen(argv[i], "r");
                    
                    if(!global.input_tag_file)
                    {
                        perror("Unable to open the tag file");
                        exit(1);
                    }
                }

                output_connection_string_flag = true;
            }
        }
    }

    if(!global.input_tag_file)
    {
        fprintf(stderr, "Input tag file is not specified.\n");
        exit(2);
    }

    if(!global.output_db_object.connection_string)
    {
        fprintf(stderr, "Out file path is not specified.\n");
        exit(3);
    }

    global.input_tag_object.input_tag_file = global.input_tag_file;

    if((code_return = db_sqlite3_initialize(&global.output_db_object)) != 0)
    {
        fprintf(stderr, "Failed to initialize sqlite3: %d\n", code_return);
        exit(4);
    }

    rec = tags_ctags_read_one_record(&global.input_tag_object);

    while(rec)
    {
        if((code_return = db_sqlite3_write_one_record(
                        &global.output_db_object, rec)) != 0)
        {
            fprintf(stderr, "Failed to write to sqlite3 database: %d\n",
                    code_return);
            break;
        }
        record_free(rec);
        rec = tags_ctags_read_one_record(&global.input_tag_object);
    }

    db_sqlite3_finalize(&global.output_db_object);
    /* close the tag file */
    fclose(global.input_tag_file);

    return 0;
}
