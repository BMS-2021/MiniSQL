#ifndef MINISQL_MACRO_H
#define MINISQL_MACRO_H

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <compare>
#include <stdexcept>

enum class value_type {
    INT,
    CHAR,  //string
    FLOAT,
};

struct sql_value_type {
    value_type type;
    uint8_t length = 0;
    bool primary = false, unique = false;
    unsigned long size_of_char = 0;
    unsigned long size() const{
        switch(type){
            case value_type::INT:
                return sizeof(int);
            case value_type::FLOAT:
                return sizeof(float);
            case value_type::CHAR:
                return size_of_char + 1;
        }
    }

    sql_value_type() = default;
    sql_value_type(value_type type) : type(type) {}
    sql_value_type(uint8_t length) : type(value_type::CHAR), length(length) {}
};

struct sql_value {
    sql_value_type sql_type;
    int sql_int;
    float sql_float;
    std::string sql_str;

    sql_value() = default;
    sql_value(int i) : sql_int(i), sql_type(value_type::INT) {};
    sql_value(float f) : sql_float(f), sql_type(value_type::FLOAT) {};
    sql_value(std::string& str) : sql_str(str), sql_type(value_type::CHAR) {};

    void reset() {
        sql_int = 0;
        sql_float = 0;
        sql_str.clear();
    }

    std::string toStr() const {
        switch (sql_type.type) {
            case value_type::INT:
                return std::to_string(sql_int);
            case value_type::FLOAT:
                return std::to_string(sql_float);
            case value_type::CHAR:
                return this->sql_str;
        }
    }

     bool operator<(const sql_value &e) const {
        switch (sql_type.type) {
            case value_type::INT:
                return sql_int < e.sql_int;
            case value_type::FLOAT:
                return sql_float < e.sql_float;
            case value_type::CHAR:
                return sql_str < e.sql_str;
            default:
                throw std::runtime_error("Undefined Type!");
        }
    }

    bool operator==(const sql_value &e) const {
        switch (sql_type.type) {
            case value_type::INT:
                return sql_int == e.sql_int;
            case value_type::FLOAT:
                return sql_float == e.sql_float;
            case value_type::CHAR:
                return sql_str == e.sql_str;
            default:
                throw std::runtime_error("Undefined Type!");
        }
    }

    bool operator!=(const sql_value &e) const { return !operator==(e); }

    bool operator>(const sql_value &e) const { return !operator<(e) && operator!=(e); }

    bool operator<=(const sql_value &e) const { return operator<(e) || operator==(e); }

    bool operator>=(const sql_value &e) const { return !operator<(e); }
};

struct schema {
    std::string name;
    sql_value_type type;
    bool unique;

    schema() = default;
    schema(std::string& name, sql_value_type& type, bool unique) : name(name), type(type), unique(unique) {}
};

enum class attribute_operator {
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
};

struct condition {
    std::string attribute_name;
    attribute_operator op;
    sql_value value;  //FIXME!!! chang type to sql_value

    condition() = default;
    condition(std::string& attribute_name, attribute_operator op, sql_value& value) :
    attribute_name(attribute_name),
    op(op),
    value(value) {}

    bool test(const sql_value &e) const {
        switch (op) {
            case attribute_operator::EQUAL:
                return e == value;
            case attribute_operator::NOT_EQUAL:
                return e != value;
            case attribute_operator::GREATER:
                return e > value;
            case attribute_operator::GREATER_EQUAL:
                return e >= value;
            case attribute_operator::LESS:
                return e < value;
            case attribute_operator::LESS_EQUAL:
                return e <= value;
            default:
                std::cerr << "Undefined condition width cond !" << std::endl;
        }
    }
};

struct row {
    std::vector<std::string> col;
};

struct result {
    std::vector<struct row> row;
};

struct sql_tuple {
    std::vector<sql_value> element;

    row fetchRow(const std::vector<std::string> &attrTable, const std::vector<std::string> &attrFetch) const {
        row row;
        bool attrFound;
        row.col.reserve(attrFetch.size());
        for (auto fetch : attrFetch) {
            attrFound = false;
            for (int i = 0; i < attrTable.size(); i++) {
                if (fetch == attrTable[i]) {
                    row.col.push_back(element[i].toStr());
                    attrFound = true;
                    break;
                }
            }
            if (!attrFound) {
                std::cerr << "Undefined attr in row fetching!!" << std::endl;
            }
        }
        return row;
    }

//    const sql_value_type &fetchElement(const std::vector<std::string> &attrTable, const std::string &attrFetch) const {
//        for (int i = 0; i < attrTable.size(); i++) {
//            if (attrFetch == attrTable[i]) {
//                return element[i];
//            }
//        }
//        std::cerr << "Undefined attr in element fetching from tuple!!" << std::endl;
//    }
};

namespace macro {

    const int BlockSize = 4096;
    const int MaxBlocks = 128;
    const int MaxFiles = 6;

    inline std::string tableFile(const std::string &tableName) { return tableName + ".tb"; }

    inline std::string indexFile(const std::string &tableName, const std::string &index) {
        return tableName + "_" + index + ".ind";
    }

    struct table {
        table() {};
        table(std::string table_name, int attr_cnt)
            :name(table_name), attribute_cnt(attr_cnt){};
    /*        table(const table &T) : name(T.name), attribute_cnt(T.attribute_cnt), record_len(T.record_len),
                                    record_cnt(T.record_cnt), size(T.size), attribute_type(T.attribute_type), attribute_names(T.attribute_names),
                                    indexNames(T.indexNames) {};*/

        std::string name;
        int attribute_cnt, record_len, record_cnt, size;

        std::vector<sql_value_type> attribute_type;
        std::vector<std::string> attribute_names;
        // for index, first stands for attr name, second stands for index name.
        std::vector<std::pair<std::string, std::string>> index;

        friend std::ostream &operator<<(std::ostream &os, const table &tab) {
            os << "name: " << tab.name << " attribute_cnt: " << tab.attribute_cnt << " record_len: " << tab.record_len
               << " record_cnt: " << tab.record_cnt << " size: " << tab.size
               << " attribute_names: " << tab.attribute_names.size();
            return os;
        }
    };
}

#endif //MINISQL_MACRO_H
