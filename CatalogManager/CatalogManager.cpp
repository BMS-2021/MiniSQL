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
        :tables(std::vector<macro::table>())
{

}

CatalogManager::~CatalogManager()
{
    Flush();
}

void CatalogManager::Flush()
{
    std::ofstream ofs(tables_info);
    ofs << tables.size() << std::endl;

    for (auto &tb: tables){
        ofs << tb.name << std::endl << tb.record_cnt << std::endl;
        std::ofstream tab_ofs(tb.name + ".catalog");

        tab_ofs << tb.attribute_names.size() << std::endl;
        for(int i = 0; i < tb.attribute_names.size(); ++i){
            tab_ofs << tb.attribute_names[i] << std::endl;
            const auto &attr = tb.attribute_type[i];
            switch (attr.type){
                case value_type::INT:
                    tab_ofs << "int" << std::endl;
                    tab_ofs << 0 << std::endl;
                    break;
                case value_type::FLOAT:
                    tab_ofs << "float" << std::endl;
                    tab_ofs << 0 << std::endl;
                    break;
                case value_type::CHAR:
                    tab_ofs << "char" << std::endl;
                    tab_ofs << std::to_string(attr.length) << std::endl;
                    break;
            }
            tab_ofs << ((attr.primary) ? 1 : 0) << std::endl;
            tab_ofs << ((attr.unique) ? 1 : 0) << std::endl;

            // Index info
            std::vector<std::pair<std::string, std::string>>::iterator ind;
            for(ind = tb.index.begin(); ind != tb.index.end(); ++ind){
                if(ind->first == tb.attribute_names[i]){
                    tab_ofs << 1 << std::endl << ind->second << std::endl;
                }
            }
            if (ind == tb.index.end()){
                tab_ofs << 0 << std::endl << "-" << std::endl;
            }
        }
        tab_ofs.close();
    }
    ofs.close();
}

void CatalogManager::LoadFromFile()
{
    std::ifstream ifs(tables_info);
    if(!ifs){
        ifs.open(tables_info);
        return;
    }

    int table_size = 0;
    ifs >> table_size;

    for (int i = 0; i < table_size; ++i){
        macro::table tb;
        ifs >> tb.name >> tb.record_cnt;
        std::string tabfile_name = tb.name + ".catalog";
        std::ifstream tab_ifs(tabfile_name);

        unsigned long long record_len = 0, attr_counts = 0;

        tab_ifs >> attr_counts;
        for (int attr_i = 0; attr_i < attr_counts; ++attr_i){

            std::string attr_name, type_name, index_name;
            tab_ifs >> attr_name;
            tb.attribute_names.push_back(attr_name);

            int type_size;
            sql_value_type type;
            tab_ifs >> type_name >> type_size;

            if (type_name == "int"){
                type.type = value_type::INT;
            }
            else if (type_name == "char"){
                type.type = value_type::CHAR;
            }
            else if (type_name == "float"){
                type.type = value_type::FLOAT;
            }
            type.length = type_size;

            short ifpri, ifind, ifuniq;
            tab_ifs >> ifpri >> ifuniq;
            tab_ifs >> ifind >> index_name;

            record_len += type.size();
            type.primary = (ifpri) ? 1 : 0;
            type.unique = (ifuniq) ? 1 : 0;
            tb.attribute_type.push_back(type);

            //TODO: Implement real index creation in tb.index if ifind==1

        }
        tb.attribute_cnt = attr_counts;
        tb.record_len = record_len;
        tables.push_back(tb);

        //TODO: Create real index for whole table

    }
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

