/**********************************************************/
/* File: interpretrun.c					  */
/* Interpretrun function implementation for PL/0 compiler */
/* Compiler of PL/0					  */
/* Bolin Young						  */
/**********************************************************/

#include "interpretrun.h"

int runStack[MAXSIZE];		/*运行时栈*/
int instruction = 0;		/*指令寄存器，存放当前要执行的指令*/
int topOfRunStack = -1;		/*栈顶指示寄存器，指向数据栈的栈顶*/
int pc = 0;			/*程序计数器，指向下一条将要执行的指令*/
int currentProcBase = 0;	/*存放当前过程的基地址*/

extern int cx;			/*代码栈指针*/
extern struct objCode *code;	/*代码栈指针*/
/*extern struct objCode code[];	注意这里的错误写法，很重要！！！*/

/*对目标代码进行解释执行*/
void InterpretRun()
{
    int temp1, temp2, baseAddr;

    while (instruction <= cx)
    {
	switch (code[instruction].cmd)
	{
	case LIT:	/*把指令中的常熟放在栈顶*/
	     topOfRunStack = topOfRunStack + 1;
	     runStack[topOfRunStack] = code[instruction].offset;
	     
	     ++pc;
	     break;
	case OPR:
	     switch (code[instruction].offset)
	     {
	     case 0:	/*过程调用结束，返回调用点并且退栈*/
	          topOfRunStack = currentProcBase - 1;	/*栈顶恢复到调用前的栈顶*/
		  if (pc == cx)
		      pc = cx + 1;
		  else 
		  {
		      pc = runStack[topOfRunStack + 3];	/*保存返回地址*/
		  }
		  currentProcBase = runStack[topOfRunStack + 2]; /*调用者活动记录首地址*/
		  break;
	     case 1:	/*栈顶元素取反*/
	          runStack[topOfRunStack] = 0 - runStack[topOfRunStack];
	          
		  ++pc;
		  break;
	     case 2:	/*栈顶元素与次栈顶元素相加，两个栈顶元素退栈，结果存到栈顶*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];
		  runStack[topOfRunStack] = temp1 + temp2;
		  
		  ++pc;
		  break;
	     case 3:	/*次栈顶元素减去栈顶元素，两个栈顶元素退栈，结果存放到栈顶*/
	     	  temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];
		  runStack[topOfRunStack] = temp2 - temp1;
		  
		  ++pc;
		  break;
	     case 4:	/*次栈顶元素乘以栈顶元素，两个栈顶元素退栈，结果存放到栈顶*/
	     	  temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];
		  runStack[topOfRunStack] = temp1 * temp2;
		  
		  ++pc;
		  break;
	     case 5:	/*次栈顶元素除以栈顶元素，两个栈顶元素退栈，结果存放到栈顶*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];
		  runStack[topOfRunStack] = temp2 / temp1;
		  
		  ++pc;
		  break;
	     case 6:	/*判断栈顶元素的奇偶性*/
	          if(runStack[topOfRunStack] % 2 == 0)
		  {
		      runStack[topOfRunStack] = 1;	/*栈顶元素为偶数*/
		  }
		  else 
		  {
		      runStack[topOfRunStack] = 0;	/*栈顶元素为奇数*/
		  }
		  
		  ++pc;
		  break;
	     case 7:	/*一个不会出现的空操作*/
	          ++pc;
		  break;
	     case 8:	/*此栈顶和栈顶元素是否相同，两个栈顶元素退栈， 结果值进栈*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];

		  if (temp1 == temp2)
		  {
		      runStack[topOfRunStack] = 1;
		  }
		  else
		  {
		      runStack[topOfRunStack] = 0;
		  }
		  
		  ++pc;
		  break;
	     case 9:	/*次栈顶元素与栈顶元素是否不相等，两个栈顶元素退栈，结果值进栈*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];

		  if (temp1 != temp2)
		  {
		      runStack[topOfRunStack] = 1;
		  }
		  else
		  {
		      runStack[topOfRunStack] = 0;
		  }
		  
		  ++pc;
		  break;
	     case 10:	/*次栈顶元素是否小于栈顶元素，两个栈顶元素退栈，结果值进栈*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];
		  
		  if (temp2 < temp1)
		  {
		      runStack[topOfRunStack] = 1;
		  }
		  else
		  {
		      runStack[topOfRunStack] = 0;
		  }
		  
		  ++pc;
		  break;
	     case 11:	/*次栈顶元素是否大于等于栈顶元素，两个栈顶元素退栈，结果值进栈*/
	     	  temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];

		  if (temp2 >= temp1)
		  {
		      runStack[topOfRunStack] = 1;
		  }
		  else
		  {
		      runStack[topOfRunStack] = 0;
		  }
		  
		  ++pc;
		  break;
	     case 12:	/*次栈顶元素是否大于栈顶元素，两个栈顶元素退栈，结果值进栈*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];

		  if (temp2 > temp1)
		  {
		      runStack[topOfRunStack] = 1;
		  }
		  else 
		  {
		      runStack[topOfRunStack] = 0;
		  }
		  
		  ++pc;
		  break;
	     case 13:	/*次栈顶元素是否小于等于栈顶元素，两个栈顶元素退栈，结果值进栈*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;
		  temp2 = runStack[topOfRunStack];

		  if (temp2 <= temp1)
		  {
		      runStack[topOfRunStack] = 1;
		  }
		  else
		  {
		      runStack[topOfRunStack] = 0;
		  }
		  
		  ++pc;
		  break;
	     case 14:	/*栈顶的值输出至屏幕*/
	          temp1 = runStack[topOfRunStack];
		  --topOfRunStack;

		  printf("Output a number: ");
		  printf("%d", temp1);
		  
		  ++pc;
		  break;
	     case 15:
	     	  printf("\n");
		  
		  ++pc;
		  break;
	     case 16:
	     	  printf("Please input a number: ");
		  scanf("%d", &temp1);

		  ++topOfRunStack;
		  runStack[topOfRunStack] = temp1;
		  
		  ++pc;
		  break;
	     default:
	          break;
	     }
	     break;
	case LOD:	/*取变量的值放在数据栈的栈顶，相对地址位a, 层差为L*/
	     baseAddr = base(code[instruction].lev);	/*根据层差来计算变量所在过程的基地址*/
	     ++topOfRunStack;
	     runStack[topOfRunStack] = runStack[baseAddr + code[instruction].offset];
	     
	     ++pc;
	     break;
	case STO:	/*将栈顶的内容存入到变量中*/
	     baseAddr = base(code[instruction].lev);
	     runStack[baseAddr + code[instruction].offset] = runStack[topOfRunStack];
	     
	     ++pc;
	     --topOfRunStack;
	     break;
	case CAL:	/*调用过程，转子指令，入口地址位a，层差为L*/
	     runStack[topOfRunStack + 1] = currentProcBase;	/*该过程的直接外围过程*/
	     runStack[topOfRunStack + 2] = currentProcBase;	/*调用者首地址*/
	     runStack[topOfRunStack + 3] = pc + 1;	/*返回地址*/
	     currentProcBase = topOfRunStack + 1;	/*被调过程的基地址*/
	     pc = code[instruction].offset;
	     break;
	case INT:	/*为被调用的过程分配栈的大小*/
	     topOfRunStack += code[instruction].offset;
	     
	     ++pc;
	     break;
	case JMP:	/*无条件转移指令*/
	     pc = code[instruction].offset;
	     break;
	case JPC:	/*条件转移指令*/
	     if (runStack[topOfRunStack] == 0)
	     {
		 pc = code[instruction].offset;
	     }
	     else
	     {
		 ++pc;
	     }
	     
	     --topOfRunStack;
	     break;
	case RED:	/*读入数据并且存入变量*/
	     baseAddr = base(code[instruction].lev);
	     runStack[baseAddr + code[instruction].offset] = runStack[topOfRunStack];
	     
	     ++pc;
	     --topOfRunStack;
	     break;
	case WRT:	/*将栈顶的内容输出*/
	     printf("Output a number: ");
	     printf("%d", runStack[topOfRunStack]);
	     
	     ++pc;
	     --topOfRunStack;
	     break;
	default:
	     break;
	}
	instruction = pc;
    }
}

/*计算当前过程的直接外层过程的首地址*/
int base(int l)
{
    int b1;

    b1 = currentProcBase;
    while (l > 0)
    {
	b1 = runStack[b1];
	l = l -1;
    }

    return b1;
}
