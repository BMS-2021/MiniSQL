#include "api.h"
#include "utils.h"

#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../utils/exception.h"

extern RecordManager rec_mgt;
extern CatalogManager cat_mgt;
extern BufferManager buf_mgt;
extern std::string file_to_exec;

namespace api {
    void create_table::exec() {
        throw_on_table_exist(this->table_name);

        // const auto attr_num = this->schema_list.size();
        for (const auto &i : this->schema_list) {
            if (i.type.type == value_type::CHAR && i.type.length == 0) {
                throw sql_exception(202, "api",
                                    "char \'" + i.name + "\' needs to have a length between 1 and 255");
            }
        }

        if (!this->primary_key.empty()) {
            // TODO
        }

        rec_mgt.creatTable(this->table_name);
        cat_mgt.CreateTable(this->table_name, this->schema_list, "" /*TODO*/);

        std::cout << "Query OK, 0 row affected" << std::endl;
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
        std::cout << "Query OK, 1 row affected" << std::endl;
    }

    void select_table::exec() {
        throw_on_table_not_exist(this->table_name);

        auto table = cat_mgt.GetTable(this->table_name);

        for (const auto &i : this->condition_list) {
            auto iter = std::find(table.attribute_names.begin(), table.attribute_names.end(), i.attribute_name);
            if (iter == table.attribute_names.end()) {
                throw sql_exception(204, "api", "attribute \'" + i.attribute_name + "\' not found");
            }
            if (table.attribute_type.at(iter - table.attribute_names.begin()).type != i.value.sql_type.type) {
                throw sql_exception(203, "api",
                                    "attribute \'" + i.attribute_name + "\' type error: except type_enum "
                                    + std::to_string(static_cast<int>(table.attribute_type.at(iter - table.attribute_names.begin()).type))
                                    + " , got type_enum "
                                    + std::to_string(static_cast<int>(i.value.sql_type.type)));
            }
        }

        // TODO: routine for index

        auto res = rec_mgt.selectRecord(table, this->attribute_list, this->condition_list);
        print_select_result(table, res);

        if (res.row.empty()) {
            std::cout << "Empty set" << std::endl;
        } else {
            std::cout << std::to_string(res.row.size()) << " row(s) in set"<< std::endl;
        }

    }

    void drop_table::exec() {
        throw_on_table_not_exist(this->table_name);
        auto table = cat_mgt.GetTable(this->table_name);

        // TODO: drop all indexes in this table

        // cat_mgt.RemoveTable(table); FIXME: implement this
        cat_mgt.Flush();

        rec_mgt.dropTable(this->table_name);
    }

    void execfile::exec() {
        file_to_exec = this->filename;
        std::cout << "context switched to file \'" << filename << "\'" << std::endl;
    }

    void exit::exec() {
        buf_mgt.closeAllFile();
        std::cout << "Bye!" << std::endl;
        std::exit(0);
    }
}

