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
    std::cerr << s << " on token " << yytext << std::endl;
    yywrap();
    return 1;
}

%}

%token
K_DATABASE K_TABLE K_INDEX K_VALUES
K_FROM K_WHERE K_ON K_INTO
K_CREATE K_SELECT K_INSERT K_DROP
K_USE K_EXIT K_PRIMARY K_KEY K_UNIQUE
T_INT T_FLOAT T_CHAR
S_APOSTROPGE S_SEMICOLON S_L_BRACKETS S_R_BRACKETS S_COMMA

%token <str> V_STRING
%type <v> V_INSERT
%type <b> E_UNIQUE
%type <insert_list> E_INSERT_LIST
%type <schema_list> E_SCHEMA_LIST
%type <schema_item> E_SCHEMA
%type <type> E_TYPE
%type <str> E_PRIMARY_KEY

%%

C_TOP_INPUT: C_TOP_STMT S_SEMICOLON
    {
        YYACCEPT;
    }
    ;

C_TOP_STMT: C_DDL
    | C_DML
    | I_EXIT
    | I_USE_DATABASE
    ;

C_DDL: I_CREATE_TABLE
    | I_DROP_TABLE
    | I_CREATE_INDEX
    | I_DROP_INDEX
    ;

C_DML: I_INSERT_TABLE
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

I_CREATE_INDEX: K_CREATE K_INDEX V_STRING K_ON V_STRING S_L_BRACKETS V_STRING S_R_BRACKETS
    {
        auto operation = new query::create_index($3, $5, $7);
        query_object_ptr = operation;
    }
    ;

I_DROP_INDEX: K_DROP K_INDEX V_STRING
    {
        auto operation = new query::drop_index($3);
        query_object_ptr = operation;
    }
    ;

I_INSERT_TABLE: K_INSERT K_INTO V_STRING K_VALUES S_L_BRACKETS E_INSERT_LIST S_R_BRACKETS
    {
        auto operation = new query::insert_table($3, $6);
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
        $$ = std::vector<schema>();
        $$.push_back($1);
    }
    ;

E_SCHEMA: V_STRING E_TYPE E_UNIQUE
    {
        $$ = schema($1, $2, $3);
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

E_UNIQUE: K_UNIQUE
    {
        $$ = true;
    }
    | E_VACANT
    {
        $$ = false;
    }
    ;

E_INSERT_LIST: E_INSERT_LIST S_COMMA V_INSERT
    {
        $$ = $1;
        $$.push_back($3);
    }
    | V_INSERT
    {
        $$ = std::vector<std::variant<int, float, std::string>>();
        $$.push_back($1);
    }
    ;

V_INSERT: S_APOSTROPGE V_STRING S_APOSTROPGE
    {
        $$ = $2;
    }
    | V_STRING
    {
        try {
            $$ = std::stoi($1);
        } catch (std::exception) {
            try {
                $$ = std::stof($1);
            } catch (std::exception) {
                $$ = $1;
            }
        }
    }
    ;

E_VACANT:
    ;

%%
