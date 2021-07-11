# Buffer Manager

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
  