# MiniSQL

This is `MiniSQL`, the final project of *Database System Concept* course.

In this project, we are required to implement a simple DBMS, which contains some basic features, like creating, dropping and selecting table, inserting and deleting rows, creating and dropping indexes which is implemented by a B+ tree, and providing persistent storage. There are three data types that must be supported: `INT`, `FLOAT` and `CHAR()`. `UNIQUE` and `PRIMARY KEY` needs to be correctly declared in the DDL, and processed properly. The querying condition could be concatenated using `AND`, and common operators in the condition expression must be supported. External `.sql` files are required to be able to execute by `EXECFILE` statement.  

The project contains six modules: *Interpreter*, *API*, *Catalog Manager*, *Record Manager*, *Buffer Manager*, and *Index Manager*.  
In this chapter, you will see a brief introduction of each module.

## Interpreter

The interpreter is the entrance of every SQL query loop. It gets the user input, passes the user input to lexer and parser, and gets the AST (abstract syntax tree), which stores in a C++ object.  

| Procedure | Input | Output | Used Tools |
| :-------: | :-------: | :-----: | :-----: |
| read user input | (command line input) | a null-terminated byte string | GNU Readline |
| lexer | a null-terminated byte string | a null-terminated byte string with tokens tagged onto it | Flex |
| parser | a null-terminated byte string with tokens tagged onto it | an AST in the C++ object | GNU Bison |

`Flex` and `Bison` has their own syntax. `Flex` constructs relations between string and token, while `Bison` matches the SQL statements to syntaxes, which are defined recursively in `Bison` source code file.  
`Flex` and `Bison` needs to be translated to C++ before the compilation progress starts. We write `CMake` rules to check if there exists the required tools and libraries, and will trigger an error if some necessary dependency cannot be found. We chose `Cmake` because it is a cross-platform building solution.  

Since this is a command-line program, various command-line features are implemented using `GNU Readline`, like moving cursor through the line, pressing `TAB` button to auto-complete a keyword, and pressing up or down button to go through the input history.  

## Catalog Manager

The *Catalog Manager* is responsible for the meta-data in our DBMS. It mainly maintains the schema which contains definitions of tables, columns with attribute names, indexes and primary keys. It provides *API* module with interface that can deal with the access to the meta-data above.

The module exposes the following public methods:

- CreateTable  
  To create a schema of table, the function receives a table name, list of attributes with their types and a primary key name if possible, then it will create the catalog of the table in memory.

- Flush  
  This function is responsible for the persistence of meta info of tables, which contains the number of tables altogether, the name and record count of each table, the attributes with names, whether unique or primary info and indexes with index names.

- LoadFromFile  
  Read in schema info from external storage with the same format into the memory, this function is called during the initialization stage of the whole system.

- TableExist  
  To check whether the table with a certain name exists in the memory.

- GetTable  
  Search in memory to find the specific table and returns its reference.

- DropTableByName  
  Drop a table in memory according to its name.

- CreateIndex  
  Create an index on specific attribute with index name.

- DropIndex  
  Drop an index of an attribute according to the index name.

In order to reduce the coupling between the modules, the Catalog module uses the form of direct access to disk files without going through the Buffer Manager.
