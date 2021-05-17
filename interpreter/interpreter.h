#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

extern "C" int yywrap ( void );

int yylex();

int yyparse();

int yyerror(const char *s);

#endif //MINISQL_INTERPRETER_H
