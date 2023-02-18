#include "file_api.h"

bool write_database_header(FILE *fp, database_header *db_header) {
    fseek(fp, 0, SEEK_SET);
    if ( fwrite(db_header, sizeof(database_header), 1, fp) != 1 ) {
        return false;
    }

    return true;
}

bool read_database_header(FILE *fp, database_header *db_header) {
    fseek(fp, 0, SEEK_SET);
    size_t read = fread(db_header, sizeof(database_header), 1, fp);
    if (read != 1) {
        return false;
    }
    return true;
}

database *read_database(FILE *fp) {
    database *db = malloc(sizeof(database));
    database_header *header = malloc(sizeof(database_header));
    if (!read_database_header(fp, header)) {
        printf("Couldn't read database header\n");
        exit(1);
    }
    db->header = header;
    db->fp = fp;
    return db;
}

database *overwrite_database(FILE *fp) {
    database *db = malloc(sizeof(database));
    database_header *header = malloc(sizeof(database_header));
    header->single_page_size = PAGE_SIZE;
    header->num_pages = 0;
    header->first_free_page_index = 0;
    db->header = header;
    db->fp = fp;
    write_database_header(fp, header);

    return db;
}

database *init_database(FILE *fp, bool overwrite) {
    if (overwrite) {
        return overwrite_database(fp);
    } else {
        return read_database(fp);
    }
}

page *page_alloc(database *db) {
    page *pg = malloc(sizeof(page));
    page_header *header = malloc(sizeof(page_header));
    header->is_free = true;
    if (db) {
        header->page_index = db->header->num_pages++;
    } else {
        header->page_index = 0;
    }
    header->num_rows = 0;
    header->free_row_offset = 0;
    header->next_page_index = 0;
    pg->header = header;
    pg->table_header = NULL;
    pg->next_page = NULL;
    pg->byte_data = NULL;

    if (db) {
        write_database_header(db->fp, db->header);
    }

    return pg;
}

void close_page(page *pg) {
    free(pg->header);
    free(pg->byte_data);
    free(pg);
}

bool write_page(FILE *fp, page *pg) {
    fseek(fp, sizeof(database_header) + pg->header->page_index * PAGE_SIZE, SEEK_SET);
    size_t page_header_size = sizeof(page_header);
    size_t table_header_size;
    if (pg->table_header != NULL) {
        table_header_size = pg->table_header->size;
    } else {
        table_header_size = 0;
    }
    bool is_written = true;
    is_written = is_written & fwrite(pg->header, page_header_size, 1, fp);
    if (pg->table_header) {
        is_written = is_written && fwrite(pg->table_header, table_header_size, 1, fp);
    }
    if (pg->byte_data) {
        is_written = is_written && fwrite(pg->byte_data, PAGE_SIZE - table_header_size - page_header_size, 1, fp);
    }
    return is_written;

}

page *read_page(database *db, uint64_t page_index) {
    if (page_index >= db->header->num_pages) {
        printf("Page index out of bounds\n");
        return NULL;
    }
    page *pg = malloc(sizeof(page));
    FILE *file = db->fp;
    fseek(file, sizeof(database_header) + page_index * PAGE_SIZE, SEEK_SET);

    page_header *p_header = malloc(sizeof(page_header));
    size_t read_p_header = fread(p_header, sizeof(page_header), 1, file);
    if (read_p_header < 1) {
        printf("Couldn't read page_header\n");
    }
    uint32_t t_header_size;
    size_t read = fread(&t_header_size, sizeof(uint32_t), 1, file);
    table_header *t_header = malloc(t_header_size);
    fseek(file, -sizeof(uint32_t), SEEK_CUR);
    size_t read_t_header = fread(t_header, t_header_size, 1, file);
    if (read_t_header < 1) {
        printf("Couldn't read table_header\n");
    }
    pg->byte_data = malloc(PAGE_SIZE - t_header_size - sizeof(page_header));
    size_t num_bytes = fread(pg->byte_data, PAGE_SIZE - t_header_size - sizeof(page_header), 1, file);
    if (num_bytes < 1) {
        printf("Couldn't read bytes\n");
    }
    pg->header = p_header;
    pg->table_header = t_header;
    pg->next_page = NULL;

    return pg;
}

bool has_space(page *pg) {
    table_header *header = pg->table_header;
    return header->size + sizeof(page_header) + pg->header->num_rows * header->row_size <= PAGE_SIZE - header->row_size;
}

void table_to_page_link(page *pg, table *tb) {
    pg->table_header = tb->header;
    pg->byte_data = malloc(PAGE_SIZE - sizeof(page_header) - pg->table_header->size);
}

page *get_first_free_page(database *db, table *tb) {
    page *pg = tb->first_page;
    while (!pg->header->is_free) {
        uint64_t next_page_index = pg->header->next_page_index;
        if (pg->header->next_page_index == 0) {
            page *new_page = page_alloc(db);
            pg->header->next_page_index = new_page->header->page_index;
            table_to_page_link(new_page, tb);
            write_page(db->fp, pg);
            write_page(db->fp, new_page);
            return new_page;
        }
        if (pg != tb->first_page) {
            close_page(pg);
        }
        pg = read_page(db, next_page_index);
    }
    return pg;
}

void close_database(database* db) {
    write_database_header(db->fp, db->header);
    free(db->header);
}
