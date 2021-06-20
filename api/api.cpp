#include <iomanip>

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
            throw sql_exception(200, "api", "table \'" + table_name + "\' exists");
        }
    }
    static void throw_on_table_not_exist(std::string &table_name) {
        if (!cat_mgt.TableExist(table_name)) {
            throw sql_exception(201, "api", "table \'" + table_name + "\' not exist");
        }
    }

    static void print_select_result_seperator(const vector<unsigned long> &col_max_length) {
        const auto size = col_max_length.size();
        cout << " +-";
        for (auto i = 0; i < size; i++) {
            cout << std::setiosflags(ios::left)
                 << std::setw(static_cast<int>(col_max_length.at(i)))
                 << std::setfill('-')
                 <<"-";
            if (i < size - 1) {
                cout << "-+-";
            }
        }
        cout << "-+ ";
        cout << endl;
    }

    static void print_select_result(const macro::table &table, const result &res) {
        const auto size = table.attribute_names.size();
        auto col_max_length = vector<unsigned long>(size, 0);

        for (auto i = 0; i < size; i++) {
            col_max_length.at(i) = std::max(table.attribute_names.at(i).size(), col_max_length.at(i));
        }
        for (auto const &row : res.row) {
            for (auto i = 0; i < size; i++) {
                col_max_length.at(i) = std::max(row.col.at(i).size(), col_max_length.at(i));
            }
        }

        {
            print_select_result_seperator(col_max_length);
            cout << " | ";
            for (auto i = 0; i < size; i++) {
                cout << setiosflags(ios::left)
                    << setw(static_cast<int>(col_max_length.at(i)))
                    << std::setfill(' ')
                    << table.attribute_names.at(i);
                cout << " | ";
            }
            cout << endl;
            print_select_result_seperator(col_max_length);
        }

        {
            for (auto const &row : res.row) {
                cout << " | ";
                for (auto i = 0; i < size; i++) {
                    cout << setiosflags(ios::left)
                        << setw(static_cast<int>(col_max_length.at(i)))
                        << std::setfill(' ')
                        << row.col.at(i);
                    cout << " | ";
                }
                cout << endl;
            }
            print_select_result_seperator(col_max_length);
        }
        std::cout << std::to_string(res.row.size()) << " row(s) in set"<< std::endl;
        std::cout << endl;
    }

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
        std::cout << "insertion finished, 1 row affected" << std::endl;
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
    }

    void exit::exec() {
        buf_mgt.closeAllFile();
        std::cout << "Bye!" << std::endl;
        std::exit(0);
    }
}

