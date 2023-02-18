#ifndef LLP1_DATA_H
#define LLP1_DATA_H

/*
 * Implementation of functions used for INSERT operation(s)
 */

typedef struct data data;
typedef struct data_header data_header;

#include "table.h"

struct __attribute__((packed)) data_header {
    bool valid;
    int32_t next_invalid_row_page;
    int32_t next_invalid_row_index;
};

struct data {
    data_header *header;
    table *table;
    void** byte_data; //intentional double asterisk next to type
    uint16_t ptr;
};

data* init_data(table *tb);
void init_int(data *dt, int32_t val, char *col_name);
void init_varchar(data *dt, char *val, char *col_name);
void init_boolean(data *dt, bool val, char *col_name);
void init_double(data *dt, double val, char *col_name);
void init_any(data *dt, column_type type, void *value, char *col_name);
bool insert_data(data *data, database * db);

#endif //LLP1_DATA_H
