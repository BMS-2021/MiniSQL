### Buffer Manager

`BufferManager` constructs and manages buffers in memory. Other modules, like `RecordManager` and `IndexManager`, asks for the data from `BufferManager` instead of directly getting them from the file system. It smooth the low speed of disk I/O and dramatically improve the performance of database.

Each file is divided into many blocks and the size of each block is 4K. While the program is running, `BufferManager` would read some blocks into memory when needed and offer them to other modules.

Main functions and simple introduction are as follow:

- Get block

  Place a specified block in buffer. It read data from file system in case of missing block in memory and use the LRU algorithm as the block-replacement-strategy. If the block is not exist, buffer manager will allocate a new block automatically.

- Set dirty

  Set the dirty-bit in block, which indicates the data in block have been modified and it shoud write back first before replacing.

- Unlock

  Unlock a block, allowing it to be replaced by other block.

- Creat file

  Creat a file in file system, which is initially blank

- Remove file

  Remove a file from file system

- Get block cnt

  Get the number of total blocks of a specified file.

- Close all file

  Save all data and close all files, called when quitting the process

