%{
#include <string>
#include <iostream>
#include "utils.h"
#include "interpreter.h"
#include "parser.tab.hh"
%}


%%

[ \t\r\n]		;

"*"         return S_STAR;
"'"         return S_APOSTROPHE;
";"         return S_SEMICOLON;
"("         return S_L_BRACKETS;
")"         return S_R_BRACKETS;
","         return S_COMMA;

"="         return S_EQUAL;
"<>"        return S_NOT_EQUAL;
">"         return S_GREATER;
">="        return S_GREATER_EQUAL;
"<"         return S_LESS;
"<="        return S_LESS_EQUAL;

[A-Za-z0-9_.]*  { return get_token(yylval.str = std::string(yytext)); }

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
