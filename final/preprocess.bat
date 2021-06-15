cd C:\Users\2play\Desktop\Flex\Project\final
bison -d compiler.y
flex compiler.l
gcc lex.yy.c compiler.tab.c  -o project