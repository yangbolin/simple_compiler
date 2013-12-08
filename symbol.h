/*********************************************/
/* File: symbol.h                            */
/* functions of symbol table for PL/0        */
/* Compiler of PL/0  			     */
/* Bolin Young				     */
/*********************************************/

#ifndef _SYMBOL_H
#define _SYMBOL_H

#include "root.h"

/*定义符号表中元素的类型*/
typedef enum
{
    MAINPROCEDURE, PROCEDURE, VARIABLE, CONSTANT
}TypeInSymbolTable;

/*符号表的结构体*/
struct symbolTable
{
    char name[MAXTOKENLEN + 1];		/*名字*/
    TypeInSymbolTable kind;		/*符号表中元素的类型*/
    int lev;				/*存储变量和过程的嵌套深度*/
    int val;				/*存储常量的值*/
    int addr;	/*过程存储其在运行栈中的首地址，变量存储其在运行栈中相对于过程首地址的偏移量*/
    int size;	/*过程的大小等于3加过程中定义的变量数目*/
};

/*平衡树的数据结构*/
typedef struct BSTNode{
    struct symbolTable table;		/*符号表*/
    int bf;				/*平衡因子*/
    struct BSTNode *lchild;
    struct BSTNode *rchild;
    struct BSTNode *next;	/*如果当前结点是过程结点，指向其直接的内层嵌套过程*/
    struct BSTNode *prev;	/*如果当前机电是过程结点，指向其直接的外层嵌套过程*/
}BSTNode, *BSTree;

#define LH	+1	/*左子树比右子树高*/
#define EH	0	/*左子树和右子树等高*/
#define RH	-1	/*右子树比左子树高*/

extern void enter(TypeInSymbolTable, const char*);
extern struct BSTNode* position(const char*);
extern struct BSTNode* search(BSTree, const char*);
extern void R_Rotate(BSTree*);
extern void L_Rotate(BSTree*);
extern void LeftBalance(BSTree*);
extern void RightBalance(BSTree*);
extern int InsertAVL(BSTree*, BSTNode*);

#endif
