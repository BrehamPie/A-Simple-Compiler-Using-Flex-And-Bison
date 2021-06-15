%{
                                /*Definition Section*/
	#include "compiler.tab.h"
	#include<string.h>
	extern int yydebug;
	int yyparse();	
%}

%%
        /*---Rules Section--*/

"root"											{return ROOT;}
":"												{return START;}
";"                                             {return EOL;}
"end"											{return END;}

"int"											{return INT_TYPE;}
"real" 										    {return REAL_TYPE;}
"string"										{return STRING_TYPE;}

"++"											{return PPLUS;}
"--"											{return MMINUS;}
"=="											{return EQL;}
"!="											{return NEQL;}	
"<="											{return LEQL;}
">="											{return GEQL;}
"->"											{return ARROW;}
"<-"											{return RARROW;}


"see"											{return SEE;}
"read"											{return READ;}

"XOR"											{return XOR;}
"AND"											{return AND;}
"OR"											{return OR;}
"~"												{return NOT;}
"SIN"											{return SIN;}
"COS"											{return COS;}
"TAN"											{return TAN;}
"SQRT"											{return SQRT;}
"log"											{return LOG;}
"log2"											{return LOG2;}
"ln"											{return LN;}
"!"												{return FACTORIAL;}

"if"											{return IF;}
"elif"											{return ELIF;}
"else"											{return ELSE;}
"choices for"									{return CHOICE;}
"none"											{return DEFAULT;}
"option"										{return OPTION;}

"repeat"										{return DO;}
"while"											{return WHILE;}
"from"											{return FROM;}
"to"											{return TO;}
"by"											{return BY;}

"module"										{return MODULE;}
"call"											{return CALL;}

"sort"											{return SORT;}
"push"											{return PUSH;}
"pop"											{return POP;}


[0-9]+  										{yylval.integer = atoi(yytext); return INTEGER;}
[0-9]*[.][0-9]+([eE][+-]?[0-9]+)?				{yylval.real = atof(yytext);return REAL;}

[A-Za-z0-9]+ 									{yylval.string = strdup(yytext);return VARIABLE;}
[$][$][A-Za-z0-9]+ 								{yylval.string = strdup(yytext);return ARRAY_VAR;}


\"(\\.|[^"\\])*\"								{
													char* tmp = strdup(yytext);
													int n = strlen(tmp);
													yylval.string = malloc(n*sizeof(char));
  													memcpy(yylval.string,&tmp[1],n-2);
													  yylval.string[n-2]='\0';
													return STRING;
												}

"."												{return yytext[0];}
[-+/*<>=,();%^{}\]\[] 							{return yytext[0];}
[#][^#]*[#]										{
													char* tmp = strdup(yytext);
													int n = strlen(tmp);
													yylval.string = malloc(n*sizeof(char));
  													memcpy(yylval.string,&tmp[1],n-2);
													   yylval.string[n-2]='\0';
													return COMMENT;
												
												}
[ \t\n]											{}
.												{printf("Unknown character.");}
%%
 /*User Code Section*/
int yywrap(){
	return 1;
}

int yyerror(char *s){
	fprintf(stderr,"error: %s\n", s);
}