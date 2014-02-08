%{
#include <string>
#include "nsbcompile2.hpp"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}

%%

[ \t\n]                 ;
"function"              return TOKEN(TFUNCTION);
"chapter"               return TOKEN(TCHAPTER);
"scene"                 return TOKEN(TSCENE);
"\""                    return TOKEN(TQUOTE);
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]*          SAVE_TOKEN; return TFLOAT;
[0-9]+                  SAVE_TOKEN; return TINTEGER;
","                     return TOKEN(TCOMMA);
";"                     return TOKEN(TSEMICOLON);
"$"                     return TOKEN(TDOLLAR);
"="                     return TOKEN(TEQUAL);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
"{"                     return TOKEN(TLBRACE);
"}"                     return TOKEN(TRBRACE);
.                       printf("Unknown token!\n"); yyterminate();

%%
