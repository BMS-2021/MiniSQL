#include <unordered_map>

#include "utils.h"
#include "parser.tab.hh"

std::unordered_map<std::string, int> token_map = {
        {"database", K_DATABASE}, {"table", K_TABLE},
        {"create", K_CREATE}, {"select", K_SELECT}, {"update", K_UPDATE}, {"drop", K_DROP},
};

int get_token(const std::string& str) {
    int token = T_STRING;

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
