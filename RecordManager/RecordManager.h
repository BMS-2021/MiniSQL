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
    RecordManager() = default;
    RecordManager (BufferManager *bm): bm(bm) {}
    ~RecordManager() = default;

    bool creatTable(const string &tableName);

    bool dropTable(const string &tableName);

    bool createIndex(const table &table, const sql_value_type &index);

    bool dropIndex(const table &table, const string &index);

    int insertRecord(const table &table, const sql_tuple &record);

    bool deleteRecord(const table &table, const vector<condition> &conditions);

    int selectRecord(const table &table, const vector<string> &attr, const vector<condition> &cond);

    static void printResult(const result &res) ;

private:

    BufferManager *bm;

    static sql_tuple genTuple(const char *blockBuffer, int offset, const std::vector<sql_value_type> &attrType);

    static bool condsTest(const vector<condition> &conds, const sql_tuple &tup, const vector<std::string> &attr);
};


#endif //MINISQL_RECORDMANAGER_H