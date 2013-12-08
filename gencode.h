/***************************************************/
/* File: gencode.h                                 */
/* functions of generating objective code for PL/0 */
/* Compiler of PL/0                                */
/* Bolin Young                                     */
/***************************************************/

#ifndef _GENCODE_H
#define _GENCODE_H

#include "root.h"

/*目标代码的存储结构*/
struct objCode
{
    InstructionType cmd;	/*目标代码的类型*/
    int lev;			/*层差即定义层和调用层之间的嵌套深度的差值*/
    int offset;			/*偏移量*/
};

extern void gen(InstructionType, int, int);
extern void printCode();

#endif
