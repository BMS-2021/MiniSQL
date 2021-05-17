%{
#include "interpreter.h"
#include "parser.tab.hh"
%}

letter      [A-Za-z]
digit       [0-9]
num         {digit}+
s_digit     [+\-]?{digit}
s_num       [+\-]?{num}

%%

[0-9]+         return NUMBER;
heat           return TOKHEAT;
on|off         return STATE;
target         return TOKTARGET;
temperature    return TOKTEMPERATURE;
\n             /* ignore end of line */;
[ \t]+         /* ignore whitespace */

%%
