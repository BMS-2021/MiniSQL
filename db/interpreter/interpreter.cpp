#ifdef READLINE_FOUND
#include <readline/readline.h>
#include <readline/history.h>
extern "C" char **keyword_completion(const char *, int, int);
#endif

#include <iomanip>
#include <memory>
#include <fstream>
#include <chrono>

#include "../api/api.h"
#include "interpreter.h"
#include "../utils/utils.h"
#include "../utils/exception.h"

bool sig_exit = false;
std::string file_to_exec;
std::unique_ptr<sql_exception> parse_exception = nullptr;
std::unique_ptr<api::base> query_object_ptr = nullptr;

const auto PRECISION = 4;

/*
code:
1: exit
0: success
-1: error
 */
int sql_execution(const char *sql) {
    parse_exception = nullptr;

    parse(sql);

    auto query_exit_ptr = dynamic_cast<api::exit*>(query_object_ptr.get());

    auto start = std::chrono::system_clock::now();

    if (parse_exception != nullptr) {
        std::cout << *parse_exception;
        return -1;
    }
    if(query_object_ptr != nullptr) {
        try {
            query_object_ptr->exec();
        } catch (sql_exception& e) {
            std::cout << e;
            return -1;
        }
    }

    auto end = std::chrono::system_clock::now();

    std::cout << "("
        << std::fixed
        << std::setprecision(PRECISION)
        << static_cast<double>(duration_cast<std::chrono::microseconds>(end - start).count())
          * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
        << " sec)"
        << std::endl;

        std::cout.unsetf(std::ios::fixed);
        std::cout.precision(6);

    if (query_exit_ptr != nullptr) {
        return 1;
    }
    return 0;
}


void interpret_entrance() {
    while (!sig_exit) {
        std::cout << std::endl;
        interpreter inter;
        parse_exception = nullptr;

        if (file_to_exec.empty()) {
            auto flag = sql_execution(inter.read());
            if(flag == 1) {
                std::exit(0);
            }
        } else {  // execfile
            std::ifstream file(file_to_exec);
            if (file.fail()) {
                std::cout << sql_exception(101, "interpreter", "file \'" + file_to_exec + "\' not found");
            } else {
                while (!file.eof()) {
                    interpreter inter_inner;
                    auto flag = sql_execution(inter_inner.read(file));
                    if(flag == 1) {
                        std::exit(0);
                    }
                }
            }
            file_to_exec = "";
        }
    }
}

const char* interpreter::read() {
#ifdef READLINE_FOUND
    rl_attempted_completion_function = keyword_completion;
    if (this->str != nullptr) {
        std::free(this->str);
        this->str = nullptr;
    }
    const char *input = readline(this->start_text());
    add_history(input);
    return input;
#else
    while (true) {
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
        first_loop = false;
    }
    return this->str;
#endif
}

const char* interpreter::read(std::istream &is) {
    is.getline(this->str, SQL_QUERY_LENGTH, ';');
    strcat_s(this->str, ";", SQL_QUERY_LENGTH);
    return this->str;
}
