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

#ifndef TAGS_GCCXML_H
#define TAGS_GCCXML_H

#include "global.h"

#ifdef ENABLED_GCCXML /* if file is only valid when gccxml is enabled */

#include "record.h"

Record* tags_gccxml_read_one_record(InputTagObject* ito);

#endif /* ENABLED_GCCXML */

#endif /* end of include guard: TAGS_GCCXML_H */
