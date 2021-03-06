cmake_minimum_required(VERSION 3.0)
project(minisql)

set(CMAKE_CXX_STANDARD 20)

set(INTERPRETER
        interpreter/interpreter.h
        interpreter/interpreter.cpp
        interpreter/readline.c)

set(API
        api/api.h
        api/api.cpp
        api/utils.cpp
        api/utils.h
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

set(CATALOG_MANAGER
        CatalogManager/CatalogManager.h
        CatalogManager/CatalogManager.cpp
)

set(BUFFER_MANAGER
        BufferManager/BufferManager.h
        BufferManager/BufferManager.cpp
)

set(RECORD_MANAGER
        RecordManager/RecordManager.h
        RecordManager/RecordManager.cpp
)

set(INDEX_MANAGER
        IndexManager/BPInclude.h
        IndexManager/BPTree.h
        IndexManager/BPTreeNode.h
        IndexManager/BPTree.cpp
        IndexManager/IndexManager.h
        IndexManager/IndexManager.cpp
        IndexManager/BPTreeNode.cpp)

# add_definitions(-DDETACH_INDEX_MANAGER)

find_package(BISON 3.0)
find_package(FLEX)
if(BISON_FOUND AND FLEX_FOUND)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/interpreter)
    BISON_TARGET(MyParser interpreter/parser.y ${CMAKE_CURRENT_BINARY_DIR}/interpreter/parser.tab.cc)
    FLEX_TARGET(MyScanner interpreter/tokenizer.l ${CMAKE_CURRENT_BINARY_DIR}/interpreter/lex.yy.cc)
    ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
    include_directories(interpreter)
    add_library(${PROJECT_NAME} STATIC macro.h external_main/external_main.cpp
            ${INTERPRETER} ${API} ${EXCEPTION}
            ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS}
            ${BUFFER_MANAGER} ${RECORD_MANAGER} ${CATALOG_MANAGER} ${INDEX_MANAGER}
            utils/utils.cpp utils/utils.h)
    add_executable(${PROJECT_NAME}_local main.cpp)
    target_link_libraries(${PROJECT_NAME}_local ${PROJECT_NAME})
else()
    message(FATAL_ERROR "GNU Bison(version >= 3.0) or Flex not found.")
endif()

file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/go-svc/main.go
        ${CMAKE_CURRENT_SOURCE_DIR}/go-svc/placeholder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/go.mod
        ${CMAKE_CURRENT_SOURCE_DIR}/go.sum
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/go-svc
    )
file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/external_main/external_main.h
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/external_main
)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/go-svc
        COMMAND go build -o ../${PROJECT_NAME}_region
        COMMENT "Building go source files"
)

# if(LIB_READLINE)
#     message(STATUS "Found READLINE: ${LIB_READLINE}")
#     add_definitions(-DREADLINE_FOUND)
#     target_link_libraries(MiniSQL ${LIB_READLINE})
# else()
#     message(WARNING "Library \"readline\" not found. \
#             This program will use C++ standard library instead... (not recommended)")
# endif()
