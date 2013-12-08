/*****************************************************/
/* File: lexical.h				     */
/* lexical analyze function for PL/0 compiler	     */
/* Compiler of PL/0				     */
/* Bolin Young					     */
/*****************************************************/

#ifndef _LEXICAL_H
#define _LEXICAL_H

#include "root.h"

extern TokenType getToken(void);
extern TokenType reservedLookup(const char*);
extern int getNextChar(void);
extern void printToken(TokenType, const char*);
extern void ungetNextChar(void);

#endif
