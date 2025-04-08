%{
#include <stdlib.h>
#include "y.tab.h"
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

"do"  {  return DO;  }

"done"  {  return DONE;  }

[A-Za-z]([A-Za-z]|[0-9])*  {  return IDENTIFIER;  }

[0-9]*	{  return INTEGER;  }

\"[^\"]*\"  {  return TEXT;  }

"=="  {  return EQ;  }

"!="  {  return NE;  }

"<="  {  return LE;  }

"<"  {  return LT;  }

">="  {  return GE;  }

">"  {  return GT;  }

[ \t\r\n]|#.*

. 	{	return *yytext; }

%%

int yywrap()
{
	return 1;
}

