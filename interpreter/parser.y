%{
#include <iostream>
#include <string>
#include <vector>

#include "interpreter.h"
#include "model.h"

extern query::base *query_object_ptr;

bool bFlag; /* no meanings. */

extern int yylineno;
extern char *yytext;
inline int yyerror(const char *s)
{
    std::cerr << s << std::endl;
    std::cerr << "line: " << yylineno << " on token " << yytext << std::endl;
    yywrap();
    return 1;
}

%}

%token
K_DATABASE K_TABLE
K_CREATE K_SELECT K_UPDATE K_DROP
K_USE K_EXIT K_PRIMARY K_KEY
T_INT T_FLOAT T_CHAR
S_SEMICOLON S_L_BRACKETS S_R_BRACKETS S_COMMA

%token <str> V_STRING
%type <schema_list> E_SCHEMA_LIST
%type <schema> E_SCHEMA
%type <type> E_TYPE
%type <str> E_PRIMARY_KEY

%%

C_TOP_INPUT: C_TOP_STMT S_SEMICOLON
    {
        YYACCEPT;
    }
    ;

C_TOP_STMT: C_DDL
    | I_EXIT
    | I_USE_DATABASE
    ;

C_DDL: I_CREATE_TABLE
    | I_DROP_TABLE
    ;

I_EXIT: K_EXIT
    {
        auto operation = new query::exit();
        query_object_ptr = operation;
    }
    ;

I_USE_DATABASE: K_USE K_DATABASE V_STRING
    {
        auto operation = new query::use_database($3);
        query_object_ptr = operation;
    }
    ;

I_CREATE_TABLE: K_CREATE K_TABLE V_STRING S_L_BRACKETS E_SCHEMA_LIST E_PRIMARY_KEY S_R_BRACKETS
    {
        auto operation = new query::create_table($3, $5, $6);
        query_object_ptr = operation;
    }
    ;


I_DROP_TABLE: K_DROP K_TABLE V_STRING
    {
        auto operation = new query::drop_table($3);
        query_object_ptr = operation;
    }
    ;

/******************************************************************************/

E_SCHEMA_LIST: E_SCHEMA_LIST S_COMMA E_SCHEMA
    {
        $$ = $1;
        $$.push_back($3);
    }
    | E_SCHEMA
    {
        $$ = std::vector<std::pair<std::string, sql_value_type>>();
        $$.push_back($1);
    }
    ;

E_SCHEMA: V_STRING E_TYPE
    {
        $$ = std::make_pair($1, $2);
    }
    ;

E_TYPE: T_INT
    {
        $$ = sql_value_type(value_type::INT);
    }
    | T_FLOAT
    {
        $$ = sql_value_type(value_type::FLOAT);
    }
    | T_CHAR S_L_BRACKETS V_STRING S_R_BRACKETS
    {
        $$ = sql_value_type(std::stoi($3));  // FIXME: exception
    }
    ;

E_PRIMARY_KEY: S_COMMA K_PRIMARY K_KEY S_L_BRACKETS V_STRING S_R_BRACKETS
    {
        $$ = $5;
    }
    | E_VACANT
    {
        $$ = "";
    }
    ;

E_VACANT:
    ;

%%
