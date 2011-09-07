#ifndef UTILS_H
#define UTILS_H

char* t2d_util_str_trim(char* str, const char* blank_chars);
int t2d_util_str_chr_count(const char* str, char chr);
char* t2d_util_str_replace(char* str, const char* substr, const char* rplstr);
char* t2d_util_str_replace_dup(
        const char* str, const char* substr, const char* rplstr);
#endif /* UTILS_H */
