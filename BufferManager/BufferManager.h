//
// Created by Apple on 2021/6/12.
//

#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <iostream>
#include <string>
#include "../macro.h"

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

    File() {
        type = 0;
        filename = "";
        recordAmount = 0;
        freeNum = 0;
        recordLength = 0;
        next = nullptr;
        firstBlock = nullptr;
    }

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

    Block() {
        blockID = 0;
        dirty = 0;
        next = nullptr;
        file = nullptr;
        usedSize = 0;
        LRUCount = 0;
        lock = 0;
        blockContent = static_cast<char *>(malloc(macro::BlockSize));
    }

    ~Block() {
        free(blockContent);
    }
};

class BufferManager {
public:
    BufferManager();

    ~BufferManager() = default;

    void closeAllFile();

    Block &getBlock(const string& fileName, int blockID);

    void setDirty(const string &filename, unsigned int blockID);

    void unlock(const string& filename, unsigned int blockID);

    static void createFile(const string& in);

    void removeFile(const string& filename);

    static int getBlockTail(const string& filename);

private:
    void closeFile(File *file);

    static Block &writeBlock(Block &block);

    static Block &resetBlock(Block &block);

    static void replace(File &file, Block &block);

    void lock(const string& filename, unsigned int blockID);

    Block &getFreeBlock();

    Block &getLRUBlock();

    Block &readBlock(const string& filename, int blockID);

    File &getFile(const string& filename);

    Block &findBlock(const string& filename, int blockID) const;

    int LRUNum;
    int fileCnt;
    int blockCnt;
    File *fileHandle;
    Block *blockHandle;
};

#endif //MINISQL_BUFFERMANAGER_H
