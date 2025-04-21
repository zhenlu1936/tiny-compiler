%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_func.h"
#include "e_stat.h"
#include "e_exp.h"
#include "e_tac.h"

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
%type <operation> assign_statement
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
                                $$ = process_program($1);
                            }

function_declaration_list : function_declaration
                            {
                                $$ = cpy_op($1);
                            }
| function_declaration_list function_declaration
                            {
                                $$ = cat_list($1,$2);
                            }
;

function_declaration : function
                            {
                                $$ = cpy_op($1);
                                //cat_tac($$->tac,"\n");
                            }
| declaration 
                            {
                                $$ = cpy_op($1);
                                //cat_tac($$->tac,"\n");
                            }
;

function : function_head '(' parameter_list ')' block
                            {
                                $$ = process_function($1,$3,$5)
                            }
| error {}
;

function_head : INT IDENTIFIER
                            {
                                $$ = process_function_head($2);
                            }
;

parameter_list : INT IDENTIFIER               
                            {
                                $$ = process_parameter_list_end($2);
                            }
| parameter_list ',' INT IDENTIFIER               
                            {
                                $$ = process_parameter_list($1,$4);
                            }
|                           {
                                $$ = new_op();
                            }
;

block: '{' declaration_list statement_list '}'					
                            {
                                $$ = cat_list($2,$3);
                            }
;

declaration_list :
                            {
                                $$ = new_op();
                            }
| declaration_list declaration
                            {
                                $$ = cat_list($1,$2);
                            }
;

declaration : INT variable_list ';'
                            {
                                $$ = cpy_op($2);
                            }
;

variable_list : IDENTIFIER
                            {
                                $$ = process_variable_list_end($1);
                            }
| variable_list ',' IDENTIFIER
                            {
                                $$ = process_variable_list($1,$3);
                            }
;

statement_list : statement			
                            {
                                $$ = cpy_op($1);
                            }  
| statement_list statement  
                            {
                                $$ = cat_list($1,$2);
                            }
;

statement_n_expression : statement
                            {
                                $$ = cpy_op($1);
                            }
| expression
                            {
                                $$ = cpy_op($1);
                            }

statement : assign_statement ';'
                            {
                                $$ = cpy_op($1);
                            }
| input_statement ';' 
                            {
                                $$ = cpy_op($1);
                            }
| output_statement ';' 
                            {
                                $$ = cpy_op($1);
                            }
| call_statement ';'
                            {
                                $$ = cpy_op($1);
                            }
| return_statement ';' 
                            {
                                $$ = cpy_op($1);
                            }
| null_statement ';'
                            {
                                $$ = cpy_op($1);
                            }
| if_statement
                            {
                                $$ = cpy_op($1);
                            }
| while_statement 
                            {
                                $$ = cpy_op($1);
                            }
| for_statement 
                            {
                                $$ = cpy_op($1);
                            }
| block 
                            {
                                $$ = cpy_op($1);
                            }
| error
                            {
                                $$ = new_op();
                            }
;

assign_statement : IDENTIFIER '=' expression	
                            {
                                $$ = process_assign($1,$3);
                            }
| IDENTIFIER '=' assign_statement 
                            {
                                $$ = process_assign($1,$3);
                            }
;

input_statement : INPUT IDENTIFIER
                            {
                                $$ = process_input($2);
                            }
;

output_statement : OUTPUT IDENTIFIER
                            {
                                $$ = process_output($2);
                            }
| OUTPUT TEXT
                            {
                                $$ = process_output($2);
                            }
;

return_statement : RETURN expression
                            {
                                $$ = process_return($2);
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

for_statement : FOR '(' assign_statement ';' expression ';' statement_n_expression ')' block
                            {
                                $$ = process_for($3,$5,$7,$9);
                            }
;

call_statement : IDENTIFIER '(' argument_list ')'
                            {
                                $$ = process_call($1,$3);
                            }
;

argument_list  :
                            {
                                $$ = new_op();
                            }
| expression_list
                            {
                                $$ = cpy_op($1)
                            }
;

expression_list : expression
                            {
                                $$ = process_expression_list_end($1);
                            }
|  expression_list ',' expression
                            {
                                $$ = process_expression_list($1,$3);
                            }
;

inc_expression : INC IDENTIFIER
                            {
                                $$ = process_inc($2,INC_HEAD);
                            }
| IDENTIFIER INC
                            {
                                $$ = process_inc($1,INC_TAIL);
                            }

dec_expression : DEC IDENTIFIER
                            {
                                $$ = process_dec($2,DEC_HEAD);
                            }
| IDENTIFIER DEC
                            {
                                $$ = process_dec($1,DEC_TAIL);
                            }

expression : inc_expression
                            {
                                $$ = cpy_op($1);
                            }
| dec_expression
                            {
                                $$ = cpy_op($1);
                            }
| expression '+' expression	
                            { 
                                $$ = process_calculate($1,$3,TAC_PLUS);
                            }
| expression '-' expression				
                            { 
                                $$ = process_calculate($1,$3,TAC_MINUS);
                            }
| expression '*' expression		
                            { 
                                $$ = process_calculate($1,$3,TAC_MULTIPLY);
                            }		
| expression '/' expression		
                            { 
                                $$ = process_calculate($1,$3,TAC_DIVIDE);
                            }		
| expression EQ expression				
                            { 
                                $$ = process_calculate($1,$3,TAC_EQ);
                            }
| expression NE expression				
                            { 
                                $$ = process_calculate($1,$3,TAC_NE);
                            }
| expression LT expression			
                            { 
                                $$ = process_calculate($1,$3,TAC_LT);
                            }
| expression LE expression			
                            { 
                                $$ = process_calculate($1,$3,TAC_LE);
                            }	
| expression GT expression			
                            { 
                                $$ = process_calculate($1,$3,TAC_GT);
                            }	
| expression GE expression			
                            { 
                                $$ = process_calculate($1,$3,TAC_GE);
                            }	
| '(' expression ')'				
                            { 
                                $$ = cpy_op($2);
                            }
| '-' expression  %prec NEGATIVE
                            {
                                $$ = process_negative($2);
                            }

| call_statement
                            {
                                $$ = cpy_op($1);
                            }
| INTEGER					
                            {
                                $$ = process_integer($1);
                            }
| IDENTIFIER                            
                            {
                                $$ = process_identifier($1);
                            }
;

%%

void yyerror(char* msg) 
{
	printf("%s: line %d\n", msg, yylineno);
	exit(0);
}