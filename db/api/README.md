# API

*API* is the separator of the front-end part and the back-end part of these project. The front-end part only contains the *Interpreter*, which eventually creates aN AST OBJECT. *API* handles these AST objects by calling the corresponding functions.  
*API* contains a base class. The name of the base class is literally `api::base`, which is almost a virtual class. Every SQL statement extends from this base class, and implements the `exec` function of the base class. This function is responsible for calling other modules in the back-end. For an example, `api::create_table::exec` needs to call *Catalog Manger* to and the meta-data of the table into the schema, then calls *Record Manager* to create an record file of this table, then may call *Index Manager* to create a B+ tree for the index on primary key. The only back-end module that will not be called directly by *API* is *Buffer Manager*, which is managed by *Record Manager*.  
*API* also manages the query result, the query duration, the exception, and needs to print the result of `SELECT` statement into a table properly. Everything related to direct interaction with the user are not related to any module below *API*.


## API base class and it's derived classes

The whole API has two types of classes: a base class named `api::base`, and multiple derived classes extended from the base class. The base class is declared as following:

```c++
struct base {
    virtual ~base() = default;
    virtual void exec() {};
};
```

There are only two functions defined in base class, both of them are all virtual functions. To explicitly set the default destructor as a virtual function is because of the language feature of C++, and I do not want to explain it here. What is important is the second function `exec()`, which is a virtual function, and will be overridden by the derived classes. Here is an example of the derived class:

```c++
class create_table final : public base {
    std::string table_name;
    std::vector<schema> schema_list;
    std::string primary_key;

    void exec() override;

    public:
    create_table(std::string& table_name,
        std::vector<schema>& schema_list,
        std::string& primary_key) :
        table_name(table_name), schema_list(schema_list), primary_key(primary_key) {}
};
```

This class will be instantiated after an SQL CREATE TABLE instruction has been correctly parsed. To instantiate, the constructor will be called, giving each private variable a specific value. The `exec()` function has been defined in another source code file, overriding the same virtual function in the base class. Virtual function is used to make the object passing more simple. The only things we need to write in the parser source code are:

```c++
extern std::unique_ptr<api::base> query_object_ptr;
// in some of the leaf nodes of the parser tree:
query_object_ptr = std::make_unique<api::create_table>($3, $5, $6);
// in other leaf nodes:
query_object_ptr = std::make_unique<api::execfile>($3);
```

Since both `api::create_table` and `api::execfile` are extended from `api::base`, calling `query_object_ptr->exec()` will call the corresponding `exec()` function of the derived class object pointed by `query_object_ptr`, while `query_object_ptr` is a unique pointer of `api::base`.

## Inside `exec()` function

The procedure inside `exec()` is what the API perform. Usually, it calls record manager, catalog manager and buffer manager, to perform the corresponding features. The implementation is highly case sensitive, so that I will not introduce them one by one here.

## Output of SELECT

The output of SELECT is also handled by API. Here is an example of the output:

```
MiniSQL> SELECT * FROM foo;
 +-----+-----------+------+-----------+ 
 | id  | bar       | buzz | barbar    | 
 +-----+-----------+------+-----------+ 
 | 1   | 51.279999 | 4    | 9.137000  | 
 | 2   | 45.189999 | 2    | 78.040001 | 
 | 3   | 18.049999 | 9    | 33.220001 | 
 | 4   | 98.809998 | 8    | 41.119999 | 
 | 5   | 19.690001 | 9    | 43.980000 | 
 | 6   | 35.400002 | 10   | 74.980003 | 
 | 7   | 46.139999 | 10   | 17.040001 | 
 +-----+-----------+------+-----------+ 
```

To implement this correctly, we need to calculate the max length of each column. This can be done by iterating the whole table. Then, use `std::setw()` and `std::setfill()` to help us to construct the format of output.

## Error handling, result returning and time keeping

We have implemented a custom exception type, which is extended from `std::exception`:

```c++
class sql_exception : public std::exception {
    unsigned int code;
    std::string module_name;
    std::string detail;

public:
    sql_exception() = default;
    sql_exception(unsigned int code, std::string& module_name, std::string& detail) :
            code(code), module_name(module_name), detail(detail) {}

    sql_exception(unsigned int code, std::string&& module_name, std::string&& detail) :
            code(code), module_name(module_name), detail(detail) {}

    friend std::ostream& operator<<(std::ostream& os, const sql_exception& e);
};

std::ostream& operator<<(std::ostream& os, const sql_exception& e) {
    os << "ERROR " << e.code << " from <" << e.module_name << ">: " << e.detail;
    return os;
}
```

When something goes wrong in the project, an exception will be thrown. The exception must be in `sql_exception` type, otherwise it cannot be handled correctly. After the exception has been handled, `std::cout` will be called to print the exception.

The result printer and time keeper also prints data at the end of execution.  
