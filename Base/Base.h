//
// Created by gwy on 2021/6/5.
//

#ifndef MINISQL_BASE_H
#define MINISQL_BASE_H

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept>

namespace MINISQL_BASE {
    const int PageSize = 4096;
    const int MaxPages = 128;

    inline std::string tableFile(const std::string &tableName) { return tableName + ".tb"; }

    inline std::string indexFile(const std::string &tableName const std::string &index) {
        return tableName + "_" + index + ".ind";
    }
}

#endif //MINISQL_BASE_H
