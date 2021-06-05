//
// Created by gwy on 2021/6/5.
//

#ifndef MINISQL_CACHEMANAGER_H
#define MINISQL_CACHEMANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <map>
#include <utility>
#include "../Base/Base.h"

using namespace std;
using namespace MINISQL_BASE;

struct Page {
    string filename;
    int id;
    unsigned int pageID;
    bool valid;
    bool dirty;
    int LRUCnt;
    char content[PageSize];

    Page() {
        reset();
    }

    void reset() {
        valid = dirty = false;
        memset(content, 0, PageSize);
    }

    void assign(string filename, int pageID) {
        this->filename = filename;
        this->pageID = pageID;
    }

    Page& flush() {
        fstream fp;
        fp.open(filename, ios::in | ios::out | ios::ate | ios::binary);
        fp.seekg(pageID * PageSize, ios::beg);
//        cout << "Flushing: " << content << endl;
        fp.write(content, PageSize);
        fp.close();
        return *this;
    }
};

class CacheManager {

public:
    CacheManager();

    ~CacheManager() = default;

    int getPageTail(string filename);

    void setDirty(const string &filename, unsigned int pageID);
    void setFree(string filename, unsigned int pageID);

    char *getPage(string filename, unsigned int pageID, bool allocate = false);

    void flushAllPages(); // write all content in page to disk

    void createFile(string);

    Page& getLRU();

    void removeFile(string filename);



private:
    typedef map<pair<string, unsigned int>, Page &> PageMap;

    PageMap pageMap;

    std::vector<Page> pages;

    void setValid(int id); // mark as using

    // find page id which is available
    Page &getFreePage();

    Page &findPagePair(string filename, unsigned int pageID) const;

    int maxLRU;
};

#endif //MINISQL_CACHEMANAGER_H
