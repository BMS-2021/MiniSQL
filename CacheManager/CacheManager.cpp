//
// Created by gwy on 2021/6/5.
//

#include <sys/stat.h>
#include "CacheManager.h"

using namespace std;

CacheManager::CacheManager() {
    pages.resize(MaxPages);
    maxLRU = 0;
    int id = 0;
    for(auto &page : pages) {
        page.id = id;
    }
}

int CacheManager::getPageTail(string filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        return (st.st_size / PageSize - 1);
    }
    cerr << "Failed to get file tail" << endl;
    return 0;
}

void CacheManager::setDirty(const string &filename, unsigned int pageID) {
    Page& page = findPagePair(filename, pageID);
    page.dirty = true;
}

void CacheManager::setValid(int id) {
    maxLRU++;
    Page& page = pages[id];
    page.valid = true;
    page.LRUCnt = maxLRU;
}

Page& CacheManager::getLRU() {
    int minLRU = maxLRU;
    Page* minPage = nullptr;
    for(auto page: pages) {
        if(page.valid) continue;
        if(page.LRUCnt < minLRU) {
            minLRU = page.LRUCnt;
            minPage = const_cast<Page *>(&page);
        }
    }
    if (minPage == nullptr) { cerr << "No LRU page found!"; }
    return *minPage;
}

void CacheManager::removeFile(string filename) {
    for(auto &page: pages) {
        if(page.filename == filename) page.reset();
    }

    if(remove(filename.c_str())) {
        cerr << "Fail to remove file: " << filename << endl;
    }
}

void CacheManager::createFile(string in) {
    ofstream f1(in);
}

Page& CacheManager::getFreePage() {
    for (auto &page: pages) {
        if (!page.dirty && !page.valid) {
            page.reset();
            setValid(page.id);
            return page;
        }
    }

    Page &p = getLRU();
    p.flush().reset();
    setValid(p.id);

    return p;
}

char* CacheManager::getPage(string filename, unsigned int pageID, bool allocate) {
    for (auto &page: pages) {
        if (page.filename == filename && page.pageID == pageID) {
            setValid(page.id);
            pageMap.insert(PageMap::value_type(make_pair(filename, pageID), page));
            return page.content;
        }
    }

    fstream fp;
    fp.open(filename, ios::in | ios::out | ios::binary);
    if (!fp.good())
        cerr << "Fail to open file: " << filename << "." << endl;
    fp.seekg(ios_base::end);
    int pageOffset = getPageTail(filename) + 1;
//    cout << "Detected pageOffset: " << pageOffset << endl;
    if (pageID >= pageOffset) {
        if (!allocate) return nullptr;
        if (pageOffset != pageID) {
            cerr << fp.tellg() << " " << pageOffset << " " << pageID << endl;
            cerr << "Requesting way beyond the tail!" << endl;
            return nullptr;
        }
//        cout << "Requesting new page..." << endl;
    }

    Page &page = getFreePage();
    page.assign(filename, pageID);
    pageMap.insert(PageMap::value_type(make_pair(filename, pageID), page));

    fp.seekg(pageID * PageSize, ios::beg);
    fp.read(page.content, PageSize);
    fp.close();
    setValid(page.id);
    page.flush();
    return page.content;
}

void CacheManager::setFree(string filename, unsigned int pageID) {
    Page &page = findPagePair(filename, pageID);
    page.valid = false;
    pageMap.erase(make_pair(filename, page.pageID));
}

Page& CacheManager::findPagePair(string filename, unsigned int pageID) const {
    auto parent = pageMap.find(make_pair(filename, pageID));
    if (parent == pageMap.end()) {
        cerr << "Element not found!";
    }
    return parent->second;
}

void CacheManager::flushAllPages() {
    for (auto &page : pages) {
        if (page.dirty) {
            page.flush();
        }
        page.reset();
    }
}