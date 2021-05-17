cmake_minimum_required(VERSION 3.0)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 20)

set(INTERPRETER main.cpp
        interpreter/interpreter.h
        interpreter/proto.h
        interpreter/proto.cpp
        interpreter/lex.yy.cc
        interpreter/parser.tab.hh
        interpreter/parser.tab.cc
)

add_executable(MiniSQL main.cpp ${INTERPRETER})