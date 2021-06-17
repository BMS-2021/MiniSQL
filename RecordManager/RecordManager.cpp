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

//bool RecordManager::insertRecord(const table &table, const sql_tuple &record) {
//    string tableFileStr = macro::tableFile(table.name);
//    int blockID = bm->getBlockTail(tableFileStr);
//    Block &block = bm->getBlock(tableFileStr, blockID);
//    if(block.usedSize == macro::BlockSize);
//    block = bm->getBlock(tableFileStr, ++blockID);
//
//    char *content = block.blockContent + block.usedSize;
//
//    int recordLen = table.record_len;
//    int recordPreBlock = macro::BlockSize / recordLen;
//
//    int offset = 0;
//    for (auto attr = record.element.begin(); attr < record.element.end(); attr++) {
//        switch (attr->type) {
//            case value_type::INT:
//                memcpy(content + offset, &attr->i, sizeof(int));
//                offset += sizeof(int);
//                break;
//            case value_type::CHAR:
//                strFixed = attr->str;
//                lengthOffset = attr->type.charSize - strFixed.length();
//                if (lengthOffset > 0) {
//                    strFixed.insert(strFixed.length(), lengthOffset, 0);
//                }
//                memcpy(block + offset, strFixed.c_str(), attr->type.charSize + 1);
//                offset += attr->type.charSize + 1;
//                break;
//            case value_type::FLOAT:
//                memcpy(content + offset, &attr->r, sizeof(float));
//                offset += sizeof(float);
//                break;
//        }
//    }
//
//    block.usedSize += offset;
//    bm->setDirty(tableName, blockID);
//
//    return true;
//}
//
//int RecordManager::selectRecord(const table &table, const vector<string> &attr, const vector<condition> &cond) {
//    string tableFileStr = tableFile(table.Name);
//    int blockID = 0;
//    Block &block = bm->getBlock(tableFileStr, blockID);
//    char *content = block.blockContent;
//
//    int recordLen = table.record_len;
//    int recordCnt = block.usedSize / recordLen;
//
//    sql_tuple tup;
//    row row;
//    result res;
//
//    while (content) {
//        for (int i = 0; i < recordCnt; i++) {
//            convertToTuple(block, i * length, table.attrType, tup);
//            if (condsTest(cond, tup, table.attrNames)) {
//                row = tup.fetchRow(table.attrNames, attr);
//                res.row.push_back(row);
//            }
//        }
//        bm->setFree(tableFile(table.Name), blockID);
//        blockID++;
//        block = bm->getBlock(tableFile(table.Name), blockID);
//    }
//}
//void RecordManager::printResult(const result &res) const {
//    for (auto const &row : res.row) {
//        cout << " | ";
//        for (auto const &col : row.col) {
//            cout << col << " | ";
//        }
//        cout << endl;
//    }
//}
//
//bool RecordManager::condsTest(const std::vector<condition> &conds, const sql_tuple &tup, const std::vector<std::string> &attr) {
//    int condPos;
//    for (condition cond : conds) {
//        condPos = -1;
//        for (int i = 0; i < attr.size(); i++) {
//            if (attr[i] == cond.attr) {
//                condPos = i;
//                break;
//            }
//        }
//        if (condPos == -1) {
//            std::cerr << "Attr not found in cond test!" << std::endl;
//        }
//        if (!cond.test(tup.element[condPos])) {
//            return false;
//        }
//    }
//    return true;
//}
//
//void RecordManager::convertToTuple(const char *blockBuffer, int offset, const std::vector<sql_value_type> &attrType, sql_tuple &tup) {
//    const char *block = blockBuffer + offset + 1; // 1 for meta bit
//    Element e;
//    tup.element.clear();
//    for (int i = 0; i < attrType.size(); i++) {
//        e.reset();
//        e.type = attrType[i];
//        switch (attrType[i].M()) {
//            case MINISQL_TYPE_INT:
//                memcpy(&e.i, block, sizeof(int));
//                block += sizeof(int);
//                break;
//            case MINISQL_TYPE_FLOAT:
//                memcpy(&e.r, block, sizeof(float));
//                block += sizeof(float);
//                break;
//            case MINISQL_TYPE_CHAR:
//                e.str = block;
//                block += attrType[i].charSize + 1;
//                break;
//        }
//        tup.element.push_back(e);
//    }
//}
