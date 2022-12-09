%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    #include <stack>
    #include <cstring>
    extern Ast ast;
    int yylex();
    #define YYERROR_VERBOSE 1
    int yyerror(char const*);
    std::stack<StmtNode*> whileStack;
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    SymbolEntry* se;
}

%start Program
%token <strtype> ID STRING
%token <itype> INTEGER
%token IF ELSE WHILE
%token INT VOID 
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET COMMA  
%token ADD SUB MUL DIV MOD OR AND LESS LESSEQUAL GREATER GREATEREQUAL ASSIGN EQUAL NOTEQUAL NOT
%token CONST
%token RETURN CONTINUE BREAK

%type<stmttype> Stmts Stmt AssignStmt ExprStmt BlockStmt IfStmt WhileStmt BreakStmt ContinueStmt ReturnStmt DeclStmt FuncDef ConstDeclStmt VarDeclStmt ConstDefList VarDef ConstDef VarDefList FuncFParam FuncFParams FuncFParamsList BlankStmt
%type<exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp MulExp ConstExp EqExp UnaryExp InitVal ConstInitVal FuncRParams FuncRParamsList
%type<type> Type

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {$$=$1;}
    | ExprStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | BlankStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    | BreakStmt {$$=$1;}
    | ContinueStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    ;
LVal
    : ID {
        SymbolEntry* se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete []$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ; 
AssignStmt
    : LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
ExprStmt
    : Exp SEMICOLON {
        $$ = new ExprStmt($1);
    }
    ;
BlankStmt
    : SEMICOLON {
        $$ = new BlankStmt();
    }
    ;
BlockStmt
    : LBRACE {
        identifiers = new SymbolTable(identifiers);
    } 
      Stmts RBRACE {
        $$ = new CompoundStmt($3);
        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | LBRACE RBRACE {
        $$ = new CompoundStmt();
    }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        ((ExprNode*)$3)->setIsCond(true);
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        ((ExprNode*)$3)->setIsCond(true);
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
WhileStmt
    : WHILE LPAREN Cond RPAREN {
        ((ExprNode*)$3)->setIsCond(true);
        StmtNode *whileStmt = new WhileStmt($3);
        whileStack.push(whileStmt);
    } Stmt {
        $$ = whileStack.top();
        whileStack.pop();
        ((WhileStmt*)$$)->setStmt($6);
    }
    ;
BreakStmt
    : BREAK SEMICOLON {
        if (whileStack.size() <= 0) {
            fprintf(stderr, "No while\n");
            exit(-1);
        }
        $$ = new BreakStmt(whileStack.top());
    }
    ;
ContinueStmt
    : CONTINUE SEMICOLON {
        if (whileStack.size() <= 0) {
            fprintf(stderr, "No while\n");
            exit(-1);
        }
        $$ = new ContinueStmt(whileStack.top());
    }
    ;
ReturnStmt
    : RETURN SEMICOLON {
        $$ = new ReturnStmt();
    }
    | RETURN Exp SEMICOLON {
        $$ = new ReturnStmt($2);
    }
    ;
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
PrimaryExp
    : LPAREN Exp RPAREN {
        $$ = $2;
    }
    | LVal {
        $$ = $1;
    }
    | STRING {
        SymbolEntry* se;
        se = globals->lookup(std::string($1));
        if(se == nullptr){
            Type* type = new StringType(strlen($1));
            se = new ConstantSymbolEntry(type, std::string($1));
            globals->install(std::string($1), se);
        }
        $$ = new ExprNode(se);
    }
    | INTEGER {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    ;
UnaryExp 
    : PrimaryExp {$$ = $1;}
    | ID LPAREN FuncRParamsList RPAREN {
        SymbolEntry* se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)$1);
            delete []$1;
            assert(se != nullptr);
        }
        $$ = new CallExpr(se, $3);
    }
    | ADD UnaryExp {$$ = $2;}
    | SUB UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    | NOT UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    ;
MulExp
    : UnaryExp {$$ = $1;}
    | MulExp MUL UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    | MulExp DIV UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    | MulExp MOD UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }
    ;
AddExp
    : MulExp {$$ = $1;}
    | AddExp ADD MulExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    | AddExp SUB MulExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;
RelExp
    : AddExp {$$ = $1;}
    | RelExp LESS AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    | RelExp LESSEQUAL AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    | RelExp GREATER AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    | RelExp GREATEREQUAL AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    }
    ;
EqExp
    : RelExp {$$ = $1;}
    | EqExp EQUAL RelExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | EqExp NOTEQUAL RelExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    ;
LAndExp
    : EqExp {$$ = $1;}
    | LAndExp AND EqExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    : LAndExp {$$ = $1;}
    | LOrExp OR LAndExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
ConstExp
    : AddExp {$$ = $1;}
    ;
FuncRParamsList
    : FuncRParams {$$ = $1;}
    | %empty {$$ = nullptr;}
    ;
FuncRParams 
    : Exp {$$ = $1;}
    | FuncRParams COMMA Exp {
        $$ = $1;
        $$->setNext($3);
    }
    ;
Type
    : INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    ;
DeclStmt
    : VarDeclStmt {$$ = $1;}
    | ConstDeclStmt {$$ = $1;}
    ;
VarDeclStmt
    : Type VarDefList SEMICOLON {
        $$ = $2;
    }
    ;
VarDefList
    : VarDefList COMMA VarDef {
        $$ = $1;
        $1->setNext($3);
    } 
    | VarDef { $$ = $1; }
    ;
VarDef
    : ID {
        SymbolEntry* se;
        se = identifiers->lookup($1, true);
        if (se != nullptr) { //重复定义了
            fprintf(stderr, "variable \"%s\" is repeated declared\n", (char*)$1);
            delete []$1;
            assert(se == nullptr);
        }
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    | ID ASSIGN InitVal {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        ((IdentifierSymbolEntry*)se)->setValue($3->getValue());
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    ;
ConstDeclStmt
    : CONST Type ConstDefList SEMICOLON {
        $$ = $3;
    }
    ;
ConstDefList
    : ConstDefList COMMA ConstDef {
        $$ = $1;
        $1->setNext($3);
    }
    | ConstDef {$$ = $1;}
    ;
ConstDef
    : ID ASSIGN ConstInitVal {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(TypeSystem::constIntType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        ((IdentifierSymbolEntry*)se)->setValue($3->getValue());
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    ;
InitVal 
    : Exp {
        $$ = $1;
    }
    ;
ConstInitVal
    : ConstExp {
        $$ = $1;
    }
    ;
FuncDef
    :
    Type ID {
        identifiers = new SymbolTable(identifiers);
    }
    LPAREN FuncFParamsList RPAREN {
        Type* funcType;
        std::vector<Type*> vec;
        DeclStmt* temp = (DeclStmt*)$5;
        while(temp){
            vec.push_back(temp->getId()->getSymPtr()->getType());
            temp = (DeclStmt*)(temp->getNext());
        }
        funcType = new FunctionType($1, vec);
        SymbolEntry* se = new IdentifierSymbolEntry(funcType, $2, identifiers->getPrev()->getLevel());
        identifiers->getPrev()->install($2, se);
    } 
    BlockStmt {
        SymbolEntry* se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, (DeclStmt*)$5, $8);
        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;
FuncFParamsList
    : FuncFParams {$$ = $1;}
    | %empty {$$ = nullptr;}
FuncFParams
    : FuncFParams COMMA FuncFParam {
        $$ = $1;
        $$->setNext($3);
    }
    | FuncFParam {$$ = $1;}
    ;
FuncFParam
    : Type ID {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se));
        delete []$2;
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr << message << std::endl;
    return -1;
}
