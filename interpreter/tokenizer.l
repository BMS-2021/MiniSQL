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

";"  return S_SEMICOLON;

([A-Za-z0-9]|_)*  { return get_token(yylval.str = std::string(yytext)); }

%%

inline int yywrap()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
    { BEGIN INITIAL; }
    return 1;
}
