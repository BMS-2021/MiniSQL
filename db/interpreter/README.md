# Interpreter

The interpreter is the entrance of every SQL query loop. It gets the user input, passes the user input to lexer and parser, and gets the AST (abstract syntax tree), which stores in a C++ object.

| Procedure | Input | Output | Used Tools |
| :-------: | :-------: | :-----: | :-----: |
| read user input | (command line input) | a null-terminated byte string | GNU Readline |
| lexer | a null-terminated byte string | a null-terminated byte string with tokens tagged onto it | Flex |
| parser | a null-terminated byte string with tokens tagged onto it | an AST in the C++ object | GNU Bison |

`Flex` and `Bison` has their own syntax. `Flex` constructs relations between string and token, while `Bison` matches the SQL statements to syntaxes, which are defined recursively in `Bison` source code file.  
`Flex` and `Bison` needs to be translated to C++ before the compilation progress starts. We write `CMake` rules to check if there exists the required tools and libraries, and will trigger an error if some necessary dependency cannot be found. We chose `Cmake` because it is a cross-platform building solution.

Since this is a command-line program, various command-line features are implemented using `GNU Readline`, like moving cursor through the line, pressing `TAB` button to auto-complete a keyword, and pressing up or down button to go through the input history.  

## Concept explanation

After getting an input string, the first thing we need to know is that what tokens are *keywords*, what are *operators*, and what are just basic string like identifiers. Here is an example:

```C++
auto      a            =          b            ;
^keyword  ^identifier  ^operator  ^identifier  ^operator
```

As the example shown above, *lexical analysis* processes the input string to a sequence of token names, identifying what each token actually is.

After lexing, we still don't know how these tokens construct an expression. It feels like that we only understand the meaning of each word in an English sentence, but we cannot understand the sentence because we currently know nothing about the grammar. Then, we need to perform a *syntactic analysis*, conforming the token sequence to the rules of a formal grammar.

```C++
definition_expr: TYPE_KEYWORD IDENTIFIER OPERATOR= IDENTIFIER OPERATOR;
// OK, now we know the format above is a definition expression!
```

I didn't really use pure C/C++ to implement the interpreter. Instead, I used `Flex` and `Bison` to generate lexer and parser.

## Lexical analysis (Flex)

The procedure of lexing is in fact a serial of regex rules:

```flex
[ \t\r\n]		;

"*"         return S_STAR;
"'"         return S_APOSTROPHE;
";"         return S_SEMICOLON;
"("         return S_L_BRACKETS;
")"         return S_R_BRACKETS;
","         return S_COMMA;
"="         return S_EQUAL;
"<>"        return S_NOT_EQUAL;
">"         return S_GREATER;
">="        return S_GREATER_EQUAL;
"<"         return S_LESS;
"<="        return S_LESS_EQUAL;

[A-Za-z0-9_.]*  { return get_token(yylval.str = std::string(yytext)); }
```

1. Do not match ` `, `\t`, `\r` and `\n` to any token.
2. Match the valid operators to the corresponding tokens.
3. Match `[A-Za-z0-9_.]*` into `get_token()`.

The procedure in `get_token()` is a matching using hash table, making token matching really fast. If the string failed to match any of the token, then mark it as `V_STRING`, representing a simple string.  
Be aware that before trying to match with a keyword, the string has been converted to down-case. This means that the keywords are case-insensitive.

For an example, after the procedure mentioned above, the SQL query `SELECT * FROM foo WHERE bar <> 1;` has been converted to the following token sequence:

```
K_SELECT S_STAR K_FROM V_STRING K_WHERE V_STRING S_NOT_EQUAL V_STRING S_SEMICOLON
```

You may be puzzled that `1` has been tagged as `V_STRING`. Yes, this actually happens. In fact, the meaning of `1` could be determined by the context. In this case, the type of these "rvalues" will be confirmed during the syntactic analysis.

There needs more detailed implementations and configurations to make the lexer work. Here, I will only talk about how to build the whole project along with Flex.  
Flex is a lexer generator. This means that Flex is not a running program that lexes your input. In fact, it has it's own grammar, and generates C language code, which is the lexer after compilation by using C compiler. This means, every time I build my project, only compiling C++ code is not enough. We must generate lexer using Flex before C compilation. A simple solution is to execute the following command before compilation:

```shell
flex -o lex.yy.cc tokenizer.l
```

However, since this project is a CMake project, there definitely exists ways to integrate this preprocessing procedure into CMake. And I eventually chose this implementation:

```cmake
if(FLEX_FOUND)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/interpreter)
    FLEX_TARGET(MyScanner interpreter/tokenizer.l ${CMAKE_CURRENT_BINARY_DIR}/interpreter/lex.yy.cc)
    ADD_FLEX_DEPENDENCY(MyScanner)
    #[[details omitted]]
endif()
```

## Syntactic analysis (Bison)

The rules of syntactic analysis are written in `*.y` file. The name-prefix of each token has a format:

1. `C` stands for compound, meaning that it is a set which contains multiple instructions.
2. `I` stands for instruction, which is a complete SQL instruction.
3. `E` stands for expression, which is a part of SQL instruction.
4. `K` stands for keyword.
5. `V` stands for value, which might be a identifier, and also might be INT, FLOAT or CHAR value.
6. `S` stands for symbol.

The content of `*.y` file looks like trees:

```bison
C_TOP_INPUT: C_TOP_STMT S_SEMICOLON
    {
        YYACCEPT;
    }
    ;

C_TOP_STMT: C_DDL
    | C_DML
    | I_EXIT
    | I_QUIT
    | I_USE_DATABASE
    | I_EXECFILE
    | I_VACANT
    ;
```

`C_TOP_INPUT` is the root of the whole tree. Matching an instruction is a tree traversing process.

Now, let's focus on an *instruction* node of the tree:

```bison
I_SELECT_TABLE: K_SELECT E_ATTRIBUTE_LIST K_FROM V_STRING E_WHERE
    {
        query_object_ptr = std::make_unique<api::select_table>($2, $4, $5);
    }
    ;

E_WHERE: K_WHERE E_CONDITION_LIST
    {
        $$ = $2;
    }
    | E_VACANT
    {
        $$ = std::vector<condition>();
    }
    ;
```

In each instruction node, only one thing has been done: generating a unique pointer of the derived objects of `api::base`. `($2, $4, $5)` passes three parameters to the constructor, which are the result of expression `E_ATTRIBUTE_LIST`, the value of `V_STRING` and the result of expression `E_WHERE`. `query_object_ptr`is an external global variable, which is a part of API, which will be discussed in the next chapter.  
Each expression will not interact with any global value. Instead, it will return values to it's ancestors, using `$$`.

Bison is a parser generator. It needs to translate from `*.y` file to C language files. To interact with this parser, we need to call its functions, which is defined in the generated files. Our implementation of parser calling is:

```c++
int parse(const char *input) {
    YY_BUFFER_STATE bp = yy_scan_string(input);
    if (bp == nullptr) {
    	std::cerr << "error on creating YY_BUFFER_STATE" << std::endl;
    	return -1;
    }
    yy_switch_to_buffer(bp);
    yyparse();  // parser calling
    yy_delete_buffer(bp);
    yylex_destroy();
    return 0;
}
```

This procedure is trivial. It just scan the file, check validation, switch buffer, parse, and finally free buffer and lexer.

To build Bison and Flex together with CMake, here is the related code in the CMakeList:

```cmake
if(BISON_FOUND AND FLEX_FOUND)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/interpreter)
    BISON_TARGET(MyParser interpreter/parser.y ${CMAKE_CURRENT_BINARY_DIR}/interpreter/parser.tab.cc)
    FLEX_TARGET(MyScanner interpreter/tokenizer.l ${CMAKE_CURRENT_BINARY_DIR}/interpreter/lex.yy.cc)
    ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
    include_directories("interpreter")
    add_executable(#[[details omitted]] ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})
	#[[details omitted]]
endif()
```

## Issues

While we are using C++ as the developing language, we are relying on many cpp-specific features, like exception handling. However, if a heap space is created using `::malloc()` or `new`, if an exception has been thrown before `::free()` or `delete`, then memory leak will happen, since destructor doesn't exist in C. This means that if Flex and Bison generates C files, then we cannot use some of the C++ features.  
Sadly, although both of these two tools have C++ mode, but they just cannot work together when C++ mode are both switched on. The only thing I can do is to use C++ compiler to compile the C files generated by them, while avoid using some of the C++ features that might break things.  
It cost me loads of times to make the generated code and my source code work together properly.  
