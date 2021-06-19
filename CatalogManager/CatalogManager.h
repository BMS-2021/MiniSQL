
#ifndef MINISQL_CATALOGMANAGER_H
#define MINISQL_CATALOGMANAGER_H

#include <list>
#include <map>
#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept>

//#include "../Base/Base.h"
#include "../macro.h"

//using namespace MINISQL_BASE;

class CatalogManager
{
public:
    void CreateTable(const std::string &table_name,
                     const std::vector<std::pair<std::string, sql_value_type>> &schema_list,
                     const std::string &primary_key_name
    );

    bool TableExist(const std::string &table_name) const;

    table &GetTable(const std::string &table_name);

    bool CheckIndexNameExists(const std::string &index_name) const;

    table &GetTableWithIndex(const std::string &index_name);

    bool RemoveTable(const table &table);

    bool DropTableByName(const std::string &table_name);

    CatalogManager();

    ~CatalogManager();

    void Flush() ;

    bool isValid() const
    { return validFlag; }

    mutable std::map<std::string, int> kv;

private:
    void LoadFromFile();

    bool valid_assert(bool cond)
    {
        return cond ? cond : (validFlag = cond);
    }

private:
    std::vector<table> tables;

    static constexpr auto meta_file_name = "tables.meta";

    bool validFlag = true;
};

#endif //MINISQL_CATALOGMANAGER_H
