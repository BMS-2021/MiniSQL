#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include <string>

void interpret_entrance();

extern "C" int yywrap ( void );

int yylex();

int yyparse();

int yyerror(const char *s);

typedef struct yystype {
    int i;
    double r;
    std::string str;
    bool b;
} YYSTYPE;

#endif //MINISQL_INTERPRETER_H
