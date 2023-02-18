#include "iterator.h"

iterator *init_iterator(database *db, table *tb) {
    iterator* iter = malloc(sizeof(iterator));
    iter->tb = tb;
    iter->db = db;
    iter->current_page = tb->first_page;
    iter->ptr = 0;
    iter->rows_read_on_page = 0;
    return iter;
}

int32_t get_offset_to_column_data(iterator *iter, const char* col_name, column_type type) {
    int32_t offset_to_column_data = sizeof(data_header);
    bool match = false;
    for (int i = 0; i < iter->tb->header->column_num; i++) {
        column_header cur_column = iter->tb->header->columns[i];
        if (cur_column.type == type && strcmp(col_name, cur_column.name) == 0) {
            match = true;
            break;
        }
        offset_to_column_data += cur_column.size;
    }
    if (!match) return -1;
    return offset_to_column_data;
}

bool has_next(iterator *iter) {
    return
    iter->ptr + iter->tb->header->row_size <= PAGE_SIZE - sizeof(table_header) - sizeof(page_header)
    &&
    iter->rows_read_on_page < iter->current_page->header->num_rows;
}

bool seek_next(iterator *iter) {
    data_header header = {
            false
    };
    uint32_t next_page_num = iter->current_page->header->next_page_index;
    while (has_next(iter) || next_page_num != 0) {
        do {
            if (iter->rows_read_on_page > 0) {
                iter->ptr += iter->tb->header->row_size;
            }
            header = get_data_header(iter);
            iter->rows_read_on_page += 1;
        } while (has_next(iter) && !header.valid);

        if (header.valid) {
            return true;
        }

        if (iter->current_page != iter->tb->first_page && iter->current_page != iter->tb->first_writable_page) {
            close_page(iter->current_page);
        }
        if (next_page_num == 0) {
            break;
        }
        iter->current_page = read_page(iter->db, next_page_num);
        next_page_num = iter->current_page->header->next_page_index;
        iter->rows_read_on_page = 0;
        iter->ptr = 0;
    }
    return false;
}

bool seek_next_where(iterator *iter, const char *col_name, column_type type, const void* val) {
    data_header header = {
            false
    };

    int32_t col_int;
    char* col_char;
    bool col_bool;
    double col_double;
    uint32_t next_page_index = iter->current_page->header->next_page_index;
    while (has_next(iter) || next_page_index != 0) {
        do {
            if (iter->rows_read_on_page > 0) {
                iter->ptr += iter->tb->header->row_size;
            }
            header = get_data_header(iter);
            iter->rows_read_on_page += 1;
        } while (has_next(iter) && !header.valid);

        if (header.valid) {
            switch (type) {
                case INT:
                    get_int(iter, col_name, &col_int);
                    if (col_int == *((int32_t *) val)) {
                        return true;
                    }
                    continue;
                case VARCHAR:
                    col_char = malloc(sizeof(char) * VARCHAR_LEN);
                    get_varchar(iter, col_name, &col_char);
                    if (strcmp(col_char, *((char**) val)) == 0) {
                        free(col_char);
                        return true;
                    }
                    free(col_char);
                    continue;
                case BOOLEAN:
                    get_bool(iter, col_name, &col_bool);
                    if (col_bool == *((bool *) val)) {
                        return true;
                    }
                    continue;
                case DOUBLE:
                    get_double(iter, col_name, &col_double);
                    if (col_double == *((double *) val)) {
                        return true;
                    }
                    continue;
                default:
                    break;
            }
        }

        if (iter->current_page != iter->tb->first_page && iter->current_page != iter->tb->first_writable_page) {
            close_page(iter->current_page);
        }
        if (next_page_index == 0) {
            break;
        }
        iter->current_page = read_page(iter->db, next_page_index);
        next_page_index = iter->current_page->header->next_page_index;
        iter->rows_read_on_page = 0;
        iter->ptr = 0;
    }
    return false;
}

bool get_any(iterator *iter, const char *col_name, void* dest) {
    for (int i = 0; i < iter->tb->header->column_num; i++) {
        column_header header = iter->tb->header->columns[i];
        if (strcmp(col_name, header.name) == 0) {
            switch (header.type) {
                case INT:
                    return get_int(iter, col_name, dest);
                case VARCHAR:
                    return get_varchar(iter, col_name, (char **) &dest);
                case BOOLEAN:
                    return get_bool(iter, col_name, dest);
                case DOUBLE:
                    return get_double(iter, col_name, dest);
                default:
                    return false;
            }
        }
    }
    return false;
}


uint32_t delete_where(database *db, table *tb, const char *col_name, column_type type, const void* val) {
    iterator *iter = init_iterator(db, tb);
    uint32_t deleted_objs = 0;

    while (seek_next_where(iter, col_name, type, val)) {
        page* pg = iter->current_page;
        void* page_bytes = pg->byte_data;
        data_header header = get_data_header(iter);
        header.valid = false;
        memcpy(page_bytes + iter->ptr, &header, sizeof(data_header));
        write_page(db->fp, pg);
        deleted_objs++;
    }
    free(iter);
    return deleted_objs;
}

uint32_t update_where(database *db, table *tb,
                      const char *where_col_name, column_type where_col_type, const void* where_val,
                      const char *update_col_name, column_type update_col_type, const void* update_val) {
    iterator *iter = init_iterator(db, tb);
    uint32_t updated_objs = 0;
    while (seek_next_where(iter, where_col_name, where_col_type, where_val)) {
        page* pg = iter->current_page;
        void* page_bytes = pg->byte_data;
        int32_t col_data_offset = get_offset_to_column_data(iter, update_col_name, update_col_type);
        column_header *header = get_column_header_by_name(iter->tb, update_col_name);
        memcpy(page_bytes + iter->ptr + col_data_offset, update_val, header->size);
        write_page(db->fp, pg);
        updated_objs++;
    }
    free(iter);
    return updated_objs;
}

data_header get_data_header(iterator *iter) {
    return *((data_header *)((char*) iter->current_page->byte_data + iter->ptr));
}

bool get_int(iterator *iter, const char *col_name, int32_t *dest) {
    int32_t offset_to_column_data = get_offset_to_column_data(iter, col_name, INT);
    if (offset_to_column_data == -1) return false;
    *dest = *((int32_t*)((char*) iter->current_page->byte_data + iter->ptr + offset_to_column_data));
    return true;
}

bool get_varchar(iterator *iter, const char *col_name, char **dest) {
    int32_t offset_to_column_data = get_offset_to_column_data(iter, col_name, VARCHAR);
    if (offset_to_column_data == -1) return false;
    strcpy(*dest, (char*) iter->current_page->byte_data + iter->ptr + offset_to_column_data);
    return true;
}

bool get_bool(iterator *iter, const char *col_name, bool *dest) {
    int32_t offset_to_column_data = get_offset_to_column_data(iter, col_name, BOOLEAN);
    if (offset_to_column_data == -1) return false;
    *dest = *((bool*)((char*) iter->current_page->byte_data + iter->ptr + offset_to_column_data));
    return true;
}

bool get_double(iterator *iter, const char *col_name, double *dest) {
    int32_t offset_to_column_data = get_offset_to_column_data(iter, col_name, DOUBLE);
    if (offset_to_column_data == -1) return false;
    *dest = *((double *)((char*) iter->current_page->byte_data + iter->ptr + offset_to_column_data));
    return true;
}

void print_join_table(database *db, table *tb1, table *tb2, const char *on_col1, const char *on_col2, column_type type) {
    iterator *iter1 = init_iterator(db, tb1);
    iterator *iter2 = init_iterator(db, tb2);
    while (seek_next(iter1)) {
        column_header * on_col_t1_header = get_column_header_by_name(tb1, on_col1);
        void* value = malloc(on_col_t1_header->size);
        get_any(iter1, on_col1, value);
        while (seek_next_where(iter2, on_col2, type, value)) {
            for (int i = 0; i < iter1->tb->header->column_num; i++) {
                column_header col_header = iter1->tb->header->columns[i];
                void* cur_col_val = malloc(col_header.size);
                get_any(iter1, col_header.name, cur_col_val);
                switch (col_header.type) {
                    case INT:
                        printf("\t %d \t", *((int32_t*) cur_col_val));
                        break;
                    case VARCHAR:
                        printf("\t%s\t", (char*) cur_col_val);
                        break;
                    case BOOLEAN:
                        printf("\t%d\t", *((bool*) cur_col_val));
                        break;
                    case DOUBLE:
                        printf("\t%f\t", *((float*) cur_col_val));
                        break;
                }
            }

            for (int i = 0; i < iter2->tb->header->column_num; i++) {
                column_header col_header = iter2->tb->header->columns[i];
                void* cur_col_val = malloc(col_header.size);
                get_any(iter2, col_header.name, cur_col_val);
                switch (col_header.type) {
                    case INT:
                        printf("\t%d\t", *((int32_t*) cur_col_val));
                        break;
                    case VARCHAR:
                        printf("\t%s\t", (char*) cur_col_val);
                        break;
                    case BOOLEAN:
                        printf("\t%d\t", *((bool*) cur_col_val));
                        break;
                    case DOUBLE:
                        printf("\t%f\t", *((float*) cur_col_val));
                        break;
                }
            }

            printf("\n");
        }
        iter2 = init_iterator(db, tb2);
    }
    free(iter1);
    free(iter2);
}
