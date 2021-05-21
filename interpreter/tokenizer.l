%{
#include <string>
#include "utils.h"
#include "interpreter.h"
#include "parser.tab.hh"
%}


%%

[ \t\n]		;

"'"         return S_APOSTROPGE;
";"         return S_SEMICOLON;
"("         return S_L_BRACKETS;
")"         return S_R_BRACKETS;
","         return S_COMMA;

[A-Za-z0-9_.]*  { return get_token(yylval.str = std::string(yytext)); }

%%

int yywrap()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
    { BEGIN INITIAL; }
    return 1;
}
