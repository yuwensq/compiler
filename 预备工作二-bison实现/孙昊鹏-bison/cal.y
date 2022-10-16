%{
#include<ctype.h>
#include<stdio.h>
#ifndef YYSTYPE
#define YYSTYPE double 
#endif
void yyerror(const char *);
int yylex();
int digit(char);
int num_val;
%}

//%token NUMBER
%left ADD
%left SUB
%left MUL
%left DIV
%token NUM
%token LBRACE
%token RBRACE
%right UMINUS
%%

lines : lines expr '\n' {printf("%g\n",$2);}
      | lines '\n'
      |
      ;
expr  : expr ADD expr { $$ = $1 + $3; }
      | expr SUB expr { $$ = $1 - $3; }
      | expr MUL expr { $$ = $1 * $3; }
      | expr DIV expr { $$ = $1 / $3; }
      | LBRACE expr RBRACE{ $$ = $2; }
      | SUB expr %prec UMINUS  { $$ = -$2; }
      | NUM
      ;
%%

int yygettoken(void)
{
      return getchar();
} 

void yyerror(const char *s){
      fprintf(stderr, "Parse error: %s\n", s);
}

int isDigit(char c){
      return c>='0'&&c<='9';
}
//如果读出的字符是空格或者制表符或者回车则返回1，在yylex中将跳过该字符继续读取
int skip(char c){
      return c==' '||c=='\t';
}
int yylex(){
      //由于可能遇到空白符，所以先重复读取，若读取字符不是空白符则结束循环，否则一直循环下去直到读取非空白符
     while(1){
          char c=getchar();
          if(skip(c)) continue;
          //如果c是数字字符,这里我们识别多位十进制整数
          else if(isDigit(c)){
              num_val=c-'0';
              //判断读取的下一个字符是不是数字，如果是，则更新num_val,如果不是，则判断是否为空白字符，如果是则跳过继续读取，否则结束
              while(isDigit(c=getchar())||skip(c)){
                  if(skip(c)) continue;
                  num_val=c-'0'+num_val*10;        
              }
              ungetc(c,stdin);
              yylval=num_val;//将具体词素返回
              return NUM;//返回单词类别
          }
          else if(c=='+') return ADD;
          else if(c=='-') return SUB;
          else if(c=='*') return MUL;    
          else if(c=='/') return DIV;
          else if(c=='(') return LBRACE;
          else if(c==')') return RBRACE;
          return c;
     }

}

int main(void)
{
      return yyparse();
}