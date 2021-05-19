cmake_minimum_required(VERSION 3.0)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-fstandalone-debug" )

set(INTERPRETER
        interpreter/interpreter.h
        interpreter/lex.yy.cc
        interpreter/parser.tab.hh
        interpreter/parser.tab.cc
        interpreter/model.cpp
        interpreter/model.h
        interpreter/interpreter.cpp interpreter/utils.cpp interpreter/utils.h
)

set(API
        api/api.h
        api/api.cpp
)

add_executable(MiniSQL macro.h main.cpp ${INTERPRETER} ${API})