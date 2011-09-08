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
#include "db_sqlite3.h"
#include "utils.h"
#include <sqlite3.h>


typedef struct stuDbSqlite3Gv
{
    sqlite3*        handle;
    const char*     table_name;
#define MAX_FIELDS_NUMBER   100
    const char*     fields_name[MAX_FIELDS_NUMBER];
#undef MAX_FIELDS_NUMBER
    int             fields_number;
} DbSqlite3Gv;

static DbSqlite3Gv db_sqlite3_gv;

/*
 * initialization function for sqlite3
 */
    int
db_sqlite3_initialize(const OutputDbObject* odo)
{
    const char*     colon_pos;
    char*           file_name;
    char*           statement;

    /*
     * To initialize sqlite3 related resources, we first clear db_sqlite3_gv,
     * then parse the connection_string: split the connection string into two
     * seperate strings according to the position of last occured colon, the
     * head part is the file name of sqlite3 database, and then second one is
     * the table name. Then we try to open the sqlite3 database, if failed
     * return 1 otherwise return 0.
     */
    memset(&db_sqlite3_gv, 0, sizeof(db_sqlite3_gv));

    colon_pos = strrchr(odo->connection_string, ':');

    if(!colon_pos)
        return 5;

    db_sqlite3_gv.table_name = strdup(colon_pos + 1);

    file_name = strndup(odo->connection_string,
            colon_pos - odo->connection_string);

    if(sqlite3_open(file_name, &db_sqlite3_gv.handle)
            != SQLITE_OK)
    {
        free(file_name);
        return 1;
    }

    free(file_name);

    /*
     * drop the table first
     */
    statement = (char*) malloc(sizeof(char) * (
                21 +                        /* DROP TABLE IF EXISTS */
                strlen(db_sqlite3_gv.table_name) + /* table name */
                2));                                  /* ; NULL */

    strcpy(statement, "DROP TABLE IF EXISTS ");
    strcat(statement, db_sqlite3_gv.table_name);
    strcat(statement, ";");

    if(sqlite3_exec(db_sqlite3_gv.handle, statement,
                NULL, NULL, NULL) != SQLITE_OK)
    {
        free(statement);
        sqlite3_close(db_sqlite3_gv.handle);
        return 3;
    }
    free(statement);

    /*
     * Now we create the table. The table has one field (tagname text). If we
     * failed to create the table, we return 2
     */
    statement = (char*) malloc((
        27 +                                /* CREATE TABLE IF NOT EXISTS */
        strlen(db_sqlite3_gv.table_name) +  /* table_name */
        28 +                                /* (tagname TEXT DEFAULT NULL); */
        1)                                  /* NULL */
        * sizeof(char));
    strcpy(statement, "CREATE TABLE IF NOT EXISTS ");
    strcat(statement, db_sqlite3_gv.table_name);
    strcat(statement, "(tags2db_tagname TEXT DEFAULT NULL);");

    if(sqlite3_exec(db_sqlite3_gv.handle, statement,
                NULL, NULL, NULL) != SQLITE_OK)
    {
        free(statement);
        sqlite3_close(db_sqlite3_gv.handle);
        return 2;
    }

    free(statement);

    /* add the field tagname to db_sqlite3_gv.fields_name */
    db_sqlite3_gv.fields_name[db_sqlite3_gv.fields_number ++] =
        strdup("tagname");

    /* begin transaction */
    if(sqlite3_exec(db_sqlite3_gv.handle, "BEGIN",
                NULL, NULL, NULL) != SQLITE_OK)
    {
        sqlite3_close(db_sqlite3_gv.handle);
        return 4;
    }

    return 0;
}

    int
db_sqlite3_write_one_record(const OutputDbObject* odo, const Record* rec)
{
    int         i, j;
    int         statement_max_len;
    char*       statement;
    char*       p;
    char**      data_strings;

    /*
     * First of all, we check wether all the fields in rec has been defined in
     * the database. If not, use "alter table" to create these fields.
     */
    for(i = 0; i < rec->number; ++ i)
    {
        bool         alter_table_flag = true;

        /*
         * If the numberth field of rec is in db_sqlite3_gv.fields_name, let
         * alter_table_flag be 0, indicating that we do not need to alter
         * table. Otherwise we keep alter_table_flag true.
         */
        for(j = 0; j < db_sqlite3_gv.fields_number; ++ j)
        {
            if(!strcmp(db_sqlite3_gv.fields_name[j], rec->fields_name[i]))
            {
                alter_table_flag = false;
                break;
            }
        }

        if(alter_table_flag)
        {
            int     type_len = 0;

            /*
             * get the length of the type first
             */
            switch(rec->data[i].type)
            {
            case VARIANT_TYPE_INT:
                type_len = 3; /* INT */
                break;
            case VARIANT_TYPE_CHAR:
                type_len = 4; /* CHAR */
                break;
            case VARIANT_TYPE_STRING:
                type_len = 4; /* TEXT */
                break;
            }

            /* 
             * create the statement
             * ALTER TABLE table ADD COLUMN column type DEFAULT NULL
             */
            statement = (char*) malloc(sizeof(char) *
                    12 +                        /* ALTER TABLE */
                    strlen(db_sqlite3_gv.table_name) + 1 + /* table */
                    11 +                        /* ADD COLUMN */
                    8 +                         /* tags2db_ */
                    strlen(rec->fields_name[i]) + 1 + /* column name */
                    type_len + 1 +                    /* type */
                    13 +                             /* DEFAULT NULL */
                    1 +                             /* ; */
                    1);                             /* '\0' */

            strcpy(statement, "ALTER TABLE ");
            strcat(statement, db_sqlite3_gv.table_name);
            strcat(statement, " ADD COLUMN ");
            strcat(statement, "tags2db_");
            strcat(statement, rec->fields_name[i]);
            switch(rec->data[i].type)
            {
            case VARIANT_TYPE_INT:
                strcat(statement, " INT");
                break;
            case VARIANT_TYPE_CHAR:
                strcat(statement, " CHAR");
                break;
            case VARIANT_TYPE_STRING:
                strcat(statement, " TEXT");
                break;
            }

            strcat(statement, " DEFAULT NULL;");

            if(sqlite3_exec(db_sqlite3_gv.handle, statement,
                        NULL, NULL, NULL) != SQLITE_OK)
            {
                free(statement);
                return 2;
            }

            free(statement);

            /* add this new field to db_sqlite3_gv.fields_name */
            db_sqlite3_gv.fields_name[db_sqlite3_gv.fields_number ++] =
                strdup(rec->fields_name[i]);
        }
    }

    /* used to store the string data whose single quotes has been replaced by
     * two single quotes
     */
    data_strings = (char**) malloc(sizeof(char*) * rec->number); 
    memset(data_strings, 0, sizeof(char**) * rec->number);

    /*
     * Insert data into the table
     * INSERT INTO table (column1, [column2, ...])
     * VALUES (value1, [value2, ...]);
     */
    statement_max_len =
        12 +                /* INSERT INTO */ 
        strlen(db_sqlite3_gv.table_name) + 1 +  /* table */
        1;                             /* ( */

    for(i = 0; i < rec->number; ++ i)
    {
        statement_max_len += 8;     /* tags2db_ */
        statement_max_len +=
            strlen(rec->fields_name[i]) + /* fields */
            1;                          /* , */
    }

    statement_max_len +=
        2 +                 /* )  */
        7 +                 /* VALUES */
        1;                  /* ( */

    for(i = 0; i < rec->number; ++ i)
    {
        switch(rec->data[i].type)
        {
        case VARIANT_TYPE_CHAR:
            statement_max_len += 3;
            break;
        case VARIANT_TYPE_STRING:
            data_strings[i] = t2d_util_str_replace_dup(
                    rec->data[i].data.string_data, "\'", "\'\'");
            statement_max_len += strlen(data_strings[i]) + 2;
            break;
        }
        statement_max_len += 1; /* , */
    }

    statement_max_len += 
        2 +              /* ); */
        1;               /* '\0' */

    statement = (char*) malloc(statement_max_len);

    strcpy(statement, "INSERT INTO ");
    strcat(statement, db_sqlite3_gv.table_name);
    strcat(statement, " (");
    for(i = 0; i < rec->number; ++ i)
    {
        strcat(statement, "tags2db_");
        strcat(statement, rec->fields_name[i]);
        /* we do not need to add ',' for the last field */
        if(i < rec->number - 1)
            strcat(statement, ",");
    }
    strcat(statement, ") VALUES (");
    for(i = 0; i < rec->number; ++ i)
    {
        int         temp_int;

        switch(rec->data[i].type)
        {
        case VARIANT_TYPE_CHAR:
            temp_int = strlen(statement);
            statement[temp_int ++] = '\'';
            statement[temp_int ++] = rec->data[i].data.char_data;
            statement[temp_int ++] = '\'';
            statement[temp_int] = '\0';
            break;
        case VARIANT_TYPE_STRING:
            strcat(statement, "\'");
            strcat(statement, data_strings[i]);
            strcat(statement, "\'");
            break;
        }
        /* we do not need to add ',' for the last field */
        if(i < rec->number - 1)
            strcat(statement, ",");
    }
    strcat(statement, ")");

    
    if(sqlite3_exec(db_sqlite3_gv.handle, statement,
                NULL, NULL, NULL) != SQLITE_OK)
    {
        free(statement);
        return 3;
    }

    free(statement);

    for(i = 0; i < rec->number; ++ i)
        if(data_strings[i])
            free(data_strings[i]);

    free(data_strings);

    return 0;
}

    int
db_sqlite3_finalize(const OutputDbObject* odo)
{

    /* commit transaction */
    if(sqlite3_exec(db_sqlite3_gv.handle, "COMMIT",
                NULL, NULL, NULL) != SQLITE_OK)
    {
        sqlite3_close(db_sqlite3_gv.handle);
        return 1;
    }

    sqlite3_close(db_sqlite3_gv.handle);

    return 0;
}
