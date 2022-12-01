%option noyywrap
%{
    /*
    * You will need to comment this line in lab5.
    */
    #define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
    #else
    #define YYSTYPE void *
    #include "parser.h"
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT

    #include <string.h>
    #include "SymbolTable.h"
    #include <stack>
    std::stack<SymbolTable*> symbolT_st;
    SymbolItem *pos;
    SymbolTable *now_symbolT = new SymbolTable;

    #ifdef ONLY_FOR_LEX
    #include <iostream>
    #include <fstream>
    #include <stack>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;

    int offset;//表示当前行中词素的偏移量(列号)

    void DEBUG_FOR_LAB4(std::string s, bool print_pos = false){
        std::string DEBUG_INFO;
        if (!print_pos)
            DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\t" + to_string(yylineno) + "\t" + to_string(offset) + "\n";
        else
            DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\t" + to_string(yylineno) + "\t" + to_string(offset) + "\t" + to_string((unsigned long long)pos) +"\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    #endif
%}
digit [0-9]
letter [A-Za-z]
ID ({letter}|_)({letter}|{digit}|_)*
DECIMIAL ([1-9][0-9]*|0)
OCTAL (0[0-7]+)
HEXADECIMAL (0[xX][0-9A-Fa-f]+)
EOL (\r\n|\n|\r)
WHITE [\t ]
commentBegin "/*"
comment .|\n
commentEnd "*/"

linecommentBegin "//"
linecomment .
%x COMMENT
%x LINECOMMENT
%%

"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("INT\tint");
        offset += strlen("int");
    #else
        return INT;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("VOID\tvoid");
        offset += strlen("void");
    #else
        return VOID;
    #endif 
}
"const" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("CONST\tconst");
        offset += strlen("const");
    #else
        return CONST;
    #endif 
}
"if" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("IF\tif");
        offset += strlen("if");
    #else
        return IF;
    #endif
}
"else" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ELSE\telse");
        offset += strlen("else");
    #else
        return ELSE;
    #endif
}
"return" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RETURN\treturn");
        offset += strlen("return");
    #else
        return RETURN;
    #endif
}
"while" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("WHILE\twhile");
        offset += strlen("while");
    #else
        return WHILE;
    #endif
}
"break" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("BREAK\tbreak");
        offset += strlen("break");
    #else
        return BREAK;
    #endif
}
"continue" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("CONTINUE\tcontinue");
        offset += strlen("continue");
    #else
        return CONTINUE;
    #endif
}
"==" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("EQUAL\t==");
        offset += strlen("==");
    #else
        return EQUAL;
    #endif
}
"!=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("NOTEQUAL\t!=");
        offset += strlen("!=");
    #else
        return NOTEQUAL;
    #endif
}
"=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ASSIGN\t=");
        offset += strlen("=");
    #else
        return ASSIGN;
    #endif
}
"<=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LESSEQUAL\t<=");
        offset += strlen("<=");
    #else
        return LESSEQUAL;
    #endif
}
">=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("GREATEREQUAL\t<=");
        offset += strlen(">=");
    #else
        return GREATEREQUAL;
    #endif
}
"<" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LESS\t<");
        offset += strlen("<");
    #else
        return LESS;
    #endif
}
">" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("GREATER\t>");
        offset += strlen(">");
    #else
        return GREATER;
    #endif
}
"+" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ADD\t+");
        offset += strlen("+");
    #else
        return ADD;
    #endif
}
"-" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SUB\t-");
        offset += strlen("-");
    #else
        return SUB;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MUL\t*");
        offset += strlen("*");
    #else
        return MUL;
    #endif
}
"/" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("DIV\t/");
        offset += strlen("/");
    #else
        return DIV;
    #endif
}
"%" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MOD\t%");
        offset += strlen("%");
    #else
        return MOD;
    #endif
}
"&&" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("AND\t&&");
        offset += strlen("&&");
    #else
        return AND;
    #endif
}
"||" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("OR\t||");
        offset += strlen("||");
    #else
        return OR;
    #endif
}
"!" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("NOT\t!");
        offset += strlen("!");
    #else
        return NOT;
    #endif
}
";" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SEMICOLON\t;");
        offset += strlen(";");
    #else
        return SEMICOLON;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LPAREN\t(");
        offset += strlen("(");
    #else
        return LPAREN;
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RPAREN\t)");
        offset += strlen(")");
    #else
    return RPAREN;
    #endif
}
"{" {
    symbolT_st.push(now_symbolT);
    now_symbolT = new SymbolTable;
    now_symbolT->prev = symbolT_st.top();
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LBRACE\t{");
        offset += strlen("{");
    #else
        return LBRACE;
    #endif
}
"}" {
    now_symbolT = symbolT_st.top();
    symbolT_st.pop();
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RBRACE\t}");
        offset += strlen("}");
    #else
        return RBRACE;
    #endif
}
"[" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LBRACKET\t[");
        offset += strlen("[");
    #else
        return LBRACKET;
    #endif
}
"]" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RBRACKET\t}");
        offset += strlen("]");
    #else
        return RBRACKET;
    #endif
}
"," {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("COMMA\t,");
        offset += strlen(",");
    #else
        return COMMA;
    #endif
}
{DECIMIAL} {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4(string("NUMBER\t") + string(yytext));
        offset += strlen(yytext);
    #else
        return INTEGER;
    #endif
}

{OCTAL} {
    #ifdef ONLY_FOR_LEX
        int num_o;
        sscanf(yytext, "%o", &num_o);
        DEBUG_FOR_LAB4(string("NUMBER\t") + to_string(num_o));
        offset += strlen(yytext);
    #else
        return INTEGER;
    #endif
}

{HEXADECIMAL} {
    #ifdef ONLY_FOR_LEX
        int num_h;
        sscanf(yytext, "%x", &num_h);
        DEBUG_FOR_LAB4(string("NUMBER\t") + to_string(num_h));
        offset += strlen(yytext);
    #else
        return INTEGER;
    #endif
}
{ID} {
    pos = look_for(now_symbolT, string(yytext));
    if (pos == nullptr) {
        pos = insert(now_symbolT, string(yytext));
    }
    DEBUG_FOR_LAB4(string("ID\t")+string(yytext), true);
    offset += strlen("yytext");
}
{EOL} {
    yylineno++;
    offset=0;
}
{WHITE} {
    offset+=strlen(yytext);
}
{commentBegin} {
    BEGIN COMMENT;
    fprintf(yyout,"%s",yytext);
}
<COMMENT>{comment} {fprintf(yyout,"%s",yytext);}
<COMMENT>{commentEnd} {BEGIN INITIAL;fprintf(yyout,"%s",yytext);fprintf(yyout,"\n");}
{linecommentBegin} {
    BEGIN LINECOMMENT;
    fprintf(yyout,"%s",yytext);
}
<LINECOMMENT>{linecomment} {fprintf(yyout,"%s",yytext);}
<LINECOMMENT>{EOL} {BEGIN INITIAL;fprintf(yyout,"%s",yytext);}
%%

#ifdef ONLY_FOR_LEX
int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Argument Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(!(yyout = fopen(argv[2], "w"))){
        fprintf(stderr, "No such file or directory: %s", argv[2]);
        exit(EXIT_FAILURE);
    }

    yylex();
    return 0;
}
#endif
