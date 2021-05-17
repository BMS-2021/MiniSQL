cmake_minimum_required(VERSION 3.0)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 20)

set(INTERPRETER main.cpp
        interpreter/interpreter.h
        interpreter/lex.yy.cc
        interpreter/parser.tab.hh
        interpreter/parser.tab.cc
        interpreter/model.cpp
        interpreter/model.h
        interpreter/interpreter.cpp interpreter/utils.cpp interpreter/utils.h)

add_executable(MiniSQL macro.h main.cpp ${INTERPRETER})