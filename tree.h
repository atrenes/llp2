#ifndef LLP2_TREE_H
#define LLP2_TREE_H

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

enum node_type {
    JOIN_QUERY,
    SELECT_QUERY,
    DELETE_QUERY,
    UPDATE_QUERY,
    INSERT_QUERY,
    CREATE_QUERY,
    DROP_QUERY,

    FILTER_CMP,
    FILTER_LOGIC,

    CREATE_COLUMN,
    CREATE_LIST,
    OLD_COLUMN,

    INT_NODE,
    VARCHAR_NODE,
    DOUBLE_NODE,
    BOOLEAN_NODE,

    NAME,
    TYPE_NODE
};

static const char *node_type_string[] = {
    "JOIN_QUERY",
    "SELECT_QUERY",
    "DELETE_QUERY",
    "UPDATE_QUERY",
    "INSERT_QUERY",
    "CREATE_QUERY",
    "DROP_QUERY",

    "FILTER_CMP",
    "FILTER_LOGIC",

    "CREATE_COLUMN",
    "CREATE_LIST",
    "OLD_COLUMN",

    "INT_NODE",
    "VARCHAR_NODE",
    "DOUBLE_NODE",
    "BOOLEAN_NODE",

    "NAME",
    "TYPE_NODE"
};

enum data_type {
    VARCHAR_DATA,
    INT_DATA,
    DOUBLE_DATA,
    BOOLEAN_DATA
};

static const char *data_type_string[] = {
    "VARCHAR",
    "INT",
    "DOUBLE",
    "BOOLEAN"
};

enum logic_operator {
    AND_OPERATOR,
    OR_OPERATOR
};

static const char *logic_operator_string[] = {
    "AND",
    "OR"
};

enum compare_type {
    SUBSTRING,
    GREATER,
    GREATER_OR_EQUALS,
    LESS,
    LESS_OR_EQUALS,
    STRICT_EQUALS,
    NOT_EQUALS
};

static const char *compare_type_string[] = {
    "SUBSTRING",
    "GREATER",
    "GREATER_OR_EQUALS",
    "LESS",
    "LESS_OR_EQUALS",
    "EQUALS",
    "NOT_EQUALS",
};

union content {
    char    *name;
    enum    data_type data_type;
    enum    logic_operator logic_operator;
    enum    compare_type compare_type;
    int     int_val;
    char    *varchar_val;
    double  double_val;
    bool    boolean_val;
};

typedef struct node node;

struct node {
    enum node_type type;
    union content main;
    union content additional;

    union content main_second;
    union content additional_second;

    node *left;
    node *right;
    node *center;
};

node *new_select_query(const char *table, const char *entity, node *columns, node *filter);
node *new_join_query(const char *left_table_name, const char *left_entity_name, const char *right_table_name,
                     const char *right_entity_name, node *left_column, node *right_column, node *column_list);
node *new_delete_query(const char *table_name, const char *entity_name, node *filter_statement);
node *new_update_query(const char *table_name, const char *entity_name, node *column_list, node *filter_statement,
                       node *value_list);
node *new_insert_query(const char *table_name, node *value_list);
node *new_create_query(const char *table_name, node *initialize_column_list);
node *new_drop_query(const char *table_name);
node *new_list(node *first_part, node *second_part);
node *old_column(const char* entity_name, const char* column_name);
node *new_column(const char* column_name, node *type);
node *new_type(enum data_type type);
node *new_varchar(const char *varchar_val);
node *new_int(int int_val);
node *new_double(double double_val);
node *new_boolean(bool boolean_val);
node *new_filter_compare_statement(enum compare_type cmp_type, node *first_operand, node *second_operand);
node *new_filter_logic_statement(enum logic_operator logic_op, node *first_operand, node *second_operand);

#endif //LLP2_TREE_H
