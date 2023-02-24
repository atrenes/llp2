%{
    #include "tree.h"
    #include "printer.h"

    #define YYERROR_VERBOSE 1

    extern int yylex();
    extern int yylineno;
    void yyerror(const char* s);
%}

%union {
    char varchar_value[255];
    int int_value;
    int boolean_value;
    double double_value;

    struct node *not_term; //typedef won't work :(

    int type;
    int compare_type;
    int logic_operator;
}


%token SELECT JOIN DELETE UPDATE INSERT CREATE DROP
%token FROM IN WHERE ON EQUALS SET
%token AND OR CMP
%token LB RB COMMA DOT QUOTE ENDQUERY
%token MY_EOF OTHER
%token TYPE

%token VARCHAR INT DOUBLE BOOLEAN


%type <varchar_value> VARCHAR
%type <int_value> INT
%type <double_value> DOUBLE
%type <boolean_value> BOOLEAN


%type <type> TYPE
%left <logic_operator> AND
%left <logic_operator> OR

%left <compare_type> CMP


%type <not_term> init

%type <not_term> query
%type <not_term> select_query
%type <not_term> join_query
%type <not_term> delete_query
%type <not_term> update_query
%type <not_term> insert_query
%type <not_term> create_query
%type <not_term> drop_query
%type <not_term> filter_statement

%type <not_term> column_list
%type <not_term> column_names
%type <not_term> column

%type <not_term> value
%type <not_term> value_list
%type <not_term> initialize_column_list

%type <not_term> create_column
%type <not_term> column_type

%type <varchar_value> table_name
%type <varchar_value> entity_name
%type <varchar_value> column_name

%%

init:
    query ENDQUERY { print_tree($1); }
;

query:
    select_query
|   join_query
|   delete_query
|   update_query
|   insert_query
|   create_query
|   drop_query
;

select_query:
    FROM entity_name IN table_name SELECT column_list { $$ = new_select_query($4, $2, $6, NULL); }
|   FROM entity_name IN table_name WHERE filter_statement SELECT column_list { $$ = new_select_query($4, $2, $8, $6); }
;

join_query:
    FROM entity_name IN table_name JOIN entity_name IN table_name ON column EQUALS column SELECT column_list { $$ = new_join_query($4, $2, $8, $6, $10, $12, $14); }
;

delete_query:
    FROM entity_name IN table_name DELETE entity_name { $$ = new_delete_query($4, $2, NULL); }
|   FROM entity_name IN table_name WHERE filter_statement DELETE entity_name { $$ = new_delete_query($4, $2, $6); }
;

update_query:
    FROM entity_name IN table_name UPDATE entity_name SET column_list EQUALS value_list { $$ = new_update_query($4, $2, $8, NULL, $10); }
|   FROM entity_name IN table_name WHERE filter_statement UPDATE entity_name SET column_list EQUALS value_list { $$ = new_update_query($4, $2, $10, $6, $12); }
;

insert_query:
    IN table_name INSERT value_list { $$ = new_insert_query($2, $4); }
;


create_query:
    CREATE table_name LB initialize_column_list RB { $$ = new_create_query($2, $4); }
;

drop_query:
    DROP table_name { $$ = new_drop_query($2); }
;

filter_statement:
    column CMP value { $$ = new_filter_compare_statement($2, $1, $3); }
|   column CMP column { $$ = new_filter_compare_statement($2, $1, $3); }
|   filter_statement AND filter_statement { $$ = new_filter_logic_statement($2, $1, $3); }
|   filter_statement OR filter_statement { $$ = new_filter_logic_statement($2, $1, $3); }
;

column_list:
    entity_name { $$ = new_list(NULL, NULL); }
|   column_names { $$ = $1; }
;

column_names:
    column { $$ = new_list($1, NULL); }
|   column_names COMMA column { $$ = new_list($1, $3); }
;

column:
    entity_name DOT column_name { $$ = old_column($1, $3); }
;

value_list:
    value { $$ = new_list($1, NULL); }
|   value_list COMMA value { $$ = new_list($1, $3); }
;

value:
    INT { $$ = new_int($1); }
|   BOOLEAN { $$ = new_boolean($1); }
|   DOUBLE { $$ = new_double($1); }
|   VARCHAR { $$ = new_varchar($1); }
;

initialize_column_list:
    create_column { $$ = new_list($1, NULL); }
|   initialize_column_list COMMA create_column { $$ = new_list($1, $3); }
;

create_column:
    column_name column_type { $$ = new_column($1, $2); }
;

column_type:
    TYPE { $$ = new_type($1); }
;

table_name:
    VARCHAR
;

entity_name:
    VARCHAR
;

column_name:
    VARCHAR
;

%%

void yyerror (char const *s) {
   fprintf (stderr, "%s on line number %d", s, yylineno);
 }

int main() {
    #if YYDEBUG == 1
    extern int yydebug;
    yydebug = 1;
    #endif
	printf("enter query:");
	yyparse();
	return 0;
}