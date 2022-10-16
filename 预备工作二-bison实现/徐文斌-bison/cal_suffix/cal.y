%token NUM
%token ADD
%token SUB
%token MUL
%token DIV
%token LBRACE
%token RBRACE

%{
#include <ctype.h>
#include <stdio.h>
#ifndef YYSTYPE
#define YYSTYPE double
#endif
int token_val;
%}

%left ADD SUB
%left MUL DIV
%right UMINUS

%%

lines	:	lines expr '\n'	
		|	lines '\n'
		|
		;

expr	:	expr ADD expr	{ printf("+ "); }
		|	expr SUB expr	{ printf("- "); }
		|	expr MUL expr	{ printf("* "); }
		|	expr DIV expr	{ printf("/ "); }
		|	LBRACE expr RBRACE
		|	{printf("0 ");} SUB expr %prec UMINUS	{ printf("- "); }
		|	NUMBER
		;

NUMBER	:	NUM				{ printf("%d ", token_val); }
		;

%%

int digit(char c) {
	return c >= '0' && c <= '9';
}

int skip(char c) {
	return c == ' ' || c == '\t';
}

int opera(char c) {
	switch (c) {
		case '+': return ADD;
		case '-': return SUB;
		case '*': return MUL;
		case '/': return DIV;
		default : return 0;
	}
}

int yylex(void) {	
	while (1) {
		char c = getchar();
		int op;
		if (digit(c)) {
			token_val = c - '0';
			while (digit(c = getchar())) {
				token_val = c - '0' + token_val * 10;
			}
			ungetc(c, stdin);
			return NUM;
		} 
		else if (op = opera(c)) {
			return op;
		}
		else if (c == '(') {
			return LBRACE;
		}
		else if (c == ')') {
			return RBRACE;
		}
		else if (skip(c)) {
			continue;	
		}
		return c;
	}
}

int yyerror(void){
    return 0;
}

int main(void)
{
	return yyparse();
}
