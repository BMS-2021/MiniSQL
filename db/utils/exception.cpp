#include "exception.h"

std::ostream& operator<<(std::ostream& os, const sql_exception& e) {
    os << "ERROR " << e.code << " from <" << e.module_name << ">: " << e.detail;
    return os;
}
