#include <iostream>
#include "interpreter/interpreter.h"
#include "RecordManager/RecordManager.h"
#include "CatalogManager/CatalogManager.h"

RecordManager rec_mgt;
CatalogManager cat_mgt;
BufferManager buf_mgt;

int main() {
    std::cout << "Welcome to the MiniSQL monitor. Commands end with ;" << std::endl;
    std::cout << "Version: v0.1-alpha.1" << std::endl;
    std::cout << "Copyright (c) 2021, RalXYZ, Enzymii and others." << std::endl << std::endl;

    ::rec_mgt = RecordManager();
    ::cat_mgt = CatalogManager();

    interpret_entrance();
}
