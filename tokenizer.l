%option noyywrap caseless yylineno nounput noinput batch

%{
    #include <stdio.h>
    #include <string.h>
    #include "parser.tab.h"
    #include "tree.h"

    void print_tokenizer_error(char*);
%}

alpha   [A-Za-z_0-9 \t\r]
id      [a-zA-Z][a-zA-Z_0-9]*
int     ([-+]?[0-9])+
double  [+-]?([0-9]*[.])?[0-9]+
blank   [ \t\r]
word    ([a-zA-Z_][a-zA-Z0-9_]*)
quoted_string \"{word}*\"

%%

"SELECT"    {return SELECT;}
"JOIN"      {return JOIN;}
"DELETE"    {return DELETE;}
"UPDATE"    {return UPDATE;}
"INSERT"    {return INSERT;}
"CREATE"    {return CREATE;}
"DROP"      {return DROP;}

"FROM"      {return FROM;}
"IN"        {return IN;}
"WHERE"     {return WHERE;}
"ON"        {return ON;}
"EQUALS"    {return EQUALS;}
"SET"       {return SET;}

"&&"        {yylval.logic_operator = 0; return AND;}
"||"        {yylval.logic_operator = 1; return OR;}

"TRUE"      {yylval.boolean_value = 1; return BOOLEAN;}
"FALSE"     {yylval.boolean_value = 0; return BOOLEAN;}

"CONTAINS"  {yylval.compare_type = 0; return CMP;}
">"         {yylval.compare_type = 1; return CMP;}
">="        {yylval.compare_type = 2; return CMP;}
"<"         {yylval.compare_type = 3; return CMP;}
"<="        {yylval.compare_type = 4; return CMP;}
"=="        {yylval.compare_type = 5; return CMP;}
"!="        {yylval.compare_type = 6; return CMP;}

"VARCHAR"   {yylval.type = 0; return TYPE;}
"INT"       {yylval.type = 1; return TYPE;}
"DOUBLE"    {yylval.type = 2; return TYPE;}
"BOOLEAN"   {yylval.type = 3; return TYPE;}

"("         {return LB;}
")"         {return RB;}
","         {return COMMA;}
"."         {return DOT;}
";"         {return ENDQUERY;}
"\""        {return QUOTE;}

{word}      {
    sscanf(yytext, "%s", yylval.varchar_value);
    return (VARCHAR);
}
{int}       {
    yylval.int_value = atoi(yytext);
    return (INT);
}
{double}     {
    yylval.double_value = atof(yytext);
    return (DOUBLE);
}

[ \t]   { /* ignore */ }
[\n]    {}
.       {
    print_tokenizer_error(yytext);
    return (OTHER);
}
%%

void print_tokenizer_error(char* token) {
    printf("Error in tokenizer while reading token = %s \n", token);
}
