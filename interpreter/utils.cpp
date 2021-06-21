#include <unordered_map>

#include "interpreter.h"
#include "utils.h"
#include "parser.tab.hh"

std::unordered_map<std::string, int> token_map = {
        {"database", K_DATABASE}, {"table", K_TABLE}, {"index", K_INDEX}, {"values", K_VALUES},
        {"from", K_FROM}, {"where", K_WHERE}, {"on", K_ON}, {"into", K_INTO}, {"and", K_AND},
        {"create", K_CREATE}, {"select", K_SELECT}, {"insert", K_INSERT}, {"drop", K_DROP},
        {"use", K_USE}, {"exit", K_EXIT}, {"primary", K_PRIMARY}, {"key", K_KEY}, {"unique", K_UNIQUE}, {"execfile", K_EXECFILE},
        {"int", T_INT}, {"float", T_FLOAT}, {"char", T_CHAR},
};

int get_token(const std::string& str) {
    int token = V_STRING;

    std::string lower_str = str;
    std::for_each(lower_str.begin(), lower_str.end(), [](char & c) {
        c = std::tolower(c);
    });

    auto token_map_find_result = token_map.find(lower_str);
    if (token_map_find_result != token_map.end()) {
        token = token_map_find_result->second;
    }

    return token;
}
