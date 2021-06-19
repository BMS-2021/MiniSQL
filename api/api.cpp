#include "api.h"

#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../utils/exception.h"

extern RecordManager rec_mgt;
extern CatalogManager cat_mgt;

namespace api {
    static void check_table_exist(std::string &table_name) {
        if (cat_mgt.TableExist(table_name)) {
            throw sql_exception(100, "api", "table exists");
        }
    }

    void create_table::exec() {
        check_table_exist(this->table_name);

        // const auto attr_num = this->schema_list.size();
        for (const auto &i : this->schema_list) {
            if (i.type.type == value_type::CHAR && i.type.length == 0) {
                throw sql_exception(101, "api",
                                    "char \"" + i.name + "\" needs to have a length between 1 and 255");
            }
        }

        if (!this->primary_key.empty()) {
            // TODO
        }

        rec_mgt.creatTable(this->table_name);
        cat_mgt.CreateTable(this->table_name, this->schema_list, "" /*TODO*/);
    }

    void insert_table::exec() {
        check_table_exist(this->table_name);


    }

    void select_table::exec() {
        check_table_exist(this->table_name);

        auto table = cat_mgt.GetTable(this->table_name);


    }
}

