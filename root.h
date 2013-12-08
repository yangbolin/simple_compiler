#ifndef _ROOT_H
#define _ROOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define	FALSE		0	/*逻辑真的宏定义*/
#define TRUE		1	/*逻辑假的宏定义*/
#define	MAXRESERVED	15      /*PL/0中出现的关键字数目*/
#define MAXTOKENLEN	40	/*一个词法单元的最大长度*/
#define BUFLEN		128	/*词法分析时缓冲区的长度*/
#define MAXSIZE		1024	/*运行时栈的大小*/
#define INITSIZE	10	/*代码栈和符号表初始大小*/

/*词法分析得到的每个词法单元对应的内部表示*/
typedef enum
{
    /*文件结束标志和出错时对应的内部表示*/
    ENDFILE, ERROR,
    
    /*关键字对应的内部表示*/
    PROGRAM_K, CONST_K, VAR_K,   PROCEDURE_K, BEGIN_K,
    END_K,     IF_K,    WHILE_K, CALL_K,      DO_K,
    ELSE_K,    THEN_K,  WRITE_K, ODD_K,       READ_K,

    /*数字和常量对应的内部表示*/
    NUM,ID,

    /*其他特殊符号对应的内部表示*/
    ASSIGN, EQ,   LT,     GT,     PLUS, MINUS, 
    TIMES,  OVER, LPAREN, RPAREN, SEMI, COMM, 
    LTEQ,   GTEQ, NOEQ
}TokenType;


/*词法分析的过程中可能出现的一些状态*/
typedef enum
{
    START, 		/*词法分析开始*/
    INASSIGN,   	/*词法分析出现赋值的提示*/
    INNUM,  		/*词法分析出现数字*/
    INID, 		/*词法分析出现ID*/
    DONE, 		/*词法分析结束*/
    INLT, 		/*词法分析出现小于号*/
    INGT,		/*词法分析出现大于号*/
    INCOMMENT		/*词法分析中出现注释*/
}StateType;

/*目标代码的类型*/
typedef enum
{
    LIT, OPR, LOD, STO, CAL, INT, JMP, JPC, RED, WRT
}InstructionType;

#endif
