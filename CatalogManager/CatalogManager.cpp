#include "CatalogManager.h"
#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <algorithm>


using namespace std;


void CatalogManager::CreateTable(const std::string &table_name,
                                 const std::vector<schema> &schema_list,
                                 const std::string &primary_key_name
)
{
    macro::table tab(table_name, schema_list.size());
    unsigned long long len = 0;
    char auto_ind = 'A';


    for (auto &schema: schema_list){
        sql_value_type elem_type = schema.type;
        len += elem_type.size();
        tab.attribute_names.push_back(schema.name);
        if (schema.name == primary_key_name){
            elem_type.primary = true;
        }
        if (schema.unique){
            elem_type.unique = true;
        }
        if (elem_type.type == value_type::CHAR){
            elem_type.length = schema.type.length;
        }
        tab.attribute_type.push_back(elem_type);
    }

    tab.record_len = len;
    tab.record_cnt = 0;

    for (auto &t: tab.attribute_type){
        if (t.unique && !t.primary){
            // TODO:Implement real index
//            tab.index.push_back(std::make_pair(t.attrName, std::string("auto_ind_") + (auto_ind++)));
//            createIndex(tab, t);
        }
    }
    tables.push_back(tab);
}

CatalogManager::CatalogManager()
        : tables(std::vector<macro::table>())
{
    LoadFromFile();
}

CatalogManager::~CatalogManager()
{
    Flush();
}

void CatalogManager::Flush()
{

    std::ofstream ofs(meta_file_name);
    ofs << tables.size() << std::endl;

    for (auto &tb: tables)
    {
        ofs << tb.name << std::endl;
        ofs << tb.record_cnt << std::endl;
        std::ofstream otbfs(tb.name + ".catalog");

        otbfs << tb.attribute_names.size() << std::endl;
        for(auto i = 0; i < tb.attribute_names.size(); ++i){
            otbfs << tb.attribute_names[i] << std::endl;
            const auto &attr = tb.attribute_type[i];
            switch (attr.type){
                case value_type::INT:
                    otbfs << "int" << std::endl;
                    otbfs << 0 << std::endl;
                    break;
                case value_type::FLOAT:
                    otbfs << "float" << std::endl;
                    otbfs << 0 << std::endl;
                    break;
                case value_type::CHAR:
                    otbfs << "char" << std::endl;
                    otbfs << attr.length << std::endl;
                    break;
            }
            // Index info
            std::vector<std::pair<std::string, std::string>>::iterator ind;
            for(ind = tb.index.begin(); ind != tb.index.end(); ++ind){
                if(ind->first == tb.attribute_names[i]){
                    otbfs << 1 << std::endl << ind->second << std::endl;
                }
            }

            if (ind == tb.index.end()){
                otbfs << 0 << std::endl << "-" << std::endl;
            }
        }
        otbfs.close();
    }
    ofs.close();
}

void CatalogManager::LoadFromFile()
{
#ifdef LoadFromFile
    std::fstream fp;
    if(!fp){
        fp.open(meta_file_name, ios::out);
        return;
    }

    auto rm = Api::ApiHelper::getApiHelper()->getRecordManager();

    size_t table_cnt = 0;
    ifs >> table_cnt;


    std::string tb_name;
    for (auto i = 0; i < table_cnt; ++i)
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
#endif
}

bool CatalogManager::TableExist(const std::string &query_name) const
{
    for (const auto & table : tables){
        if(table.name == query_name){
            return true;
        }
    }
    return false;
}

macro::table &CatalogManager::GetTable(const std::string &query_name)
{
    for (auto & table : tables){
        if(table.name == query_name){
            return table;
        }
    }
}


bool CatalogManager::DropTableByName(const std::string &table_name)
{

    std::vector<macro::table>::iterator table_to_drop;
    for ( auto table = tables.begin(); table != tables.end(); ++table){
        if((*table).name == table_name){
            table_to_drop = table;
        }
    }
    if(table_to_drop != tables.end()){
        return true;
    }
    tables.erase(table_to_drop);
    return true;
}

macro::table &CatalogManager::GetTableWithIndex(const std::string &index_name)
{
    for (auto & table : tables){
        for (auto i = table.index.begin(); i != table.index.end(); ++i){
            if ((*i).second == index_name){
                return table;
            }
        }

    }
}

