#ifndef TABLE_H
#define TABLE_H

#define NAME_LEN 20
#define VARCHAR_LEN 255

typedef enum column_type column_type;
typedef struct column_header column_header;
typedef struct column column;
typedef struct table_header table_header;
typedef struct table table;

#include "database.h"
#include "file_api.h"
#include <string.h>
#include <stdlib.h>

enum column_type {
    INT,
    VARCHAR,
    DOUBLE,
    BOOLEAN
};

struct column_header {
    column_type type;
    uint32_t size;
    char name[NAME_LEN];
};

struct table_header {
    uint32_t size;
    uint16_t row_size;
    uint16_t column_num;
    uint16_t name_len;
    char name[NAME_LEN];
    column_header columns[];
};

struct table {
    table_header *header;
    database *database;
    page *first_page;
    page *first_writable_page;
};

struct column {
    column_header *header;
    char *name;
};

table *new_table(const char *name, uint8_t num_columns);
table *open_table(database *db, const char* name);
void close_table(table *tb);
uint8_t type_to_column_size(column_type type);
void table_commit(database *db, table *table);
void init_fixed_column(table *tb, uint16_t index, const char *name, column_type type);
void init_var_column(table *tb, uint16_t index, const char *name, column_type type, uint32_t size);
column_header *get_column_header_by_name(table *tb, const char *name);

#endif
