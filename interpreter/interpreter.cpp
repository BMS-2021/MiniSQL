#ifdef READLINE_FOUND
#include <readline/readline.h>
#endif

#include <memory>
#include <fstream>
#include "../api/api.h"
#include "interpreter.h"
#include "../utils/utils.h"
#include "../utils/exception.h"

bool sig_exit = false;
std::string file_to_exec;
std::unique_ptr<sql_exception> parse_exception = nullptr;
std::unique_ptr<api::base> query_object_ptr = nullptr;

void interpret_entrance() {
    while (!sig_exit) {
        std::cout << std::endl;
        interpreter inter;
        parse_exception = nullptr;

        if (file_to_exec.empty()) {
            parse(inter.read());

            if (parse_exception != nullptr) {
                std::cout << *parse_exception << std::endl;
                continue;
            }

            if (query_object_ptr != nullptr) {
                try {
                    query_object_ptr->exec();
                } catch (sql_exception &e) {
                    std::cout << "awsl" << std::endl;
                    std::cout << e << std::endl;
                }
            }
        } else {  // execfile
            std::ifstream file(file_to_exec);
            while (!file.eof()) {
                parse_exception = nullptr;

                interpreter inter_inner;
                parse(inter_inner.read(file));


                if (parse_exception != nullptr) {
                    std::cout << *parse_exception << std::endl;
                    continue;
                }
                if (query_object_ptr != nullptr) {
                    try {
                        query_object_ptr->exec();
                    } catch (sql_exception &e) {
                        std::cout << e << std::endl;
                    }
                }
            }
            file_to_exec = "";
        }
    }
}

const char* interpreter::read() {
    while (true) {
#ifdef READLINE_FOUND
        char *src = readline(this->start_text());
        if (!first_loop) {
            strcat_s(this->str, "\n", SQL_QUERY_LENGTH);
        }
        strcat_s(this->str, src, SQL_QUERY_LENGTH);
        if (std::strlen(src) >= 1 && src[std::strlen(src) - 1] == ';') {
            std::free(src);
            break;
        }
        std::free(src);
#else
        std::string src;
        std::cout << this->start_text();
        std::getline(std::cin, src);
        while (src.ends_with(' ')) {
            src.pop_back();
        }
        if (!first_loop) {
            strcat_s(this->str, "\n", SQL_QUERY_LENGTH);
        }
        strcat_s(this->str, src.c_str(), SQL_QUERY_LENGTH);
        if (src.length() >= 1 && *src.rbegin() == ';') {
            break;
        }
#endif
        first_loop = false;
    }
    return this->str;
}

const char* interpreter::read(std::istream &is) {
    is.getline(this->str, SQL_QUERY_LENGTH, ';');
    strcat_s(this->str, ";", SQL_QUERY_LENGTH);
    return this->str;
}
