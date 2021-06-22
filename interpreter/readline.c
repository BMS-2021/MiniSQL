#ifdef READLINE_FOUND

#include <string.h>

#include <readline/readline.h>

char **keyword_completion(const char *, int, int);
char *keyword_generator(const char *, int);

char *keywords[] = {
        "database", "table", "index", "values",
        "from", "where", "on", "into", "and",
        "create", "select", "insert", "delete", "drop",
        "use", "exit", "quit", "primary", "key", "unique", "execfile",
        "int", "float", "char",
        NULL
};

char **
keyword_completion(const char *text, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, keyword_generator);
}

char *
keyword_generator(const char *text, int state)
{
    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = keywords[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

#endif
