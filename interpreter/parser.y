%{
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "interpreter.h"
#include "../api/api.h"
#include "../utils/exception.h"

extern std::unique_ptr<sql_exception> parse_exception;
extern std::unique_ptr<api::base> query_object_ptr;

bool bFlag; /* no meanings. */

extern int yylineno;
extern char *yytext;
inline int yyerror(const char *s)
{
    // std::cerr << s << " on token " << yytext << std::endl;
    parse_exception = std::make_unique<sql_exception>(100, "interpreter", std::string(s) + " near \'" + std::string(yytext) + "\' at line " + std::to_string(yylineno));
    yywrap();
    return 1;
}

%}

%token
K_DATABASE K_TABLE K_INDEX K_VALUES
K_FROM K_WHERE K_ON K_INTO K_AND
K_CREATE K_SELECT K_INSERT K_DROP
K_USE K_EXIT K_PRIMARY K_KEY K_UNIQUE K_EXECFILE
T_INT T_FLOAT T_CHAR
S_APOSTROPHE S_SEMICOLON S_L_BRACKETS S_R_BRACKETS S_COMMA S_STAR
S_EQUAL S_NOT_EQUAL S_GREATER S_GREATER_EQUAL S_LESS S_LESS_EQUAL

%token <str> V_STRING
%type <v> V_INSERT
%type <b> E_UNIQUE
%type <attribute_list> E_ATTRIBUTE_LIST
%type <condition_list> E_WHERE
%type <condition_list> E_CONDITION_LIST
%type <condition_item> E_CONDITION
%type <op> E_OPERATOR
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
    | I_EXECFILE
    | I_VACANT
    ;

C_DDL: I_CREATE_TABLE
    | I_DROP_TABLE
    | I_CREATE_INDEX
    | I_DROP_INDEX
    ;

C_DML: I_INSERT_TABLE
    | I_SELECT_TABLE
    ;

I_EXIT: K_EXIT
    {
        query_object_ptr = std::make_unique<api::exit>();
        // query_object_ptr = operation;
    }
    ;

I_USE_DATABASE: K_USE K_DATABASE V_STRING
    {
        query_object_ptr = std::make_unique<api::use_database>($3);
        // query_object_ptr = operation;
    }
    ;

I_CREATE_TABLE: K_CREATE K_TABLE V_STRING S_L_BRACKETS E_SCHEMA_LIST E_PRIMARY_KEY S_R_BRACKETS
    {
        query_object_ptr = std::make_unique<api::create_table>($3, $5, $6);
        // query_object_ptr = operation;
    }
    ;


I_DROP_TABLE: K_DROP K_TABLE V_STRING
    {
        query_object_ptr = std::make_unique<api::drop_table>($3);
        // query_object_ptr = operation;
    }
    ;

I_CREATE_INDEX: K_CREATE K_INDEX V_STRING K_ON V_STRING S_L_BRACKETS V_STRING S_R_BRACKETS
    {
        query_object_ptr = std::make_unique<api::create_index>($3, $5, $7);
    }
    ;

I_DROP_INDEX: K_DROP K_INDEX V_STRING
    {
        query_object_ptr = std::make_unique<api::drop_index>($3);
    }
    ;

I_INSERT_TABLE: K_INSERT K_INTO V_STRING K_VALUES S_L_BRACKETS E_INSERT_LIST S_R_BRACKETS
    {
        query_object_ptr = std::make_unique<api::insert_table>($3, $6);
    }
    ;

I_SELECT_TABLE: K_SELECT E_ATTRIBUTE_LIST K_FROM V_STRING E_WHERE
    {
        query_object_ptr = std::make_unique<api::select_table>($2, $4, $5);
    }
    ;

I_EXECFILE: K_EXECFILE S_APOSTROPHE V_STRING S_APOSTROPHE
    {
        query_object_ptr = std::make_unique<api::execfile>($3);
    }
    ;

I_VACANT: E_VACANT
    {
        query_object_ptr = std::make_unique<api::base>();
    }

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
        $$ = std::vector<sql_value>();
        $$.push_back($1);
    }
    ;

E_ATTRIBUTE_LIST: S_STAR
    {
        $$ = std::vector<std::string>();
    }
    | E_ATTRIBUTE_LIST S_COMMA V_STRING
    {
        $$.push_back($3);
    }
    | V_STRING
    {
        $$ = std::vector<std::string>();
        $$.push_back($1);
    }
    ;

E_WHERE: K_WHERE E_CONDITION_LIST
    {
        $$ = $2;
    }
    | E_VACANT
    {
        $$ = std::vector<condition>();
    }
    ;

E_CONDITION_LIST: E_CONDITION_LIST K_AND E_CONDITION
    {
        $$ = $1;
        $$.push_back($3);
    }
    | E_CONDITION
    {
        $$ = std::vector<condition>();
        $$.push_back($1);
    }
    ;

E_CONDITION: V_STRING E_OPERATOR V_INSERT
    {
        $$.attribute_name = $1;
        $$.op = $2;
        $$.value = $3;
    }

E_OPERATOR: 
      S_EQUAL {$$ = attribute_operator::EQUAL;}
    | S_NOT_EQUAL {$$ = attribute_operator::NOT_EQUAL;}
    | S_GREATER {$$ = attribute_operator::GREATER;}
    | S_GREATER_EQUAL {$$ = attribute_operator::GREATER_EQUAL;}
    | S_LESS {$$ = attribute_operator::LESS;}
    | S_LESS_EQUAL {$$ = attribute_operator::LESS_EQUAL;}


V_INSERT: S_APOSTROPHE V_STRING S_APOSTROPHE
    {
        $$ = $2;
    }
    | V_STRING
    {
        if ($1.find('.') != std::string::npos) {
            $$ = sql_value(std::stof($1));
        }
        else {
            try {
                $$ = sql_value(std::stoi($1));
            } catch (std::exception) {
                $$ = sql_value($1);
            }
        }
    }
    ;

E_VACANT:
    ;

%%
