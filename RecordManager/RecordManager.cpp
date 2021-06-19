//
// Created by Apple on 2021/6/12.
//

#include"RecordManager.h"

using namespace std;

bool RecordManager::creatTable(const string &tableName) {
    string tableFileStr = macro::tableFile(tableName);
    bm->createFile(tableFileStr);
    return true;
}

bool RecordManager::dropTable(const string &tableName) {
    string tableFileStr = macro::tableFile(tableName);
    bm->removeFile(tableFileStr);
    return true;
}

int RecordManager::insertRecord(const table &table, const sql_tuple &record) {
    string tableFileStr = macro::tableFile(table.name);
    int blockID = bm->getBlockTail(tableFileStr);
    Block &block = bm->getBlock(tableFileStr, blockID);
    if(block.usedSize == macro::BlockSize);
    block = bm->getBlock(tableFileStr, ++blockID);

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
        bm->unlock(tableFileStr, blockID);
        block = bm->getBlock(tableFileStr, ++blockID);
        content = block.blockContent;
    }

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

    bm->setDirty(tableFileStr, blockID);

    return blockID * recordsPreBlock + recordOffset;
}

bool RecordManager::deleteRecord(const table &table, const vector<condition> &conditions) {
    int blockID = 0;
    Block block = bm->getBlock(macro::tableFile(table.name), blockID);
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
//                for (auto &col: tup.element) {
//                    for (auto &attr : table.index) {
//                        if (col.type.attrName == attr.first) {
//                            im->removeKey(indexFile(table.name, attr.first), col);
//                        }
//                    }
//                }
            }
        }
        bm->setDirty(macro::tableFile(table.name), blockID);
        bm->unlock(macro::tableFile(table.name), blockID);
        block = bm->getBlock(macro::tableFile(table.name), ++blockID);
        content = block.blockContent;
    }
}

int RecordManager::selectRecord(const table &table, const vector<string> &attr, const vector<condition> &cond) {
    string tableFileStr = macro::tableFile(table.name);
    int blockID = 0;
    Block &block = bm->getBlock(tableFileStr, blockID);
    char *content = block.blockContent;

    int recordLen = table.record_len;
    int recordCnt = block.usedSize / recordLen;

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
        bm->unlock(macro::tableFile(table.name), blockID);
        blockID++;
        block = bm->getBlock(macro::tableFile(table.name), blockID);
    }
}

void RecordManager::printResult(const result &res) const {
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
    for (condition cond : conds) {
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
    for (int i = 0; i < attrType.size(); i++) {
        e.reset();
        e.sql_type = attrType[i];
        switch (attrType[i].type) {
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
                curContent += attrType[i].length + 1;
                break;
        }
        tup.element.push_back(e);
    }
}
