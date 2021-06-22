#include "api.h"
#include "utils.h"

#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../IndexManager/IndexManager.h"
#include "../utils/exception.h"

extern RecordManager rec_mgt;
extern CatalogManager cat_mgt;
extern BufferManager buf_mgt;
extern IndexManager idx_mgt;
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

        // create index for primary key
        if (!this->primary_key.empty()) {
            bool pk_is_valid = false;
            value_type pk_type;
            for (const auto &i : this->schema_list) {
                if (i.name == this->primary_key) {
                    pk_is_valid = true;
                    pk_type = i.type.type;
                    break;
                }
            }

            if (!pk_is_valid) {
                throw sql_exception(206, "api", "invalid primary key \'" + primary_key + "\'");
            }

            idx_mgt.create(this->table_name + ".pk", pk_type);
        }

        rec_mgt.creatTable(this->table_name);
        cat_mgt.CreateTable(this->table_name, this->schema_list, this->primary_key);

        std::cout << "query OK, 0 row affected";
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

        // idx_mgt.insert();

        // TODO: check unique and duplication

        sql_tuple tuple;
        tuple.element = this->insert_list;
        rec_mgt.insertRecord(table, tuple);

        // TODO: update index

        table.record_cnt++;
        std::cout << "query OK, 1 row affected";
    }

    void select_table::exec() {
        throw_on_table_not_exist(this->table_name);

        auto table = cat_mgt.GetTable(this->table_name);

        validate_condition(table, this->condition_list);

        // TODO: routine for index

        auto res = rec_mgt.selectRecord(table, this->attribute_list, this->condition_list);
        print_select_result(table, res);

        if (res.row.empty()) {
            std::cout << "empty set";
        } else {
            std::cout << res.row.size() << " row(s) in set";
        }

    }

    void delete_table::exec() {
        throw_on_table_not_exist(this->table_name);
        auto table = cat_mgt.GetTable(this->table_name);

        validate_condition(table, this->condition_list);

        auto delete_row_count = rec_mgt.deleteRecord(table, this->condition_list);
        if (delete_row_count == -1) {
            throw sql_exception(205, "api", "delete failed");
        }
        std::cout << "query OK, " << delete_row_count << " row(s) affected";
    }

    void drop_table::exec() {
        throw_on_table_not_exist(this->table_name);
        auto table = cat_mgt.GetTable(this->table_name);

        // TODO: drop all indexes in this table
        cat_mgt.DropTableByName(this->table_name);

        cat_mgt.Flush();

        rec_mgt.dropTable(this->table_name);
    }

    void execfile::exec() {
        file_to_exec = this->filename;
        std::cout << "context switched to file \'" << filename << "\'";
    }

    void exit::exec() {
        buf_mgt.closeAllFile();
        cat_mgt.Flush();
        std::cout << "bye!" << std::endl;
        std::exit(0);
    }
}

