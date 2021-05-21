%{
#include <string>
#include "utils.h"
#include "interpreter.h"
#include "parser.tab.hh"
%}

num         {digit}+
s_digit     [+\-]?{digit}
s_num       [+\-]?{num}

%%

";"         return S_SEMICOLON;
"("         return S_L_BRACKETS;
")"         return S_R_BRACKETS;
","         return S_COMMA;

([A-Za-z0-9]|_)*  { return get_token(yylval.str = std::string(yytext)); }

%%

int yywrap()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
    { BEGIN INITIAL; }
    return 1;
}
