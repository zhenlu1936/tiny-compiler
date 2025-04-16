%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_helper.h"
#include "e_generator.h"

extern FILE * yyin;
extern int yylineno;
int line;

extern struct id identifiers[MAX];
extern int identifiers_amount;
extern int num_amount;
extern int temp_amount;
extern int label_amount;

char character;
char token[1000];

int yylex();
void yyerror(char* msg);
%}

%union {
    struct op* operation;
    char* name;
    char* string;
    int num;
}

%token <name> IDENTIFIER
%token <num> INTEGER
%token <string> TEXT
%type <operation> program
%type <operation> function_declaration_list
%type <operation> function_declaration
%type <operation> function
%type <operation> function_head
%type <operation> parameter_list
%type <operation> block
%type <operation> declaration_list
%type <operation> declaration
%type <operation> variable_list
%type <operation> statement_list
%type <operation> statement_n_expression
%type <operation> statement
%type <operation> assignment_statement
%type <operation> input_statement
%type <operation> output_statement
%type <operation> return_statement
%type <operation> null_statement
%type <operation> if_statement
%type <operation> while_statement
%type <operation> for_statement
%type <operation> call_statement
%type <operation> argument_list
%type <operation> expression_list
%type <operation> inc_expression
%type <operation> dec_expression
%type <operation> expression

%token INT
%token EQ NE LT LE GT GE
%token IF THEN ELSE FI WHILE FOR DO DONE CONTINUE FUNC INPUT OUTPUT RETURN

%left INC DEC
%left EQ NE LT LE GT GE
%left '+' '-'
%left '*' '/'
%nonassoc NEGATIVE

%%

program : function_declaration_list
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                                printf("program compiled to tac!\n\n");

                                FILE *fp = fopen("tac.txt", "w");
                                if (fp == NULL) {
                                    perror("failed to open file");
                                }
                                fputs($$->tac, fp);

                                puts($$->tac);
                            }

function_declaration_list : function_declaration
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| function_declaration_list function_declaration
                            {
                                $$ = new_op();
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,$2->tac);
                            }
;

function_declaration : function
                            {
                                $$ = new_op();
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,"\n");
                            }
| declaration 
                            {
                                $$ = new_op();
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,"\n");
                            }
;

function : function_head '(' parameter_list ')' block
                            {
                                $$ = new_op();
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,$3->tac);
                                cat_tac($$->tac,$5->tac);
                                BUF_ALLOC(buf);
                                sprintf(buf,"end\n");
                                cat_tac($$->tac,buf);
                            }
| error {}
;

function_head : INT IDENTIFIER
                            {
                                $$ = new_op();
                                find_identifier($2,ADD,INT_FUNC);
                                BUF_ALLOC(buf_1);
                                sprintf(buf_1,"label %s\n",$2);
                                BUF_ALLOC(buf_2);
                                sprintf(buf_2,"start\n");
                                cat_tac($$->tac,buf_1);
                                cat_tac($$->tac,buf_2);
                            }
;

parameter_list : INT IDENTIFIER               
                            {
                                $$ = new_op();
                                find_identifier($2,ADD,INT_VAR); // 可能要改成局部
                                BUF_ALLOC(buf);
                                sprintf(buf,"param %s\n",$2);
                                cpy_tac($$->tac,buf);
                            }
| parameter_list ',' INT IDENTIFIER               
                            {
                                $$ = new_op();
                                find_identifier($4,ADD,INT_VAR); 
                                BUF_ALLOC(buf);
                                sprintf(buf,"param %s\n",$4);
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,buf);
                            }
|                           {
                                $$ = new_op();
                            }
;

block: '{' declaration_list statement_list '}'					
                            {
                                $$ = new_op();
                                cat_tac($$->tac,$2->tac);
                                cat_tac($$->tac,$3->tac);
                            }
;

declaration_list :
                            {
                                $$ = new_op();
                            }
| declaration_list declaration
                            {
                                $$ = new_op();
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,$2->tac);
                            }
;

declaration : INT variable_list ';'
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$2->tac);
                            }
;

variable_list : IDENTIFIER
                            {
                                $$ = new_op();
                                find_identifier($1,ADD,INT_VAR);
                                BUF_ALLOC(buf);
                                sprintf(buf,"var %s\n",$1);
                                cpy_tac($$->tac,buf);
                            }
| variable_list ',' IDENTIFIER
                            {
                                $$ = new_op();
                                find_identifier($3,ADD,INT_VAR);
                                BUF_ALLOC(buf);
                                sprintf(buf,"var %s\n",$3);
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,buf);
                            }
;

statement_list : statement			
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }  
| statement_list statement  
                            {
                                $$ = new_op();
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,$2->tac);
                            }
;

statement_n_expression : statement
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| expression
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                                $$->addr = $1->addr;
                            }

statement : assignment_statement ';'
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| input_statement ';' 
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| output_statement ';' 
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| call_statement ';'
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| return_statement ';' 
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| null_statement ';'
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| if_statement
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| while_statement 
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| for_statement 
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| block 
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
| error
                            {
                                $$ = new_op();
                            }
;

assignment_statement : IDENTIFIER '=' expression	
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = %s\n",$1,identifiers[$3->addr].name);
                                cat_tac($$->tac,$3->tac);
                                cat_tac($$->tac,buf);
                                $$->addr = $3->addr;
                            }
| IDENTIFIER '=' assignment_statement 
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = %s\n",$1,identifiers[$3->addr].name);
                                cat_tac($$->tac,$3->tac);
                                cat_tac($$->tac,buf);
                                $$->addr = $3->addr;
                            }
;

input_statement : INPUT IDENTIFIER
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"input %s\n",$2);
                                cpy_tac($$->tac,buf);
                            }
;

output_statement : OUTPUT IDENTIFIER
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"output %s\n",$2);
                                cpy_tac($$->tac,buf);
                            }
| OUTPUT TEXT
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"output %s\n",$2);
                                cpy_tac($$->tac,buf);
                            }
;

return_statement : RETURN expression
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"return %s\n",identifiers[$2->addr].name);
                                cat_tac($$->tac,$2->tac);
                                cat_tac($$->tac,buf);
                            }
;

null_statement : CONTINUE   
                            {
                                $$ = new_op();
                            }
;

if_statement : IF '(' expression ')' block
                            {
                                $$ = process_if_only($3,$5);
                            }
| IF '(' expression ')' block ELSE block
                            {
                                $$ = process_if_else($3,$5,$7);
                            }
;

while_statement : WHILE '(' expression ')' block
                            {
                                $$ = process_while($3,$5);
                            }
;

for_statement : FOR '(' assignment_statement ';' expression ';' statement_n_expression ')' block
                            {
                                $$ = process_for($3,$5,$7,$9);
                            }
;

call_statement : IDENTIFIER '(' argument_list ')'
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = call %s\n",identifiers[t].name,$1);
                                cat_tac($$->tac,$3->tac);
                                cat_tac($$->tac,buf);
                                $$->addr = t;
                            }
;

argument_list  :
                            {
                                $$ = new_op();
                            }
| expression_list
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                            }
;

expression_list : expression
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"arg %s\n",identifiers[$1->addr].name);
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,buf);
                            }
|  expression_list ',' expression
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"arg %s\n",identifiers[$3->addr].name);
                                cat_tac($$->tac,$1->tac);
                                cat_tac($$->tac,$3->tac);
                                cat_tac($$->tac,buf);
                            }
;

inc_expression : INC IDENTIFIER
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = %s + 1\n",identifiers[t].name,$2);
                                cpy_tac($$->tac,buf);
                                sprintf(buf,"%s = %s + 1\n",$2,identifiers[t].name);
                                cat_tac($$->tac,buf);
                                $$->addr = t;
                            }
| IDENTIFIER INC
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = %s\n",identifiers[t].name,$1);
                                cpy_tac($$->tac,buf);
                                sprintf(buf,"%s = %s + 1\n",$1,identifiers[t].name);
                                cat_tac($$->tac,buf);
                                $$->addr = t;   
                            }

dec_expression : DEC IDENTIFIER
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = %s - 1\n",identifiers[t].name,$2);
                                cpy_tac($$->tac,buf);
                                sprintf(buf,"%s = %s - 1\n",$2,identifiers[t].name);
                                cat_tac($$->tac,buf);
                                $$->addr = t;
                            }
| IDENTIFIER DEC
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = %s\n",identifiers[t].name,$1);
                                cpy_tac($$->tac,buf);
                                sprintf(buf,"%s = %s - 1\n",$1,identifiers[t].name);
                                cat_tac($$->tac,buf);
                                $$->addr = t;
                            }

expression : inc_expression
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                                $$->addr = $1->addr;
                            }
| dec_expression
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                                $$->addr = $1->addr;
                            }
| expression '+' expression	
                            { 
                                $$ = process_calculate($1,$3,"+");
                            }
| expression '-' expression				
                            { 
                                $$ = process_calculate($1,$3,"-");
                            }
| expression '*' expression		
                            { 
                                $$ = process_calculate($1,$3,"*");
                            }		
| expression '/' expression		
                            { 
                                $$ = process_calculate($1,$3,"/");
                            }		
| expression EQ expression				
                            { 
                                $$ = process_calculate($1,$3,"==");
                            }
| expression NE expression				
                            { 
                                $$ = process_calculate($1,$3,"!=");
                            }
| expression LT expression			
                            { 
                                $$ = process_calculate($1,$3,"<");
                            }
| expression LE expression			
                            { 
                                $$ = process_calculate($1,$3,"<=");
                            }	
| expression GT expression			
                            { 
                                $$ = process_calculate($1,$3,">");
                            }	
| expression GE expression			
                            { 
                                $$ = process_calculate($1,$3,">=");
                            }	
| '(' expression ')'				
                            { 
                                $$ = new_op();
                                cpy_tac($$->tac,$2->tac);
                                $$->addr = $2->addr;
                            }
| '-' expression  %prec NEGATIVE
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = -%s\n",identifiers[t].name,identifiers[$2->addr].name);
                                cat_tac($$->tac,$2->tac);
                                cat_tac($$->tac,buf);
                                $$->addr = t;
                            }

| call_statement
                            {
                                $$ = new_op();
                                cpy_tac($$->tac,$1->tac);
                                $$->addr = $1->addr;
                            }
| INTEGER					
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%d",$1);
                                $$->addr = find_identifier(buf,ADD,INT_NUM);
                            }
| IDENTIFIER                            
                            {
                                $$ = new_op();
                                $$->addr = find_identifier($1,NOT_ADD,INT_VAR);
                            }
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


