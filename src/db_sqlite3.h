#ifndef DB_SQLITE3_H
#define DB_SQLITE3_H

#include "record.h"


int db_sqlite3_initialize(const OutputDbObject* odo);
int db_sqlite3_write_one_record(const OutputDbObject* odo, const Record* rec);
int db_sqlite3_finalize(const OutputDbObject* odo);


#endif /* DB_SQLITE3_H */
