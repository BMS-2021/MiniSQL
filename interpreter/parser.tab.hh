/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_TAB_HH_INCLUDED
# define YY_YY_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    K_DATABASE = 258,
    K_TABLE = 259,
    K_INDEX = 260,
    K_VALUES = 261,
    K_FROM = 262,
    K_WHERE = 263,
    K_ON = 264,
    K_INTO = 265,
    K_AND = 266,
    K_CREATE = 267,
    K_SELECT = 268,
    K_INSERT = 269,
    K_DELETE = 270,
    K_DROP = 271,
    K_USE = 272,
    K_EXIT = 273,
    K_QUIT = 274,
    K_PRIMARY = 275,
    K_KEY = 276,
    K_UNIQUE = 277,
    K_EXECFILE = 278,
    T_INT = 279,
    T_FLOAT = 280,
    T_CHAR = 281,
    S_APOSTROPHE = 282,
    S_SEMICOLON = 283,
    S_L_BRACKETS = 284,
    S_R_BRACKETS = 285,
    S_COMMA = 286,
    S_STAR = 287,
    S_EQUAL = 288,
    S_NOT_EQUAL = 289,
    S_GREATER = 290,
    S_GREATER_EQUAL = 291,
    S_LESS = 292,
    S_LESS_EQUAL = 293,
    V_STRING = 294
  };
#endif

/* Value type.  */


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_HH_INCLUDED  */
