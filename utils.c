#include "global.h"

/*
 * Trim a string. Blank chars are specified by blank_chars
 */
    char*
t2d_util_str_trim(char* str, const char* blank_chars)
{
    int         i;
    int         blank_char_count; /* count of blank chars */
    int         len_str; /* the length of str */
    char*       p;

    len_str = strlen(str);

    /* if blank chars are not specified, use space and tab */
    if(!blank_chars)
        blank_chars = " \t\r\n";

    blank_char_count = strlen(blank_chars);

    /*
     * check blank chars at the end of the string, if we meet a nonempty char, 
     * then we know that it's time to trim
     */
    for(i = len_str - 1; i >= 0; -- i)
    {
        if(!strchr(blank_chars, str[i]))
        {
            if(i < len_str - 1)
            {
                str[i + 1] = '\0';
                len_str = strlen(str);
            }
            break;
        }
    }

    /*
     * now we search from the begin of the string, trim if we find a nonempty
     * string and we really need trim
     */
    for(p = str; *p; ++ p)
    {
        if(!strchr(blank_chars, *p))
        {
            if(p != str)
                memmove(str, p, strlen(p) + 1);
            break;
        }
    }

    return str;
}

/*
 * count how many times has a character appeared in a string
 */
    int
t2d_util_str_chr_count(const char* str, char chr)
{
    const char*     p;
    int             count = 0;

    if(!chr)
        return 1;

    for(p = strchr(str, chr); p; p = strchr(p + 1, chr))
        ++ count;

    return count;
}
