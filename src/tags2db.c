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

    const char*
get_help_string(void)
{
    return
        "Usage: " PACKAGE " [options] input_tags_file output_database\n"
        "input_tags_file is the tags file to be input\n"
        "output_database is \"database_file:table_name\" for sqlite3\n"
        "\n"
        "Options:\n"
        "\n"
        "-t                 Input tags type (ctags, etc.). Default ctags\n"
        "-d                 Output database type (sqlite3, etc.). Default sqlite3\n"
        "-h or --help       Print this help message\n"
        "\n"
        "e.g. tags2db -t ctags -d sqlite3 ./tags tags.sqlite:tags\n\n";
}

    int
main(int argc, const char *argv[])
{
    int             i;
    Record*         rec;
    int             code_return;
    int             tags_type = 0;
    int             db_type = 0;

    /* zero global */
    memset(&global, 0, sizeof(global));

    global.tags_type_infos[0].name = "ctags";
    global.tags_type_infos[0].func_read_one_record =
        tags_ctags_read_one_record;

    global.db_type_infos[0].name = "sqlite3";
    global.db_type_infos[0].func_initialize = db_sqlite3_initialize;
    global.db_type_infos[0].func_write_one_record =
        db_sqlite3_write_one_record;
    global.db_type_infos[0].func_finalize = db_sqlite3_finalize;

    /* parse argument */
    {
        bool     output_connection_string_flag = false;
        bool     tags_type_flag = false;
        bool     db_type_flag = false;

        for(i = 1; i < argc; ++i)
        {
            if(output_connection_string_flag)
            {
                global.output_db_object.connection_string = strdup(argv[i]);
            }
            else if(tags_type_flag)
            {
                int     j;

                tags_type_flag = false;

                /* 
                 * Check whether the specified tags type is available. If not
                 * available, print an error message and exit.
                 */
                tags_type = -1;

                for(j = 0; j < TAGS_TYPE_INFO_COUNT; ++ j)
                {
                    if(!strcmp(global.tags_type_infos[j].name, argv[i]))
                    {
                        tags_type = j;
                        break;
                    }
                }

                if(tags_type == -1)
                {
                    fprintf(stderr, "Tags type \"");
                    fprintf(stderr, argv[i]);
                    fprintf(stderr, "\" is not available.\n");
                    exit(5);
                }
            }
            else if(db_type_flag)
            {
                int     j;

                db_type_flag = false;

                /* 
                 * Check whether the specified tags type is available. If not
                 * available, print an error message and exit.
                 */
                db_type = -1;

                for(j = 0; j < DB_TYPE_INFO_COUNT; ++ j)
                {
                    if(!strcmp(global.db_type_infos[j].name, argv[i]))
                    {
                        db_type = j;
                        break;
                    }
                }

                if(db_type == -1)
                {
                    fprintf(stderr, "Database type \"");
                    fprintf(stderr, argv[i]);
                    fprintf(stderr, "\" is not available.\n");
                    exit(6);
                }
            }
            else if(!strcmp(argv[i], "-t"))
                tags_type_flag = true;
            else if(!strcmp(argv[i], "-d"))
                db_type_flag = true;
            else if((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")))
            {
                fprintf(stdout, get_help_string());
                exit(0);
            }
            else
            {
                if(!strcmp(argv[i], "-"))
                    global.input_tag_object.input_tag_file = stdin;
                else
                {
                    global.input_tag_object.input_tag_file = fopen(
                            argv[i], "r");
                    
                    if(!global.input_tag_object.input_tag_file)
                    {
                        perror("Unable to open the tag file");
                        exit(1);
                    }
                }

                output_connection_string_flag = true;
            }
        }
    }

    if(!global.input_tag_object.input_tag_file)
    {
        fprintf(stderr, "Input tag file is not specified.\n");
        exit(2);
    }

    if(!global.output_db_object.connection_string)
    {
        fprintf(stderr, "Out file path is not specified.\n");
        exit(3);
    }

    if((code_return = (*global.db_type_infos[db_type].func_initialize) (
                    &global.output_db_object)) != 0)
    {
        fprintf(stderr, "Failed to initialize sqlite3: %d\n", code_return);
        exit(4);
    }

    rec = (*global.tags_type_infos[tags_type].func_read_one_record) (
            &global.input_tag_object);

    while(rec)
    {
        if((code_return =
                    (*global.db_type_infos[db_type].func_write_one_record) (
                        &global.output_db_object, rec)) != 0)
        {
            fprintf(stderr, "Failed to write to sqlite3 database: %d\n",
                    code_return);
            break;
        }
        record_free(rec);
        rec = (*global.tags_type_infos[tags_type].func_read_one_record) (
                &global.input_tag_object);
    }

    if((code_return = (*global.db_type_infos[db_type].func_finalize) (
                    &global.output_db_object)) != 0)
    {
        fprintf(stderr, "Failed to finalize the sqlite3 database: %d\n",
                code_return);
    }
    /* close the tag file */
    fclose(global.input_tag_object.input_tag_file);

    return 0;
}
