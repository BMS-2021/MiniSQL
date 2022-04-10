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

libminisql_resp external_main(char *str) {

    // redirect cout to buffer
    std::stringstream buffer;
    auto old = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());

    external_execute(str);

    // redirect cout back
    std::cout.rdbuf(old);

    // get buffer content
    auto buf = buffer.str().c_str();
    auto res = reinterpret_cast<char*>(malloc(strlen(buf) + 1));
    strcpy(res, buf);

    auto code; // TODO: set status code during execution
    return libminisql_resp{ code, res };
}
