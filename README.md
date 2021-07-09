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

## API

*API* is the separator of the front-end part and the back-end part of these project. The front-end part only contains the *Interpreter*, which eventually creates aN AST OBJECT. *API* handles these AST objects by calling the corresponding functions.  
*API* contains a base class. The name of the base class is literally `api::base`, which is almost a virtual class. Every SQL statement extends from this base class, and implements the `exec` function of the base class. This function is responsible for calling other modules in the back-end. For an example, `api::create_table::exec` needs to call *Catalog Manger* to and the meta-data of the table into the schema, then calls *Record Manager* to create an record file of this table, then may call *Index Manager* to create a B+ tree for the index on primary key. The only back-end module that will not be called directly by *API* is *Buffer Manager*, which is managed by *Record Manager*.  
*API* also manages the query result, the query duration, the exception, and needs to print the result of `SELECT` statement into a table properly. Everything related to direct interaction with the user are not related to any module below *API*.

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


## Buffer Manager

*Buffer Manager* constructs and manages buffers in memory. Other modules, like *Record Manager* and *Index Manager*, asks for the data from *Buffer Manager* instead of directly getting them from the file system. It smooth the low speed of disk I/O and dramatically improve the performance of database.  
Each file is divided into many blocks, and the size of each block is 4K. While the program is running, *Buffer Manager* would read some blocks into memory when needed and offer them to other modules.

Main functions and simple introduction are as follows:

- Get block  
  Place a specified block in buffer. It read data from file system in case of missing block in memory and use the LRU algorithm as the block-replacement-strategy. If the block is not exist, buffer manager will allocate a new block automatically.

- Set dirty  
  Set the dirty-bit in block, which indicates the data in block have been modified and it should write back first before replacing.

- Unlock  
  Unlock a block, allowing it to be replaced by other block.

- Create file  
  Create a file in file system, which is initially blank

- Remove file  
  Remove a file from file system

- Get block count  
  Get the number of total blocks of a specified file.

- Close all file  
  Save all data and close all files, called when quitting the process


## Record Manager

*Record Manager* manages records in database. It supports three major operations about records: insertion, selection and deletion. Also, it offer interfaces to *API* and *Index Manager*.

Main functions and simple introduction are as follows:

- Create Table  
  Create data file via *Buffer Manager*

- Drop Table  
  Remove data file via *Buffer Manager*

- Insert Record  
  Insert a record. Records are stored in data file block by block. Records in the same table will be convert to a fixed-length and won't be stored across the block.

- Delete Record  
  Delete records with a batch of conditions.  When doing deletion, we traverse the whole table and delete the records which satisfy the conditions. This operation can be optimized in *Index Manager* using B+ Tree.

- Select Record  
  Select record with a batch of conditions. Similar to deletion, we traverse the whole table and select the records which satisfy the conditions. This operation can be optimized in *Index Manager* using B+ Tree. Not support for selecting part columns.

- Get Record  
  Get a record by its position in data file.

- Get Record Pairs  
  Return all records paired with their position in data file.


## Index Manager

As what is just mentioned in its name, the module *Index Manager* is a class that works for the management of the index.

The index manager supports the following functions, which are listed as public member methods:

- Create  
  The create method supports two types of index creation:
    1. Create an empty index on a column of table, for example when called at create table
    2. Create an index on existing values

- Drop  
  Remove an index from index manager

- Insert  
  Insert a single element to a certain index

- Search  
  Search for a single element in a certain index, the return value is position of the target row in table (in int)

- Remove  
  Remove a single element from a certain index

- Save  
  Save all the indexes to local index file, should be called when quitting the process

- Load  
  Load the indexes from local file to memory, should be called when the process starts

Each of the index is individually managed by a customize B+ tree, and we use an unordered_map (the only private member in the class) to relate the index name with its content.

In order to save the space in B+ tree storage, the keys stored in B+ tree is actually the row number. And we use an external function provided by *Record Manager* to get the real value for comparing when operating the B+ tree.

The B+ tree currently has three main functions: insert, delete and search. Some other values and method are marked public, but they are *unused* and *left for future accomplishment*.
