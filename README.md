# MiniSQL

This is `MiniSQL`, the final project of *Database System Concept* course.

In this project, we are required to implement a simple DBMS, which contains some basic features, like creating, dropping and selecting table, inserting and deleting rows, creating and dropping indexes which is implemented by a B+ tree, and providing persistent storage. There are three data types that must be supported: `INT`, `FLOAT` and `CHAR()`. `UNIQUE` and `PRIMARY KEY` needs to be correctly declared in the DDL, and processed properly. The querying condition could be concatenated using `AND`, and common operators in the condition expression must be supported. External `.sql` files are required to be able to execute by `EXECFILE` statement.  

The project contains six modules: *Interpreter*, *API*, *Catalog Manager*, *Record Manager*, *Buffer Manager*, and *Index Manager*.  
