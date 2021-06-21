//
// Created by Apple on 2021/6/12.
//

#include "BufferManager.h"
#include "../macro.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "../utils/exception.h"

using namespace std;

File::File(const string& _filename) {
    type = 0;
    filename = _filename;
    blockCnt = 0;

    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        totBlockCnt = (st.st_size / macro::BlockSize);
    } else {
        totBlockCnt = 0;
    }
    if(totBlockCnt <= 0) totBlockCnt = 1;

    next = nullptr;
    firstBlock = nullptr;
}


BufferManager::BufferManager() {
    LRUNum = 0;
    fileCnt = 0;
    fileHandle = nullptr;
    blockHandle = nullptr;
}

Block &BufferManager::writeBlock(Block &block) {
    fstream fp;
    fp.open(block.file->filename, ios::in | ios::out | ios::ate | ios::binary);
    fp.seekg(block.blockID * macro::BlockSize, ios::beg);
    fp.write(block.blockContent, macro::BlockSize);
    fp.close();
    memset(block.blockContent, 0, macro::BlockSize);
    return block;
}

Block &BufferManager::resetBlock(Block &block) {
    block.next = nullptr;
    block.dirty = 0;
    memset(block.blockContent, 0, macro::BlockSize);
    return block;
}

void BufferManager::replace(File &file, Block &block) {
    block.file = &file;
    Block* curBlock = file.firstBlock;
    if(!curBlock) file.firstBlock = &block;
    else {
        block.next = file.firstBlock;
        file.firstBlock = &block;
    }
}

Block &BufferManager::getFreeBlock(File &file) {
    if(!blockHandle) {
        if(file.blockCnt < macro::MaxBlocks) {
            file.blockCnt++;
            Block* blockPtr = new Block;
            resetBlock(*blockPtr);
            replace(file, *blockPtr);
            return *blockPtr;
        }
        Block &b = getLRUBlock(file);
        if(b.dirty) {
            writeBlock(b);
        }
        return b;
    }

    Block &b = *blockHandle;
    blockHandle = blockHandle->next;
    return b;
}

Block &BufferManager::getLRUBlock(File &file){
    int minLRU = LRUNum;
    Block *detect = nullptr;
    Block* curBlock = file.firstBlock;
    while(curBlock) {
        if (curBlock->LRUCount < minLRU) {
            minLRU = curBlock->LRUCount;
            detect = curBlock;
        }
        curBlock = curBlock->next;
    }
    return *detect;
}

void BufferManager::closeFile(File *file) {
    if(!file) return;
    Block *curBlock = file->firstBlock;
    Block *nextBlock = nullptr;
    while(curBlock) {
        nextBlock = curBlock->next;
        if(curBlock->dirty) writeBlock(*curBlock);
        curBlock = nextBlock;
    }

    Block *bh = blockHandle;
    if(!bh) blockHandle = file->firstBlock;
    else {
        while(bh->next) bh = bh->next;
        bh->next = file->firstBlock;
    }
    fileCnt--;
}

void BufferManager::closeAllFile() {
    File *curFile = fileHandle;
    while(curFile) {
        closeFile(curFile);
        curFile = curFile->next;
    }
}

File &BufferManager::getFile(const string& filename) {
    File *preFile = nullptr;
    File *curFile = fileHandle;
    while(curFile && curFile->filename != filename) {
        preFile = curFile;
        curFile = curFile->next;
    }
    if(!curFile) { //file not in memory
        if(fileCnt == macro::MaxFiles) closeFile(preFile);
        File* newFile = new File(filename);
        newFile->filename = filename;
        newFile->next = fileHandle;
        fileCnt++;
        fileHandle = newFile;
        curFile = newFile;
    }
    return *curFile;
}

Block &BufferManager::readBlock(const string& filename, int blockID) {
    fstream fp;
    fp.open(filename, ios::in | ios::out | ios::binary);
    if (!fp.good()) {
        throw sql_exception(401, "buffer manager", "element not found");
    }

    File &file = getFile(filename);
    Block &block = getFreeBlock(file);
    block.blockID = blockID;

    if(file.totBlockCnt <= blockID) {
        file.totBlockCnt = blockID + 1;
        memset(block.blockContent, 0, macro::BlockSize);
    } else {
        fp.seekg(blockID * macro::BlockSize, ios::beg);
        fp.read(block.blockContent, macro::BlockSize);
    }

    return block;
}

Block &BufferManager::getBlock(const string& filename, int blockID) {
    File &file = getFile(filename);
    Block *curBlock = file.firstBlock;
    while(curBlock && curBlock->blockID != blockID) {
        curBlock = curBlock->next;
    }
    if(!curBlock) { //block not found
        curBlock = &readBlock(filename, blockID);
    }
    lock(*curBlock);
    return *curBlock;
}

Block &BufferManager::findBlock(const string& filename, int blockID) const {
    File *curFile = fileHandle;
    while(curFile && curFile->filename != filename) {
        curFile = curFile->next;
    }
    if(!curFile) {
        throw sql_exception(401, "buffer manager", "fail to open file: " + filename);
    }

    Block *curBlock = curFile->firstBlock;
    while(curBlock && curBlock->blockID != blockID) {
        curBlock = curBlock->next;
    }
    if(!curBlock) {
        throw sql_exception(401, "buffer manager", "element not found");
    }

    return *curBlock;
}

void BufferManager::setDirty(const string &filename, unsigned int blockID) {
    Block &block = findBlock(filename, blockID);
    block.dirty = true;
}

void BufferManager::unlock(const string& filename, unsigned int blockID) {
    Block &block = findBlock(filename, blockID);
    block.lock = false;
}

void BufferManager::lock(Block &block) {
    LRUNum++;
    block.lock = true;
    block.LRUCount = LRUNum;
}

void BufferManager::createFile(const string& in) {
    ofstream f1(in);
}

void BufferManager::removeFile(const string& filename) {
    File *curFile = fileHandle;
    while(curFile && curFile->filename != filename) {
        curFile = curFile->next;
    }
    if(curFile) closeFile(curFile);
    if (remove(filename.c_str())) {
        throw sql_exception(402, "buffer manager", "fail to remove file: " + filename);
    }
}

int BufferManager::getBlockCnt(const string& filename) {
    return getFile(filename).totBlockCnt;
    return 0;
}