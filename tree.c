#include "tree.h"
#include "printer.h"

node *new_select_query(const char *table, const char *entity, node *columns, node *filter) {
    node *n = malloc(sizeof(node));
    n->type = SELECT_QUERY;
    n->main.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, table);
    n->additional.name = malloc(sizeof(char) * 255);
    strcpy(n->additional.name, entity);
    n->left = columns;
    n->right = filter;
    n->center = NULL;
    return n;
}

node *new_join_query(const char *left_table_name, const char *left_entity_name, const char *right_table_name,
                     const char *right_entity_name, node *left_column, node *right_column, node *column_list) {
    node *n = malloc(sizeof(node));
    n->type = JOIN_QUERY;

    n->main.name = malloc(sizeof(char) * 255);
    n->additional.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, left_table_name);
    strcpy(n->additional.name, left_entity_name);

    n->main_second.name = malloc(sizeof(char) * 255);
    n->additional_second.name = malloc(sizeof(char) * 255);
    strcpy(n->main_second.name, right_table_name);
    strcpy(n->additional_second.name, right_entity_name);

    n->left = left_column;
    n->right = right_column;
    n->center = column_list;
    return n;
}

node *new_delete_query(const char *table_name, const char *entity_name, node *filter_statement) {
    node *n = malloc(sizeof(node));
    n->type = DELETE_QUERY;
    n->main.name = malloc(sizeof(char) * 255);
    n->additional.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, table_name);
    strcpy(n->additional.name, entity_name);
    n->left = filter_statement;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_update_query(const char *table_name, const char *entity_name, node *column_list, node *filter_statement,
                       node *value_list) {
    node *n = malloc(sizeof(node));
    n->type = UPDATE_QUERY;
    n->main.name = malloc(sizeof(char) * 255);
    n->additional.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, table_name);
    strcpy(n->additional.name, entity_name);
    n->left = column_list;
    n->right = filter_statement;
    n->center = value_list;
    return n;
}

node *new_insert_query(const char *table_name, node *value_list) {
    node *n = malloc(sizeof(node));
    n->type = INSERT_QUERY;
    n->main.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, table_name);
    n->left = value_list;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_create_query(const char *table_name, node *initialize_column_list) {
    node *n = malloc(sizeof(node));
    n->type = CREATE_QUERY;
    n->main.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, table_name);
    n->left = initialize_column_list;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_drop_query(const char *table_name) {
    node *n = malloc(sizeof(node));
    n->type = DROP_QUERY;
    n->main.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, table_name);
    n->left = NULL;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_list(node *first_part, node *second_part) {
    node *n = malloc(sizeof(node));
    n->type = CREATE_LIST;
    n->left = first_part;
    n->right = second_part;
    n->center = NULL;
    return n;
}

node *old_column(const char* entity_name, const char* column_name) {
    node *n = malloc(sizeof(node));
    n->type = OLD_COLUMN;
    n->main.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, entity_name);
    n->additional.name = malloc(sizeof(char) * 255);
    strcpy(n->additional.name, column_name);
    n->left = NULL;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_column(const char* column_name, node *type) {
    node *n = malloc(sizeof(node));
    n->type = CREATE_COLUMN;
    n->main.name = malloc(sizeof(char) * 255);
    strcpy(n->main.name, column_name);
    n->left = type;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_type(enum data_type type) {
    node *n = malloc(sizeof(node));
    n->type = TYPE_NODE;
    n->main.data_type = type;
    n->left = NULL;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_varchar(const char *varchar_val) {
    node *n = malloc(sizeof(node));
    n->type = VARCHAR_NODE;
    n->main.varchar_val = malloc(sizeof(char) * 255);
    strcpy(n->main.varchar_val, varchar_val);
    n->left = NULL;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_int(int int_val) {
    struct node *n = malloc(sizeof(node));
    n->type = INT_NODE;
    n->main.int_val = int_val;
    n->left = NULL;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_double(double double_val) {
    node *n = malloc(sizeof(node));
    n->type = DOUBLE_NODE;
    n->main.double_val = double_val;
    n->left = NULL;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_boolean(bool boolean_val) {
    node *n = malloc(sizeof(node));
    n->type = BOOLEAN_NODE;
    n->main.boolean_val = boolean_val;
    n->left = NULL;
    n->right = NULL;
    n->center = NULL;
    return n;
}

node *new_filter_compare_statement(enum compare_type cmp_type, node *first_operand, node *second_operand) {
    node *n = malloc(sizeof(node));
    n->type = FILTER_CMP;
    n->main.compare_type = cmp_type;
    n->left = first_operand;
    n->right = second_operand;
    n->center = NULL;
    return n;
}

node *new_filter_logic_statement(enum logic_operator logic_op, node *first_operand, node *second_operand) {
    node *n = malloc(sizeof(node));
    n->type = FILTER_LOGIC;
    n->main.logic_operator = logic_op;
    n->left = first_operand;
    n->right = second_operand;
    n->center = NULL;
    return n;
}
