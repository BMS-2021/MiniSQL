cmake_minimum_required(VERSION 3.0)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 20)

set(INTERPRETER
        interpreter/interpreter.h
        interpreter/interpreter.cpp
        interpreter/utils.cpp
        interpreter/utils.h
)

set(API
        api/api.h
        api/api.cpp
)

set(EXCEPTION
        utils/exception.cpp
        utils/exception.h
)

set(DEPRECATED_INTERPRETER
        interpreter/lex.yy.cc
        interpreter/parser.tab.hh
        interpreter/parser.tab.cc
        utils/exception.cpp utils/exception.h
)

set(CACHE_MANAGER
        Base/Base.h
        CacheManager/CacheManager.cpp
        CacheManager/CacheManager.h
)

set(CATALOG_MANAGER
        CatalogManager/CatalogManager.cpp
        CatalogManager/CatalogManager.h
)

find_package(BISON)
find_package(FLEX)
#[[
if(BISON_FOUND AND FLEX_FOUND)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/interpreter)
    BISON_TARGET(MyParser interpreter/parser.y ${CMAKE_CURRENT_BINARY_DIR}/interpreter/parser.tab.cc)
    FLEX_TARGET(MyScanner interpreter/tokenizer.l ${CMAKE_CURRENT_BINARY_DIR}/interpreter/lex.yy.cc)
    ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
    include_directories("interpreter")
    add_executable(MiniSQL macro.h main.cpp ${INTERPRETER} ${API} ${EXCEPTION} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS} ${CACHE_MANAGER})
else()
    message(WARNING "GNU Bison or Flex not found. \
                     This program will use pre-generated files instead... (deprecated)")
                     ]]
    add_executable(MiniSQL macro.h main.cpp ${INTERPRETER} ${API} ${EXCEPTION} ${DEPRECATED_INTERPRETER} ${CATALOG_MANAGER} ${CACHE_MANAGER} utils/utils.cpp utils/utils.h)
# endif()

#[[
find_library(LIB_READLINE "readline")
if(LIB_READLINE)
    message(STATUS "Found READLINE: ${LIB_READLINE}")
    add_definitions(-DREADLINE_FOUND)
    target_link_libraries(MiniSQL ${LIB_READLINE})
else()
    message(WARNING "Library \"readline\" not found. \
            This program will use C++ standard library instead... (not recommended)")
endif()
]]