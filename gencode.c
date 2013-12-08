/**************************************************/
/* File: gencode.c                                */
/* generate code function implementation for PL/0 */
/* Compiler of PL/0                               */
/* Bolin Young                                    */
/**************************************************/

#include "gencode.h"

struct objCode *code;		/*代码栈数组*/
int 	cs_length = 0;		/*代码栈的长度*/

int cx = -1;			/*代码栈指针*/

/*生成目标代码*/
void gen(InstructionType cmd, int lev, int offset)
{
    cx = cx + 1;
    if (cs_length == 0)
    {
	code = (struct objCode *)malloc(sizeof(struct objCode) * INITSIZE);
	if (code == NULL)
	{
	    printf("内存空间分配失败!\n");
	    exit(1);
	}
	cs_length = INITSIZE;
    }
    else if (cx == cs_length)
    {
	int i;
	cs_length = 2 * cs_length;
	struct objCode *newBase = (struct objCode *)malloc(sizeof (struct objCode) * (cs_length));
	if (newBase == NULL)
	{
	    printf("内存空间分配失败!\n");
	    exit(1);
	}

	for (i = 0; i < cx; ++i)
	{
	    newBase[i].cmd = code[i].cmd;
	    newBase[i].lev = code[i].lev;
	    newBase[i].offset = code[i].offset;
	}

	free(code);
	code = newBase;
    }

    code[cx].cmd = cmd;
    code[cx].lev = lev;
    code[cx].offset = offset;
}

/*输出目标代码*/
void printCode()
{
    int i;
    for (i = 0; i <= cx; ++i)
    {
	switch (code[i].cmd)
	{
	case LIT:
	     printf("%4d: LIT %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case OPR:
	     printf("%4d: OPR %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case LOD:
	     printf("%4d: LOD %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case STO:
	     printf("%4d: STO %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case CAL:
	     printf("%4d: CAL %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case INT:
	     printf("%4d: INT %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case JMP:
	     printf("%4d: JMP %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case JPC:
	     printf("%4d: JPC %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case RED:
	     printf("%4d: RED %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	case WRT:
	     printf("%4d: WRT %d %d\n", i, code[i].lev, code[i].offset);
	     break;
	default:
	     break;
	}
    }
}
