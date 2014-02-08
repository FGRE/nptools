%{
    #include "nsbcompile2.hpp"
    #include <cstdio>
    #include <cstdlib>

    Block* pRoot;
    extern int yylex();
    void yyerror(const char* s) { std::printf("Error: %s\n", s); std::exit(1); }
%}

%union
{
    Node* node;
    Block* block;
    Statement* stmt;
    Argument* arg;
    std::vector<Argument*>* argvec;
    std::string* string;
    int token;
}

%token <string> TIDENTIFIER TFLOAT TINTEGER
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TFUNCTION TSEMICOLON TDOLLAR TEQUAL TCOMMA TQUOTE TCHAPTER TSCENE

%type <arg> arg 
%type <argvec> func_args
%type <block> program stmts block
%type <stmt> stmt func_decl call

%start program

%%

program : stmts { pRoot = $1; }
        ;

stmts : stmt { $$ = new Block(); $$->Statements.push_back($<stmt>1); }
      | stmts stmt { $1->Statements.push_back($<stmt>2); }
      ;

stmt : func_decl | call
     ;

block : TLBRACE stmts TRBRACE { $$ = $2; }
      | TLBRACE TRBRACE { $$ = new Block(); }
      ;

func_decl : TFUNCTION arg TLPAREN func_args TRPAREN block { $$ = new Function(*$2, *$4, *$6); delete $4; }
          | TCHAPTER arg block { $$ = new Chapter(*$2, *$3); delete $3; }
          | TSCENE arg block { $$ = new Scene(*$2, *$3); delete $3; }
          ;

func_args : { $$ = new ArgumentList(); }
          | arg { $$ = new ArgumentList(); $$->push_back($<arg>1); }
          | func_args TCOMMA arg { $1->push_back($<arg>3); }
          ;

arg : TDOLLAR TIDENTIFIER { $$ = new Argument(string("$") + *$2, ARG_VARIABLE); delete $2; }
      | TIDENTIFIER { $$ = new Argument(*$1, ARG_FUNCTION); delete $1; }
      | TQUOTE TIDENTIFIER TQUOTE { $$ = new Argument(*$2, ARG_STRING); delete $2; }
      ;

call : arg TLPAREN func_args TRPAREN TSEMICOLON { $$ = new Call(*$1, *$3); delete $3; }
     ;

%%
