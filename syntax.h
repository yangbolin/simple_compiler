/************************************************/
/* File: syntax.h                               */
/* Syntax function for PL/0 compiler            */
/* Compiler of PL/0                             */
/* Bolin Young                                  */
/************************************************/

#ifndef _SYNTAX_H
#define _SYNTAX_H

#include "root.h"
#include "lexical.h"
#include "symbol.h"
#include "gencode.h"
#include "interpretrun.h"

extern void syntaxError(char *);
extern void match(TokenType);
extern void syntaxAnalyze(void);
extern void prog(void);
extern void block(void);
extern void condecl(void);
extern void Const(void);
extern void vardecl(void);
extern void proc(void);
extern void body(void);
extern void statement(void);
extern void lexp(void);
extern void Exp(void);
extern void term(void);
extern void factor(void);
extern void lop(void);
extern void aop(void);
extern void mop(void);

#endif
