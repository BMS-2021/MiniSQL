//
// Created by Apple on 2021/6/12.
//

#include"RecordManager.h"

using namespace std;

extern BufferManager buf_mgt;

bool RecordManager::creatTable(const string &tableName) {
    string tableFileStr = macro::tableFile(tableName);
    BufferManager::createFile(tableFileStr);
    return true;
}

bool RecordManager::dropTable(const string &tableName) {
    string tableFileStr = macro::tableFile(tableName);
    buf_mgt.removeFile(tableFileStr);
    return true;
}

int RecordManager::insertRecord(const macro::table &table, const sql_tuple &record) {
    string tableFileStr = macro::tableFile(table.name);
    int blockID = BufferManager::getBlockTail(tableFileStr);
    Block &block = buf_mgt.getBlock(tableFileStr, blockID);
    block = buf_mgt.getBlock(tableFileStr, ++blockID);

    char *content = block.blockContent;

    int recordLen = table.record_len;
    int recordsPreBlock = macro::BlockSize / recordLen;
    int recordOffset = 0;
    bool isValid = false;
    while (recordOffset < recordsPreBlock) {
        if (content[recordOffset * recordLen] != 0) {
            recordOffset++;
            continue;
        }
        isValid = true;
        content += recordOffset * recordLen;
        break;
    }

    if (!isValid) {
        recordOffset = 0;
        buf_mgt.unlock(tableFileStr, blockID);
        block = buf_mgt.getBlock(tableFileStr, ++blockID);
        content = block.blockContent;
    }

    content[0] = 1;
    int offset = 1;

    string str;
    int lenOffset;
    for (auto attr = record.element.begin(); attr < record.element.end(); attr++) {
        switch (attr->sql_type.type) {
            case value_type::INT:
                memcpy(content + offset, &attr->sql_int, sizeof(int));
                offset += sizeof(int);
                break;
            case value_type::CHAR:
                str = attr->sql_str;
                lenOffset = attr->sql_type.length - str.length();
                if (lenOffset > 0) {
                    str.insert(str.length(), lenOffset, 0);
                }
                memcpy(content + offset, str.c_str(), attr->sql_type.length + 1);
                offset += attr->sql_type.length + 1;
                break;
            case value_type::FLOAT:
                memcpy(content + offset, &attr->sql_float, sizeof(float));
                offset += sizeof(float);
                break;
        }
    }

    buf_mgt.setDirty(tableFileStr, blockID);

    return blockID * recordsPreBlock + recordOffset;
}

bool RecordManager::deleteRecord(const macro::table &table, const vector<condition> &conditions) {
    int blockID = 0;
    Block block = buf_mgt.getBlock(macro::tableFile(table.name), blockID);
    char *content = block.blockContent;
    int length = table.record_len + 1;
    int blocks = macro::BlockSize / length;
    sql_tuple tup;

    while (*content) {
        for (int i = 0; i < blocks; i++) {
            if (content[i * length] == 0) { continue; }
            tup = genTuple(content, i * length, table.attribute_type);
            if (condsTest(conditions, tup, table.attribute_names)) {
                content[i * length] = 0;
            }
        }
        buf_mgt.setDirty(macro::tableFile(table.name), blockID);
        buf_mgt.unlock(macro::tableFile(table.name), blockID);
        block = buf_mgt.getBlock(macro::tableFile(table.name), ++blockID);
        content = block.blockContent;
    }

    return true;
}

int RecordManager::selectRecord(const macro::table &table, const vector<string> &attr, const vector<condition> &cond) {
    string tableFileStr = macro::tableFile(table.name);
    int blockID = 0;
    Block &block = buf_mgt.getBlock(tableFileStr, blockID);
    char *content = block.blockContent;

    int recordLen = table.record_len;
    int recordCnt = macro::BlockSize / recordLen;

    sql_tuple tup;
    row row;
    result res;

    while (content) {
        for (int i = 0; i < recordCnt; i++) {
            genTuple(content, i * recordLen, table.attribute_type);
            if (condsTest(cond, tup, table.attribute_names)) {
                row = tup.fetchRow(table.attribute_names, attr);
                res.row.push_back(row);
            }
        }
        buf_mgt.unlock(macro::tableFile(table.name), blockID);
        blockID++;
        block = buf_mgt.getBlock(macro::tableFile(table.name), blockID);
    }

    printResult(res);
    return res.row.size();
}

void RecordManager::printResult(const result &res) {
    for (auto const &row : res.row) {
        cout << " | ";
        for (auto const &col : row.col) {
            cout << col << " | ";
        }
        cout << endl;
    }
}

bool RecordManager::condsTest(const std::vector<condition> &conds, const sql_tuple &tup, const std::vector<std::string> &attr) {
    int condPos;
    for (const condition& cond : conds) {
        condPos = -1;
        for (int i = 0; i < attr.size(); i++) {
            if (attr[i] == cond.attribute_name) {
                condPos = i;
                break;
            }
        }
        if (condPos == -1) {
            std::cerr << "Attr not found in cond test!" << std::endl;
        }
        if (!cond.test(tup.element[condPos])) {
            return false;
        }
    }
    return true;
}

sql_tuple RecordManager::genTuple(const char *content, int offset, const std::vector<sql_value_type> &attrType) {
    const char *curContent = content + offset + 1; // 1 for meta bit
    sql_value e;
    sql_tuple tup;
    tup.element.clear();
    for (auto i : attrType) {
        e.reset();
        e.sql_type = i;
        switch (i.type) {
            case value_type::INT:
                memcpy(&e.sql_int, curContent, sizeof(int));
                curContent += sizeof(int);
                break;
            case value_type::FLOAT:
                memcpy(&e.sql_float, curContent, sizeof(float));
                curContent += sizeof(float);
                break;
            case value_type::CHAR:
                e.sql_str = curContent;
                curContent += i.length + 1;
                break;
        }
        tup.element.push_back(e);
    }
    return tup;
}
