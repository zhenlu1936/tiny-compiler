%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE * yyin;
extern int yylineno;

int yylex();
void yyerror(char* msg);
%}

%token IDENTIFIER INTEGER TEXT INT
%token EQ NE LT LE GT GE UMINUS
%token IF THEN ELSE FI WHILE DO DONE CONTINUE FUNC INPUT OUTPUT RETURN

%left EQ NE LT LE GT GE
%left '+' '-'
%left '*' '/'
%right UMINUS

%%
program : function_declaration_list
;

function_declaration_list : function_declaration
| function_declaration_list function_declaration
;

function_declaration : function
| declaration
;

declaration : INT variable_list ';'
;

variable_list : IDENTIFIER
| variable_list ',' IDENTIFIER
;

function : function_head '(' parameter_list ')' block
| error
;

function_head : IDENTIFIER
;

parameter_list : IDENTIFIER               
| parameter_list ',' IDENTIFIER               
|
;

statement : assignment_statement ';'
| input_statement ';'
| output_statement ';'
| call_statement ';'
| return_statement ';'
| null_statement ';'
| if_statement
| while_statement
| block
| error
;

block : '{' declaration_list statement_list '}'               
;

declaration_list        :
| declaration_list declaration
;

statement_list : statement
| statement_list statement               
;

assignment_statement : IDENTIFIER '=' expression
;

expression : expression '+' expression
| expression '-' expression
| expression '*' expression
| expression '/' expression
| '-' expression  %prec UMINUS
| expression EQ expression
| expression NE expression
| expression LT expression
| expression LE expression
| expression GT expression
| expression GE expression
| '(' expression ')'
| INTEGER
| IDENTIFIER
| call_expression
| error
;

argument_list  :
| expression_list
;

expression_list : expression
|  expression_list ',' expression
;

input_statement : INPUT IDENTIFIER
;

output_statement : OUTPUT IDENTIFIER
| OUTPUT TEXT
;

return_statement : RETURN expression
;

null_statement : CONTINUE
;

if_statement : IF '(' expression ')' block
| IF '(' expression ')' block ELSE block
;

while_statement : WHILE '(' expression ')' block
;

call_statement : IDENTIFIER '(' argument_list ')'
;

call_expression : IDENTIFIER '(' argument_list ')'
;


%%
	
void yyerror(char* msg) 
{
	printf("%s: line %d\n", msg, yylineno);
	exit(0);
}

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


