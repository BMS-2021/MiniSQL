%{
#include <iostream>

#include "interpreter.h"
#include "proto.h"

int yywrap()
{
    return  1;
}

int yyerror()
{
    return  1;
}

%}

%token  NUMBER TOKHEAT STATE TOKTARGET TOKTEMPERATURE

%%

commands: /* empty */
    | commands command
;

command: heat_switch
    | target_set
;

heat_switch:
    TOKHEAT STATE
    {
        std::cout << test(1) << std::endl;
        std::cout << "Heat turned on or off";
    }
;

target_set:
    TOKTARGET TOKTEMPERATURE NUMBER
    {
        std::cout << "Temperature set";
    }
;

%%

bool bFlag; /* no meanings. */

inline int yyerror(const char *s)
{
    //std::cerr << "error: ";
    std::cerr << s << std::endl;
    //std::cerr << "line: " << yylineno << " on token " << yytext << std::endl;
    yywrap();
    return 1;
}
