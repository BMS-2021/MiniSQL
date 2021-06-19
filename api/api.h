#ifndef MINISQL_API_H
#define MINISQL_API_H

#include <string>
#include <vector>

#include "../macro.h"

#include <string>
#include <vector>
#include <iostream>

#include "../macro.h"

namespace api {
    struct base {
        /*
         * If expression evaluates to a pointer to a base class subobject
         * of the object that was allocated with new, the destructor of
         * the base class must be virtual, otherwise the behavior is undefined.
         */
        virtual ~base() = default;

        virtual void exec() = 0;
    };

    class create_table final : public base {
        std::string table_name;
        std::vector<schema> schema_list;
        std::string primary_key;

        void exec() override;

    public:
        create_table(std::string& table_name,
                     std::vector<schema>& schema_list,
                     std::string& primary_key) :
                table_name(table_name), schema_list(schema_list), primary_key(primary_key) {}
    };

    class create_index final : public base {
        std::string index_name;
        std::string table_name;
        std::string attribute_name;

        void exec() override{};

    public:
        create_index(std::string& index_name,
                     std::string& table_name,
                     std::string& attribute_name) :
                index_name(index_name), table_name(table_name), attribute_name(attribute_name) {}
    };

    class drop_table final : public base {
        std::string table_name;

        inline void exec() override {
            std::cout << "Table to drop: " << table_name << std::endl;
            std::cout << "Interpreter Yes!!!" << std::endl;
        }

    public:
        explicit drop_table(std::string &table_name) : table_name(table_name) {}
    };

    class drop_index final : public base {
        std::string index_name;

        inline void exec() override {
            std::cout << "Interpreter Yes!!!" << std::endl;
        }

    public:
        explicit drop_index(std::string &index_name) : index_name(index_name) {}
    };

    class insert_table final : public base {
        std::string table_name;
        std::vector<sql_value> insert_list;


        void exec() override;

    public:
        insert_table(std::string& table_name,
                     std::vector<sql_value>& insert_list) :
                table_name(table_name), insert_list(insert_list) {}
    };

    class select_table final : public base {
        std::vector<std::string> attribute_list;
        std::string table_name;
        std::vector<condition> condition_list;


        void exec() override;

    public:
        select_table(std::vector<std::string>& attribute_list,
                     std::string& table_name,
                     std::vector<condition>& condition_list) :
                attribute_list(attribute_list), table_name(table_name), condition_list(condition_list) {}
    };

    class use_database final : public base {
        std::string database_name;

        inline void exec() override {

        }

    public:
        explicit use_database(std::string &database_name) : database_name(database_name) {}
    };

    class exit final : public base {
        inline void exec() override {
            std::cout << "Bye!" << std::endl;
            std::exit(0);
        }
    };
}

#endif //MINISQL_API_H
