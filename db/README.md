# MiniSQL Database Engine

**Disclaimer: This project is the final project of a course, which can only be considered as a toy. It shows some undergraduates' ideas on how to build a proper database. Suggestions are welcomed.**

This is `MiniSQL Database Engine`, the final project of *Database System Concept* course.

Currently, the **HTTP server version** of this project, which is called *Region Server*, is a component of `MiniSQL cluster`, a distributed database cluster. This project also has a **standalone CLI version**, which will be discussed later in this document.  

![MiniSQL.png](https://i.loli.net/2021/07/11/quRsbFZpzL5r7gY.png)  

> This screen shot shows MiniSQL standalone version

In this project, we are required to implement a *toy* database engine, which contains some basic features, like creating, dropping and selecting table, inserting and deleting rows, creating and dropping indexes which is implemented by a B+ tree, and providing persistent storage. There are three data types that must be supported: 
`INT`, `FLOAT` and `CHAR()`. `UNIQUE` and `PRIMARY KEY` needs to be correctly declared in the DDL, and processed properly. The querying condition could be concatenated using `AND`, and common operators in the condition expression must be supported. External `.sql` files are required to be able to execute by `EXECFILE` statement.  

## Infrastructure

![MiniSQL.png](https://raw.githubusercontent.com/RalXYZ/repo-pictures/47c8cad9a82fa1a3ce59af6799a0c03ddffd4954/MiniSQL/minisql.svg)  

The project contains six modules: *Interpreter*, *API*, *Catalog Manager*, *Record Manager*, *Buffer Manager*, and *Index Manager*.
A golang HTTP server is provided to serve the database, while you can also choose to use the CLI version of this database.

## Build

Firstly, make sure you've installed *CMake*, *Bison* and *Flex*.  
Golang should also be installed, which version must be higher than 1.17.
Then, in the project director, run the following command:  

```shell
cmake .
make
```

Then, two executables, one called `MiniSQL` and another called `minisql_local` will appear in the project directory.  
- `minisql_region` is expected to be executed on the server, which exposes HTTP API. Since it is a component of MiniSQL cluster, it is started by master node.  
- `minisql_local` is the standalone version, which has a CLI interface.

If you haven't installed *GNU Readline* library, the program will fallback, using `std::cin` to read input lines, 
which will affect the using experience.  

## Usage

Start MiniSQL local by the following command:  
```sh
./minisql_local
```

After the executable has been executed, press `TAB`, then you will see all valid keywords. 
Pressing `TAB` can also auto-complete the keywords.  

These are some examples of the supported SQL statements:  
```sql
CREATE TABLE foo (id INT, bar FLOAT, buzz CHAR(255), PRIMARY KEY id);  -- valid length of CHAR is between 1 and 255
CREATE TABLE foo (id INT);  -- a table can have no primary key
CREATE INDEX bar_idx ON foo (bar);
SELECT * FROM foo;  -- brute-force select
SELECT bar, id FROM foo WHERE buzz <> 'bee' AND bar > 3.14;
INSERT INTO foo VALUES (4, 2.71, 'bomb');
DELETE FROM foo;  -- delete all
DELETE FROM foo WHERE id = 5;
DROP INDEX bar_idx;
DROP TABLE foo;
EXECFILE your_sql_script.sql;
QUIT;
```
