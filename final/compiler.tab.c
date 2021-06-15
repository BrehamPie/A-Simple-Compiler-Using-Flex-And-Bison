
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "compiler.y"

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


/* Line 189 of yacc.c  */
#line 414 "compiler.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ROOT = 258,
     END = 259,
     START = 260,
     VARIABLE = 261,
     ARRAY_VAR = 262,
     EOL = 263,
     ARROW = 264,
     RARROW = 265,
     INTEGER = 266,
     REAL = 267,
     STRING = 268,
     INT_TYPE = 269,
     REAL_TYPE = 270,
     STRING_TYPE = 271,
     SEE = 272,
     READ = 273,
     AND = 274,
     OR = 275,
     NOT = 276,
     XOR = 277,
     LOG = 278,
     LOG2 = 279,
     LN = 280,
     SIN = 281,
     COS = 282,
     TAN = 283,
     FACTORIAL = 284,
     SQRT = 285,
     IF = 286,
     ELIF = 287,
     ELSE = 288,
     CHOICE = 289,
     DEFAULT = 290,
     OPTION = 291,
     FOREACH = 292,
     FROM = 293,
     TO = 294,
     DO = 295,
     WHILE = 296,
     BY = 297,
     AS = 298,
     COMMENT = 299,
     MODULE = 300,
     CALL = 301,
     PUSH = 302,
     POP = 303,
     SORT = 304,
     MMINUS = 305,
     PPLUS = 306,
     GEQL = 307,
     LEQL = 308,
     NEQL = 309,
     EQL = 310
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 343 "compiler.y"

	int integer;
	double real;
	char* string;



/* Line 214 of yacc.c  */
#line 513 "compiler.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 525 "compiler.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1001

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  136
/* YYNRULES -- Number of states.  */
#define YYNSTATES  307

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   310

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    62,     2,     2,
      70,    71,    60,    58,    64,    59,    72,    61,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      52,    65,    53,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    68,     2,    69,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    66,     2,    67,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    54,    55,    56,
      57
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,     9,    12,    14,    16,    19,    22,
      25,    28,    31,    33,    35,    37,    39,    41,    43,    46,
      49,    52,    56,    58,    62,    64,    66,    72,    76,    78,
      82,    84,    88,    90,    92,    98,   102,   104,   108,   110,
     114,   116,   118,   124,   128,   130,   134,   138,   142,   149,
     151,   153,   158,   162,   166,   173,   175,   180,   184,   186,
     190,   197,   201,   206,   207,   209,   212,   214,   219,   222,
     224,   230,   236,   238,   241,   243,   248,   251,   253,   259,
     269,   275,   283,   288,   293,   298,   303,   308,   313,   322,
     324,   328,   330,   333,   336,   339,   345,   347,   351,   353,
     355,   362,   368,   372,   374,   376,   378,   381,   384,   387,
     390,   394,   398,   402,   406,   410,   414,   418,   422,   426,
     430,   434,   438,   442,   446,   450,   453,   457,   462,   467,
     472,   477,   482,   487,   492,   495,   498
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      74,     0,    -1,     3,     5,    75,     4,    -1,    -1,    75,
      76,    -1,     8,    -1,    44,    -1,    77,     8,    -1,    91,
       8,    -1,    87,     8,    -1,    89,     8,    -1,   116,     8,
      -1,    93,    -1,    99,    -1,   105,    -1,   107,    -1,   111,
      -1,   115,    -1,    14,    78,    -1,    15,    81,    -1,    16,
      84,    -1,    78,    64,    79,    -1,    79,    -1,     6,    65,
     116,    -1,     6,    -1,     7,    -1,     7,    65,    66,    80,
      67,    -1,    80,    64,   116,    -1,   116,    -1,    81,    64,
      82,    -1,    82,    -1,     6,    65,   116,    -1,     6,    -1,
       7,    -1,     7,    65,    66,    83,    67,    -1,    83,    64,
     116,    -1,   116,    -1,    84,    64,    85,    -1,    85,    -1,
       6,    65,    13,    -1,     6,    -1,     7,    -1,     7,    65,
      66,    86,    67,    -1,    86,    64,    13,    -1,    13,    -1,
      17,     9,    88,    -1,    88,    64,     6,    -1,    88,    64,
       7,    -1,    88,    64,     7,    68,   116,    69,    -1,     6,
      -1,     7,    -1,     7,    68,   116,    69,    -1,    18,    10,
      90,    -1,    90,    64,     6,    -1,    90,    64,     7,    68,
      11,    69,    -1,     6,    -1,     7,    68,    11,    69,    -1,
      91,    64,    92,    -1,    92,    -1,     6,    65,   116,    -1,
       7,    68,    11,    69,    65,   116,    -1,    31,    94,    95,
      -1,   116,     5,    76,     4,    -1,    -1,    97,    -1,    97,
      96,    -1,    96,    -1,    33,     5,    76,     4,    -1,    97,
      98,    -1,    98,    -1,    32,   116,     5,    76,     4,    -1,
      34,   100,     5,   101,     4,    -1,     6,    -1,   103,   102,
      -1,   102,    -1,    35,     5,    76,     4,    -1,   103,   104,
      -1,   104,    -1,    36,   116,     5,    76,     4,    -1,    38,
     116,    39,   116,    42,   116,     5,    76,     4,    -1,    41,
     106,     5,    76,     4,    -1,    40,     5,   116,     4,    41,
     106,     8,    -1,     6,    51,    52,   116,    -1,     6,    51,
      55,   116,    -1,     6,    51,    56,   116,    -1,     6,    50,
      53,   116,    -1,     6,    50,    54,   116,    -1,     6,    50,
      56,   116,    -1,    45,   108,    70,   109,    71,     5,    75,
       4,    -1,     6,    -1,   109,    64,   110,    -1,   110,    -1,
      14,     6,    -1,    15,     6,    -1,    16,     6,    -1,    46,
     112,    70,   113,    71,    -1,     6,    -1,   113,    64,   114,
      -1,   114,    -1,     6,    -1,     7,    72,    47,    70,   116,
      71,    -1,     7,    72,    48,    70,    71,    -1,     7,    72,
      49,    -1,    11,    -1,    12,    -1,     6,    -1,    58,   116,
      -1,    59,   116,    -1,    51,   116,    -1,    50,   116,    -1,
     116,    58,   116,    -1,   116,    59,   116,    -1,   116,    60,
     116,    -1,   116,    61,   116,    -1,   116,    63,   116,    -1,
     116,    62,   116,    -1,   116,    52,   116,    -1,   116,    53,
     116,    -1,   116,    55,   116,    -1,   116,    54,   116,    -1,
     116,    57,   116,    -1,   116,    56,   116,    -1,   116,    19,
     116,    -1,   116,    20,   116,    -1,   116,    22,   116,    -1,
      21,   116,    -1,    70,   116,    71,    -1,    26,    70,   116,
      71,    -1,    27,    70,   116,    71,    -1,    28,    70,   116,
      71,    -1,    23,    70,   116,    71,    -1,    24,    70,   116,
      71,    -1,    25,    70,   116,    71,    -1,    30,    70,   116,
      71,    -1,     6,    51,    -1,     6,    50,    -1,   116,    29,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   373,   373,   379,   380,   383,   384,   390,   391,   392,
     393,   394,   401,   403,   404,   405,   425,   426,   429,   430,
     431,   434,   435,   438,   450,   456,   461,   470,   476,   484,
     485,   488,   500,   506,   512,   521,   527,   535,   536,   539,
     552,   558,   564,   574,   580,   588,   591,   595,   599,   603,
     607,   611,   617,   620,   624,   628,   632,   639,   640,   644,
     661,   685,   688,   705,   706,   707,   708,   710,   735,   736,
     739,   766,   770,   785,   786,   789,   805,   806,   809,   835,
     852,   861,   869,   884,   899,   914,   929,   944,   962,   965,
     979,   980,   983,   992,  1001,  1012,  1025,  1037,  1038,  1041,
    1062,  1079,  1087,  1100,  1104,  1109,  1119,  1123,  1127,  1131,
    1135,  1139,  1143,  1147,  1151,  1155,  1160,  1164,  1168,  1172,
    1176,  1180,  1184,  1188,  1192,  1196,  1200,  1204,  1209,  1213,
    1217,  1221,  1225,  1229,  1233,  1245,  1256
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ROOT", "END", "START", "VARIABLE",
  "ARRAY_VAR", "EOL", "ARROW", "RARROW", "INTEGER", "REAL", "STRING",
  "INT_TYPE", "REAL_TYPE", "STRING_TYPE", "SEE", "READ", "AND", "OR",
  "NOT", "XOR", "LOG", "LOG2", "LN", "SIN", "COS", "TAN", "FACTORIAL",
  "SQRT", "IF", "ELIF", "ELSE", "CHOICE", "DEFAULT", "OPTION", "FOREACH",
  "FROM", "TO", "DO", "WHILE", "BY", "AS", "COMMENT", "MODULE", "CALL",
  "PUSH", "POP", "SORT", "MMINUS", "PPLUS", "'<'", "'>'", "GEQL", "LEQL",
  "NEQL", "EQL", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "','", "'='",
  "'{'", "'}'", "'['", "']'", "'('", "')'", "'.'", "$accept", "program",
  "statements", "statement", "declaration", "int_variables", "int_var",
  "ints", "real_variables", "real_var", "reals", "string_variables",
  "string_var", "strings", "show", "print_vars", "read", "read_vars",
  "assigns", "assign", "if_blocks", "if_block", "else_statement",
  "single_else", "elif_statement", "single_elif", "choice_block",
  "choice_variable", "options", "default", "optionlist", "option",
  "loop_block", "while_conditions", "module_declare", "module_name",
  "module_variable", "single_var", "module_call", "user_module_name",
  "user_parameters", "single_param", "array_operations", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,    60,    62,   307,   308,   309,   310,    43,    45,
      42,    47,    37,    94,    44,    61,   123,   125,    91,    93,
      40,    41,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    75,    75,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    77,    77,
      77,    78,    78,    79,    79,    79,    79,    80,    80,    81,
      81,    82,    82,    82,    82,    83,    83,    84,    84,    85,
      85,    85,    85,    86,    86,    87,    88,    88,    88,    88,
      88,    88,    89,    90,    90,    90,    90,    91,    91,    92,
      92,    93,    94,    95,    95,    95,    95,    96,    97,    97,
      98,    99,   100,   101,   101,   102,   103,   103,   104,   105,
     105,   105,   106,   106,   106,   106,   106,   106,   107,   108,
     109,   109,   110,   110,   110,   111,   112,   113,   113,   114,
     115,   115,   115,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     0,     2,     1,     1,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     3,     1,     3,     1,     1,     5,     3,     1,     3,
       1,     3,     1,     1,     5,     3,     1,     3,     1,     3,
       1,     1,     5,     3,     1,     3,     3,     3,     6,     1,
       1,     4,     3,     3,     6,     1,     4,     3,     1,     3,
       6,     3,     4,     0,     1,     2,     1,     4,     2,     1,
       5,     5,     1,     2,     1,     4,     2,     1,     5,     9,
       5,     7,     4,     4,     4,     4,     4,     4,     8,     1,
       3,     1,     2,     2,     2,     5,     1,     3,     1,     1,
       6,     5,     3,     1,     1,     1,     2,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     4,     4,     4,
       4,     4,     4,     4,     2,     2,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     3,     1,     0,     2,   105,     0,     5,
     103,   104,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       6,     0,     0,     0,     0,     0,     0,     0,     4,     0,
       0,     0,     0,    58,    12,    13,    14,    15,    16,    17,
       0,   135,   134,     0,     0,     0,    24,    25,    18,    22,
      32,    33,    19,    30,    40,    41,    20,    38,     0,     0,
     105,   125,     0,     0,     0,     0,     0,     0,     0,    63,
       0,    72,     0,     0,     0,     0,     0,    89,     0,    96,
       0,   109,   108,   106,   107,     0,     7,     9,    10,     8,
       0,    11,     0,     0,     0,   136,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    59,     0,
       0,     0,   102,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    49,    50,    45,    55,     0,    52,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    61,    66,    64,
      69,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     126,     0,     0,    57,   122,   123,   124,   116,   117,   119,
     118,   121,   120,   110,   111,   112,   113,   115,   114,     0,
       0,     0,    23,     0,    21,    31,     0,    29,    39,     0,
      37,     0,     0,     0,     0,   130,   131,   132,   127,   128,
     129,   133,     0,     0,    65,    68,     0,     0,     0,     0,
      74,     0,    77,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    91,    99,     0,    98,
       0,     0,   101,     0,    28,     0,    36,    44,     0,     0,
      46,    47,     0,    53,     0,     0,     0,    62,     0,     0,
      71,    73,    76,     0,     0,    85,    86,    87,    82,    83,
      84,    80,    92,    93,    94,     0,     0,     0,    95,    60,
     100,     0,    26,     0,    34,     0,    42,    51,     0,    56,
       0,     0,    67,     0,     0,     0,     0,    90,     3,    97,
      27,    35,    43,     0,     0,    70,    75,     0,     0,    81,
       0,    48,    54,    78,     0,    88,    79
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     5,    38,    39,    58,    59,   233,    62,    63,
     235,    66,    67,   238,    40,   134,    41,   137,    42,    43,
      44,    79,   147,   148,   149,   150,    45,    82,   209,   210,
     211,   212,    46,    86,    47,    88,   225,   226,    48,    90,
     228,   229,    49,    50
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -131
static const yytype_int16 yypact[] =
{
      10,    33,    15,  -131,  -131,   200,  -131,   -39,     2,  -131,
    -131,  -131,    58,    98,   101,    31,    61,    18,    14,    44,
      59,    62,    63,    64,    65,    18,   131,    18,   133,   134,
    -131,   135,   138,    18,    18,    18,    18,    18,  -131,   139,
     140,   141,    -2,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
     479,  -131,  -131,    18,   128,     5,    81,    85,    89,  -131,
      86,    90,    92,  -131,    93,    94,    96,  -131,   104,   106,
     -46,   938,    18,    18,    18,    18,    18,    18,    18,    84,
     351,  -131,   152,   859,    18,    69,   156,  -131,    95,  -131,
      97,   926,   926,   -28,   -28,   491,  -131,  -131,  -131,  -131,
     115,  -131,    18,    18,    18,  -131,    18,    18,    18,    18,
      18,    18,    18,    18,    18,    18,    18,    18,   926,   102,
     100,   103,  -131,    18,   121,    58,    18,   122,    98,   149,
     126,   101,  -131,   125,   108,  -131,   127,   130,   536,   556,
     601,   621,   666,   686,   731,    18,   159,  -131,  -131,    84,
    -131,   317,    88,    18,   123,   -31,    -5,   317,    87,   183,
    -131,   132,   137,  -131,   938,   938,   938,    20,    20,    20,
      20,    20,    20,   -28,   -28,   -26,   -26,   -26,   -26,   144,
      18,   148,   926,    18,  -131,   926,    18,  -131,  -131,   177,
    -131,    18,   119,   185,   124,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,   364,   317,  -131,  -131,   206,   215,    18,   218,
    -131,    88,  -131,   881,   188,    18,    18,    18,    18,    18,
      18,   228,   227,   229,   231,   -62,  -131,  -131,   -57,  -131,
      18,   751,  -131,   -36,   926,    -1,   926,  -131,     8,   796,
    -131,   171,   173,  -131,   175,   317,   243,  -131,   317,   410,
    -131,  -131,  -131,    18,   134,   926,   926,   926,   926,   926,
     926,  -131,  -131,  -131,  -131,    87,   244,   183,  -131,   926,
    -131,    18,  -131,    18,  -131,   235,  -131,  -131,    18,  -131,
     241,   249,  -131,   251,   317,   423,   252,  -131,  -131,  -131,
     926,   926,  -131,   814,   193,  -131,  -131,   253,   317,  -131,
     260,  -131,  -131,  -131,   259,  -131,  -131
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -131,  -131,   -23,  -130,  -131,  -131,   154,  -131,  -131,   145,
    -131,  -131,   151,  -131,  -131,  -131,  -131,  -131,  -131,   169,
    -131,  -131,  -131,   143,  -131,   146,  -131,  -131,  -131,    78,
    -131,    82,  -131,    26,  -131,  -131,  -131,    32,  -131,  -131,
    -131,    29,  -131,   -17
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      71,   105,   265,   105,    51,    52,    99,   267,    80,   266,
      83,    51,    52,     1,   268,     4,    91,    92,    93,    94,
      95,   206,   215,   216,    70,   217,    53,   221,   271,    10,
      11,   272,   114,   115,   116,   117,   118,   117,     3,    17,
      68,    18,    19,    20,    21,    22,    23,   218,    24,   105,
     219,   220,   120,   121,   122,   138,   139,   140,   141,   142,
     143,   144,   100,   273,    56,    57,   274,   154,    33,    34,
      54,    69,   275,   246,    55,   276,    35,    36,   112,   113,
     114,   115,   116,   117,    72,   164,   165,   166,    37,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   222,   223,   224,    60,    61,   182,    64,    65,   185,
     132,   133,   135,   136,    73,   281,   145,   146,   283,   155,
     156,   161,   162,   207,   208,   240,   241,   214,   202,    74,
     243,   244,    75,    76,    77,    78,   213,    81,    84,   119,
      85,    87,   102,   103,    89,   104,   123,    96,    97,    98,
     124,   126,   105,   125,   297,   127,   128,   152,   129,   130,
     131,   157,   188,   231,   203,   158,   234,   159,   304,   236,
     180,   179,   192,   181,   239,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   183,   186,   227,
     237,   249,   189,   191,   194,   193,   242,    53,   255,   256,
     257,   258,   259,   260,     6,    54,     7,     8,     9,   230,
     247,    10,    11,   269,    12,    13,    14,    15,    16,   232,
     248,    17,   250,    18,    19,    20,    21,    22,    23,   254,
      24,    25,   261,   262,    26,   263,   285,   264,    27,   278,
      28,    29,   279,   280,    30,    31,    32,   282,   292,   288,
      33,    34,   294,   295,   290,   296,   291,   303,    35,    36,
     299,   293,   302,   306,   305,   300,     7,     8,     9,   163,
      37,    10,    11,   187,    12,    13,    14,    15,    16,   184,
     286,    17,   190,    18,    19,    20,    21,    22,    23,   251,
      24,    25,   204,   252,    26,   205,   289,   287,    27,     0,
      28,    29,     0,     0,    30,    31,    32,     0,     0,     0,
      33,    34,     0,     0,     0,     0,     0,     0,    35,    36,
       0,     0,     0,     7,     8,     9,     0,     0,    10,    11,
      37,    12,    13,    14,    15,    16,     0,     0,    17,     0,
      18,    19,    20,    21,    22,    23,     0,    24,    25,     0,
       0,    26,     0,     0,     0,    27,   151,    28,    29,     0,
       0,    30,    31,    32,     0,     0,     0,    33,    34,   245,
     102,   103,     0,   104,     0,    35,    36,     0,     0,     0,
     105,     0,     0,   102,   103,     0,   104,    37,     0,     0,
       0,     0,     0,   105,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   284,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   298,   102,
     103,     0,   104,     0,     0,     0,     0,     0,     0,   105,
       0,     0,   102,   103,     0,   104,     0,     0,     0,     0,
       0,     0,   105,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,     0,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   101,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   102,   103,
       0,   104,     0,     0,     0,     0,     0,     0,   105,     0,
     102,   103,     0,   104,     0,     0,     0,     0,     0,     0,
     105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   102,   103,     0,   104,     0,
       0,     0,   160,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   102,   103,     0,   104,     0,
       0,     0,     0,     0,     0,   105,     0,     0,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
       0,     0,     0,     0,     0,     0,     0,   195,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     102,   103,     0,   104,     0,     0,     0,   196,     0,     0,
     105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     102,   103,     0,   104,     0,     0,     0,     0,     0,     0,
     105,     0,     0,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,     0,     0,     0,     0,     0,
       0,     0,   197,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   102,   103,     0,   104,     0,
       0,     0,   198,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   102,   103,     0,   104,     0,
       0,     0,     0,     0,     0,   105,     0,     0,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
       0,     0,     0,     0,     0,     0,     0,   199,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     102,   103,     0,   104,     0,     0,     0,   200,     0,     0,
     105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     102,   103,     0,   104,     0,     0,     0,     0,     0,     0,
     105,     0,     0,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,     0,     0,     0,     0,     0,
       0,     0,   201,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   102,   103,     0,   104,     0,
       0,     0,   270,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,   102,   103,     0,   104,     0,     0,     0,
       0,     0,     0,   105,     0,     0,     0,     0,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
       0,     0,     0,     0,     0,   277,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   102,   103,
       0,   104,     0,   301,     0,     0,     0,     0,   105,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,     0,
     102,   103,     0,   104,     0,     0,     0,     0,     0,     0,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   253,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   102,   103,     0,   104,     0,
       0,     0,     0,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   105,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117
};

static const yytype_int16 yycheck[] =
{
      17,    29,    64,    29,    50,    51,     8,    64,    25,    71,
      27,    50,    51,     3,    71,     0,    33,    34,    35,    36,
      37,   151,    53,    54,     6,    56,    65,   157,    64,    11,
      12,    67,    60,    61,    62,    63,    53,    63,     5,    21,
       9,    23,    24,    25,    26,    27,    28,    52,    30,    29,
      55,    56,    47,    48,    49,    72,    73,    74,    75,    76,
      77,    78,    64,    64,     6,     7,    67,    84,    50,    51,
      68,    10,    64,   203,    72,    67,    58,    59,    58,    59,
      60,    61,    62,    63,    70,   102,   103,   104,    70,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    14,    15,    16,     6,     7,   123,     6,     7,   126,
       6,     7,     6,     7,    70,   245,    32,    33,   248,    50,
      51,     6,     7,    35,    36,     6,     7,     4,   145,    70,
       6,     7,    70,    70,    70,    70,   153,     6,     5,    11,
       6,     6,    19,    20,     6,    22,    65,     8,     8,     8,
      65,    65,    29,    64,   284,    65,    64,     5,    65,    65,
      64,     5,    13,   180,     5,    70,   183,    70,   298,   186,
      70,    69,    64,    70,   191,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    66,    66,     6,
      13,   208,    66,    68,    64,    68,    11,    65,   215,   216,
     217,   218,   219,   220,     4,    68,     6,     7,     8,    65,
       4,    11,    12,   230,    14,    15,    16,    17,    18,    71,
       5,    21,     4,    23,    24,    25,    26,    27,    28,    41,
      30,    31,     4,     6,    34,     6,   253,     6,    38,    68,
      40,    41,    69,    68,    44,    45,    46,     4,    13,     5,
      50,    51,    11,     4,   271,     4,   273,     4,    58,    59,
       8,   278,    69,     4,     4,   288,     6,     7,     8,   100,
      70,    11,    12,   128,    14,    15,    16,    17,    18,   125,
     254,    21,   131,    23,    24,    25,    26,    27,    28,   211,
      30,    31,   149,   211,    34,   149,   267,   265,    38,    -1,
      40,    41,    -1,    -1,    44,    45,    46,    -1,    -1,    -1,
      50,    51,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,
      -1,    -1,    -1,     6,     7,     8,    -1,    -1,    11,    12,
      70,    14,    15,    16,    17,    18,    -1,    -1,    21,    -1,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    34,    -1,    -1,    -1,    38,     5,    40,    41,    -1,
      -1,    44,    45,    46,    -1,    -1,    -1,    50,    51,     5,
      19,    20,    -1,    22,    -1,    58,    59,    -1,    -1,    -1,
      29,    -1,    -1,    19,    20,    -1,    22,    70,    -1,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,     5,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,     5,    19,
      20,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    -1,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,     8,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,
      19,    20,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    19,    20,    -1,    22,    -1,
      -1,    -1,    71,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    20,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      19,    20,    -1,    22,    -1,    -1,    -1,    71,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    20,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    19,    20,    -1,    22,    -1,
      -1,    -1,    71,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    20,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      19,    20,    -1,    22,    -1,    -1,    -1,    71,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    20,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    19,    20,    -1,    22,    -1,
      -1,    -1,    71,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      -1,    -1,    -1,    -1,    -1,    69,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    19,    20,
      -1,    22,    -1,    69,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,
      19,    20,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    19,    20,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    74,     5,     0,    75,     4,     6,     7,     8,
      11,    12,    14,    15,    16,    17,    18,    21,    23,    24,
      25,    26,    27,    28,    30,    31,    34,    38,    40,    41,
      44,    45,    46,    50,    51,    58,    59,    70,    76,    77,
      87,    89,    91,    92,    93,    99,   105,   107,   111,   115,
     116,    50,    51,    65,    68,    72,     6,     7,    78,    79,
       6,     7,    81,    82,     6,     7,    84,    85,     9,    10,
       6,   116,    70,    70,    70,    70,    70,    70,    70,    94,
     116,     6,   100,   116,     5,     6,   106,     6,   108,     6,
     112,   116,   116,   116,   116,   116,     8,     8,     8,     8,
      64,     8,    19,    20,    22,    29,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,   116,    11,
      47,    48,    49,    65,    65,    64,    65,    65,    64,    65,
      65,    64,     6,     7,    88,     6,     7,    90,   116,   116,
     116,   116,   116,   116,   116,    32,    33,    95,    96,    97,
      98,     5,     5,    39,   116,    50,    51,     5,    70,    70,
      71,     6,     7,    92,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,    69,
      70,    70,   116,    66,    79,   116,    66,    82,    13,    66,
      85,    68,    64,    68,    64,    71,    71,    71,    71,    71,
      71,    71,   116,     5,    96,    98,    76,    35,    36,   101,
     102,   103,   104,   116,     4,    53,    54,    56,    52,    55,
      56,    76,    14,    15,    16,   109,   110,     6,   113,   114,
      65,   116,    71,    80,   116,    83,   116,    13,    86,   116,
       6,     7,    11,     6,     7,     5,    76,     4,     5,   116,
       4,   102,   104,    42,    41,   116,   116,   116,   116,   116,
     116,     4,     6,     6,     6,    64,    71,    64,    71,   116,
      71,    64,    67,    64,    67,    64,    67,    69,    68,    69,
      68,    76,     4,    76,     5,   116,   106,   110,     5,   114,
     116,   116,    13,   116,    11,     4,     4,    76,     5,     8,
      75,    69,    69,     4,    76,     4,     4
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 374 "compiler.y"
    {
                    printf("\n\n     -------Program Compiled Successfully-------\n\n\n");
                ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 379 "compiler.y"
    {/*Do Nothing.*/;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 383 "compiler.y"
    {;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 385 "compiler.y"
    {
                    SetColor(1);
                    printf("  %s\n",(yyvsp[(1) - (1)].string));
                    SetColor(2);
                ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 390 "compiler.y"
    {;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 391 "compiler.y"
    {;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 392 "compiler.y"
    {;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 393 "compiler.y"
    {;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 395 "compiler.y"
    {
                    SetColor(8);
                    printf("Value of the expression:%.4lf\n",(yyvsp[(1) - (2)].real));
                    (yyval.real) = (yyvsp[(1) - (2)].real);
                    SetColor(2);
                ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 402 "compiler.y"
    {conditionMatched=0;;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 403 "compiler.y"
    {;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 404 "compiler.y"
    {;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 406 "compiler.y"
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
                ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 425 "compiler.y"
    {;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 426 "compiler.y"
    {;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 434 "compiler.y"
    {;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 435 "compiler.y"
    {;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 439 "compiler.y"
    {
                    int exists = checkExistance((yyvsp[(1) - (3)].string));
                    if(exists){
                        alreadyExist((yyvsp[(1) - (3)].string));
                    }
                    else{
                        int value = (yyvsp[(3) - (3)].real);
                        insertData((yyvsp[(1) - (3)].string),&value,0,varCnt,1,0);
                        varCnt++;
                    }
                ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 451 "compiler.y"
    {
                    int value = rand();
                    insertData((yyvsp[(1) - (1)].string),&value,0,varCnt,1,0);
                    varCnt++;
                ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 457 "compiler.y"
    {
                    insertData((yyvsp[(1) - (1)].string),itmp,0,varCnt,cnt,1);
                    varCnt++;
                ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 462 "compiler.y"
    {
                    insertData((yyvsp[(1) - (5)].string),itmp,0,varCnt,cnt,1);
                    varCnt++;
                    cnt = 0;
                    free(itmp);
                ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 471 "compiler.y"
    {
                    cnt++;
                    itmp = realloc(itmp,cnt*sizeof(int));;
                    itmp[cnt-1]=(yyvsp[(3) - (3)].real);
                ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 477 "compiler.y"
    {
                    cnt++;
                    itmp = realloc(itmp,cnt*sizeof(int));;
                    itmp[cnt-1]=(int)(yyvsp[(1) - (1)].real);
                ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 484 "compiler.y"
    {;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 485 "compiler.y"
    {;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 489 "compiler.y"
    {
                    int exists = checkExistance((yyvsp[(1) - (3)].string));
                    if(exists){
                        alreadyExist((yyvsp[(1) - (3)].string));
                    }
                    else{
                        double value = (yyvsp[(3) - (3)].real);
                        insertData((yyvsp[(1) - (3)].string),&value,1,varCnt,1,0);
                        varCnt++;
                    }
                ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 501 "compiler.y"
    {
                double value = rand();
                insertData((yyvsp[(1) - (1)].string),&value,1,varCnt,1,0);
                varCnt++;
            ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 507 "compiler.y"
    {
                    printf("Real Array Declaration.\n");
                    insertData((yyvsp[(1) - (1)].string),dtmp,1,varCnt,cnt,1);
                    varCnt++;
                ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 513 "compiler.y"
    {
                    insertData((yyvsp[(1) - (5)].string),dtmp,1,varCnt,cnt,1);
                    varCnt++;
                    cnt = 0;
                    free(dtmp);
                ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 522 "compiler.y"
    {
                    cnt++;
                    dtmp = realloc(dtmp,cnt*sizeof(double));;
                    dtmp[cnt-1]=(yyvsp[(3) - (3)].real);
                ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 528 "compiler.y"
    {
                    cnt++;
                    dtmp = realloc(dtmp,cnt*sizeof(double));;
                    dtmp[cnt-1]=(yyvsp[(1) - (1)].real);
                ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 535 "compiler.y"
    {;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 536 "compiler.y"
    {;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 540 "compiler.y"
    {
                    int exists = checkExistance((yyvsp[(1) - (3)].string));
                    if(exists){
                    printf("Variable Already declared.\n");
                    }
                    else{
                    char *value= (yyvsp[(3) - (3)].string);
                    insertData((yyvsp[(1) - (3)].string),&value,2,varCnt,1,0);
                    varCnt++;
                    printf("New variable initialized.\n");
                    }
                ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 553 "compiler.y"
    {
                    char* value = "";
                    insertData((yyvsp[(1) - (1)].string),&value,2,varCnt,1,0);
                    varCnt++;
                ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 559 "compiler.y"
    {
                    printf("Integer Array Declaration.\n");
                    insertData((yyvsp[(1) - (1)].string),stmp,2,varCnt,cnt,1);
                    varCnt++;
                ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 565 "compiler.y"
    {
                    printf("String Array of size:%d\n",cnt);
                    insertData((yyvsp[(1) - (5)].string),stmp,2,varCnt,cnt,1);
                    varCnt++;
                    cnt = 0;
                    free(stmp);
                ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 575 "compiler.y"
    {
                    cnt++;
                    stmp = realloc(stmp,cnt*sizeof(char*));;
                    stmp[cnt-1]=(yyvsp[(3) - (3)].string);
                ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 581 "compiler.y"
    {
                    cnt++;
                    stmp = realloc(stmp,cnt*sizeof(char*));;
                    stmp[cnt-1]=(yyvsp[(1) - (1)].string);
                ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 592 "compiler.y"
    {
                    printVariable((yyvsp[(3) - (3)].string));
                ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 596 "compiler.y"
    {
                    printVariable((yyvsp[(3) - (3)].string));
                ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 600 "compiler.y"
    {
                    printArrayIndex((yyvsp[(3) - (6)].string),(yyvsp[(5) - (6)].real));
                ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 604 "compiler.y"
    {
                    printVariable((yyvsp[(1) - (1)].string));
                ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 608 "compiler.y"
    {
                    printVariable((yyvsp[(1) - (1)].string));
                ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 612 "compiler.y"
    {
                    printArrayIndex((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].real));
                ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 617 "compiler.y"
    {;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 621 "compiler.y"
    {
                    takeInput((yyvsp[(3) - (3)].string),0);
                ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 625 "compiler.y"
    {
                    takeInput((yyvsp[(3) - (6)].string),(yyvsp[(5) - (6)].integer));
                ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 629 "compiler.y"
    {
                    takeInput((yyvsp[(1) - (1)].string),0);
                ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 633 "compiler.y"
    {
                    takeInput((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].integer));
                ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 645 "compiler.y"
    {
                    int index = getVariableIndex((yyvsp[(1) - (3)].string));
                    if (index == -1)
                    {
                        printf("there is no variable named %s.\n", (yyvsp[(1) - (3)].string));
                    }
                    else
                    {
                        {
                            if (vptr[index].type == 1)
                                vptr[index].dval[0] = (yyvsp[(3) - (3)].real);
                            if (vptr[index].type == 0)
                                vptr[index].ival[0] = (yyvsp[(3) - (3)].real);
                        }
                    }
                ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 662 "compiler.y"
    {
                    int id = (yyvsp[(3) - (6)].integer);
                    int index = getVariableIndex((yyvsp[(1) - (6)].string));
                    if (index == -1)
                    {
                        doesNotExist((yyvsp[(1) - (6)].string));
                    }
                    else
                    {
                        if(id>=vptr[index].size){
                            outOfRange();
                        }
                        else
                        {
                            if (vptr[index].type == 1)
                                vptr[index].dval[id] = (yyvsp[(6) - (6)].real);
                            if (vptr[index].type == 0)
                                vptr[index].ival[id] =  (yyvsp[(6) - (6)].real);
                        }
                    }
                ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 685 "compiler.y"
    {;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 689 "compiler.y"
    {
                    int isTrue = (fabs((yyvsp[(1) - (4)].real))>1e-9);
                    if(isTrue){
                        SetColor(8);
                        printf("Condition in if block is true.\n");
                        SetColor(2);
                        printf("Value of expression in if block is %.4lf\n",(yyvsp[(3) - (4)].real));
                        conditionMatched = 1;
                    }
                    else{
                        SetColor(8);
                        printf("Condition in if block is false.\n");
                        SetColor(2);
                    }
                ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 711 "compiler.y"
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
                            printf("Value of expression in else block is %.4lf\n",(yyvsp[(4) - (4)].integer));
                            conditionMatched = 1;
                        }
                        else{
                            SetColor(8);
                            printf("Condition in else block is false.\n");
                            SetColor(2);
                        }
                    }  
                ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 740 "compiler.y"
    {
                    if(conditionMatched){
                        SetColor(8);
                        printf("Condition already fulfilled.Ignoring elif block.\n");
                        SetColor(2);
                    }
                    else{
                            int isTrue = (fabs((yyvsp[(2) - (5)].real))>1e-9);
                            if(isTrue){
                                SetColor(8);
                                printf("Condition in elif block is true.\n");
                                SetColor(2);
                                printf("Value of expression in elif block is %.4lf\n",(yyvsp[(4) - (5)].real));
                                conditionMatched = 1;
                            }
                            else{
                                SetColor(8);
                                printf("Condition in elif block is false.\n");
                                SetColor(2);
                            }
                        }
                ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 767 "compiler.y"
    {conditionMatched = 0;;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 771 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (1)].string));
                    if(id==-1) printf("No such variable");
                    else if(vptr[id].type==2){
                        SetColor(4);
                        printf("can't assign string in choices.");
                        SetColor(2);
                    }
                    else if(vptr[id].type==0) choiceValue = vptr[id].ival[0];
                    else choiceValue = vptr[id].dval[0];
                ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 790 "compiler.y"
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
                        printf("Value of expression: %.4lf\n",(yyvsp[(3) - (4)].real));
                    }
                ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 810 "compiler.y"
    {
                    if(conditionMatched){
                        SetColor(8);
                        printf("Condition already fulfilled.Ignoring current option\n");
                        SetColor(2);
                        }
                    else{
                        int isTrue = (fabs((yyvsp[(2) - (5)].real)-choiceValue)<1e-9);
                            if(isTrue){
                                SetColor(8);
                                printf("Option matched.\n");
                                SetColor(2);
                                printf("Value of expression in current option %.4lf\n",(yyvsp[(4) - (5)].real));
                                conditionMatched = 1;
                            }
                            else{
                                SetColor(8);
                                printf("Condition of current option doesn't match.\n");
                                SetColor(2);
                            }
                    }
                ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 836 "compiler.y"
    {
                    double begin = (yyvsp[(2) - (9)].real);
                    double end = (yyvsp[(4) - (9)].real);
                    double add = (yyvsp[(6) - (9)].real);
                    double x = end-begin;
                    if(x*add < 0){
                        SetColor(4);
                        printf("Infinite  FROM loop\n");
                        SetColor(2);
                    }
                    else{
                          for(double i = begin ; i<=end ; i+=add){
                                printf("Runnning Inside From Loop and value of expression is:%.4lf\n",(yyvsp[(8) - (9)].real));
                             }
                    }   
                ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 853 "compiler.y"
    {
                    SetColor(8);
                    printf("While will run for %d times\n",(yyvsp[(2) - (5)].integer));
                    SetColor(2);
                    for(int i = 0;i<(yyvsp[(2) - (5)].integer);i++){
                        printf("Value of Statement %.4lf\n",(yyvsp[(4) - (5)].real));
                    }
                ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 862 "compiler.y"
    {
                    SetColor(8);
                    printf("repeat while will run for %d times\n",(yyvsp[(6) - (7)].integer));
                    SetColor(2);
                ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 870 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (4)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (4)].string));
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value> (yyvsp[(4) - (4)].real)){
                                (yyval.integer) = -1;
                            }
                            else{
                                (yyval.integer) = (int) (yyvsp[(4) - (4)].real) - value;
                            }
                    }
                ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 885 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (4)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (4)].string));
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value> (yyvsp[(4) - (4)].real)){
                                (yyval.integer) = -1;
                            }
                            else{
                                (yyval.integer) = (int) (yyvsp[(4) - (4)].real) - value+1;
                            }
                        }
                ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 900 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (4)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (4)].string));
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value> (yyvsp[(4) - (4)].real)){
                                (yyval.integer) = -1;
                            }
                            else{
                                (yyval.integer) = (int) (yyvsp[(4) - (4)].real) - value;
                            }
                    }
                ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 915 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (4)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (4)].string));
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value< (yyvsp[(4) - (4)].real)){
                                (yyval.integer) = -1;
                            }
                            else{
                                (yyval.integer) =value -(int) (yyvsp[(4) - (4)].real);
                            }
                    }
                ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 930 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (4)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (4)].string));
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value< (yyvsp[(4) - (4)].real)){
                                (yyval.integer) = -1;
                            }
                            else{
                                (yyval.integer) =value -(int) (yyvsp[(4) - (4)].real)+1;
                            }
                    }
                ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 945 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (4)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (4)].string));
                    else if(vptr[id].type!=0) printf("Only applicable for Integer.\n");
                    else {
                        long long value = vptr[id].ival[0];
                            if(value< (yyvsp[(4) - (4)].real)){
                                (yyval.integer) = -1;
                            }
                            else{
                                (yyval.integer) =value -(int) (yyvsp[(4) - (4)].real);
                            }
                    }
                ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 966 "compiler.y"
    {
                    int id = getFunctionIndex((yyvsp[(1) - (1)].string));
                    if(id!=-1){printf("Module Already Declared");}
                    else{
                        printf("Declaring Module\n");
                        stk[funCnt].fname = malloc((strlen((yyvsp[(1) - (1)].string))+10)*sizeof(char));
                        strcpy(stk[funCnt].fname,(yyvsp[(1) - (1)].string));
                        stk[funCnt].varCnt = 0; 
                        stk[funCnt].fptr = malloc(4*sizeof(stack));
                    }
                ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 984 "compiler.y"
    {
                    int id = stk[funCnt].varCnt;
                    int value = rand();
                    insertData((yyvsp[(2) - (2)].string),&value,0,varCnt,1,0);
                    stk[funCnt].fptr[id] = vptr[varCnt];
                    varCnt++;
                    stk[funCnt].varCnt++;
                ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 993 "compiler.y"
    {
                    int id = stk[funCnt].varCnt;
                    double value = rand();
                    insertData((yyvsp[(2) - (2)].string),&value,1,varCnt,1,0);
                    stk[funCnt].fptr[id] = vptr[varCnt];
                    varCnt++;
                    stk[funCnt].varCnt++;
                ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 1002 "compiler.y"
    {
                    int id = stk[funCnt].varCnt;
                    char* value = "";
                    insertData((yyvsp[(1) - (2)].string),&value,2,varCnt,1,0);
                    stk[funCnt].fptr[id] = vptr[varCnt];
                    varCnt++;
                    stk[funCnt].varCnt++;
                ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 1013 "compiler.y"
    {
                    if(functionRejected){
                        SetColor(4);
                        printf("Module was not called as declared\n");
                        SetColor(2);
                    }
                    else{
                        printf("Module called Successfully.\n");
                    }
                ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 1026 "compiler.y"
    {
                    int id = getFunctionIndex((yyvsp[(1) - (1)].string));
                    if(id==-1){printf(" No Function Exist with this name.");}
                    else{
                        currentFunction = id;
                        currentParam = 0;
                        functionRejected = 0;
                    }
                ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 1042 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (1)].string));
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
                ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 1063 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (6)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (6)].string));
                    else if(vptr[id].isArray==0)
                        notArray((yyvsp[(1) - (6)].string));
                    else{
                            if(vptr[id].type== 2){
                                notNumeric();
                            }
                            else{
                                double x = (yyvsp[(5) - (6)].real);
                               appendData(id,&x);
                            }
                        }
                    
                ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 1080 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (5)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (5)].string));
                    else{
                        popLast(id);
                    }
                ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 1088 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (3)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (3)].string));
                    else if(vptr[id].isArray==0)
                        notArray((yyvsp[(1) - (3)].string));
                    else{
                            sort(id);
                        }
                
                ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 1101 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(1) - (1)].integer);
                ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 1105 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(1) - (1)].real);
                ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1110 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (1)].string));
                    if(id==-1) doesNotExist;
                    else if(vptr[id].type==2){
                        notNumeric();
                    }
                    else if(vptr[id].type==0) (yyval.real) = vptr[id].ival[0];
                    else (yyval.real) = vptr[id].dval[0];
                ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1120 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(2) - (2)].real);
                ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1124 "compiler.y"
    {
                    (yyval.real) = -(yyvsp[(2) - (2)].real);
                ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 1128 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(2) - (2)].real);
                ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 1132 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(2) - (2)].real);
                ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 1136 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(1) - (3)].real) + (yyvsp[(3) - (3)].real);
                ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1140 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(1) - (3)].real) - (yyvsp[(3) - (3)].real);
                ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1144 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(1) - (3)].real) * (yyvsp[(3) - (3)].real);
                ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 1148 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(1) - (3)].real) / (yyvsp[(3) - (3)].real);
                ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1152 "compiler.y"
    {
                    (yyval.real) = pow((yyvsp[(1) - (3)].real),(yyvsp[(3) - (3)].real));
                ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 1156 "compiler.y"
    {

                    (yyval.real) = (int)(yyvsp[(1) - (3)].real) % (int)(yyvsp[(3) - (3)].real);
                ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 1161 "compiler.y"
    {
                    (yyval.real) = ((yyvsp[(1) - (3)].real) < (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 1165 "compiler.y"
    {
                    (yyval.real) = ((yyvsp[(1) - (3)].real) > (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 1169 "compiler.y"
    {
                    (yyval.real) = ((yyvsp[(1) - (3)].real) <= (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1173 "compiler.y"
    {
                    (yyval.real) = ((yyvsp[(1) - (3)].real) >= (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1177 "compiler.y"
    {
                    (yyval.real) = ((yyvsp[(1) - (3)].real) == (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1181 "compiler.y"
    {
                    (yyval.real) = ((yyvsp[(1) - (3)].real) != (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1185 "compiler.y"
    {
                    (yyval.real) = ( (yyvsp[(1) - (3)].real) && (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1189 "compiler.y"
    {
                    (yyval.real) = ((yyvsp[(1) - (3)].real) || (yyvsp[(3) - (3)].real));
                ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1193 "compiler.y"
    {
                    (yyval.real) = ((int)(yyvsp[(1) - (3)].real) ^ (int)(yyvsp[(3) - (3)].real));
                ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1197 "compiler.y"
    {
                    (yyval.real) = !(yyvsp[(2) - (2)].real);
                ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1201 "compiler.y"
    {
                    (yyval.real) = (yyvsp[(2) - (3)].real);
                ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1205 "compiler.y"
    {
                    (yyval.real) = sin((yyvsp[(3) - (4)].real));

                ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1210 "compiler.y"
    {
                    (yyval.real) = cos((yyvsp[(3) - (4)].real));
                ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1214 "compiler.y"
    {
                    (yyval.real) = tan((yyvsp[(3) - (4)].real));
                ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1218 "compiler.y"
    {
                    (yyval.real) = log10((yyvsp[(3) - (4)].real));
                ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1222 "compiler.y"
    {
                    (yyval.real) = log2((yyvsp[(3) - (4)].real));
                ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 1226 "compiler.y"
    {
                    (yyval.real) = log((yyvsp[(3) - (4)].real));
                ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1230 "compiler.y"
    {
                    (yyval.real) = sqrt((yyvsp[(3) - (4)].real));
                ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1234 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (2)].string));
                    if(id==-1) doesNotExist;
                    else if(vptr[id].type==2) notNumeric();
                    else if(vptr[id].type==1) printf("can't increment real.");
                    else {
                        vptr[id].ival[0]++;
                        (yyval.real) = vptr[id].ival[0];
                    }
                
                ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1246 "compiler.y"
    {
                    int id = getVariableIndex((yyvsp[(1) - (2)].string));
                    if(id==-1) doesNotExist((yyvsp[(1) - (2)].string));
                    else if(vptr[id].type==2) notNumeric();
                    else if(vptr[id].type==1) printf("can't decrement real.");
                    else {
                        vptr[id].ival[0]--;
                        (yyval.real) = vptr[id].ival[0];
                    }
                ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1257 "compiler.y"
    {
                    int cl = ceil((yyvsp[(1) - (2)].real));int fl = floor((yyvsp[(1) - (2)].real));
                    if(cl!=fl) printf("can't find FACTORIAL of real number.");
                    else {
                        long long x = 1;
                        for(long long i=1;i<=(yyvsp[(1) - (2)].real);i++) x*=i;
                        (yyval.real) = x;
                    }
                ;}
    break;



/* Line 1455 of yacc.c  */
#line 3494 "compiler.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1268 "compiler.y"



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



    
     



