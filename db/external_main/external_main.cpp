extern "C"
{
#include "external_main.h"
}

#include "../interpreter/interpreter.h"
#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../IndexManager/IndexManager.h"

RecordManager rec_mgt;
CatalogManager cat_mgt;
BufferManager buf_mgt;
IndexManager idx_mgt;

// int main() {
//     std::cout << "Welcome to the MiniSQL monitor. Commands end with ;" << std::endl;
//     std::cout << "Version: v1.0-beta.1" << std::endl;
//     std::cout << "Copyright (c) 2021, Cupcake, Enzymii, L1ght and RalXYZ." << std::endl;
//
//     cat_mgt.LoadFromFile();
//     idx_mgt.load();
//     interpret_entrance();
// }

libminisql_resp external_main(const char *str) {

    // redirect cout to buffer
    std::stringstream buffer;
    auto old = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());

    auto code = sql_execution(str);

    // redirect cout back
    std::cout.rdbuf(old);

    // get buffer content
    auto buffer_content = buffer.str();
    auto buffer_c_str = buffer_content.c_str();
    auto res = reinterpret_cast<char*>(malloc(strlen(buffer_c_str) + 1));
    strcpy(res, buffer_c_str);

    // auto code = 0; // TODO: set status code during execution
    return libminisql_resp{ code, res };
}

char* get_table_names() {
    auto table_names = cat_mgt.get_table_names();
    auto reduced = std::string();
    for(auto x: table_names) {
        if(reduced.size() > 0) {
            reduced += "," + x;
        } else {
            reduced += x;
        }
    }

    auto reduced_c_str = reduced.c_str();
    auto res = reinterpret_cast<char*>(malloc(strlen(reduced_c_str) + 1));
    strcpy(res, reduced_c_str);

    return res;
}
