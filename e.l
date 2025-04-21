%{
#include <stdlib.h>
#include "e.y.h"
#include "e_tac.h"
#define MAX 100

%}

%option yylineno

%%

"int"  {  return INT;  }

"input"  {  return INPUT;  }

"output"  {  return OUTPUT;  }

"return"  {  return RETURN;  }

"continue"  {  return CONTINUE;  }

"if"  {  return IF;  }

"then"  {  return THEN;  }

"else"  {  return ELSE;  }

"fi"  {  return FI;  }

"while"  {  return WHILE;  }

"for"  {  return FOR;  }

"do"  {  return DO;  }

"done"  {  return DONE;  }

[A-Za-z]([A-Za-z]|[0-9])*  {  yylval.name = strdup(yytext); return IDENTIFIER;  }

[0-9]+  {  yylval.num = atoi(yytext); return INTEGER;  }

\"[^\"]*\" { 
			//char* tmp = process_string(strdup(yytext));
			char* tmp = strdup(yytext);
			strcpy(yylval.string,tmp);
			free(tmp);
			return TEXT;
		}

"++"  {  return INC; }

"--"  {  return DEC; }

"=="  {  return EQ;  }

"!="  {  return NE;  }

"<="  {  return LE;  }

"<"  {  return LT;  }

">="  {  return GE;  }

">"  {  return GT;  }

[ \t\r\n]

. 	{	return *yytext; }

%%

int yywrap()
{
	return 1;
}

