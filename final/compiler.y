%{
        /* C declaration-contains header, functions and other c variables.*/
	#include<stdio.h>
	#include<stdlib.h>
	#include<conio.h>
	#include<string.h>
	#include<math.h>
    #include <windows.h>
    #include <io.h>
    #include <direct.h>
    #define YYDEBUG 1

    /*Function to change color of text at ease
        ref:https://stackoverflow.com/questions/29574849/how-to-change-text-color-and-console-color-in-codeblocks */
    void SetColor(int ForgC)
    {
        WORD wColor;

        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
            //We use csbi for the wAttributes word.
        if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
        {
            //Mask out all but the background attribute, and add in the forgournd color
            wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
            SetConsoleTextAttribute(hStdOut, wColor);
        }
        return;
    }

    /* Flex handling */
    extern FILE *yyin;
	int yylex();
	int yyparse();
	int yyerror(char *s);

    /* structure for storing variables.*/
    typedef struct variable{
        char *name;
        int type; /* indicates data type- 0:int,1:double,2:char* */
        int *ival;
        double *dval;
        char** sval;
        int size;
        int isArray;
    }var;

    /*structure for storing user-defined modules(functions).*/
    typedef struct functionstack{
        char *fname;
        var *fptr;
        int varCnt;
    }stack;

    /*Initial Pointers to store variable and modules.*/
    var *vptr;
    stack *stk;
    /* Helper variables.*/
    int varCnt = 0,funCnt=0; /* variable and functions taken so far.*/
    int vartaken = 0,funtaken=0; /* memory assigned for them so far.*/
    int cnt = 0; /* counts array element during declaration. */
    int *itmp;
    double *dtmp;  /* Stores array value temporarily to insert later*/
    char **stmp;
    int conditionMatched; /* 1 if condition is matched in conditional statements(if-else,choice) */
    double choiceValue; /* stores the value of choice to check in options. */
    int currentFunction; /* module index that is being called currently. */
    int currentParam; /* paramaters that have been processed of the calling function so far. */
    int functionRejected; /* 1 if calling module structure doesn't match. */

    /* ------------------- Helper Functions------------------- */
        /* Checks if a variable is already declared. */
        int checkExistance(char *varName){
            for(int i = 0 ; i<varCnt; i++){
                if(strcmp(vptr[i].name,varName)==0){
                    return 1;
                }
            }
            return 0;
        }

        /* Get the index of variable that is called. If not found return -1. */
        int getVariableIndex(char *varName){
            for(int i = 0 ; i<varCnt; i++){
                if(strcmp(vptr[i].name,varName)==0){
                    return i;
                }
            }
            return -1;
        }
        /* Error Handling Function: No variable */
        void doesNotExist(char *varName){
            SetColor(4);
            printf("There is No Such Variable Named: %s\n",varName);
            SetColor(2);
        } 
        /* Error Handling Function: Not an array */ 
        void notArray(char *varName){
            SetColor(4);
            printf("%s is not an Array.\n",varName);
            SetColor(2);
        }
        /* Error Handling Function: out of bound */ 
        void outOfRange(){
            SetColor(4);
            printf("Trying to Access index out of Range.\n");
            SetColor(2);
        }
        /* Error Handling Function: empty Array. */ 
        void emptyArray(){
            SetColor(4);
            printf("Array is already empty.\n");
            SetColor(2);
        }
        /* Error Handling Function: Already Declared. */
        void alreadyExist(char *varName){
            SetColor(4);
            printf("%s is already Declared.\n",varName);
            SetColor(2);
        }
        /* Error Handling Function: Applying arithmetic on string. */
        void notNumeric(){
             SetColor(4);
            printf("String literals not applicable.\n");
             SetColor(2);
        }
        /* Error Handling Function: Applying arithmetic on double. */
        void notInt(){
             SetColor(4);
            printf("Applicable of Integer only.\n");
             SetColor(2);
        }
        /* Insert New variable in array. */
        /* void pointer is used so that I can typecast it to the type defined by type variable. */
        void insertData(char *varname,void* value,int type,int id,int size,int isArray){
            vptr[id].name = varname;
            vptr[id].size = size;
            vptr[id].isArray = isArray;
            if(type==0){
                int *x = ((int*)value);
                vptr[id].ival = malloc(size*sizeof(int));
                for(int i=0;i<size;i++){
                    vptr[id].ival[i] = x[i];
                }
                vptr[id].type = 0;
                
            }
            else if(type==1){
                double* x = ((double*)value);
                vptr[id].dval = malloc(size*sizeof(double));
                for(int i=0;i<size;i++){
                    vptr[id].dval[i] = x[i];
                }
                vptr[id].type = 1;
                
            }
            else if(type==2){
                char **s =((char**)value);
                vptr[id].sval = malloc(size*sizeof(char**));
                for(int i=0;i<size;i++){
                    vptr[id].sval[i] = s[i];
                }
                vptr[id].type = 2;
            }
        }

        /* Printing a variable (including array)*/
        void printVariable(char *varName){
            int index = getVariableIndex(varName);
            if (index == -1)
            {
                doesNotExist(varName);
            }
            else
            {
                if (vptr[index].isArray)
                {
                    printf("%s is an array with %d elements.Elements are:\n", varName, vptr[index].size);
                    for (int i = 0; i < vptr[index].size; i++)
                    {
                        if (vptr[index].type == 1)
                            printf("%lf ", vptr[index].dval[i]);
                        if (vptr[index].type == 0)
                            printf("%d ", vptr[index].ival[i]);
                        if (vptr[index].type == 2)
                            printf("%s ", vptr[index].sval[i]);
                        if (i % 10 == 9)
                            puts("");
                    }
                    puts("");
                }
                else
                {
                    printf("Value of %s is:",varName);
                    if (vptr[index].type == 1)
                        printf("%lf\n", vptr[index].dval[0]);
                    if (vptr[index].type == 0)
                        printf("%d\n", vptr[index].ival[0]);
                    if (vptr[index].type == 2)
                        printf("%s\n", vptr[index].sval[0]);
                }
            }
        }


        /* Printing an Array Index */
        int printArrayIndex(char *varName, int position){
            int index = getVariableIndex(varName);
            if (index == -1)
            {
               doesNotExist(varName);
            }
            else if (vptr[index].isArray == 0)
            {
                notArray(varName);
            }
            else if (position >= vptr[index].size)
            {
                printf("index out of range\n");
            }
            else
            {
                printf("Value of Array %s at Index %d is: ", varName, position);
                if (vptr[index].type == 1)
                    printf("%lf\n", vptr[index].dval[position]);
                if (vptr[index].type == 0)
                    printf("%d\n", vptr[index].ival[position]);
                if (vptr[index].type == 2)
                    printf("%s\n", vptr[index].sval[position]);
            }
        }

        /* Taking Input From User */
         int takeInput(char *varName, int id ){
             SetColor(8);
             printf("Enter Value of %s: ",varName);
             SetColor(2);
            int index = getVariableIndex(varName);
            if (index == -1)
            {
                doesNotExist(varName);
            }
            else
            {
                if(id>=vptr[index].size){
                    outOfRange();
                }
                else
                {
                    if (vptr[index].type == 1)
                        scanf("%lf", &vptr[index].dval[id]);
                    if (vptr[index].type == 0)
                        scanf("%d", &vptr[index].ival[id]);
                    if (vptr[index].type == 2){
                        char str [100000];
                        scanf("%s",str);
                        vptr[index].sval[id]=str;
                    }
                       
                }
            }
        }

        /* ---------Array Helper Functions-------- */

        /* Push new element at the end of array. */
        void appendData(int index,void *value){
            int curSize = vptr[index].size;
            double x = *(double*) value;
            if(vptr[index].type==0){
                vptr[index].ival = realloc(vptr[index].ival,(curSize+1)*sizeof(int));
                vptr[index].ival[curSize] = (int)x;
                vptr[index].size++;
            }
            else{
                vptr[index].dval = realloc(vptr[index].dval,(curSize+1)*sizeof(double));
                vptr[index].dval[curSize] = x;
                vptr[index].size++;
            }
        }

        /* pop last element from the array. */
        void popLast(int index){
            int curSize = vptr[index].size;
            if(curSize==0){
                emptyArray();
                return ;
            }
            if(vptr[index].type==0){
                vptr[index].ival = realloc(vptr[index].ival,(curSize-1)*sizeof(int));
                vptr[index].size--;
            }
            else if (vptr[index].type==1){
                vptr[index].dval = realloc(vptr[index].dval,(curSize-1)*sizeof(double));
                vptr[index].size--;
            }
            else if (vptr[index].type==1){
                vptr[index].sval = realloc(vptr[index].sval,(curSize-1)*sizeof(char**));
                vptr[index].size--;
            }
        }
        /* functions for quicksort */
        int cmpfunInt (const void * a, const void * b) {
            return ( *(int*)a - *(int*)b );
        }

        int cmpfunDouble ( const void *a , const void *b){
            return ( *(double*)a - *(double*)b );
        }

        int cmpfunString(const void *a, const void *b) { 
            const char **str_a = (const char **)a;
            const char **str_b = (const char **)b;
            return strcmp(*str_a, *str_b);
        } 

        void sort(int id){
            if(vptr[id].type==0){
                qsort (vptr[id].ival, vptr[id].size, sizeof(int), cmpfunInt);
            }
            else if(vptr[id].type==1){
                qsort (vptr[id].dval, vptr[id].size, sizeof(double), cmpfunDouble);
            }
            else{
                qsort (vptr[id].sval, vptr[id].size, sizeof(char*), cmpfunString);
            }
        }

        /* --- Helper Function for Modules. ---*/
        int getFunctionIndex(char *varName){
            for(int i = 0 ; i<funCnt; i++)
            {
                if(strcmp(stk[i].fname,varName)==0){
                    return i;
                }
            }
            return -1;
        }
%}
/* ---------------Bison Declaration. ---------------*/
%error-verbose /* shows syntax error. */
%debug /* helps to see all states one by one. */
%union{
	int integer;
	double real;
	char* string;
}

%token ROOT END START VARIABLE ARRAY_VAR EOL ARROW RARROW 
%token INTEGER REAL STRING INT_TYPE REAL_TYPE STRING_TYPE
%token SEE READ
%token AND OR NOT XOR LOG LOG2 LN SIN COS TAN FACTORIAL SQRT
%token IF ELIF ELSE CHOICE DEFAULT OPTION
%token FOREACH FROM TO DO WHILE BY AS
%token COMMENT MODULE CALL
%token PUSH POP SORT
%type <integer> INTEGER ROOT END START program while_conditions
%type <string> VARIABLE INT_TYPE REAL_TYPE STRING_TYPE STRING ARRAY_VAR COMMENT
%type <real> expr REAL statements statement 
%nonassoc ELIF 
%nonassoc ELSE
%left PPLUS MMINUS
%left AND OR XOR NOT
%left LOG LOG2 LN SQRT 
%left '<' '>' EQL NEQL LEQL GEQL
%left  '+' '-'
%left  '*' '/' '%'
%right  '^' FACTORIAL
%left SIN COS TAN
%%
                     /*  RULES  */
program:    /* program will be selected as start node by default. */
            ROOT START statements END 
                {
                    printf("\n\n     -------Program Compiled Successfully-------\n\n\n");
                }
    ;
statements: /* program consists of multiple statements . each statement will be called recursively. */
                {/*Do Nothing.*/} 
            |   statements statement                             
    ;
statement: /* Types of statement we will see. */
            EOL                 {}
            | COMMENT            
                {
                    SetColor(1);
                    printf("  %s\n",$1);
                    SetColor(2);
                }
            | declaration EOL    {}
            | assigns EOL        {}
            | show EOL           {}
            | read EOL           {}
            | expr EOL           
                {
                    SetColor(8);
                    printf("Value of the expression:%.4lf\n",$1);
                    $$ = $1;
                    SetColor(2);
                }
            | if_blocks          
                {conditionMatched=0;}
            | choice_block       {}
            | loop_block         {}
            | module_declare     
                {
                        SetColor(8);
                        printf("New Module Declared-");
                        printf("Module Name: %s\n",stk[funCnt].fname);
                        printf("Parameters of the Module\n");
                        int ttl = stk[funCnt].varCnt;
                        for(int i = 0;i<ttl;i++)
                        {
                            printf("Parameter Name:%s Data Type: ",stk[funCnt].fptr[i].name);
                            if (stk[funCnt].fptr[i].type == 1)
                                printf("REAL\n");
                            if (stk[funCnt].fptr[i].type == 0)
                                printf("INT\n");
                            if (stk[funCnt].fptr[i].type == 2)
                                printf("STRING\n");
                        }
                        funCnt++;
                        SetColor(2);
                }
            | module_call        {}
            | array_operations   {}
    ;
declaration: /* Variable Declaration */
            INT_TYPE int_variables
            | REAL_TYPE real_variables
            | STRING_TYPE string_variables
    ;
int_variables: 
            int_variables ',' int_var    {}
            | int_var                    {}
    ;
int_var:  
            VARIABLE '=' expr               
                {
                    int exists = checkExistance($1);
                    if(exists){
                        alreadyExist($1);
                    }
                    else{
                        int value = $3;
                        insertData($1,&value,0,varCnt,1,0);
                        varCnt++;
                    }
                }
            | VARIABLE                        
                {
                    int value = rand();
                    insertData($1,&value,0,varCnt,1,0);
                    varCnt++;
                }                       
            |   ARRAY_VAR                       
                {
                    insertData($1,itmp,0,varCnt,cnt,1);
                    varCnt++;
                }
            |   ARRAY_VAR '=' '{' ints '}'   
                {
                    insertData($1,itmp,0,varCnt,cnt,1);
                    varCnt++;
                    cnt = 0;
                    free(itmp);
                }
    ; 
ints:                   
            ints ',' expr                   
                {
                    cnt++;
                    itmp = realloc(itmp,cnt*sizeof(int));;
                    itmp[cnt-1]=$3;
                }
            | expr                            
                {
                    cnt++;
                    itmp = realloc(itmp,cnt*sizeof(int));;
                    itmp[cnt-1]=(int)$1;
                }
    ;  
real_variables: 
            real_variables ',' real_var {}
            | real_var                  {}
    ;
real_var:  
            VARIABLE '=' expr 
                {
                    int exists = checkExistance($1);
                    if(exists){
                        alreadyExist($1);
                    }
                    else{
                        double value = $3;
                        insertData($1,&value,1,varCnt,1,0);
                        varCnt++;
                    }
                }
            | VARIABLE                        
            {
                double value = rand();
                insertData($1,&value,1,varCnt,1,0);
                varCnt++;
            }                       
            | ARRAY_VAR                      
                {
                    printf("Real Array Declaration.\n");
                    insertData($1,dtmp,1,varCnt,cnt,1);
                    varCnt++;
                }
            | ARRAY_VAR '=' '{' reals '}'      
                {
                    insertData($1,dtmp,1,varCnt,cnt,1);
                    varCnt++;
                    cnt = 0;
                    free(dtmp);
                }
    ; 
reals:                   
            reals ',' expr
                {
                    cnt++;
                    dtmp = realloc(dtmp,cnt*sizeof(double));;
                    dtmp[cnt-1]=$3;
                }
            | expr                            
                {
                    cnt++;
                    dtmp = realloc(dtmp,cnt*sizeof(double));;
                    dtmp[cnt-1]=$1;
                }       
    ;  
string_variables: 
            string_variables ',' string_var            {}
            | string_var                               {}
    ;
string_var:  
            VARIABLE '=' STRING               
                {
                    int exists = checkExistance($1);
                    if(exists){
                    printf("Variable Already declared.\n");
                    }
                    else{
                    char *value= $3;
                    insertData($1,&value,2,varCnt,1,0);
                    varCnt++;
                    printf("New variable initialized.\n");
                    }
                }
            | VARIABLE                        
                {
                    char* value = "";
                    insertData($1,&value,2,varCnt,1,0);
                    varCnt++;
                }                       
            | ARRAY_VAR                       
                {
                    printf("Integer Array Declaration.\n");
                    insertData($1,stmp,2,varCnt,cnt,1);
                    varCnt++;
                }
            | ARRAY_VAR '=' '{' strings '}'      
                {
                    printf("String Array of size:%d\n",cnt);
                    insertData($1,stmp,2,varCnt,cnt,1);
                    varCnt++;
                    cnt = 0;
                    free(stmp);
                }
    ; 
strings:                   
            strings ',' STRING                   
                {
                    cnt++;
                    stmp = realloc(stmp,cnt*sizeof(char*));;
                    stmp[cnt-1]=$3;
                }
            | STRING                           
                {
                    cnt++;
                    stmp = realloc(stmp,cnt*sizeof(char*));;
                    stmp[cnt-1]=$1;
                }
    ;
show:
            SEE ARROW print_vars
    ;
print_vars:
            print_vars ',' VARIABLE             
                {
                    printVariable($3);
                }
            | print_vars ',' ARRAY_VAR        
                {
                    printVariable($3);
                }
            | print_vars ',' ARRAY_VAR '[' expr ']'   
                {
                    printArrayIndex($3,$5);
                }
            | VARIABLE   
                {
                    printVariable($1);
                }
            | ARRAY_VAR                       
                {
                    printVariable($1);
                }
            | ARRAY_VAR'[' expr ']'           
                {
                    printArrayIndex($1,$3);
                }
    ;
read:
        READ RARROW read_vars   {}
    ;
read_vars: 
            read_vars ',' VARIABLE 
                {
                    takeInput($3,0);
                }
            | read_vars ',' ARRAY_VAR '[' INTEGER ']' 
                {
                    takeInput($3,$5);
                }
            | VARIABLE
                {
                    takeInput($1,0);
                }
            | ARRAY_VAR '[' INTEGER ']' 
                {
                    takeInput($1,$3);
                }

    ;
assigns:
             assigns ',' assign
            | assign
    ;
assign:

            VARIABLE '=' expr
                {
                    int index = getVariableIndex($1);
                    if (index == -1)
                    {
                        printf("there is no variable named %s.\n", $1);
                    }
                    else
                    {
                        {
                            if (vptr[index].type == 1)
                                vptr[index].dval[0] = $3;
                            if (vptr[index].type == 0)
                                vptr[index].ival[0] = $3;
                        }
                    }
                }
            | ARRAY_VAR '[' INTEGER ']' '=' expr
                {
                    int id = $3;
                    int index = getVariableIndex($1);
                    if (index == -1)
                    {
                        doesNotExist($1);
                    }
                    else
                    {
                        if(id>=vptr[index].size){
                            outOfRange();
                        }
                        else
                        {
                            if (vptr[index].type == 1)
                                vptr[index].dval[id] = $6;
                            if (vptr[index].type == 0)
                                vptr[index].ival[id] =  $6;
                        }
                    }
                }
    ;
if_blocks:
            IF if_block else_statement   {}
    ;
if_block:
            expr START statement END 
                {
                    int isTrue = (fabs($1)>1e-9);
                    if(isTrue){
                        SetColor(8);
                        printf("Condition in if block is true.\n");
                        SetColor(2);
                        printf("Value of expression in if block is %.4lf\n",$3);
                        conditionMatched = 1;
                    }
                    else{
                        SetColor(8);
                        printf("Condition in if block is false.\n");
                        SetColor(2);
                    }
                }
    ;
else_statement:
            | elif_statement
            | elif_statement   single_else
            | single_else
    ;
single_else: ELSE START statement END
                {
                    if(conditionMatched){
                        SetColor(8);
                        printf("Condition already fulfilled.Ignoring else block.\n");
                        SetColor(2);
                    }
                    else{
                        double isTrue =1;
                        if(isTrue){
                            SetColor(8);
                            printf("Condition in else block is true.\n");
                            SetColor(2);
                            printf("Value of expression in else block is %.4lf\n",$4);
                            conditionMatched = 1;
                        }
                        else{
                            SetColor(8);
                            printf("Condition in else block is false.\n");
                            SetColor(2);
                        }
                    }  
                }
    ;
elif_statement:
            elif_statement  single_elif
            | single_elif 
    ;
single_elif:
            ELIF expr START statement END
                {
                    if(conditionMatched){
                        SetColor(8);
                        printf("Condition already fulfilled.Ignoring elif block.\n");
                        SetColor(2);
                    }
                    else{
                            int isTrue = (fabs($2)>1e-9);
                            if(isTrue){
                                SetColor(8);
                                printf("Condition in elif block is true.\n");
                                SetColor(2);
                                printf("Value of expression in elif block is %.4lf\n",$4);
                                conditionMatched = 1;
                            }
                            else{
                                SetColor(8);
                                printf("Condition in elif block is false.\n");
                                SetColor(2);
                            }
                        }
                }
    ;


choice_block: 
            CHOICE choice_variable START options END  
                {conditionMatched = 0;}
    ;
choice_variable: 
            VARIABLE
                {
                    int id = getVariableIndex($1);
                    if(id==-1) printf("No such variable");
                    else if(vptr[id].type==2){
                        SetColor(4);
                        printf("can't assign string in choices.");
                        SetColor(2);
                    }
                    else if(vptr[id].type==0) choiceValue = vptr[id].ival[0];
                    else choiceValue = vptr[id].dval[0];
                }

    ;
options:    
            optionlist default
            | default
    ;
default: 
            DEFAULT START statement END
                {
                    if(conditionMatched){
                        SetColor(8);
                        printf("Condition already fulfilled.Ignoring default option.\n");
                        SetColor(2);
                        }
                    else{
                        SetColor(8);
                        printf("Executing Default Option.No match found.\n");
                        SetColor(2);
                        printf("Value of expression: %.4lf\n",$3);
                    }
                }
    ;
optionlist:
            optionlist option
            | option
    ;
option: 
            OPTION expr START statement END 
                {
                    if(conditionMatched){
                        SetColor(8);
                        printf("Condition already fulfilled.Ignoring current option\n");
                        SetColor(2);
                        }
                    else{
                        int isTrue = (fabs($2-choiceValue)<1e-9);
                            if(isTrue){
                                SetColor(8);
                                printf("Option matched.\n");
                                SetColor(2);
                                printf("Value of expression in current option %.4lf\n",$4);
                                conditionMatched = 1;
                            }
                            else{
                                SetColor(8);
                                printf("Condition of current option doesn't match.\n");
                                SetColor(2);
                            }
                    }
                }
    ;

loop_block: 
            FROM expr TO expr BY expr START statement END 
                {
                    double begin = $2;
                    double end = $4;
                    double add = $6;
                    double x = end-begin;
                    if(x*add < 0){
                        SetColor(4);
                        printf("Infinite  FROM loop\n");
                        SetColor(2);
                    }
                    else{
                          for(double i = begin ; i<=end ; i+=add){
                                printf("Runnning Inside From Loop and value of expression is:%.4lf\n",$8);
                             }
                    }   
                }
            | WHILE while_conditions START statement END
                {
                    SetColor(8);
                    printf("While will run for %d times\n",$2);
                    SetColor(2);
                    for(int i = 0;i<$2;i++){
                        printf("Value of Statement %.4lf\n",$4);
                    }
                }
            | DO START expr END WHILE while_conditions EOL 
                {
                    SetColor(8);
                    printf("repeat while will run for %d times\n",$6);
                    SetColor(2);
                }
    ;
while_conditions: 
              VARIABLE PPLUS '<' expr
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value> $4){
                                $$ = -1;
                            }
                            else{
                                $$ = (int) $4 - value;
                            }
                    }
                }
            | VARIABLE PPLUS LEQL expr
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value> $4){
                                $$ = -1;
                            }
                            else{
                                $$ = (int) $4 - value+1;
                            }
                        }
                }
            | VARIABLE PPLUS NEQL expr
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value> $4){
                                $$ = -1;
                            }
                            else{
                                $$ = (int) $4 - value;
                            }
                    }
                }
            | VARIABLE MMINUS '>' expr
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value< $4){
                                $$ = -1;
                            }
                            else{
                                $$ =value -(int) $4;
                            }
                    }
                }
            | VARIABLE MMINUS GEQL expr
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value< $4){
                                $$ = -1;
                            }
                            else{
                                $$ =value -(int) $4+1;
                            }
                    }
                }
            | VARIABLE MMINUS NEQL expr
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value< $4){
                                $$ = -1;
                            }
                            else{
                                $$ =value -(int) $4;
                            }
                    }
                }     
    ;   

module_declare:
             MODULE module_name '(' module_variable ')' START statements END 
    ;
module_name:
            VARIABLE
                {
                    int id = getFunctionIndex($1);
                    if(id!=-1){printf("Module Already Declared");}
                    else{
                        printf("Declaring Module\n");
                        stk[funCnt].fname = malloc((strlen($1)+10)*sizeof(char));
                        strcpy(stk[funCnt].fname,$1);
                        stk[funCnt].varCnt = 0; 
                        stk[funCnt].fptr = malloc(4*sizeof(stack));
                    }
                }
    ;
module_variable:
            module_variable ',' single_var
            | single_var
    ;
single_var: 
            INT_TYPE VARIABLE
                {
                    int id = stk[funCnt].varCnt;
                    int value = rand();
                    insertData($2,&value,0,varCnt,1,0);
                    stk[funCnt].fptr[id] = vptr[varCnt];
                    varCnt++;
                    stk[funCnt].varCnt++;
                }
            | REAL_TYPE VARIABLE
                {
                    int id = stk[funCnt].varCnt;
                    double value = rand();
                    insertData($2,&value,1,varCnt,1,0);
                    stk[funCnt].fptr[id] = vptr[varCnt];
                    varCnt++;
                    stk[funCnt].varCnt++;
                }
            | STRING_TYPE VARIABLE
                {
                    int id = stk[funCnt].varCnt;
                    char* value = "";
                    insertData($1,&value,2,varCnt,1,0);
                    stk[funCnt].fptr[id] = vptr[varCnt];
                    varCnt++;
                    stk[funCnt].varCnt++;
                }
    ;  
module_call:
            CALL user_module_name '(' user_parameters ')'
                {
                    if(functionRejected){
                        SetColor(4);
                        printf("Module was not called as declared\n");
                        SetColor(2);
                    }
                    else{
                        printf("Module called Successfully.\n");
                    }
                }
    ;
user_module_name:
            VARIABLE
                {
                    int id = getFunctionIndex($1);
                    if(id==-1){printf(" No Function Exist with this name.");}
                    else{
                        currentFunction = id;
                        currentParam = 0;
                        functionRejected = 0;
                    }
                }
    ;
user_parameters:
            user_parameters ',' single_param
            | single_param
    ;
single_param: 
            VARIABLE
                {
                    int id = getVariableIndex($1);
                    if(currentParam>=stk[currentFunction].varCnt){
                        SetColor(8);
                        printf("More variable than declared");
                        SetColor(2);
                        functionRejected = 1;
                    }
                    else if(stk[currentFunction].fptr[currentParam].type!=vptr[id].type){
                        SetColor(8);
                        printf("Data Type doesn't match\n");SetColor(2);
                        functionRejected = 1;
                    }
                    else{
                        currentParam++;
                    }
                }
            
    ;
array_operations:
             ARRAY_VAR '.' PUSH '(' expr ')'
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].isArray==0)
                        notArray($1);
                    else{
                            if(vptr[id].type== 2){
                                notNumeric();
                            }
                            else{
                                double x = $5;
                               appendData(id,&x);
                            }
                        }
                    
                }
            | ARRAY_VAR '.' POP '('')'
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else{
                        popLast(id);
                    }
                }
            | ARRAY_VAR '.' SORT 
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].isArray==0)
                        notArray($1);
                    else{
                            sort(id);
                        }
                
                }
    ;
expr:             
            INTEGER                  
                {
                    $$ = $1;
                }                         
            | REAL                  
                {
                    $$ = $1;
                }

            | VARIABLE             
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist;
                    else if(vptr[id].type==2){
                        notNumeric();
                    }
                    else if(vptr[id].type==0) $$ = vptr[id].ival[0];
                    else $$ = vptr[id].dval[0];
                }
            | '+' expr
                {
                    $$ = $2;
                }
            | '-' expr
                {
                    $$ = -$2;
                }
            | PPLUS expr
                {
                    $$ = $2;
                }
            | MMINUS expr
                {
                    $$ = $2;
                }
            | expr '+' expr         
                {
                    $$ = $1 + $3;
                }
            | expr '-' expr         
                {
                    $$ = $1 - $3;
                }
            | expr '*' expr
                {
                    $$ = $1 * $3;
                } 
            | expr '/' expr         
                {
                    $$ = $1 / $3;
                }
            | expr '^' expr         
                {
                    $$ = pow($1,$3);
                }
            | expr '%' expr         
                {

                    $$ = (int)$1 % (int)$3;
                }
            | expr '<' expr         
                {
                    $$ = ($1 < $3);
                }
            | expr '>' expr         
                {
                    $$ = ($1 > $3);
                }
            | expr LEQL expr        
                {
                    $$ = ($1 <= $3);
                }
            | expr GEQL expr        
                {
                    $$ = ($1 >= $3);
                }
            | expr EQL expr         
                {
                    $$ = ($1 == $3);
                }
            | expr NEQL expr        
                {
                    $$ = ($1 != $3);
                }
            | expr AND expr         
                {
                    $$ = ( $1 && $3);
                }
            | expr OR expr          
                {
                    $$ = ($1 || $3);
                }
            | expr XOR expr         
                {
                    $$ = ((int)$1 ^ (int)$3);
                }
            | NOT expr              
                {
                    $$ = !$2;
                }
            | '(' expr ')'          
                {
                    $$ = $2;
                }
            | SIN '(' expr ')'      
                {
                    $$ = sin($3);

                }
            | COS '(' expr ')'      
                {
                    $$ = cos($3);
                }
            | TAN '(' expr ')'      
                {
                    $$ = tan($3);
                }
            | LOG '(' expr ')'      
                {
                    $$ = log10($3);
                }
            | LOG2 '(' expr ')'     
                {
                    $$ = log2($3);
                }
            | LN '(' expr ')'       
                {
                    $$ = log($3);
                }
            | SQRT '(' expr ')'     
                {
                    $$ = sqrt($3);
                }
            | VARIABLE PPLUS        
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist;
                    else if(vptr[id].type==2) notNumeric();
                    else if(vptr[id].type==1) printf("can't increment real.");
                    else {
                        vptr[id].ival[0]++;
                        $$ = vptr[id].ival[0];
                    }
                
                }
            | VARIABLE MMINUS       
                {
                    int id = getVariableIndex($1);
                    if(id==-1) doesNotExist($1);
                    else if(vptr[id].type==2) notNumeric();
                    else if(vptr[id].type==1) printf("can't decrement real.");
                    else {
                        vptr[id].ival[0]--;
                        $$ = vptr[id].ival[0];
                    }
                }
            | expr FACTORIAL   
                {
                    int cl = ceil($1);int fl = floor($1);
                    if(cl!=fl) printf("can't find FACTORIAL of real number.");
                    else {
                        long long x = 1;
                        for(long long i=1;i<=$1;i++) x*=i;
                        $$ = x;
                    }
                }
    ; 

%%


int main(int argc, char **argv){
//yydebug= 1;
    vptr = realloc(vptr,8*sizeof(var));
    stk =  malloc(5*sizeof(stack));
    vartaken = 100;
	yyin = fopen (argv[1],"r");
    //printf("Done");
    SetColor(2);
    printf("\n\n     -------Starting Program Execution-------\n\n\n");
	yyparse();
	return 0;
} 



    
     


