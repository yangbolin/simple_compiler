/************************************************/
/* File: main.c					*/
/* Main program for PL/0			*/
/* Compiler of PL/0				*/
/* Bolin Young					*/
/************************************************/

#include "root.h"
#include "lexical.h"
#include "syntax.h"

extern int lex_error;
extern int lineno;
extern int syntax_error;
extern int EOF_Flag;
FILE * source;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
	printf("Please use ./compile filename...\n");
	exit(0);
    }

    source = fopen(argv[1], "r");

    if (source == NULL)
    {
	printf("文件%s打开失败!\n", argv[1]);
	exit(1);
    }

    /*词法分析*/
    printf("Press any key to begin lexical ananlyze...\n");
    getchar();

    while (getToken() != ENDFILE)
	;

    fclose(source);
    printf("%d lexical errors!\n", lex_error);
    
    if (lex_error != 0)
    {
	exit(1);
    }

    lineno = 0;
    lex_error = 0;
    EOF_Flag = FALSE;
    source = fopen(argv[1], "r");

    if (source == NULL)
    {
	printf("文件%s打开失败!\n", argv[1]);
	exit(2);
    }

    /*语法分析，填符号表并且生成代码*/
    printf("Press any key to begin syntax analyze...\n");
    getchar();
    syntaxAnalyze();
    printf("%d syntax errors!\n", syntax_error);
    fclose(source);

    if (syntax_error != 0)
    {
	exit(1);
    }

    /*打印我们门生成的目标代码*/
    printf("Press any key to generate objective code...\n");
    getchar();
    printCode();

    /*对目标代码解释执行*/
    printf("Press any key to interpret run the objective code...\n");
    getchar();
    InterpretRun();

    return 0;
}
