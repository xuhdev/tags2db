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
        int     output_connection_string_flag = 0;

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
                        perror("Unable to open the tag file.\n");
                        exit(1);
                    }
                }

                output_connection_string_flag = 1;
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

    rec = ctags_read_one_record(&global.input_tag_object);

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
        rec = ctags_read_one_record(&global.input_tag_object);
    }

    db_sqlite3_finalize(&global.output_db_object);
    /* close the tag file */
    fclose(global.input_tag_file);

    return 0;
}
