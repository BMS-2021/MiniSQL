#include <iostream>
#include "interpreter/interpreter.h"
#include "RecordManager/RecordManager.h"
#include "CatalogManager/CatalogManager.h"

RecordManager rec_mgt;
CatalogManager cat_mgt;
BufferManager buf_mgt;

int main() {
    std::cout << "Welcome to the MiniSQL monitor. Commands end with ;" << std::endl;
    std::cout << "Version: v0.1-alpha.2" << std::endl;
    std::cout << "Copyright (c) 2021, Cupcake, Enzymii, L1ght and RalXYZ." << std::endl;

    cat_mgt.LoadFromFile();

    interpret_entrance();
}
