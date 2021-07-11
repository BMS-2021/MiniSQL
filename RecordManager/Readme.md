## Record Manager module design report

Writer: 3190104509 Gao Weiyuan

### Overview

`RecordManager `  manages records in database. It supports three major operations about records: insertion, selection and deletion. Also, it offer interfaces to `API` and `IndexManager`.

### Module Design

`RecordManager` is responsible for organizing data in data block returned by `BufferManager`. We store each table in a single file. Because every records in one table has the fixed length and records don't need to across blocks, we can store records in a simple way. We write the records into block linerly, and for ease of deleting, I add a valid sign at the head of the record. The valid sign being zero represents this record is deleted.

When traversing a table, we firstly the maxmun blockID through `BufferManager`, then we request block one by one. For each block, we traverse the records in it. Because the length of the record is fixed, we easily find the head of each record and then get the information.

### implementation

- Data Structure

  - struct table is defined as follow:

    ```c++
    struct table {
        table() {};
        table(std::string table_name, int attr_cnt)
            :name(table_name), attribute_cnt(attr_cnt){};
    /*        table(const table &T) : name(T.name), attribute_cnt(T.attribute_cnt), record_len(T.record_len),
                                    record_cnt(T.record_cnt), size(T.size), attribute_type(T.attribute_type), attribute_names(T.attribute_names),
                                    indexNames(T.indexNames) {};*/
    
        std::string name;
        int attribute_cnt, record_len, record_cnt, size;
    
        std::vector<sql_value_type> attribute_type;
        std::vector<std::string> attribute_names;
        // for index, first stands for attr name, second stands for index name.
        std::vector<std::pair<std::string, std::string>> index;
    
        friend std::ostream &operator<<(std::ostream &os, const table &tab) {
            os << "name: " << tab.name << " attribute_cnt: " << tab.attribute_cnt << " record_len: " << tab.record_len
               << " record_cnt: " << tab.record_cnt << " size: " << tab.size
               << " attribute_names: " << tab.attribute_names.size();
            return os;
        }
    };
    ```

  - sql_value
    each item in recors is a sql_value

    ```c++
    struct sql_value {
        sql_value_type sql_type;
        int sql_int;
        float sql_float;
        std::string sql_str;
    
        sql_value() = default;
        sql_value(int i) : sql_int(i), sql_type(value_type::INT) {};
        sql_value(float f) : sql_float(f), sql_type(value_type::FLOAT) {};
        sql_value(std::string& str) : sql_str(str), sql_type(value_type::CHAR) {};
    
        void reset() {
            sql_int = 0;
            sql_float = 0;
            sql_str.clear();
        }
    
        std::string toStr() const {
            switch (sql_type.type) {
                case value_type::INT:
                    return std::to_string(sql_int);
                case value_type::FLOAT:
                    return std::to_string(sql_float);
                case value_type::CHAR:
                    return this->sql_str;
            }
        }
    
         bool operator<(const sql_value &e) const {
            switch (sql_type.type) {
                case value_type::INT:
                    return sql_int < e.sql_int;
                case value_type::FLOAT:
                    return sql_float < e.sql_float;
                case value_type::CHAR:
                    return sql_str < e.sql_str;
                default:
                    throw std::runtime_error("Undefined Type!");
            }
        }
    
        bool operator==(const sql_value &e) const {
            switch (sql_type.type) {
                case value_type::INT:
                    return sql_int == e.sql_int;
                case value_type::FLOAT:
                    return sql_float == e.sql_float;
                case value_type::CHAR:
                    return sql_str == e.sql_str;
                default:
                    throw std::runtime_error("Undefined Type!");
            }
        }
    
        bool operator!=(const sql_value &e) const { return !operator==(e); }
    
        bool operator>(const sql_value &e) const { return !operator<(e) && operator!=(e); }
    
        bool operator<=(const sql_value &e) const { return operator<(e) || operator==(e); }
    
        bool operator>=(const sql_value &e) const { return !operator<(e); }
    };
    ```

  - sql_value_type

    the type of the value is a struct sql_value_type:

    ```c++
    struct sql_value_type {
        value_type type;
        uint8_t length = 0;
        bool primary = false, unique = false;
        uint8_t size() const{
            switch(type){
                case value_type::INT:
                    return sizeof(int);
                case value_type::FLOAT:
                    return sizeof(float);
                case value_type::CHAR:
                    return length + 1;
            }
        }
    
        sql_value_type() = default;
        sql_value_type(value_type type) : type(type) {}
        sql_value_type(uint8_t length) : type(value_type::CHAR), length(length) {}
    };
    ```

  - sql_tuple
    A sql_tuple is used to represent a record, it is a vector of sql_value essentially.

    ```c++
    struct sql_tuple {
        std::vector<sql_value> element;
    
        /*
         * attrTable: all columns
         * attrFetch: columns to be selected
         */
        row fetchRow(const std::vector<std::string> &attrTable, const std::vector<std::string> &attrFetch) const {
            row row;
            bool attrFound;
            row.col.reserve(attrFetch.size());
            for (auto fetch : attrFetch) {
                attrFound = false;
                for (int i = 0; i < attrTable.size(); i++) {
                    if (fetch == attrTable[i]) {
                        row.col.push_back(element[i].toStr());
                        attrFound = true;
                        break;
                    }
                }
                if (!attrFound) {
                    std::cerr << "Undefined attr in row fetching!!" << std::endl;
                }
            }
            return row;
        }
    };
    ```

  - Condition
    To represent conditons of select and deletion statement, we also define condition struct.

    ```c++
    struct condition {
        std::string attribute_name;
        attribute_operator op;
        sql_value value;
    
        condition() = default;
        condition(std::string& attribute_name, attribute_operator op, sql_value& value) :
        attribute_name(attribute_name),
        op(op),
        value(value) {}
    
        bool test(const sql_value &e) const {
            switch (op) {
                case attribute_operator::EQUAL:
                    return e == value;
                case attribute_operator::NOT_EQUAL:
                    return e != value;
                case attribute_operator::GREATER:
                    return e > value;
                case attribute_operator::GREATER_EQUAL:
                    return e >= value;
                case attribute_operator::LESS:
                    return e < value;
                case attribute_operator::LESS_EQUAL:
                    return e <= value;
                default:
                    std::cerr << "Undefined condition width cond !" << std::endl;
            }
        }
    };
    ```

- Major Function

  - Creat Table

    ```c++
    bool creatTable(const string &tableName);
    ```

    tableName: the name of table.

    function: Creat data file via `BufferManager`. ``BufferManager`` will create a file which type is  ``.tb`` 

  - Drop Table

    ```c++
    bool dropTable(const string &tableName);
    ```

    tableName: the name of table.

    function: Remove data file via `BufferManager`, ``BufferManager`` firstly call closeFile() to close the file, then remove the file in disk.

  - Insert Record

    ```c++
    int insertRecord(const macro::table &table, const sql_tuple &record);
    ```

    table: the table instance

    record: the record to be insert

    return: the insert position in the table

    function: Insert a record. Records are stored in data file block by block. Records in the same table will be convert to a fixed-length and won't be stored across the block. This function traverse the records one by one until it find a record whose valid sign is zero. Then put the record into that slot and return the position.

  - Delete Record

    ```c++
    vector<pair<uint32_t, sql_tuple>> deleteRecord(const macro::table &table, const vector<condition> &conditions);
    ```

    table:  the table instance

    conditions: the delete conditions

    return: a vector, each element in vector is a pair which contains a deleted record and its position.

    function: Delete records with a batch of conditions.  When doing deletion, we traverse the whole table and delete the records which satisfy the conditions. This opration can be optimized in `IndexManager` using B+ Tree.

  - Select Record

    ```c++
    result selectRecord(const macro::table &table, const vector<string> &attr, const vector<condition> &cond);
    ```

    table:  the table instance

    attr: the columns to be selected, because our mini sql is not support for partial selection, this parameter is always equal to all columns of the table.

    conditions: the select conditions

    return: a vector, each element in vector is a pair which contains a selected record and its position.

    Select record with a batch of conditions. Similar to deletion, we traverse the whole table and select the records which satisfy the conditions. This opration can be optimized in `IndexManager` using B+ Tree. Not support for selecting part columns.

  - Get Record

    ```c++
    sql_tuple getRecord(const macro::table &table, uint32_t id);
    ```

    table:  the table instance

    id: the position

    return: the record in that position.

    function: Get a record by its positon in data file, mainly used by index.

  - Get Record Pairs

    ```c++
    vector<pair<uint32_t, sql_tuple>> getRecordPairs(const macro::table &table);
    ```

    table:  the table instance

    return: a vector, each element in vector is a pair which contains a record and its position.

    function: Return all records paired with their position in data file, mainly used by index.