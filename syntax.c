/****************************************************/
/* File: syntax.c			            */
/* syntax function implementation for compiler PL/0 */
/* Compiler of PL/0				    */
/* Bolin Young					    */
/****************************************************/

#include "syntax.h"

int syntax_error = 0;                    /*记录语法分析过程中出现的错误*/
extern char tokenString[];               /*存储当前词法单元中的字符串*/
TokenType token;			 /*词法单元的内部表示*/
extern int lineno;			 /*源程序中的行号*/

int numVar = 0;				 /*记录一个过程中声明的变量的数目*/
extern struct BSTNode* currentProcAddr;	 /*指向当前过程在平衡树中的界限*/
extern struct BSTNode* currentNodePtr;	 /*指向当前插入到平衡树中的结点*/

int tag = 0;				 /*tag值为1表示是statement调用body,*/
                                         /*此时就不用生成分配空间的指令和推*/
					 /*栈指令*/

extern int cx;				 /*代码栈指针*/
extern struct objCode *code;		 /*存储目标代码*/
int level = 0;				 /*当前过程的嵌套深度*/
int procNest = 0;			 /*为0表示过程并行，为1表示过程嵌套*/

/*处理语法出的情形*/
void syntaxError(char* message)
{
    assert(message != NULL);

    if (message == NULL)
	return;

    printf("\n>>>");
    printf("syntax error at line %d:%s", lineno, message);
    ++syntax_error;
}

/*判断当前的词法单元是不是我们我期望的词法单元*/
void match(TokenType expected)
{
    if (token == expected)
    {
	token = getToken();
    }
    else
    {
	syntaxError("unexpected token-> ");
	printToken(token, tokenString);
	printf("	");
    }
}

/*语法分析函数*/
void syntaxAnalyze(void)
{
    prog();
}

/******************************/
/*<prog>->program<id>;<block> */
/******************************/
void prog(void)
{
    token = getToken();

    if (token == ENDFILE)
    {
	return;
    }

    match(PROGRAM_K);

    if (position(tokenString) != NULL)
    {
	syntaxError("过程名字已经被使用");
    }
    
    /*过程进入符号表*/
    enter(MAINPROCEDURE, tokenString);

    match(ID);
    match(SEMI);

    block();
}

/***********************************************/
/*<block>->[<condecl>][<vardecl>][<proc>]<body>*/
/***********************************************/
void block(void)
{
    int cx0;
    int tempAddr;

    gen(JMP, 0, 0);
    cx0 = cx;			/*记录要回填的位置*/

    if (token == ENDFILE)
	return;
    if (token == CONST_K)
    {
	condecl();
    }
    if (token == VAR_K)
    {
	vardecl();
    }

    /*********************************************/
    /*当变量声明结束后我们就知道了变量的数目，从 */
    /*而就 可以计算出当前过程在运行时需要的栈空间*/
    /*********************************************/
    currentProcAddr->table.size = numVar + 3;
    numVar = 0;

    if (token == PROCEDURE_K)
    {
	procNest = 1;
	proc();				/*这里的过程属于过程嵌套*/
    }
    
    code[cx0].offset = cx + 1;		/*回填JMP的目标地址*/
    body();
    
    /*修改当前过程的指针，此时当前过程发生变化*/
    currentProcAddr = currentProcAddr->prev;
}

/************************************/
/*<condecl>->const<cosnt>{,<const>};*/
/************************************/
void condecl(void)
{
    if (token == ENDFILE)
    {
	return;
    }

    match(CONST_K);
    Const();

    while (token == COMM)
    {
	match(COMM);
	Const();
    }

    match(SEMI);
}

/**************************/
/*<const>-><id>:=<integer>*/
/**************************/
void Const(void)
{
    if (token == ENDFILE)
    {
	return;
    }

    if (position(tokenString) != NULL)
    {
	syntaxError("常量名已经被使用");
    }
    enter(CONSTANT, tokenString);

    match(ID);
    match(ASSIGN);

    currentNodePtr->table.val = atoi(tokenString);	/*把常量的值存储在符号表中*/

    match(NUM);
}

/***************************
*<vardecl>->var<id>{,<id>};*
****************************/
void vardecl(void)
{
    if (token == ENDFILE)
    {
	return;
    }

    match(VAR_K);

    if (position(tokenString) != NULL)
    {
	syntaxError("变量名已经被使用");
    }
    enter(VARIABLE, tokenString);

    ++numVar;
    match(ID);

    while (token == COMM)
    {
	match(COMM);

	if (position(tokenString) != NULL)
	{
	    syntaxError("变量名已经被使用");
	}
	enter(VARIABLE, tokenString);

	++numVar;
	match(ID);
    }

    match(SEMI);
}

/*****************************************************/
/*<proc>->procedure<id>(<id>{,<id>});<block>{;<proc>}*/
/*****************************************************/
void proc(void)
{
    struct BSTNode * tempAddr;

    if (token == ENDFILE)
    {
	return;
    }

    match(PROCEDURE_K);
    if (position(tokenString) != NULL)
    {
	syntaxError("过程名已经被使用");
    }
    ++level;

    /*保存当前过程在平衡树中的指针*/
    tempAddr = currentProcAddr;

    enter(PROCEDURE, tokenString);
    if (procNest == 1)
    {
	/*填写直接外层过程的直接内层过程*/
	tempAddr->next = currentProcAddr;

	/*填写当前过程的直接外层过程*/
	currentProcAddr->prev = tempAddr;

	procNest = 0;
    }

    match(ID);
    match(LPAREN);
    match(ID);

    while (token == COMM)
    {
	match(COMM);
	match(ID);
    }

    match(RPAREN);
    match(SEMI);

    if (token == VAR_K || token == CONST_K ||
       token == PROCEDURE_K || token == BEGIN_K)
    {
	block();
    }
    
    /*这里是过程的并行声明，不是过程的嵌套声明*/
    while (token == SEMI && token != ENDFILE)
    {
	match(SEMI);
	proc();
    }
}

/******************************************
*<body>->begin<statement>{;<statement>}end*
*******************************************/
void body(void)
{
    struct BSTNode * temp;

    if (token == ENDFILE)
    {
	return;
    }

    match(BEGIN_K);

    if (tag == 0)
    {
	gen(INT, 0, currentProcAddr->table.size);	 /*生成位过程分配空间的指令*/
	currentProcAddr->table.addr = cx;		 /*把过程在代码栈中的开始指*/
	                                                 /*令的位置记录在符号表中  */
	/*从符号表中找出嵌套深度比当前过程的嵌套深度大的过程*/
        temp = currentProcAddr->next;
	if (temp != NULL && currentProcAddr->table.kind != MAINPROCEDURE)
	{
	    gen(CAL, abs(level - temp->table.lev), temp->table.addr);
	}
    }

    statement();
    while (token == SEMI)
    {
	match(SEMI);
	statement();
    }

    match(END_K);
    if (tag == 0)
    {
	--level;		/*遇见过程结束后嵌套深度就要减1*/
    }

    if (tag == 0)
    {
	gen(OPR, 0, 0);
    }
}

/*********************************************************/
/*<statement>-><id>:=<exp>                               */
/*      	|if<lexp>then<statement>[else<statement>]*/
/*		|while<lexp>do<statement>                */
/*		|call<id>[(<exp>{,<exp>})]               */
/*		|<body>                                  */
/*		|read(<id>{,<id>})                       */
/*		|write(<exp>{,<exp>})                    */
/*********************************************************/
void statement(void)
{
    int cx0, cx1;
    struct BSTNode *index;

    if (token == ENDFILE)
    {
	return;
    }

    if (token == ID)
    {
	index = position(tokenString);
	match(ID);
	match(ASSIGN);
	Exp();
	gen(STO, abs(index->table.lev - level), index->table.addr);
    }
    else if (token == IF_K)
    {
	match(IF_K);
	lexp();
	match(THEN_K);

	gen(JPC, 0, 0);
	cx0 = cx;	/*记录回填位置*/
	
	statement();
	code[cx0].offset = cx + 1;
	if (token == ELSE_K)
	{
	    match(ELSE_K);
	    statement();
	}
    }
    else if (token == WHILE_K)
    {
	match(WHILE_K);

	cx1 = cx + 1;		/*记录循环开始的目标代码的位置*/
	
	lexp();
	gen(JPC, 0, 0);
	cx0 = cx;

	match(DO_K);
	statement();
	gen(JMP, 0, cx1);
	code[cx0].offset = cx + 1;	/*循环条件不满足就跳至cx+1处，退出循环*/
    }
    else if (token == CALL_K)
    {
	match(CALL_K);
	index = position(tokenString);
	if (index == NULL)
	{
	    syntaxError("当前过程名没有定义");
	    exit(2);
	}

	gen(CAL, abs(index->table.lev - level), index->table.addr);
	match(ID);

	if (token == LPAREN)
	{
	    match(LPAREN);
	    Exp();
	    while (token == COMM)
	    {
		match(COMM);
		Exp();
	    }
	    match(RPAREN);
	}
    }
    else if (token == BEGIN_K)
    {
	tag = 1;
	body();
	tag = 0;
    }
    else if (token == READ_K)
    {
	match(READ_K);
	match(LPAREN);
	
	gen(OPR, 0, 16);
	index = position(tokenString);
	if (index == NULL)
	{
	    syntaxError("当前变量名没有定义");
	    exit(2);
	}

	gen(RED, abs(index->table.lev - level), index->table.addr);

	match(ID);
	while (token == COMM)
	{
	    match(COMM);
	    match(ID);
	}
	match(RPAREN);
    }
    else if (token == WRITE_K)
    {
	match(WRITE_K);
	match(LPAREN);
	Exp();

	gen(WRT, 0, 0);		/*将栈顶内容输出*/
	gen(OPR, 0, 15);	/*屏幕输出换行*/

	while (token == COMM)
	{
	    match(COMM);
	    Exp();

	    gen(WRT, 0, 0);
	    gen(OPR, 0, 15);
	}
	match(RPAREN);
    }
    else if (token != ENDFILE)
    {
	syntaxError("unexpected token-> ");
	printToken(token, tokenString);
	token = getToken();
    }
}

/**********************************/
/*<lexp>-><exp><lop><exp>|odd<exp>*/
/**********************************/
void lexp(void)
{
    TokenType temp;

    if (token == ENDFILE)
    {
	return;
    }

    if (token == ODD_K)
    {
	match(ODD_K);
	Exp();
	gen(OPR, 0, 6);
    }
    else
    {
	Exp();

	temp = token;

	lop();
	Exp();
	if (temp == EQ)
	{
	    gen(OPR, 0, 8);
	}
	else if (temp == NOEQ)
	{
	    gen(OPR, 0, 9);
	}
	else if (temp == LT)
	{
	    gen(OPR, 0, 10);
	}
	else if (temp == LTEQ)
	{
	    gen(OPR, 0, 13);
	}
	else if (temp == GTEQ)
	{
	    gen(OPR, 0, 11);
	}
	else if (temp == GT)
	{
	    gen(OPR, 0, 12);
	}
    }
}

/*********************************/
/*<exp>->[+|-]<term>{<aop><term>}*/
/*********************************/
void Exp(void)
{
    TokenType temp;

    if (token == ENDFILE)
    {
	return;
    }

    if (token == PLUS)
    {
	match(PLUS);
    }
    else if (token == MINUS)
    {
	match(MINUS);
    }

    term();
    while (token == PLUS || token == MINUS)
    {
	temp = token;
	aop();
	term();

	if (temp == PLUS)
	{
	    gen(OPR, 0, 2);
	}
	else if (temp == MINUS)
	{
	    gen(OPR, 0, 3);
	}
    }
}

/*********************************/
/*<term>-><factor>{<mop><factor>}*/
/*********************************/
void term(void)
{
    TokenType temp;

    if (token == ENDFILE)
    {
	return;
    }

    factor();
    while (token == TIMES || token == OVER)
    {
	temp = token;
	mop();
	factor();
	if (temp == TIMES)
	{
	    gen(OPR, 0, 4);
	}
	else if (temp == OVER)
	{
	    gen(OPR, 0, 5);
	}
    }
}

/***********************************/
/*<factor>-><id>|<interger>|(<exp>)*/
/***********************************/
void factor(void)
{
    struct BSTNode* index;

    if (token == ENDFILE)
    {
	return;
    }
    if (token == ID)
    {
	index = position(tokenString);
	if (index == NULL)
	{
	    syntaxError("当前变量名没有定义");
	    exit(2);
	}

	if (index->table.kind == CONSTANT)
	{
	    gen(LIT, 0, index->table.val);
	}
	else if (index->table.kind == VARIABLE)
	{
	    gen(LOD, abs(index->table.lev - level), index->table.addr);
	}
	match(ID);
    }
    else if (token == NUM)
    {
	gen(LIT, 0, atoi(tokenString));
	match(NUM);
    }
    else if (token == LPAREN)
    {
	match(LPAREN);
	Exp();
	match(RPAREN);
    }
}

/***********************/
/*<lop>->=|<>|<|<=|>|>=*/
/***********************/
void lop(void)
{
    if (token == ENDFILE)
    {
	return;
    }

    if (token == EQ)
    {
	match(EQ);
    }
    else if (token == NOEQ)
    {
	match(NOEQ);
    }
    else if (token == LT)
    {
	match(LT);
    }
    else if (token == LTEQ)
    {
	match(LTEQ);
    }
    else if (token == GT)
    {
	match(GT);
    }
    else if (token == GTEQ)
    {
	match(GTEQ);
    }
}

/************/
/*<aop>->+|-*/
/************/
void aop(void)
{
    if (token == ENDFILE)
    {
	return;
    }

    if (token == PLUS)
    {
	match(PLUS);
    }
    else if (token == MINUS)
    {
	match(MINUS);
    }
}

/***********/
/*<mop>->*|*/
/***********/
void mop(void)
{
    if (token == ENDFILE)
    {
	return;
    }

    if (token == TIMES)
    {
	match(TIMES);
    }
    else if (token == OVER)
    {
	match(OVER);
    }
}
