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

#ifndef RECORD_H
#define RECORD_H

#include "global.h"

typedef struct stuRecord /* a record */
{
    int          number; /* number of fields */
    char**       fields_name; /* field names */
    Variant*     data;  /* data in a field in this record */
} Record;

void record_free(Record* r);

#endif /* RECORD_H */
