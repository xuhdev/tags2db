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
#include "record.h"

/*
 * Release a record object
 */
    void
record_free(Record* r)
{
    int     i;

    for(i = 0; i < r->number; ++ i)
    {
        free(r->fields_name[i]);

        /* if the data type is string, release the string first */
        if(r->data[i].type == VARIANT_TYPE_STRING)
            free(r->data[i].data.string_data);
    }

    free(r->fields_name);
    free(r->data);

    free(r);
}
