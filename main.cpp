#include <iostream>
#include "interpreter/interpreter.h"

int main() {
    std::cout << "Welcome to the MiniSQL monitor. Commands end with ;" << std::endl;
    std::cout << "Version: v0.1-alpha.1" << std::endl;
    std::cout << "Copyright (c) 2021, RalXYZ, Enzymii and others." << std::endl << std::endl;

    interpret_entrance();
}
