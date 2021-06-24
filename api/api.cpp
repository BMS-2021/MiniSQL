#include "api.h"
#include "utils.h"
#include <unordered_set>
#include <unordered_map>

#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../IndexManager/IndexManager.h"
#include "../utils/exception.h"
#include "../utils//utils.h"

extern RecordManager rec_mgt;
extern CatalogManager cat_mgt;
extern BufferManager buf_mgt;
extern IndexManager idx_mgt;
extern std::string file_to_exec;

namespace api {
    void base::exec() {
        throw sql_exception(209, "api", "no query specified");
    }

    void create_table::exec() {
        throw_on_table_exist(this->table_name);

        std::unordered_set<std::string> attribute_duplication_check;

        for (const auto &i : this->schema_list) {
            if (i.type.type == value_type::CHAR && i.type.length == 0) {
                throw sql_exception(202, "api",
                                    "char \'" + i.name + "\' needs to have a length between 1 and 255");
            }
            if (attribute_duplication_check.contains(i.name)) {
                throw sql_exception(210, "api", "duplicate column name \'" + i.name + "\'");
            } else {
                attribute_duplication_check.insert(i.name);
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
#ifndef DETACH_INDEX_MANAGER
            idx_mgt.create(generate_table_pk_name(this->table_name));
#endif
        }

        rec_mgt.creatTable(this->table_name);
        cat_mgt.CreateTable(this->table_name, this->schema_list, this->primary_key);

        std::cout << "query OK, 0 row affected";
    }

    void insert_table::exec() {
        throw_on_table_not_exist(this->table_name);
        auto& table = cat_mgt.GetTable(this->table_name);

        // validate input amount
        if (table.attribute_names.size() != this->insert_list.size()) {
            throw sql_exception(203, "api",
                                "insert attribute amount mismatch: needs "
                                + std::to_string(table.attribute_names.size())
                                + " attributes, got "
                                + std::to_string(this->insert_list.size()));
        }

        // validate the input  type
        {
            for (int i = 0; i < this->insert_list.size(); i++) {
                if (table.attribute_type.at(i).type != this->insert_list.at(i).sql_type.type) {
                    throw sql_exception(208, "api",
                                        "insert attribute type mismatch: at attribute "
                                        + std::to_string(i)
                                        + ", needs "
                                        + return_value_name(table.attribute_type.at(i).type)
                                        + ", got "
                                        + return_value_name(this->insert_list.at(i).sql_type.type));
                }
            }
        }

        // check unique and duplication
        {
            std::vector<condition> duplication_check_list;
            for (int i = 0; i < table.attribute_names.size(); i++) {
                if (table.attribute_type.at(i).unique) {
                    duplication_check_list.emplace_back(table.attribute_names.at(i),
                                                               attribute_operator::EQUAL,
                                                               this->insert_list.at(i));
                }
            }
            for (const auto &i: duplication_check_list) {
                std::vector<condition> cond;
                cond.push_back(i);
                auto res = rec_mgt.selectRecord(table, vector<std::string>(), cond);
                if(!res.row.empty()) {
                    throw sql_exception(207, "api",
                                        "there exists a duplicated value on attribute \'"
                                        + i.attribute_name +"\'");
                }
            }
        }

        sql_tuple tuple;
        tuple.element = this->insert_list;
        auto index_row_in_record = rec_mgt.insertRecord(table, tuple);
#ifndef DETACH_INDEX_MANAGER
        for (const auto &i : table.index) {
            // i : pair<attr, index>
            int j = 0;
            for (; j < table.attribute_names.size(); j++) {
                if (i.first == table.attribute_names.at(j)) {
                    break;
                }
            }
            if (j == table.attribute_names.size()) {
                throw sql_exception(211, "api",
                                    "attribute \'"
                                    + i.first
                                    + "\' of index \'"
                                    + i.second
                                    + "cannot be found in table \'"
                                    + table_name
                                    + "\'");
            }
            auto value = rec_mgt.getRecord(table, index_row_in_record).element.at(j);
            idx_mgt.insert(i.second, value, j, table, index_row_in_record);
        }
#endif
        table.record_cnt++;
        std::cout << "query OK, 1 row affected";
    }

    void select_table::exec() {
        throw_on_table_not_exist(this->table_name);

        auto& table = cat_mgt.GetTable(this->table_name);

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
        auto& table = cat_mgt.GetTable(this->table_name);

        validate_condition(table, this->condition_list);

        auto delete_row_count = rec_mgt.deleteRecord(table, this->condition_list);
        if (delete_row_count.empty()) {
            throw sql_exception(205, "api", "delete failed");
        }

#ifndef DETACH_INDEX_MANAGER
        unordered_map<uint32_t, sql_tuple> umap;
        for (const auto &x: delete_row_count)
            umap.insert(x);

        for (const auto &i : table.index) {
            // i : pair<attr, index>
            int j = 0;
            for (; j < table.attribute_names.size(); j++) {
                if (i.first == table.attribute_names.at(j)) {
                    break;
                }
            }
            if (j == table.attribute_names.size()) {
                throw sql_exception(211, "api",
                                    "attribute \'"
                                    + i.first
                                    + "\' of index \'"
                                    + i.second
                                    + "cannot be found in table \'"
                                    + table_name
                                    + "\'");
            }
            // TODO: remove all deleted rows in B+ tree
            for(auto &x: delete_row_count)
                idx_mgt.remove(i.second, x.second.element.at(j), j, table, umap);
        }
#endif

        std::cout << "query OK, " << delete_row_count.size() << " row(s) affected";
    }

    void drop_table::exec() {
        throw_on_table_not_exist(this->table_name);
        auto table = cat_mgt.GetTable(this->table_name);

#ifndef DETACH_INDEX_MANAGER
        // drop every index in the table
        for (auto & i : table.index) {
            idx_mgt.drop(i.second);
        }
#endif

        cat_mgt.DropTableByName(this->table_name);
        cat_mgt.Flush();

        rec_mgt.dropTable(this->table_name);

        std::cout << "table \'" << this->table_name << "\' has been dropped";
    }

    void create_index::exec() {
        throw_on_table_not_exist(this->table_name);
        auto& table = cat_mgt.GetTable(this->table_name);

        cat_mgt.CreateIndex(table, this->attribute_name, this->index_name);
#ifndef DETACH_INDEX_MANAGER
        auto record_row_tuple_pairs = rec_mgt.getRecordPairs(table);
        for (const auto &i : table.index) {
            // i : pair<attr, index>
            int j = 0;
            for (; j < table.attribute_names.size(); j++) {
                if (i.first == table.attribute_names.at(j)) {
                    break;
                }
            }
            if (j == table.attribute_names.size()) {
                throw sql_exception(211, "api",
                                    "attribute \'"
                                    + i.first
                                    + "\' of index \'"
                                    + i.second
                                    + "cannot be found in table \'"
                                    + table_name
                                    + "\'");
            }

            auto record_to_insert = std::vector<std::pair<uint32_t, sql_value>>();
            for (const auto &iter : record_row_tuple_pairs) {
                record_to_insert.emplace_back(iter.first, iter.second.element.at(j));
            }
            idx_mgt.create(this->index_name, record_to_insert, j, table);
        }
#endif
        std::cout << "index \'" << this->index_name << "\' created on \'" << this->table_name << "." << this->attribute_name << "\'";
    }

    void drop_index::exec() {
        cat_mgt.DropIndex(this->index_name);
#ifndef DETACH_INDEX_MANAGER
        idx_mgt.drop(this->index_name);
#endif
        std::cout << "index \'" << this->index_name << "\' dropped";
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

