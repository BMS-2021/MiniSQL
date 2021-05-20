%{
#include <iostream>

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
S_SEMICOLON

%token <str> T_STRING

%%

C_TOP_INPUT: C_TOP_STMT S_SEMICOLON
    {
        YYACCEPT;
    }
    ;

C_TOP_STMT: E_DROP_TABLE
    ;

E_DROP_TABLE: K_DROP K_TABLE T_STRING
    {
        auto drop_table = new query::drop_table($3);
        query_object_ptr = drop_table;
    }
    ;

%%
