#include "printer.h"
#define TAB_SIZE "  "
#define NUMBER_SIZE 30

void draw_tabs(char *buf, int tab) {
    for (int i = 0 ; i < tab ; i++) strcat(buf, TAB_SIZE);
}

void draw_branch(char *buf, int tab) {
    draw_tabs(buf, tab);
    strcat(buf, "│\n");
    draw_tabs(buf, tab);
    strcat(buf, "└ ");
}

void draw_double_branch(char *buf, int tab) {
    draw_tabs(buf, tab);
    strcat(buf, "├ ");
}

void to_string(char *buf, node *n, int tab) {
    char *tmp;
    if (n == NULL) {
        strcat(buf, "null");
        return;
    }
    switch (n->type) {
        case JOIN_QUERY:
            strcat(buf, node_type_string[JOIN_QUERY]);
            strcat(buf, "\n");

            draw_double_branch(buf, 0);
            strcat(buf, "left table name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_double_branch(buf, 0);
            strcat(buf, "right table name: ");
            strcat(buf, n->additional.name);
            strcat(buf, "\n");
            draw_double_branch(buf, 0);
            strcat(buf, "right table name: ");
            strcat(buf, n->main_second.name);
            strcat(buf, "\n");
            draw_double_branch(buf, 0);
            strcat(buf, "right entity name: ");
            strcat(buf, n->additional_second.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            n->right == NULL ? : to_string(buf, n->right, ++tab);
            n->center == NULL ? : to_string(buf, n->center, ++tab);
            break;

        case SELECT_QUERY:
            strcat(buf, node_type_string[SELECT_QUERY]);
            strcat(buf, "\n");

            draw_double_branch(buf, 0);
            strcat(buf, "table name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_double_branch(buf, 0);
            strcat(buf, "entity name: ");
            strcat(buf, n->additional.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            n->right == NULL ? : to_string(buf, n->right, ++tab);
            break;

        case DELETE_QUERY:
            strcat(buf, node_type_string[DELETE_QUERY]);
            strcat(buf, "\n");

            draw_double_branch(buf, 0);
            strcat(buf, "table name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_double_branch(buf, 0);
            strcat(buf, "entity name: ");
            strcat(buf, n->additional.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            break;

        case UPDATE_QUERY:
            strcat(buf, node_type_string[UPDATE_QUERY]);
            strcat(buf, "\n");

            draw_double_branch(buf, tab);
            strcat(buf, "table name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_double_branch(buf, tab);
            strcat(buf, "entity name: ");
            strcat(buf, n->additional.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            n->right == NULL ? : to_string(buf, n->right, ++tab);
            n->center == NULL ? : to_string(buf, n->center, ++tab);
            break;

        case INSERT_QUERY:
            strcat(buf, node_type_string[INSERT_QUERY]);
            strcat(buf, "\n");

            draw_double_branch(buf, tab);
            strcat(buf, "table name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            n->left == NULL ? : to_string(buf, n->left, ++tab);
            break;

        case CREATE_QUERY:
            strcat(buf, node_type_string[CREATE_QUERY]);
            strcat(buf, "\n");

            draw_double_branch(buf, tab);
            strcat(buf, "table name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            break;

        case DROP_QUERY:
            strcat(buf, node_type_string[DROP_QUERY]);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            strcat(buf, "table name: ");
            strcat(buf, n->main.name);
            break;

        case FILTER_CMP:
            strcat(buf, node_type_string[FILTER_CMP]);
            strcat(buf, "\n");
            draw_double_branch(buf, tab);
            strcat(buf, "cmp type: ");
            strcat(buf, compare_type_string[n->main.compare_type]);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            n->right == NULL ? : to_string(buf, n->right, ++tab);
            break;

        case FILTER_LOGIC:
            strcat(buf, node_type_string[FILTER_LOGIC]);
            strcat(buf, "\n");
            n->left == NULL ? : to_string(buf, n->left, ++tab);
            n->right == NULL ? : to_string(buf, n->right, ++tab);
            break;

        case CREATE_COLUMN:
            strcat(buf, node_type_string[CREATE_COLUMN]);

            strcat(buf, "\n");
            draw_double_branch(buf, tab);
            strcat(buf, "column name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            break;

        case CREATE_LIST:
            strcat(buf, node_type_string[CREATE_LIST]);
            strcat(buf, "\n");
            draw_branch(buf, tab);

            n->left == NULL ? : to_string(buf, n->left, ++tab);
            n->right == NULL ? : to_string(buf, n->right, ++tab);
            break;

        case OLD_COLUMN:
            strcat(buf, node_type_string[OLD_COLUMN]);
            strcat(buf, "\n");

            draw_double_branch(buf, tab);
            strcat(buf, "entity name: ");
            strcat(buf, n->main.name);
            strcat(buf, "\n");
            draw_double_branch(buf, tab);
            strcat(buf, "column name: ");
            strcat(buf, n->additional.name);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            break;

        case INT_NODE:
            strcat(buf, node_type_string[INT_NODE]);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            strcat(buf, "value: ");
            tmp = malloc(sizeof(char) * NUMBER_SIZE);
            sprintf(tmp, "%d", n->main.int_val);
            strcat(buf, tmp);
            strcat(buf, "\n");
            free(tmp);

            break;

        case VARCHAR_NODE:
            strcat(buf, node_type_string[VARCHAR_NODE]);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            strcat(buf, "value: ");
            strcat(buf, n->main.varchar_val);
            strcat(buf, "\n");

            break;

        case DOUBLE_NODE:
            strcat(buf, node_type_string[DOUBLE_NODE]);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            strcat(buf, "value: ");
            tmp = malloc(sizeof(char) * NUMBER_SIZE);
            sprintf(tmp, "%f", n->main.double_val);
            strcat(buf, tmp);
            strcat(buf, "\n");
            free(tmp);

            break;

        case BOOLEAN_NODE:
            strcat(buf, node_type_string[BOOLEAN_NODE]);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            strcat(buf, "value: ");
            strcat(buf, n->main.boolean_val == true ? "true" : "false");
            strcat(buf, "\n");

            break;

        case NAME:
            strcat(buf, node_type_string[NAME]);
            break;

        case TYPE_NODE:
            strcat(buf, node_type_string[TYPE_NODE]);
            strcat(buf, "\n");
            draw_branch(buf, tab);
            strcat(buf, "data type: ");
            strcat(buf, data_type_string[n->main.data_type]);
            strcat(buf, "\n");
            break;

    }
}

void print_tree(node *root) {
    if (root == NULL) {
        printf("root in null\n");
    } else {
        char *buf = malloc(sizeof(char) * 1000);
        strcat(buf, "\n");
        to_string(buf, root, 0);
        printf("%s\n", buf);
    }
}

