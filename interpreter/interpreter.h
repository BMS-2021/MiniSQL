#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include <string>
#include <vector>
#include <variant>

#include "../macro.h"

void interpret_entrance();

extern "C" int yywrap ( void );

int yylex();

int yyparse();

int yyerror(const char *s);

typedef struct yystype {
    bool b;
    std::string str;
    std::vector<std::variant<int, float, std::string>> insert_list;
    std::vector<std::string> attribute_list;
    std::vector<condition> condition_list;
    condition condition_item;
    attribute_operator op;
    std::vector<schema> schema_list;
    schema schema_item;
    sql_value_type type;
    std::variant<int, float, std::string> v;

} YYSTYPE;

#endif //MINISQL_INTERPRETER_H
