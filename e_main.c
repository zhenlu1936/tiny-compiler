#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e.y.h"

extern int yyparse();
extern FILE * yyin;

int main(int argc, char *argv[]) 
{
	if(argc!=2) 
	{
		printf("usage: %s filename\n", argv[0]);
		exit(0);
	}			

	if( (yyin=fopen(argv[1], "r")) ==NULL )
	{
		printf("open file %s failed\n", argv[1]);
		exit(0);
	}

	yyparse();
	fclose(yyin);
	return 0;
}