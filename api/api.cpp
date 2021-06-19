#include "api.h"

#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../utils/exception.h"

extern RecordManager rec_mgt;
extern CatalogManager cat_mgt;
extern BufferManager buf_mgt;

namespace api {
    static void throw_on_table_exist(std::string &table_name) {
        if (cat_mgt.TableExist(table_name)) {
            throw sql_exception(100, "api", "table \"" + table_name + "\" exists");
        }
    }
    static void throw_on_table_not_exist(std::string &table_name) {
        if (!cat_mgt.TableExist(table_name)) {
            throw sql_exception(101, "api", "table \"" + table_name + "\" not exist");
        }
    }

    void create_table::exec() {
        throw_on_table_exist(this->table_name);

        // const auto attr_num = this->schema_list.size();
        for (const auto &i : this->schema_list) {
            if (i.type.type == value_type::CHAR && i.type.length == 0) {
                throw sql_exception(102, "api",
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
        throw_on_table_not_exist(this->table_name);

        auto table = cat_mgt.GetTable(this->table_name);
        if (table.attribute_names.size() != this->insert_list.size()) {
            throw sql_exception(103, "api",
                                "insert number mismatch: expect "
                                + std::to_string(table.attribute_names.size())
                                + " attributes, got "
                                + std::to_string(this->insert_list.size()));
        }

        // TODO: validate

        // TODO: index

        // TODO: check unique and duplication

        sql_tuple tuple;
        tuple.element = this->insert_list;
        rec_mgt.insertRecord(table, tuple);

        // TODO: update index

        table.record_cnt++;
        std::cout << "insertion finished, 1 row affected";
    }

    void select_table::exec() {
        throw_on_table_not_exist(this->table_name);

        auto table = cat_mgt.GetTable(this->table_name);

        for (const auto &i : this->condition_list) {
            auto iter = std::find(table.attribute_names.begin(), table.attribute_names.end(), i.attribute_name);
            if (iter == table.attribute_names.end()) {
                throw sql_exception(104, "api", "attribute \"" + i.attribute_name + "\" not found");
            }
            if (table.attribute_type.at(iter - table.attribute_names.begin()).type != i.value.sql_type.type) {
                throw sql_exception(103, "api",
                                    "attribute \"" + i.attribute_name + "\" type error: except type_enum "
                                    + std::to_string(static_cast<int>(table.attribute_type.at(iter - table.attribute_names.begin()).type))
                                    + " , got type_enum "
                                    + std::to_string(static_cast<int>(i.value.sql_type.type)));
            }
        }

        // TODO: routine for index
        rec_mgt.selectRecord(table, this->attribute_list, this->condition_list);

    }

    void exit::exec() {
        buf_mgt.closeAllFile();
        std::cout << "Bye!" << std::endl;
        std::exit(0);
    }
}

