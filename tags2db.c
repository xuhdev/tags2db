#include "global.h"
#include "tags_ctags.h"
#include "record.h"

Global global;

    int
main(int argc, const char *argv[])
{
    int     i;
    Record* rec;

    /* zero global */
    memset(&global, 0, sizeof(global));

    /* parse argument */
    {
        int     out_file_flag = 0;

        for(i = 1; i < argc; ++i)
        {
            if(out_file_flag)
            {
                global.out_path = strdup(argv[i]);
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

                out_file_flag = 1;
            }
        }
    }

    if(!global.input_tag_file)
    {
        fprintf(stderr, "Input tag file is not specified.\n");
        exit(2);
    }

    if(!global.out_path)
    {
        fprintf(stderr, "Out file path is not specified.\n");
        exit(3);
    }

    global.input_tag_object.input_tag_file = global.input_tag_file;

    rec = ctags_read_one_record(&global.input_tag_object);

    while(rec)
    {
        record_free(rec);
        rec = ctags_read_one_record(&global.input_tag_object);
    }

    /* close the tag file */
    fclose(global.input_tag_file);

    return 0;
}
