//
// Created by Apple on 2021/6/12.
//

#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <string>
#include "../BufferManager/BufferManager.h"
#include "../macro.h"

using namespace std;

class RecordManager {
public:
    RecordManager (BufferManager *bm): bm(bm) {}
    ~RecordManager() = default;

    bool creatTable(const string &tableName);

    bool dropTable(const string &tableName);

    bool createIndex(const table &table, const sql_value_type &index);

    bool dropIndex(const table &table, const string &index);

    bool insertRecord(const table &table, const sql_tuple &record);

    int selectRecord(const table &table, const vector<string> &attr, const vector<condition> &cond);

private:

    bool condsTest(const std::vector<condition> &conds, const sql_tuple &tup, const std::vector<std::string> &attr);

    BufferManager *bm;
};


#endif //MINISQL_RECORDMANAGER_H