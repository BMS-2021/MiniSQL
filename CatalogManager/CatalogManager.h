
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

    void LoadFromFile();

    bool CreateIndex(const std::string &table_name, const std::string &attr_name, const std::string &index_name);

    bool CreateIndex(macro::table &table, const std::string &attr_name, const std::string &index_name);

    bool DropIndex(const std::string &table_name, const std::string &attr_name);

    bool DropIndex(const std::string &index_name);

    std::string GetAttrByIndex(const std::string &index_name);


private:
    std::vector<macro::table> tables;

    std::map<std::string, std::string> indexes; // <index_name, attr_name>

    const std::string tables_info = "tables.meta";

    bool validFlag = true;
};

#endif //MINISQL_CATALOGMANAGER_H
