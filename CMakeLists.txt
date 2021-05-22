cmake_minimum_required(VERSION 3.0)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 20)


find_library(READLINE "readline")

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
if(READLINE)
    message(WARNING "Library \"readline\" not found. Install this library for better experience.")
    target_link_libraries(MiniSQL ${READLINE})
else()
    message(NOTICE "Library \"readline\" not found. Install this library for better experience.")
endif()
