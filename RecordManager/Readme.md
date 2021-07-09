### Record Manager

`RecordManager `  manages records in database. It supports three major operations about records: insertion, selection and deletion. Also, it offer interfaces to `API` and `IndexManager`.

Main functions and simple introduction are as follow:

- Creat Table

  Creat data file via `BufferManager`

- Drop Table

  Remove data file via `BufferManager`

- Insert Record

  Insert a record. Records are stored in data file block by block. Records in the same table will be convert to a fixed-length and won't be stored across the block.

- Delete Record

  Delete records with a batch of conditions.  When doing deletion, we traverse the whole table and delete the records which satisfy the conditions. This opration can be optimized in `IndexManager` using B+ Tree.

- Select Record

  Select record with a batch of conditions. Similar to deletion, we traverse the whole table and select the records which satisfy the conditions. This opration can be optimized in `IndexManager` using B+ Tree. Not support for selecting part columns.

- Get Record

  Get a record by its positon in data file.

- Get Record Pairs

  Return all records paired with their position in data file.