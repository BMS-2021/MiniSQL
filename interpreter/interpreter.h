#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include <string>
#include <vector>

#include "../macro.h"

void interpret_entrance();

extern "C" int yywrap ( void );

int yylex();

int yyparse();

int yyerror(const char *s);

typedef struct yystype {
    std::string str;
    std::vector<std::pair<std::string, sql_value_type>> schema_list;
    std::pair<std::string, sql_value_type> schema;
    sql_value_type type;

} YYSTYPE;

#endif //MINISQL_INTERPRETER_H
