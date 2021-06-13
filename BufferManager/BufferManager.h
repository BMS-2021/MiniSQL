//
// Created by Apple on 2021/6/12.
//

#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <iostream>
#include <string>

using namespace std;

typedef struct File File;
typedef struct Block Block;

struct File {
    int type; //0:db file 1:table file 2: index file

    string filename; //the name of the file
    int recordAmount; //the number of record in the file
    int freeNum; //the free block number which could be used for the file
    int recordLength; //the length of the record in the file
    File *next; //the pointer points to the next file
    Block *firstBlock;
};

struct Block {
    int blockID;
    bool dirty; //whether the block been changed
    Block *next;
    File *file;
    int usedSize;
    int LRUCount;
    int lock; //prevent the block from replacing
    char *blockContent;
};

class BufferManager {
public:
    BufferManager();

    ~BufferManager() = default;

    void closeAllFile();

    Block &getBlock(string fileName, int blockID);

    void setDirty(const string &filename, unsigned int blockID);

    void createFile(string in);

    int getBlockTail(string filename);

private:
    void closeFile(File *file);

    Block &writeBlock(Block &block);

    Block &resetBlock(Block &block);

    void replace(File &file, Block &block);

    Block &getFreeBlock();

    Block &getLRUBlock();

    Block &readBlock(string filename, int blockID);

    File &getFile(string filename);

    Block &findBlock(string filename, int blockID) const;

    int LRUNum;
    int fileCnt;
    File *fileHandle;
    Block *blockHandle;
};

#endif //MINISQL_BUFFERMANAGER_H
