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

/*
 * replace string. Returns the address of the place where the replacing
 * occurs.
 */
    char*
t2d_util_str_replace(char* str, const char* substr, const char* rplstr)
{
    char*       subpos;
    int         len_substr;
    int         len_rplstr;
    
    subpos = strstr(str, substr);

    if(!subpos)
        return NULL;

    len_substr = strlen(substr);
    len_rplstr = strlen(rplstr);

    if(len_substr >= len_rplstr)
    {
        memcpy(subpos, rplstr, strlen(rplstr) * sizeof(char));

        if(len_substr > len_rplstr)
            memmove(subpos + len_rplstr, subpos + len_substr,
                    (strlen(subpos + len_substr) + 1) * sizeof(char));
    }
    else
    {
        memmove(subpos + len_rplstr, subpos + len_substr,
                (strlen(subpos + len_substr) + 1) * sizeof(char));
        memcpy(subpos, rplstr, strlen(rplstr) * sizeof(char));
    }

    return subpos;
}

/*
 * replace all substr with rplstr and return a duplicated replaced string
 */
    char*
t2d_util_str_replace_dup(const char* str, const char* substr,
        const char* rplstr)
{
    char*       ret;
    bool        increasement_flag; /* whether the string is increasing */
    int         len_rplstr;
    int         len_substr;
    int         len_delta;
    char*       p;

    increasement_flag = (strlen(rplstr) > strlen(substr));

    len_rplstr = strlen(rplstr);
    len_substr = strlen(substr);
    len_delta = len_rplstr - len_substr;

    ret = (char*) malloc(sizeof(char) * (strlen(str) + 1 +
            (increasement_flag ? len_delta : 0)));

    strcpy(ret, str);

    p = ret;

    while(true)
    {
        p = t2d_util_str_replace(p, substr, rplstr);

        if(!p)
            break;

        p += len_rplstr;

        if(increasement_flag)
        {
            char*       tmp;

            tmp = (char*) malloc(sizeof(char) * (strlen(ret) + len_delta + 1));

            strcpy(tmp, ret);

            p = tmp + (p - ret);

            free(ret);

            ret = tmp;
        }
    }

    return ret;
}

#ifndef HAVE_STRNDUP

/*
 * strndup
 */
    char*
strndup (const char* s, size_t n)
{
  size_t    len = strlen(s);
  size_t    new_len;
  char*     new_str;

  new_len = len < n ? len : n;

  new_str = (char*) malloc(sizeof(char) * (new_len + 1));

  if(!new_str)
    return NULL;

  new_str[new_len] = '\0';
  return (char*) memcpy(new_str, s, new_len);
}

#endif /* HAVE_STRNDUP */
