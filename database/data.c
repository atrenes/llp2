#include "data.h"

void close_data(data* dt) {
    free(dt->header);
    free(dt->byte_data);
    free(dt);
}

data* init_data(table* tb) {
    data* dt = malloc(sizeof(data));
    data_header *header = malloc(sizeof(data_header));
    header->valid = true;
    header->next_invalid_row_page = -1;
    header->next_invalid_row_index = -1;
    dt->table = tb;
    dt->byte_data = malloc(tb->header->row_size);
    memcpy(dt->byte_data, header, sizeof(data_header));
    dt->ptr = sizeof(data_header);
    dt->header = header;
    return dt;
}

void init_int(data* dt, int32_t val, char* col_name) {
    column_header *header = get_column_header_by_name(dt->table, col_name);
    char* ptr = (char*) dt->byte_data + dt->ptr;
    *((int32_t*) (ptr)) = val;
    dt->ptr += header->size;
}

void init_varchar(data* dt, char* val, char* col_name) {
    column_header *header = get_column_header_by_name(dt->table, col_name);
    char* ptr = (char*) dt->byte_data + dt->ptr;
    strcpy((char *) (ptr), val);
    dt->ptr += header->size;
}

void init_boolean(data* dt, bool val, char* col_name) {
    column_header *header = get_column_header_by_name(dt->table, col_name);
    char* ptr = (char*) dt->byte_data + dt->ptr;
    *((bool*) (ptr)) = val;
    dt->ptr += header->size;
}

void init_double(data* dt, double val, char* col_name) {
    column_header *header = get_column_header_by_name(dt->table, col_name);
    char* ptr = (char*) dt->byte_data + dt->ptr;
    *((double*) (ptr)) = val;
    dt->ptr += header->size;
}

void init_any(data* dt, column_type type, void* value, char* col_name) {
    switch (type) {
        case INT:
            init_int(dt, *((int32_t*) value), col_name);
            break;
        case VARCHAR:
            init_varchar(dt, *((char**) value), col_name);
            break;
        case BOOLEAN:
            init_boolean(dt, *((bool*) value), col_name);
            break;
        case DOUBLE:
            init_double(dt, *((double*) value), col_name);
            break;
        default:
            return;
    }

}

bool insert_data(data *data, database *db) {
    bool success = false;
    page *pg = data->table->first_writable_page;
    uint32_t offset = data->table->header->row_size * pg->header->num_rows;
    memcpy(pg->byte_data + offset, data->byte_data, data->ptr);
    pg->header->num_rows++;
    pg->header->free_row_offset += pg->table_header->row_size;
    if (!has_space(pg)) {
        pg->header->is_free = false;
        data->table->first_writable_page = page_alloc(db);
        pg->header->next_page_index = data->table->first_writable_page->header->page_index;
        table_to_page_link(data->table->first_writable_page, data->table);
        if (!pg->next_page) {
            pg->next_page = data->table->first_writable_page;
        }
        if (pg != data->table->first_page) {
            if (db) {
                success = write_page(db->fp, pg);
                write_page(db->fp, data->table->first_writable_page);
                close_page(pg);
            }
            close_data(data);
            return success;
        }
    }
    success = write_page(db->fp, pg);
    close_data(data);
    return success;
}
