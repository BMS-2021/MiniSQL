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

[A-Za-z]([A-Za-z0-9]|_)*  { return get_token(std::string(yytext)); }

%%
