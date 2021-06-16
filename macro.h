#ifndef MINISQL_MACRO_H
#define MINISQL_MACRO_H

#include <variant>
#include <iostream>

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
    void setPrimary(){
        primary = true;
    }

    void setUnique(){
        unique = true;
    }
    sql_value_type() = default;
    sql_value_type(value_type type) : type(type) {}
    sql_value_type(uint8_t length) : type(value_type::CHAR), length(length) {}
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
    std::variant<int, float, std::string> value;

    condition() = default;
    condition(std::string& attribute_name, attribute_operator op, std::variant<int, float, std::string>& value) :
    attribute_name(attribute_name),
    op(op),
    value(value) {}
};

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
    /// for index, first stands for attr name, second stands for index name.
    std::vector<std::pair<std::string, std::string>> index;

    friend std::ostream &operator<<(std::ostream &os, const table &tab) {
        os << "name: " << tab.name << " attribute_cnt: " << tab.attribute_cnt << " record_len: " << tab.record_len
           << " record_cnt: " << tab.record_cnt << " size: " << tab.size
           << " attribute_names: " << tab.attribute_names.size();
        return os;
    }
};


#endif //MINISQL_MACRO_H
