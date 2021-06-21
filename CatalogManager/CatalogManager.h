
#ifndef MINISQL_CATALOGMANAGER_H
#define MINISQL_CATALOGMANAGER_H

#include <list>
#include <map>
#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept>

#include "../macro.h"

//using namespace MINISQL_BASE;

class CatalogManager
{
public:
    void CreateTable(const std::string &table_name,
                     const std::vector<schema> &schema_list,
                     const std::string &primary_key_name
    );

    bool TableExist(const std::string &table_name) const;

    macro::table &GetTable(const std::string &table_name);

    macro::table &GetTableWithIndex(const std::string &index_name);

    bool DropTableByName(const std::string &table_name);

    CatalogManager();

    ~CatalogManager();

    void Flush() ;

    bool isValid() const
    { return validFlag; }

    mutable std::map<std::string, int> kv;

private:
    void LoadFromFile();


private:
    std::vector<macro::table> tables;

    const std::string tables_info = "tables.meta";

    bool validFlag = true;
};

#endif //MINISQL_CATALOGMANAGER_H
