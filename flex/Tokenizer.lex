    //------------------------------------------------- options -------------------------------------------------//
%option noyywrap
%option c++

    //------------------------------------------------- declarations -------------------------------------------------// (regular C++ code)

%{
#include "TokenTypes.h"

%}

    //------------------------------------------------- definitions -------------------------------------------------// (optional)

%x comment
IDENT_VAR   [abcd]
VAR         mnijk
OP          +-/
DIGIT       0-9

SYM_LBRACKET             "("
SYM_RBRACKET             ")"
SYM_T                    "t"
SYM_X                    "x"
SYM_U                    "u"
SYM_Z                    "z"

DELIM_COMMA              ","
DELIM_MUL                "*"

OP_POW                   "^"
OP_EQUAL                 "="
                
SYM_IDENTITY             "W"

    int line_num = 1;
    std::string a;
    //------------------------------------------------- rules -------------------------------------------------//

%%
            // brackets will be included in the argument //
"\\delta("[0-9mnijk+/ -]+")" { 
         a = "";
         for ( int i = 6; i < yyleng; i++)
            a += yytext[i];
         gCurrentToken = new Token( FUNC_DELTA, a);  return 1;
}


"c("[0-9mnijk+/ -]+")" { 
         a = "";
         for ( int i = 1; i < yyleng; i++)
            a += yytext[i];
         gCurrentToken = new Token( FUNC_C, a);  return 1;
}

"["[0-9mnijk+ -]+"]" {
    a = "";
         for ( int i = 1; i < yyleng - 1; i++)
            a += yytext[i];
         gCurrentToken = new Token( FUNC_SQRBRAC, a);  return 1;
}

[0-9mnijk+/-] {
    gCurrentToken = new Token( EXPR_PART, yytext ); return 1;
}

"x^("[ ]*"\\e"[ ]*")" {
    gCurrentToken = new Token ( SYM_X_END, "eps"); return 1;
}

"x^("[ ]*"\\s"[ ]*")" {
    gCurrentToken = new Token ( SYM_X_END, "sigma"); return 1;
}

"x"/[\^] {
    gCurrentToken = new Token ( SYM_X ); return 1;
}

"**" {
    gCurrentToken = new Token( DELIM_ELEMENT_MUL ); return 1;
}

"sgn("{IDENT_VAR}")" {
   a = "";
   for ( int i = 4; i < yyleng - 1; i++)
      a += yytext[i];
    gCurrentToken = new Token( FUNC_SGN, a); return 1;
}

{IDENT_VAR} {
   a = "";
   for ( int i = 0; i < yyleng; i++)
      a += yytext[i];
   gCurrentToken = new Token( IDENT_VAR, a ); return 1;
}


{SYM_LBRACKET}             { gCurrentToken = new Token( SYM_LBRACKET );   return 1; }    
{SYM_RBRACKET}             { gCurrentToken = new Token( SYM_RBRACKET );    return 1; }          
{SYM_T}                    { gCurrentToken = new Token( SYM_T );    return 1; }
{SYM_X}                    { gCurrentToken = new Token( SYM_X_END, "1" );    return 1; }
{SYM_U}                    { gCurrentToken = new Token( SYM_U );    return 1; }
{SYM_Z}                    { gCurrentToken = new Token( SYM_Z );    return 1; }

{DELIM_MUL}                { gCurrentToken = new Token( DELIM_MUL );  return 1; }
{DELIM_COMMA}              { gCurrentToken = new Token( DELIM_COMMA );    return 1; }

{OP_POW}                   { gCurrentToken = new Token( OP_POW );  return 1; }
{OP_EQUAL}                 { gCurrentToken = new Token( OP_EQUAL );  return 1; }

{SYM_IDENTITY}             { gCurrentToken = new Token( SYM_IDENTITY ); return 1; }

[ \t]+|[ \t]*"#"[^\n]*         /* eat up whitespace and comments */
[\n]        { line_num++; }
<<EOF>>     { gCurrentToken = new Token ( SYM_EOF ); return 0;  } // return 0 to signalize EOF
.           { throw std::runtime_error("TOKENIZER - ERROR: Unrecognized character: " + std::string(1, yytext[0])); }
%%

    //------------------------------------------------- user subroutines -------------------------------------------------// (regular C++ code)

#ifdef DEBUG
static yyFlexLexer lexer;

int main( int argc, char** argv )
{

 while(lexer.yylex() != 0)
 {
    std::cout << gCurrentToken->print() << std::endl;
    delete gCurrentToken;
 }
 return 0;
}

#endif
