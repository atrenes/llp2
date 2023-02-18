#include "table.h"
#include "data.h"

table *open_table(database *db, const char* name) {
    page* cur_page = read_page(db, 0);
    int count = 0;
    while (strcmp(cur_page->table_header->name, name) != 0 && count < db->header->num_pages) {
        close_page(cur_page);
        cur_page = read_page(db, ++count);
    }
    table* tb = malloc(sizeof(table));
    tb->header = cur_page->table_header;
    tb->first_page = cur_page;
    tb->database = db;
    tb->first_writable_page = get_first_free_page(db, tb);
    return tb;

}

void close_table(table *tb) {
    close_page(tb->first_writable_page);
    if (tb->first_writable_page != tb->first_page) {
        close_page(tb->first_page);
    }
    free(tb->header);
    free(tb);
}

uint8_t type_to_column_size(column_type type) {
    switch (type) {
        case INT:
            return sizeof(int32_t);
        case BOOLEAN:
            return sizeof(bool);
        case VARCHAR:
            return sizeof(char) * VARCHAR_LEN;
        case DOUBLE:
            return sizeof(double);
        default:
            return 0;
    }
}

column_header *get_column_header_by_name(table *tb, const char *name) {
    for (int i = 0 ; i < tb->header->column_num ; i++) {
        column_header *current = &tb->header->columns[i];
        if (strcmp(name, current->name) == 0) {
            return current;
        }
    }
    return NULL;
}

table *new_table(const char *name, uint8_t num_columns) {
    table* tb = malloc(sizeof (table));

    table_header *header = malloc(sizeof (table_header) + num_columns * sizeof (column_header));
    header->column_num = num_columns;
    header->name_len = strlen(name);
    header->row_size = sizeof(data_header);
    header->size = sizeof(table_header) + num_columns * sizeof(column_header);

    if (header->name_len > NAME_LEN) {
        printf("Name length exceeded");
        return NULL;
    }

    for (int i = 0; i < header->name_len; i++) {
        header->name[i] = name[i];
    }
    header->name[header->name_len] = '\0';

    tb->header = header;

    return tb;

}

void table_commit(database* db, table* table) {
    page* pg = page_alloc(db);
    table_to_page_link(pg, table);
    table->first_page = pg;
    table->first_writable_page = pg;
    table->database = db;
    if (db) {
        write_page(db->fp, pg);
    }
}

void init_fixed_column(table *tb, uint16_t index, const char *name, column_type type) {
    column_header header = {type, type_to_column_size(type)};
    tb->header->columns[index] = header;
    tb->header->row_size += header.size;
    int len = -1;
    while (name[++len] != '\0') tb->header->columns[index].name[len] = name[len];
    tb->header->columns[index].name[len] = '\0';

}

void init_var_column(table *tb, uint16_t index, const char *name, column_type type, uint32_t size) {
    column_header header = {type, size};
    tb->header->columns[index] = header;
    tb->header->row_size += header.size;
    int len = -1;
    while (name[++len] != '\0') tb->header->columns[index].name[len] = name[len];
    tb->header->columns[index].name[len] = '\0';
}
