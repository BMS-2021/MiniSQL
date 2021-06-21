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
typedef struct BufferManager BufferManager;

struct File {
    int type; //0:db file 1:table file 2: index file

    string filename; //the name of the file
    int blockCnt;
    File *next; //the pointer points to the next file
    Block *firstBlock;

    File(const string& _filename);

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
        blockContent = static_cast<char *>(calloc(1, macro::BlockSize));

    }

    Block(bool isNull) {
        blockID = 0;
        dirty = 0;
        next = nullptr;
        file = nullptr;
        usedSize = 0;
        LRUCount = 0;
        lock = 0;
        if(isNull) blockContent = nullptr;
    }
    ~Block() {
//        if(blockContent) free(blockContent);
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

    int getBlockCnt(const string& filename);

    Block nullBlock = Block(true);
    File *fileHandle;
private:
    void closeFile(File *file);

    static Block &writeBlock(Block &block);

    static Block &resetBlock(Block &block);

    static void replace(File &file, Block &block);

    void lock(Block &block);

    Block &getFreeBlock(File &file);

    Block &getLRUBlock();

    Block &readBlock(const string& filename, int blockID);

    File &getFile(const string& filename);

    Block &findBlock(const string& filename, int blockID) const;

    int LRUNum;
    int fileCnt;
    int blockCnt;

    Block *blockHandle;
};

#endif //MINISQL_BUFFERMANAGER_H
