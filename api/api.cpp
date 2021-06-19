#include "api.h"

#include "../RecordManager/RecordManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../utils/exception.h"

extern RecordManager rec_mgt;
extern CatalogManager cat_mgt;

namespace api {
    void create_table::exec() {
        if (cat_mgt.TableExist(this->table_name)) {
            throw sql_exception(100, "api", "table exists");
        }
        auto attr_num = this->schema_list.size();

    }
}

