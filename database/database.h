#ifndef DATABASE_H
#define DATABASE_H

/*
 * Only structs here.
 * All functions related to database manipulation are inside of file_api
 */

typedef struct database database;
typedef struct database_header database_header;

#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <malloc.h>
#include "table.h"
#include "file_api.h"

struct database {
    FILE *fp;
    database_header *header;
};

struct database_header {
    uint64_t num_pages;
    uint64_t first_free_page_index;
    uint64_t single_page_size;
};

#endif
