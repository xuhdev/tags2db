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

#ifndef DB_SQLITE3_H
#define DB_SQLITE3_H

#include "record.h"


int db_sqlite3_initialize(const OutputDbObject* odo);
int db_sqlite3_write_one_record(const OutputDbObject* odo, const Record* rec);
int db_sqlite3_finalize(const OutputDbObject* odo);


#endif /* DB_SQLITE3_H */
