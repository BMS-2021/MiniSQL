#include "../interpreter/interpreter.h"
#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../IndexManager/IndexManager.h"

RecordManager rec_mgt;
CatalogManager cat_mgt;
BufferManager buf_mgt;
IndexManager idx_mgt;

int main() {
    std::cout << "Welcome to the MiniSQL monitor. Commands end with ;" << std::endl;
    std::cout << "Version: v1.0-beta.1" << std::endl;
    std::cout << "Copyright (c) 2021, Cupcake, Enzymii, L1ght and RalXYZ." << std::endl;
    
    interpret_entrance();
}