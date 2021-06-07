//
// Created by l1ght on 2021/6/5.
//

#include "CatalogManager.h"
#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <algorithm>

//#include "../API/ApiHelper.h"

using namespace std;


void CatalogManager::CreateTable(const std::string &table_name,
                                 const std::vector<std::pair<std::string, sql_value_type>> &schema_list,
                                 const std::string &primary_key_name
)
{
    table tb;
    tb.name = table_name;
    tb.attribute_cnt = (int) schema_list.size();
    uint32_t len{0};
    char auto_ind{'A'};

    auto rm = Api::ApiHelper::getApiHelper()->getRecordManager();

    for (const auto &sch: schema_list)
    {
        len += sch.second.getSize();
        tb.attribute_names.push_back(sch.first);
        auto t = sch.second;
        t.attrName = sch.first;
        tb.attribute_type.push_back(t);
        if (sch.first == primary_key_name)
        {
            (tb.attribute_type.end() - 1)->primary = true;
            (tb.attribute_type.end() - 1)->unique = true;
        }
    }

    tb.record_len = len;
    tb.record_cnt = 0;
    for (auto &t: tb.attribute_type)
    {
        if (t.unique && !t.primary)
        {
            tb.index.push_back(std::make_pair(t.attrName, std::string("auto_ind_") + (auto_ind++)));
            rm->createIndex(tb, t);
        }
    }
    tables.push_back(tb);
    kv[tb.name] = 0;
}

CatalogManager::CatalogManager()
        : tables(std::vector<table>())
{
    LoadFromFile();
}

CatalogManager::~CatalogManager()
{
    Flush();
}

void CatalogManager::Flush() const
{
    std::ofstream ofs(meta_file_name);
    ofs << tables.size() << std::endl;

    for (const auto &tb: tables)
    {
        ofs << tb.name << std::endl;
        ofs << tb.record_cnt << std::endl;
        ofs << kv[tb.name] << std::endl;
        std::ofstream otbfs(tb.name + ".catalog");
        uint16_t i{0};

        otbfs << tb.attribute_names.size() << std::endl;
        for (const auto &attr_name: tb.attribute_names)
        {
            otbfs << attr_name << std::endl;
            const auto &attr = tb.attribute_type[i];
            switch (attr.type)
            {
                case value_type::INT:
                    otbfs << "int" << std::endl;
                    break;
                case value_type::FLOAT:
                    otbfs << "float" << std::endl;
                    break;
                case value_type::CHAR:
                    otbfs << "char" << std::endl;
                    break;
            }
            if (attr.type == value_type::CHAR)
            {
                otbfs << attr.length << std::endl;
            } else
            {
                otbfs << 0 << std::endl;
            }

            auto ind = std::find_if(tb.index.begin(), tb.index.end(),
                                    [&attr_name](const std::pair<std::string, std::string> &p)
                                    {
                                        return p.first == attr_name;
                                    });
            if (ind != tb.index.end())
            {
                otbfs << 1 << std::endl << ind->second << std::endl;
            } else
            {
                otbfs << 0 << std::endl << "-" << std::endl;
            }
            ++i;
        }
        otbfs.close();
    }
    ofs.close();
}

void CatalogManager::LoadFromFile()
{
    std::ifstream ifs(meta_file_name);
    if (!ifs.is_open())
    {
        std::ofstream touch(meta_file_name);
        return;
    }
    size_t tables_count{0};
    ifs >> tables_count;

    auto rm = Api::ApiHelper::getApiHelper()->getRecordManager();

    std::string tb_name;
    for (auto i = 0; i < tables_count; ++i)
    {
        ifs >> tb_name;
        auto file_name = tb_name + ".catalog";
        std::ifstream itbfs(file_name);

        table tb;
        std::vector<sql_value_type> ind_vec;
        //size_t len{0};
        ifs >> tb.record_cnt;

        int v;
        ifs >> v;
        kv[tb_name] = v;

        tb.name = tb_name;

        uint16_t attr_cnts{0};
        uint16_t record_length{0};

        size_t attr_counts{0};
        itbfs >> attr_counts;
        for (auto ci = 0; ci < attr_counts; ++ci)
        {
            std::string attr_name, type_name, index_name;
            uint16_t isPri, isUni, isInd, size;
            sql_value_type type;

            itbfs >> attr_name >> type_name >> size >> isPri >> isUni >> isInd >> index_name;
            ++attr_cnts;

            tb.attribute_names.push_back(attr_name);
            if (type_name == "int")
            {
                type.type = value_type::INT;
            } else if (type_name == "char")
            {
                type.type = value_type::CHAR;
                type.length = size;
            } else if (type_name == "float")
            {
                type.type = value_type::FLOAT;
            } else
            {
                valid_assert(false);
            }
            //TODO: Implement the member funcs
//            record_length += type.getSize();
//            type.primary = isPri != 0;
//            type.unique = isUni != 0;
//            type.attrName = attr_name;
//            tb.attribute_type.push_back(type);
//            if (isInd)
//            {
//                auto ind = std::make_pair(attr_name, index_name);
//                tb.index.push_back(ind);
//                ind_vec.push_back(type);
//            }
        }
        tb.attribute_cnt = attr_cnts;
        tb.record_len = record_length;
        tables.push_back(tb);
        for(auto &it: ind_vec)
        {
            rm->createIndex(tb, it);
        }
    }
}

bool CatalogManager::TableExist(const std::string &table_name) const
{
    return std::find_if(tables.begin(), tables.end(), [&table_name](const table &tb)
    {
        return (tb.name == table_name);
    }) != tables.end();
}

table &CatalogManager::GetTable(const std::string &table_name)
{
    return *std::find_if(tables.begin(), tables.end(), [&table_name](const table &tb)
    {
        return (tb.name == table_name);
    });
}

bool CatalogManager::CheckIndexNameExists(const std::string &index_name) const
{
    for (const auto &t: tables)
    {
        for (auto &i: t.index)
        {
            if (i.second == index_name) return true;
        }
    }
    return false;
}

bool CatalogManager::RemoveTable(const table &table)
{
    if (std::find_if(tables.begin(), tables.end(), [&table](const struct table &tb)
    { return tb.name == table.name; }) == tables.end())
        return false;
    tables.erase(std::find_if(tables.begin(), tables.end(), [&table](const struct table &tb)
    { return tb.name == table.name; }));
    return true;
}

table &CatalogManager::GetTableWithIndex(const std::string &index_name)
{
    for (auto &t: tables)
    {
        for (const auto &i: t.index)
        {
            if (i.second == index_name) return t;
        }
    }
}

