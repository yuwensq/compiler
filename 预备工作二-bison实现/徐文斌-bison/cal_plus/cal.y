%token NUM
%token ADD
%token SUB
%token MUL
%token DIV
%token LBRACE
%token RBRACE
%token ENDSTAT

%{
#include <ctype.h>
#include <stdio.h>
#ifndef YYSTYPE
#define YYSTYPE double
#endif
%}

%left ADD SUB
%left MUL DIV
%right UMINUS

%%

lines	:	lines expr ENDSTAT	{ printf("%g\n", $2); }
		|	lines ENDSTAT
		|
		;

expr	:	expr ADD expr	{ $$ = $1 + $3; }
		|	expr SUB expr	{ $$ = $1 - $3; }
		|	expr MUL expr	{ $$ = $1 * $3; }
		|	expr DIV expr	{ $$ = $1 / $3; }
		|	LBRACE expr RBRACE	{ $$ = $2; }
		|	SUB expr %prec UMINUS	{ $$ = -$2; }
		|	NUM
		;

%%

int digit(char c) {
	return c >= '0' && c <= '9';
}

int skip(char c) {
	return c == ' ' || c == '\t' || c == '\n';
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
		double token_val;
		int op;
		if (digit(c)) {
			token_val = c - '0';
			while (digit(c = getchar())) {
				token_val = c - '0' + token_val * 10;
			}
			if (c == '.') {
				double i = 1.0;
				double frac = 0;
				while (digit(c = getchar())) {
					i *= 10;
					frac = c - '0' + frac * 10;
				}
				token_val += frac / i;
			}
			ungetc(c, stdin);
			yylval = token_val;
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
		else if (c == ';') {
			return ENDSTAT;
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
