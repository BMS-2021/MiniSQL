//
// Created by Apple on 2021/6/12.
//

#include "BufferManager.h"
#include "../macro.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

File::File(const string& _filename) {
    type = 0;
    filename = _filename;
    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        blockCnt = (st.st_size / macro::BlockSize);
    } else {
        blockCnt = 0;
    }
    if(blockCnt <= 0) blockCnt = 1;
    next = nullptr;
    firstBlock = nullptr;
}

BufferManager::BufferManager() {
    LRUNum = 0;
    fileCnt = 0;
    blockCnt = 0;
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
        if(blockCnt < macro::MaxBlocks) {
            blockCnt++;
            Block* blockPtr = new Block;
            resetBlock(*blockPtr);
            replace(file, *blockPtr);
            return *blockPtr;
        }
        Block &b = getLRUBlock();
        if(b.dirty) {
            writeBlock(b);
            return b;
        }
    }

    Block &b = *blockHandle;
    blockHandle = blockHandle->next;
    return b;
}

Block &BufferManager::getLRUBlock(){
    File* curFile = fileHandle;
    int minLRU = LRUNum;
    Block *detect = nullptr;
    while(curFile) {
        Block* curBlock = curFile->firstBlock;
        while(curBlock) {
            if (curBlock->LRUCount < minLRU) {
                minLRU = curBlock->LRUCount;
                detect = curBlock;
            }
            curBlock = curBlock->next;
        }
        curFile = curFile->next;
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
    if (!fp.good())
        cerr << "Fail to open file: " << filename << "." << endl;

    File &file = getFile(filename);
    Block &block = getFreeBlock(file);
    block.blockID = blockID;

    int tail = file.blockCnt - 1;
    if(tail < blockID) {
        file.blockCnt = blockID + 1;
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
    if(!curFile) cerr << "Element not found!";

    Block *curBlock = curFile->firstBlock;
    while(curBlock && curBlock->blockID != blockID) {
        curBlock = curBlock->next;
    }
    if(!curBlock) cerr << "Element not found!";

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
        cerr << "Fail to remove file: " << filename << endl;
    }
}

int BufferManager::getBlockCnt(const string& filename) {
    return getFile(filename).blockCnt;
    return 0;
}