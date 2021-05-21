#ifndef MINISQL_MODEL_H
#define MINISQL_MODEL_H

#include <string>
#include <vector>
#include <iostream>

#include "../macro.h"

namespace query {
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

        inline void exec() override {
            std::cout << "Interpreter Yes!!!" << std::endl;
        }

    public:
        create_table(std::string& table_name,
                     std::vector<schema>& schema_list,
                     std::string& primary_key) :
                     table_name(table_name), schema_list(schema_list), primary_key(primary_key) {}
    };

    class create_index final : public base {
        std::string index_name;
        std::string element_name;

        inline void exec() override {
            std::cout << "Interpreter Yes!!!" << std::endl;
        }
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

#endif //MINISQL_MODEL_H
