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

#ifndef UTILS_H
#define UTILS_H

char* t2d_util_str_trim(char* str, const char* blank_chars);
int t2d_util_str_chr_count(const char* str, char chr);
char* t2d_util_str_replace(char* str, const char* substr, const char* rplstr);
char* t2d_util_str_replace_dup(
        const char* str, const char* substr, const char* rplstr);
#endif /* UTILS_H */
