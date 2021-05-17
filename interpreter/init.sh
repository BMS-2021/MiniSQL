flex -o lex.yy.cc tokenizer.l
bison -d parser.y -o parser.tab.cc