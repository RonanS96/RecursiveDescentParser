%{
#include "tokens.h"
%}

DIGIT    [0-9]
LETTER	 [a-zA-Z][a-zA-Z0-9]*
         
%%

"function"	{ return FUNCTION;}
"returns"	{ return RETURNS; }
"variables"	{ return VARS;}
"begin"		{ return TBEGIN;}
"end"		{ return END;}
"if"		{ return IF;}
"then" 		{ return THEN;}
"else" 		{ return ELSE;}
"while" 	{ return WHILE;}
"loop"  	{ return LOOP;}
"read" 		{ return READ;}
"write"  	{ return WRITE;}

"(" 		{ return LBRACK;}
")" 		{ return RBRACK;}
","			{ return COMMA;}
":" 		{ return COLON;}
";" 		{ return SEMI;}
":=" 		{ return ASSIGN;}
"Less" 		{ return LT;}
"LessEq"	{ return LTE;}
"Eq" 		{ return EQ;}
"NEq" 		{ return NEQ;}

{DIGIT}+ 	{ return NUMBER;}
{LETTER}	{ return NAME;}
<<EOF>>	 	{ return EOF;}

[ \t\n]+    /* eat up whitespace */

%%

int yywrap() { return EOF; }
