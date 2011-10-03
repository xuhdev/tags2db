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
#ifdef ENABLED_GCCXML
# include "tags_gccxml.h"
#endif /* ENABLED_GCCXML */
#include "db_sqlite3.h"
#include "record.h"

Global global;

    static void
print_help_string(void)
{
    printf("Usage:" PACKAGE " [options] input_tags_file output_database\n");
    printf("Or:   " PACKAGE " -f input_tags_file [options] output_database\n");
    printf("input_tags_file is the tags file to be input\n");
    printf("output_database is \"database_file:table_name\" for sqlite3\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("-f                 Input tags file name(\"-\" means reading from stdin\n");
    printf("-t                 Input tags type (ctags, etc.). Default ctags\n");
    printf("-d                 Output database type (sqlite3, etc.). Default sqlite3\n");
    printf("-p or --prefix     Prefix added to field names of the databse. Default \"tags2db_\"\n");
    printf("-h or --help       Print this help message\n");
    printf("--version          Print version information\n");
    printf("\n");
    printf("e.g.\n");
    printf("Convert a ctags file to a sqlite database in which table \"tags\" contains the tags information:\n");
    printf("tags2db -t ctags -d sqlite3 ./tags tags.sqlite:tags\n");
    printf("Generate tags for /usr/include by Exuberant Ctags and put them into a sqlite database:\n");
    printf("ctags -R  -f - -u /usr/include | tags2db -f - tags.sqlite:tags\n");
    printf("Convert a gccxml output file to a sqlite database:\n");
    printf("tags2db -t gccxml out.xml gccxml.sqlite:tags\n");
    printf("Then you could execute \"sqlite3 -header tags.sqlite3 \'select * from tags;\'\" on shell to check the database.\n");
    printf("\n");
}

    const char*
get_version_string(void)
{
    return
        PACKAGE " " PACKAGE_VERSION "\n"
        "Copyright (C) 2011 Hong Xu\n"
        "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
        "This is free software: you are free to change and redistribute it.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n";
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

    i = 0;
    global.tags_type_infos[i].name = "ctags";
    global.tags_type_infos[i].func_read_one_record =
        tags_ctags_read_one_record;
    ++ i;
#ifdef ENABLED_GCCXML
    global.tags_type_infos[i].name = "gccxml";
    global.tags_type_infos[i].func_read_one_record =
        tags_gccxml_read_one_record;
#endif

    global.db_type_infos[0].name = "sqlite3";
    global.db_type_infos[0].func_initialize = db_sqlite3_initialize;
    global.db_type_infos[0].func_write_one_record =
        db_sqlite3_write_one_record;
    global.db_type_infos[0].func_finalize = db_sqlite3_finalize;

    /* default field name prefix is "tags2db_" */
    global.output_db_object.field_prefix = "tags2db_";

    /* parse argument */
    {
        bool     tags_file_name_flag = false; /* indicates it's "-f" switch */
        /* indicates tags file name has already been specified */
        bool     tags_file_name_already_specified_flag = false;
        bool     tags_type_flag = false;
        bool     db_type_flag = false;
        bool     db_field_prefix_flag = false; /* -p or --prefix */

        for(i = 1; i < argc; ++i)
        {
            if(tags_file_name_flag)
            {
                tags_file_name_flag = false;

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

                tags_file_name_already_specified_flag = true;

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
            else if(db_field_prefix_flag)
            {
                db_field_prefix_flag = false;

                global.output_db_object.field_prefix = argv[i];
            }
            else if(!strcmp(argv[i], "-t"))
                tags_type_flag = true;
            else if(!strcmp(argv[i], "-d"))
                db_type_flag = true;
            else if(!strcmp(argv[i], "-f"))
                tags_file_name_flag = true;
            else if((!strcmp(argv[i], "-p")) ||
                    (!strcmp(argv[i], "--prefix")))
            {
                db_field_prefix_flag = true;
            }
            else if((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")))
            {
                print_help_string();
                exit(0);
            }
            else if(!strcmp(argv[i], "--version"))
            {
                fprintf(stdout, get_version_string());
                exit(0);
            }
            else if(strlen(argv[i]) > 0 && argv[i][0] == '-')
            {
                fprintf(stderr, "Uknown option \"");
                fprintf(stderr, argv[i]);
                fprintf(stderr, "\"\n");
                exit(7);
            }
            else /* when no "-" siwtch is specified */
            {
                if(tags_file_name_already_specified_flag)
                    global.output_db_object.connection_string = strdup(
                            argv[i]);
                else
                {
                    -- i;
                    tags_file_name_flag = true;
                }
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
