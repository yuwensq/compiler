%{
#include <ctype.h>
#include<stdlib.h>
#include <stdio.h>
#include<string.h>
#ifndef YYSTYPE
#define YYSTYPE double 
#endif
void yyerror(const char *);
int yylex();
int digit(char);
int num_val;
char idStr[50];//用来存储读取的标识符
//之前尝试用map没做出来，这里用一个数组id存储每个标识符，用id_val存储标识符对应的值，两个数组用下标一一对应
//当读取标识符时，返回它在符号表中的序号(下标)
char*id[100];
double id_val[100];
int id_num=0;//记录符号表中含有多少个标识符
%}

//%token NUMBER
%left ADD
%left SUB
%left MUL
%left DIV
%token ID
%token NUM
%token LBRACE
%token RBRACE
%right UMINUS
%right ASSIGN
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
      | NUM {$$=$1;}
      | ID {$$=id_val[(int)$1];}
      | ID ASSIGN expr {id_val[(int)$1]=$3;$$=$3;}
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
          }else if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c=='_')){
              int idcount=0;
              while((c>='a'&&c<='z')||(c>='A'&&c<='Z')
                     ||(c=='_')||(c>='0'&& c<='9')){
                      idStr[idcount++]=c;
                      c=getchar();
                  }
               idStr[idcount]='\0';
               //读出标识符后，查询其是否在符号表中，若在则返回下标，若不在则添加入符号表并返回其下标
               int inSymbolTable=0;
               for(int i=0;i<id_num;i++){
                  if(strcmp(idStr,id[i])==0){
                        inSymbolTable=1;
                        yylval=i; //返回的是标识符在符号表中的下标
                        break;
                  }
               }
              if(inSymbolTable==0){
                  id[id_num]=(char*)malloc(50*sizeof(char));
                  strcpy(id[id_num],idStr);
                  id_val[id_num]=0;
                  yylval=id_num;
                  id_num++;
              }
               ungetc(c,stdin);
               return ID;
             }
          else if(c=='+') return ADD;
          else if(c=='-') return SUB;
          else if(c=='*') return MUL;    
          else if(c=='/') return DIV;
          else if(c=='(') return LBRACE;
          else if(c==')') return RBRACE;
          else if(c=='=') return ASSIGN;
          return c;
     }

}

int main(void)
{
      return yyparse();
}