## Buffer Manager module design report

writer: 3190104509 Gao Weiyuan

###  Overview

`BufferManager` constructs and manages buffers in memory. Other modules, like `RecordManager` and `IndexManager`, asks for the data from `BufferManager` instead of directly getting them from the file system. It smooth the low speed of disk I/O and dramatically improve the performance of database.

Each file is divided into many blocks and the size of each block is 4K. While the program is running, `BufferManager` would read some blocks into memory when needed and offer them to other modules.

### Module Design

`RecordManager` and `IndexManager` request data from `BufferManager`. When a request occur, `BufferManager` firstly check if the data exist in buffer. If exist, return the data directly. If not, `BufferManager` will read the data from disk, then check if the buffer has free space, if so add the new block to the buffer. If not a block will be choosen to be replaced using LRU algorithm.

LRU(Least recently used), always choose the least recently used blcok. There is a LRU count variable in each block. Whenever the block is used, the LRU count increases by 1. Then the block that has the least LRU count is the block to be replaced.

In buffer, I maintain the following two structures:

- File Blocks

  This structure contains blocks store useful data. The FileHandle points to a file linked list, each node represents a file. And each node in file linked list points to a batch of block. When `BufferManager` handle a request, it firstly find in file linked list. If the file is not found, it will create a new node and open the file. Then it search the blocks under the file node. If the block is in buffer, it will be returned directly. If not, the manager will fetch a block in free block linked list and add it to file blocks.

- Free Blocks

  Free block is  a linked list headed by BlockHandle which stores blocks not used. When a file is closed or be replaced, it will reduce a lot of free blocks, these blocks will be added in to free blocks linked list. When we need new blocks, we can fetch blocked in this linked list.

### Implementation

1. Constant defined
   There are several parameters defined as follow:

   ```c++
   const int BlockSize = 4096;
   const int MaxBlocks = 40;
   const int MaxFiles = 6;
   ```

   BlockSize defines each block's size. MaxBlocks defines the maximum of the number of blocks. MaxFiles defines the maximum of the number of opening files.

2. data structure

   - struct File

     ```c++
     struct File {
         int type; //0:db file 1:table file 2: index file
     
         string filename; //the name of the file
         int blockCnt;
         int totBlockCnt;
         File *next; //the pointer points to the next file
         Block *firstBlock;
     
         File(const string& _filename);
     
     };
     ```

     

   - struct Block

     ```c++
     struct Block {
         int blockID;
         bool dirty; //whether the block been changed
         Block *next;
         File *file;
         int usedSize;
         int LRUCount;
         int lock; //prevent the block from replacing
         char *blockContent;
     
         Block();
         Block(bool isNull);
         ~Block() = default;
     };
     ```

   

3. Major function introduction

   - Get Block

     ```c++
     Block &getBlock(const string& fileName, int blockID);
     ```

     filename: name of the file

     blockID: ID of requested block. 

     function: Place a specified block in buffer. It read data from file system in case of missing block in memory and use the LRU algorithm as the block-replacement-strategy. If the block is not exist, buffer manager will allocate a new block automatically.

   - Set Dirty

     ```c++
     void setDirty(const string &filename, unsigned int blockID);
     ```

     filename: name of the file

     blockID: ID of the block. 

     Set the dirty-bit in block, which indicates the data in block have been modified and it shoud write back first before replacing.

   - Unlock

     ```c++
     void unlock(const string& filename, unsigned int blockID);
     ```

     filename: name of the file

     blockID: ID of the block. function: Unlock a block, allowing it to be replaced by other block.

   - Create File

     ```c++
     static void createFile(const string& in);
     ```

     in: file name

     function: Creat a file in file system, which is initially blank

   - Remove File

     ```c++
     void removeFile(const string& filename);
     ```

     filename: name of the file needed to be remove.

     function: Remove a file from file system.

   - Get Block Cnt

     ```c++
     int getBlockCnt(const string& filename);
     ```

     filename: name of the file.

     function: Get the number of total blocks of a specified file.

   - Close All File

     ```c++
     void closeAllFile();
     ```

     function: Save all data and close all files, called when quitting the process.

