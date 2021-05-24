cmake_minimum_required(VERSION 3.0)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 20)

set(INTERPRETER
        interpreter/interpreter.h
        interpreter/model.cpp
        interpreter/model.h
        interpreter/interpreter.cpp
        interpreter/utils.cpp
        interpreter/utils.h
)

set(API
        api/api.h
        api/api.cpp
)

set(DEPRECATED_INTERPRETER
        interpreter/lex.yy.cc
        interpreter/parser.tab.hh
        interpreter/parser.tab.cc
)

find_package(BISON)
find_package(FLEX)
if(BISON_FOUND AND FLEX_FOUND)
    message(STATUS "GNU Bison found")
    message(STATUS "Flex found")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/interpreter)
    BISON_TARGET(MyParser interpreter/parser.y ${CMAKE_CURRENT_BINARY_DIR}/interpreter/parser.tab.cc)
    FLEX_TARGET(MyScanner interpreter/tokenizer.l ${CMAKE_CURRENT_BINARY_DIR}/interpreter/lex.yy.cc)
    ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
    include_directories("interpreter")
    add_executable(MiniSQL macro.h main.cpp ${INTERPRETER} ${API} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})
else()
    message(WARNING "GNU Bison or Flex not found. \
                     This program will use pre-generated files instead... (deprecated)")
    add_executable(MiniSQL macro.h main.cpp ${INTERPRETER} ${API} ${DEPRECATED_INTERPRETER})
endif()

find_library(LIB_READLINE "readline")

if(LIB_READLINE)
    add_definitions(-DREADLINE_FOUND)
    target_link_libraries(MiniSQL ${LIB_READLINE})
else()
    message(WARNING "Library \"readline\" not found. \
            This program will use C++ standard library instead... (not recommended)")
endif()
