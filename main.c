#include <stdio.h>
#include "database/file_api.h"
#include "database/table.h"
#include "database/data.h"
#include "database/database.h"
#include "database/iterator.h"
#include <time.h>

int main() {
    char* filename = "data_basis.txt";
    FILE* fp = fopen(filename, "wb+");
    if (!fp) {
        printf("Unable to open fp.");
        exit(EXIT_FAILURE);
    }
    database* db = init_database(fp, true);

    printf("Some works needs to be done...\n");

    close_database(db);
    fclose(fp);
    return 0;
}