//
// Created by enzymii on 4/6/22.
//

#ifndef MINISQL_MAIN_H
#define MINISQL_MAIN_H

typedef struct {
    int code;
    char *msg;
} libminisql_resp;

libminisql_resp external_main(const char *str);
char* get_table_names();

#endif // MINISQL_MAIN_H
