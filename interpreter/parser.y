%{
#include <iostream>

#include "interpreter.h"
#include "model.h"

extern query::base *query_object_ptr;

int yywrap()
{
    return  1;
}

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

%token <str> T_STRING

%%

top_input: top_stmt
    {
        YYACCEPT;
    }
    ;

top_stmt: drop_table
    ;

drop_table: K_DROP K_TABLE T_STRING
    {
        auto drop_table = new query::drop_table();
        drop_table->table_name = $3;

        query_object_ptr = drop_table;
    }
    ;

%%
