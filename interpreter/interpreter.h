#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include <string>
#include <cstring>
#include <vector>

#include "../macro.h"

const int SQL_QUERY_LENGTH = 200;

void interpret_entrance();

extern "C" int yywrap ( void );

int yylex();

int yyparse();

int yyerror(const char *);

int parse(const char *);

typedef struct yystype {
    bool b;
    std::string str;
    std::vector<sql_value> insert_list;
    std::vector<std::string> attribute_list;
    std::vector<condition> condition_list;
    condition condition_item;
    attribute_operator op;
    std::vector<schema> schema_list;
    schema schema_item;
    sql_value_type type;
    sql_value v;

} YYSTYPE;

class interpreter {
    bool first_loop = true;
    char *str = nullptr;

    char* start_text() const {
        if (first_loop) {
            return const_cast<char *>("MiniSQL> ");
        } else {
            return const_cast<char *>("      -> ");
        }
    }

public:
    inline interpreter() {
        str = reinterpret_cast<char*>(std::malloc(SQL_QUERY_LENGTH));
        str[0] = '\0';
    }
    inline ~interpreter() {
        if (str != nullptr) {
            std::free(str);
        }
        str = nullptr;
    }

    const char* read();
};

#endif //MINISQL_INTERPRETER_H
