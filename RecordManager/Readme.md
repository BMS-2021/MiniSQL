# Record Manager

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
  