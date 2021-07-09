%{
#include <string>
#include <iostream>
#include "interpreter.h"
#include "parser.tab.hh"
%}


%%

[ \t\r\n]		    ;

"*"                 return S_STAR;
"'"                 return S_APOSTROPHE;
";"                 return S_SEMICOLON;
"("                 return S_L_BRACKETS;
")"                 return S_R_BRACKETS;
","                 return S_COMMA;

"="                 return S_EQUAL;
"<>"                return S_NOT_EQUAL;
">"                 return S_GREATER;
">="                return S_GREATER_EQUAL;
"<"                 return S_LESS;
"<="                return S_LESS_EQUAL;

database|DATABASE   return K_DATABASE;
table|TABLE         return K_TABLE;
index|INDEX         return K_INDEX;
values|VALUES       return K_VALUES;

from|FROM           return K_FROM;
where|WHERE         return K_WHERE;
on|ON               return K_ON;
into|INTO           return K_INTO;
and|AND             return K_AND;

create|CREATE       return K_CREATE;
select|SELECT       return K_SELECT;
insert|INSERT       return K_INSERT;
delete|DELETE       return K_DELETE;
drop|DROP           return K_DROP;

use|USE             return K_USE;
exit|EXIT           return K_EXIT;
quit|QUIT           return K_QUIT;
primary|PRIMARY     return K_PRIMARY;
key|KEY             return K_KEY;
unique|UNIQUE       return K_UNIQUE;
execfile|EXECFILE   return K_EXECFILE;

int|INT             return T_INT;
float|FLOAT         return T_FLOAT;
char|CHAR           return T_CHAR;

[A-Za-z0-9_.]*  {
    yylval.str = std::string(yytext);
    return V_STRING;
}

%%

int yywrap()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
    { BEGIN INITIAL; }
    return 1;
}

int parse(const char *input) {
    YY_BUFFER_STATE bp = yy_scan_string(input);
    if (bp == nullptr) {
    	std::cerr << "error on creating YY_BUFFER_STATE" << std::endl;
    	return -1;
    }
    yy_switch_to_buffer(bp);
    yyparse();
    yy_delete_buffer(bp);
    yylex_destroy();
    return 0;
}
