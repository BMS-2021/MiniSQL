cmake_minimum_required(VERSION 3.0)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 20)

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

find_library(LIB_READLINE "readline")

if(LIB_READLINE)
    add_definitions(-DREADLINE_FOUND)
    target_link_libraries(MiniSQL ${LIB_READLINE})
else()
    message(NOTICE "Library \"readline\" not found. Install this library for better command line experience.")
endif()
