#include <iomanip>

#include "utils.h"

#include "../CatalogManager/CatalogManager.h"
#include "../utils/exception.h"

extern CatalogManager cat_mgt;

namespace api {
    void throw_on_table_exist(std::string &table_name) {
        if (cat_mgt.TableExist(table_name)) {
            throw sql_exception(200, "api", "table \'" + table_name + "\' exists");
        }
    }
    void throw_on_table_not_exist(std::string &table_name) {
        if (!cat_mgt.TableExist(table_name)) {
            throw sql_exception(201, "api", "table \'" + table_name + "\' doesn't exist");
        }
    }

    void print_select_result_seperator(const std::vector<unsigned long> &col_max_length) {
        using namespace std;
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

    void print_select_result(const macro::table &table, const result &res) {
        using namespace std;
        const auto size = table.attribute_names.size();

        if (res.row.empty()) {
            return;
        }

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
    }

    void validate_condition(const macro::table &table, const std::vector<condition> &condition) {
        for (const auto &i : condition) {
            auto iter = std::find(table.attribute_names.begin(), table.attribute_names.end(), i.attribute_name);
            if (iter == table.attribute_names.end()) {
                throw sql_exception(204, "api", "attribute \'" + i.attribute_name + "\' not found");
            }
            if (table.attribute_type.at(iter - table.attribute_names.begin()).type != i.value.sql_type.type) {
                throw sql_exception(203, "api",
                                    "attribute \'" + i.attribute_name + "\' type error: except "
                                    + return_value_name(table.attribute_type.at(iter - table.attribute_names.begin()).type)
                                    + " , got "
                                    + return_value_name(i.value.sql_type.type));
            }
        }
    }

    std::string return_value_name(value_type v) {
        switch (v) {
            case value_type::CHAR:
                return "CHAR";
            case value_type::INT:
                return "INT";
            case value_type::FLOAT:
                return "FLOAT";
        }
    }

    result convert_sql_tuple_to_result(
        const std::vector<std::string> &attrTable, 
        const std::vector<std::string> &attrFetch,
        const sql_tuple& tup
    ) {
        tup.fetchRow(attrTable, attrFetch);
    }
}
