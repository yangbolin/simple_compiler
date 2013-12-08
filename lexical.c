/*************************************************************/
/* File: lexical.c                                           */
/* lexical analyze function implementation for compiler PL/0 */
/* Compiler of PL/0                                          */
/* Bolin Young                                               */
/*************************************************************/

#include "lexical.h"

char tokenString[MAXTOKENLEN + 1];     	/*保存我们词法分析的词法单元*/
char lineBuf[BUFLEN];			/*先把文件中的内容读入到该缓冲区中*/
int  linepos = 0;			/*缓冲区的指针*/
int  EOF_Flag = FALSE;			/*文件结尾标识*/
int  lineno = 0;			/*指示文件的第几行*/
int  lex_error = 0;			/*词法错误的次数*/
int  buffersize = 0;                    /*记录一次从文件读多少字符到缓冲区中*/
extern FILE *source;                    /*存放源代码的文件*/

/*把诸关键字和其对应的内部表示按字典序存储在一个结构体数组中*/
struct{
    char *str;
    TokenType tok;
}reservedWords[MAXRESERVED]
={{"begin", BEGIN_K},     {"call", CALL_K},   {"const", CONST_K},   
  {"do", DO_K},           {"else",  ELSE_K},  {"end", END_K},     
  {"if", IF_K},           {"odd",   ODD_K},   {"procedure", PROCEDURE_K},
  {"program", PROGRAM_K}, {"read",  READ_K},  {"then", THEN_K},
  {"var", VAR_K},         {"while", WHILE_K}, {"write", WRITE_K}};

/*查找当前词法单元是否是一个关键字，二分查找*/
TokenType reservedLookup(const char* str)
{
    assert(str != NULL);

    int low = 0;
    int high = MAXRESERVED - 1;

    while (low <= high)
    {
	int mid = (low + high) / 2;
	if (strcmp(str, reservedWords[mid].str) == 0)
	{
	    return reservedWords[mid].tok; /*当前词法单元是一个关键字*/
	}
	else if (strcmp(str, reservedWords[mid].str) < 0)
	{
	    high = mid - 1;
	}
	else if (strcmp(str, reservedWords[mid].str) > 0)
	{
	    low = mid + 1;
	}
    }

    return ID;      /*当前词法单元是一个ID*/
}

/*从缓冲区中获取一个一个字符，如果缓冲区中没有字符，
  重新从文件中读取一行到缓冲区中*/
int getNextChar(void)
{
    if (!(linepos < buffersize))  /*判断是否需要再读取一行到我们的缓冲区中*/
    {
	lineno++;
	if (fgets(lineBuf, BUFLEN - 1, source))
	{
	    printf("%4d: %s", lineno, lineBuf);
	    buffersize = strlen(lineBuf);
	    linepos = 0;
	    return lineBuf[linepos++];
	}
	else
	{
	    EOF_Flag = TRUE;
	    return EOF;
	}
    }
    else
    {
	return lineBuf[linepos++];
    }
}

/*把一个字符回退到缓冲区中去*/
void ungetNextChar(void)
{
    if (!EOF_Flag)
    {
	linepos--;       
    }
}

/*从缓冲区中把词法单元一个一个解析出来*/
TokenType getToken(void)
{
    int 	tokenStringIndex = 0;   /*字符数组tokenString的下标指针*/
    TokenType 	currentToken;           /*当前词法单元的类型*/
    StateType 	state = START;          /*表示词法分析开始*/
    int		save;			/*指示当前字符是否要保存*/
    
    while (state != DONE)
    {
	int c = getNextChar();
	save = TRUE;
	switch (state)
	{
	case START:
	     if (isdigit(c))
	     {
		 state = INNUM;		 /*当前处于数字识别状态中*/
	     }
	     else if (isalpha(c))
	     {
		 state = INID;		 /*当前处于ID识别状态中*/
	     }
	     else if (c == ':')
	     {
		 state = INASSIGN;       /*当前处于赋值符号的识别状态中*/
	     }
             else if (c == ' ' || c == '\t' || c == '\n')
	     {
		 save = 0;
	     }
	     else if (c == '>')
	     {
		 state = INGT;
	     }
             else if (c == '<')
	     {
		 state = INLT;
	     }
	     else if (c == '{')
	     {
		 save = 0;
		 state = INCOMMENT;
	     }
	     /*上面的情况需要我们进一步考虑词法单元到底是什么*/
	     else
	     {
		 state = DONE;
		 switch (c)
		 {
		 case EOF:
		      save = FALSE;
		      currentToken = ENDFILE;
		      break;
		 case '=':
		      currentToken = EQ;
		      break;
		 case '+':
		      currentToken = PLUS;
		      break;
		 case '-':
		      currentToken = MINUS;
		      break;
		 case '*':
		      currentToken = TIMES;
		      break;
		 case '/':
		      currentToken = OVER;
		      break;
		 case ';':
		      currentToken = SEMI;
		      break;
		 case ',':
		      currentToken = COMM;
		      break;
		 case ')':
		      currentToken = RPAREN;
		      break;
		 case '(':
		      currentToken = LPAREN;
		      break;
		 default:
		      currentToken = ERROR;
		      break;
		 }
	     }
	     break;
	case INLT:
	     if (c == '=')
	     {
		 currentToken = LTEQ;
	     }
	     else if (c == '>')
	     {
		 currentToken = NOEQ;
	     }
	     else
	     {
		 /*把当前字符回退到缓冲区中*/
		 ungetNextChar();
		 save = FALSE;
		 currentToken = LT;
	     }
	     state = DONE;
	     break;
	case INGT:
	     if (c == '=')
	     {
		 currentToken = GTEQ;
	     }
	     else
	     {
		 /*把当前字符回退到缓冲区中*/
		 ungetNextChar();
		 save = FALSE;
		 currentToken = GT;
	     }
	     state = DONE;
	     break;
	case INASSIGN:
	     if (c == '=')
	     {
		 currentToken = ASSIGN;
	     }
	     else
	     {
		 /*把当前字符回退到缓冲区中*/
		 ungetNextChar();
		 save = FALSE;
		 currentToken = ERROR;
	     }
	     state = DONE;
	     break;
	case INNUM:
	     if (!isdigit(c))
	     {
		 /*把当前字符回退到缓冲区*/
		 ungetNextChar();
		 save = FALSE;
		 state = DONE;
		 currentToken = NUM;
	     }
	     break;
	case INID:
	     if (!isalpha(c) && !isdigit(c))
	     {
		 /*把当前字符回退到缓冲区中*/
		 ungetNextChar();
		 save = FALSE;
		 state = DONE;
		 currentToken = ID;
	     }
	     break;
	case INCOMMENT:
	     save = 0;
	     if (c == EOF)
	     {
		 state = DONE;
		 currentToken = ENDFILE;
	     }
	     else if (c == '}')
		 state = START;
	     break;
	case DONE:
	     break;
	default:
	     printf("Scanner bug: state = %d\n", state);
	     state = DONE;
	     currentToken = ERROR;
	}
	
	if ((save) && (tokenStringIndex <= MAXTOKENLEN))
	{
	    tokenString[tokenStringIndex++] = (char)c;  /*保存当前字符*/
	}
	if (state == DONE)
	{
	    tokenString[tokenStringIndex] = '\0';
	    if (currentToken == ID)
	    {
		currentToken = reservedLookup(tokenString);
	    }
	    printf("\t%d: ", lineno);
	    printToken(currentToken, tokenString);
	}
    }
    return currentToken;                  /*返回当前的词法单元类别*/
}

/*把源程序中的每一个词法单元和与之对应的内部表示答应出来*/
void printToken(TokenType token, const char* tokString)
{
    assert(tokString != NULL);

    switch (token)
    {
    case PROGRAM_K:
    case CONST_K:
    case VAR_K:
    case PROCEDURE_K:
    case BEGIN_K:
    case END_K:
    case IF_K:
    case WHILE_K:
    case CALL_K:
    case DO_K:
    case ELSE_K:
    case THEN_K:
    case WRITE_K:
    case ODD_K:
    case READ_K:
         printf("reserved word: %s\n", tokString);
	 break;
    case ENDFILE:
         printf("EOF\n");
	 break;
    case ERROR:
         printf("ERROR: %s\n", tokString);
	 ++lex_error;
	 break;
    case NUM:
         printf("NUM, val = %s\n", tokString);
	 break;
    case ID:
         printf("ID, name = %s\n", tokString);
	 break;
    case ASSIGN:
         printf(":=\n");
	 break;
    case EQ:
         printf("=\n");
	 break;
    case LT:
         printf("<\n");
	 break;
    case GT:
         printf(">\n");
	 break;
    case PLUS:
         printf("+\n");
	 break;
    case TIMES:
         printf("*\n");
         break;
    case OVER:
         printf("/\n");
	 break;
    case LPAREN:
         printf("(\n");
	 break;
    case RPAREN:
         printf(")\n");
	 break;
    case SEMI:
         printf(";\n");
	 break;
    case COMM:
         printf(",\n");
	 break;
    case LTEQ:
         printf("<=\n");
	 break;
    case GTEQ:
         printf(">=\n");
	 break;
    case NOEQ:
         printf("<>\n");
	 break;
    default:
         printf("Unkown token: %d\n", token);
	 break;
    }
}
