# CatalogManager

The `CatalogManager` is responsible for the metadata in our DBMS. It mainly maintains the schema which contains definitions of tables, columns with attribute names, indexes and primary keys. It provides API module with interface that can deal with the access to the metadata above.

The module exposes the following public methods:

- `CreateTable`
  To create a schema of table, the function receives a table name,  list of attributes with their types and a primary key name if possible, then it will create the catalog of the table in memory.
  
- `Flush`
  This function is responsible for the persistence of meta info of tables, which contains the number of tables altogether, the name and record count of each table, the attributes with names, whether unique or primary info and indexes with index names.
  
- `LoadFromFile`

  Read in schema info from external storage with the same format into the memory, this function is called during the initialization stage of the whole system.

- `TableExist`
To check whether the table with a certain name exists in the memory.

- `GetTable`
Search in memory to find the specific table and returns its reference.

- `DropTableByName`
Drop a table in memory according to its name.

- `CreateIndex`
Create an index on specific attribute with index name. 

- `DropIndex`
Drop an index of an attribute according to the index name.

In order to reduce the coupling between the modules, the Catalog module uses the form of direct access to disk files without going through the Buffer Manager.