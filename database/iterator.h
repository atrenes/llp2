#ifndef LLP1_ITERATOR_H
#define LLP1_ITERATOR_H

typedef struct iterator iterator;

#include "database.h"
#include "data.h"

struct iterator {
    database *db;
    table *tb;
    page *current_page;
    uint16_t ptr;
    uint16_t rows_read_on_page;
};

iterator *init_iterator(database *db, table *tb);
int32_t get_offset_to_column_data(iterator *iter, const char* col_name, column_type type);
bool has_next(iterator *iter);
bool seek_next(iterator *iter);
bool seek_next_where(iterator *iter, const char *col_name, column_type type, const void* val);
uint32_t delete_where(database *db, table *tb, const char *col_name, column_type type, const void* val);
uint32_t update_where(database *db, table *tb,
                     const char *where_col_name, column_type where_col_type, const void* where_val,
                     const char *update_col_name, column_type update_col_type, const void* update_val);
data_header get_data_header(iterator *iter);
bool get_int(iterator *iter, const char *col_name, int32_t *dest);
bool get_varchar(iterator *iter, const char *col_name, char **dest);
bool get_bool(iterator *iter, const char *col_name, bool *dest);
bool get_double(iterator *iter, const char *col_name, double *dest);
void print_join_table(database *db, table *tb1, table *tb2, const char *on_col1, const char *on_col2, column_type type);


#endif //LLP1_ITERATOR_H
