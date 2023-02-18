#ifndef FILE_API_H
#define FILE_API_H

#define PAGE_SIZE 4096

typedef struct page page;
typedef struct page_header page_header;

#include "table.h"
#include "database.h"

struct page {
    page_header *header;
    table_header *table_header;
    page *next_page;
    void* byte_data; // intentional asterisk next to type
};

struct page_header {
    bool is_free;
    uint64_t page_index;
    uint64_t next_page_index;
    uint32_t num_rows;
    uint32_t free_row_offset;
};

database *init_database(FILE *fp, bool overwrite);
void close_database(database *db);
bool write_database_header(FILE *fp, database_header *db_header);
page *page_alloc(database *db);
page *read_page(database *db, uint64_t page_index);
page *get_first_free_page(database *db, table *t);
bool write_page(FILE *fp, page *pg);
bool has_space(page *pg);
void close_page(page *pg);
void table_to_page_link(page *pg, table *t);

#endif
