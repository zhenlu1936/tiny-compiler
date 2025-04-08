%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_helper.h"

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
	//struct id* identifier;
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
%type <operation> expression

%token INT
%token EQ NE LT LE GT GE UMINUS
%token IF THEN ELSE FI WHILE FOR DO DONE CONTINUE FUNC INPUT OUTPUT RETURN

%left EQ NE LT LE GT GE
%left '+' '-'
%left '*' '/'
%nonassoc NEGATIVE

%%

program : function_declaration_list
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
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
                                $$->tac = copy_tac($1->tac);
                            }
| function_declaration_list function_declaration
                            {
                                $$ = new_op();
                                $$->tac = cat_tac($1->tac,$2->tac);
                            }
;

function_declaration : function
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| declaration 
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
;

function : function_head '(' parameter_list ')' block
                            {
                                $$ = new_op();
                                $$->tac = cat_tac($1->tac,$3->tac);
                                $$->tac = cat_tac($$->tac,$5->tac);
                                BUF_ALLOC(buf);
                                sprintf(buf,"end\n\n");
                                $$->tac = cat_tac($$->tac,buf);
                            }
| error {}
;

function_head : INT IDENTIFIER
                            {
                                $$ = new_op();
                                find_identifier($2,ADD,INT_FUNC);
                                BUF_ALLOC(buf_1)
                                sprintf(buf_1,"label %s \n",$2); // %s与\n之间必须有空格，否则start后会凭空多出一个param1。我不知道为什么
                                BUF_ALLOC(buf_2)
                                sprintf(buf_2,"start\n");
                                $$->tac = cat_tac(buf_1,buf_2);
                            }
;

parameter_list : INT IDENTIFIER               
                            {
                                $$ = new_op();
                                find_identifier($2,ADD,INT_VAR); // 可能要改成局部
                                BUF_ALLOC(buf);
                                sprintf(buf,"param %s \n",$2);
                                $$->tac = copy_tac(buf);
                            }
| parameter_list ',' INT IDENTIFIER               
                            {
                                $$ = new_op();
                                find_identifier($4,ADD,INT_VAR); 
                                BUF_ALLOC(buf);
                                sprintf(buf,"param %s \n",$4);
                                $$->tac = cat_tac($1->tac,buf);
                            }
|                           {
                                $$ = new_op();
                            }
;

block: '{' declaration_list statement_list '}'					
                            {
                                $$ = new_op();
                                $$->tac = cat_tac($2->tac,$3->tac);
                            }
;

declaration_list :
                            {
                                $$ = new_op();
                            }
| declaration_list declaration
                            {
                                $$ = new_op();
                                $$->tac = cat_tac($1->tac,$2->tac);
                            }
;

declaration : INT variable_list ';'
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($2->tac);
                            }
;

variable_list : IDENTIFIER
                            {
                                $$ = new_op();
                                find_identifier($1,ADD,INT_VAR);
                                BUF_ALLOC(buf);
                                sprintf(buf,"var %s \n",$1);
                                $$->tac = copy_tac(buf);
                            }
| variable_list ',' IDENTIFIER
                            {
                                $$ = new_op();
                                find_identifier($3,ADD,INT_VAR);
                                BUF_ALLOC(buf);
                                sprintf(buf,"var %s \n",$3);
                                $$->tac = cat_tac($1->tac,buf);
                            }
;

statement_list : statement			
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }  
| statement_list statement  
                            {
                                $$ = new_op();
                                $$->tac = cat_tac($1->tac,$2->tac);
                            }
;

statement : assignment_statement ';'
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| input_statement ';' 
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| output_statement ';' 
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| call_statement ';'
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| return_statement ';' 
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| null_statement ';'
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| if_statement
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| while_statement 
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| for_statement 
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
                            }
| block 
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
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
                                sprintf(buf,"%s = %s \n",$1,identifiers[$3->addr].name);
                                $$->tac = cat_tac($3->tac,buf);
                                $$->addr = $3->addr;
                            }
| IDENTIFIER '=' assignment_statement 
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = %s \n",$1,identifiers[$3->addr].name);
                                $$->tac = cat_tac($3->tac,buf);
                                $$->addr = $3->addr;
                            }
;

input_statement : INPUT IDENTIFIER
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"input %s \n",$2);
                                $$->tac = copy_tac(buf);
                            }
;

output_statement : OUTPUT IDENTIFIER
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"output %s \n",$2);
                                $$->tac = copy_tac(buf);
                            }
| OUTPUT TEXT
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"output %s \n",$2);
                                $$->tac = copy_tac(buf);
                            }
;

return_statement : RETURN expression
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"return %s \n",identifiers[$2->addr].name);
                                $$->tac = cat_tac($2->tac,buf);
                            }
;

null_statement : CONTINUE   
                            {
                                $$ = new_op();
                            }
;

if_statement : IF '(' expression ')' block
                            {
                                $$ = new_op();
                                NAME_ALLOC(label_name);
                                sprintf(label_name,"label_%d",label_amount++);
                                find_identifier(label_name,ADD,LABEL_IFZ);
                                BUF_ALLOC(buf);
                                sprintf(buf,"ifz %s goto %s \n",identifiers[$3->addr].name,label_name);
                                $$->tac = cat_tac($3->tac,buf);
                                $$->tac = cat_tac($$->tac,$5->tac);
                                sprintf(buf,"label %s \n",label_name);
                                $$->tac = cat_tac($$->tac,buf);
                            }
| IF '(' expression ')' block ELSE block
                            {
                                $$ = new_op();
                                NAME_ALLOC(label_name_1);
                                sprintf(label_name_1,"label_%d",label_amount++);
                                find_identifier(label_name_1,ADD,LABEL_IFZ);
                                BUF_ALLOC(buf);
                                sprintf(buf,"ifz %s goto %s \n",identifiers[$3->addr].name,label_name_1);
                                $$->tac = cat_tac($3->tac,buf);
                                $$->tac = cat_tac($$->tac,$5->tac);

                                NAME_ALLOC(label_name_2);
                                sprintf(label_name_2,"label_%d",label_amount++);
                                find_identifier(label_name_2,ADD,LABEL_IFZ);
                                sprintf(buf,"goto %s \n",label_name_2);
                                $$->tac = cat_tac($$->tac,buf);
                                sprintf(buf,"label %s \n",label_name_1);
                                $$->tac = cat_tac($$->tac,buf);

                                $$->tac = cat_tac($$->tac,$7->tac);
                                sprintf(buf,"label %s \n",label_name_2);
                                $$->tac = cat_tac($$->tac,buf);
                            }
;

while_statement : WHILE '(' expression ')' block
                            {
                                $$ = new_op();
                                NAME_ALLOC(label_name_1);
                                sprintf(label_name_1,"label_%d",label_amount++);
                                find_identifier(label_name_1,ADD,LABEL_IFZ);
                                NAME_ALLOC(label_name_2);
                                sprintf(label_name_2,"label_%d",label_amount++);
                                find_identifier(label_name_2,ADD,LABEL_IFZ);

                                BUF_ALLOC(buf);
                                sprintf(buf,"label %s \n",label_name_1);
                                $$->tac = cat_tac(buf,$3->tac);
                                sprintf(buf,"ifz %s goto %s \n",identifiers[$3->addr].name,label_name_2);
                                $$->tac = cat_tac($$->tac,buf);
                                $$->tac = cat_tac($$->tac,$5->tac);

                                sprintf(buf,"goto %s \n",label_name_1);
                                $$->tac = cat_tac($$->tac,buf);
                                sprintf(buf,"label %s\n",label_name_2); // 我不知道为什么，这儿最后加上空格反倒会出错。
                                $$->tac = cat_tac($$->tac,buf);
                            }
;

for_statement : FOR '(' assignment_statement ';' expression ';' assignment_statement ')' block
                            {
                                $$ = new_op();
                                
                                NAME_ALLOC(label_name_1);
                                sprintf(label_name_1,"label_%d",label_amount++);
                                find_identifier(label_name_1,ADD,LABEL_IFZ);
                                NAME_ALLOC(label_name_2);
                                sprintf(label_name_2,"label_%d",label_amount++);
                                find_identifier(label_name_2,ADD,LABEL_IFZ);

                                BUF_ALLOC(buf);
                                sprintf(buf,"label %s \n",label_name_1);
                                $$->tac = cat_tac($3->tac,buf);
                                $$->tac = cat_tac($$->tac,$5->tac);
                                sprintf(buf,"ifz %s goto %s \n",identifiers[$5->addr].name,label_name_2);
                                $$->tac = cat_tac($$->tac,buf);
                                $$->tac = cat_tac($$->tac,$9->tac);
                                $$->tac = cat_tac($$->tac,$7->tac);

                                sprintf(buf,"goto %s \n",label_name_1);
                                $$->tac = cat_tac($$->tac,buf);
                                sprintf(buf,"label %s \n",label_name_2);
                                $$->tac = cat_tac($$->tac,buf);
                            }
;

call_statement : IDENTIFIER '(' argument_list ')'
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = call %s \n",identifiers[t].name,$1);
                                $$->tac = cat_tac($3->tac,buf);
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
                                $$->tac = copy_tac($1->tac);
                            }
;

expression_list : expression
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"arg %s \n",identifiers[$1->addr].name);
                                $$->tac = cat_tac($1->tac,buf);
                            }
|  expression_list ',' expression
                            {
                                $$ = new_op();
                                BUF_ALLOC(buf);
                                sprintf(buf,"arg %s \n",identifiers[$3->addr].name);
                                $$->tac = cat_tac($1->tac,$3->tac);
                                $$->tac = cat_tac($$->tac,buf);
                            }
;

expression : expression '+' expression	
                            { 
                                $$ = calculate($1,$3,"+");
                            }
| expression '-' expression				
                            { 
                                $$ = calculate($1,$3,"-");
                            }
| expression '*' expression		
                            { 
                                $$ = calculate($1,$3,"*");
                            }		
| expression '/' expression		
                            { 
                                $$ = calculate($1,$3,"/");
                            }		
| expression EQ expression				
                            { 
                                $$ = calculate($1,$3,"==");
                            }
| expression NE expression				
                            { 
                                $$ = calculate($1,$3,"!=");
                            }
| expression LT expression			
                            { 
                                $$ = calculate($1,$3,"<");
                            }
| expression LE expression			
                            { 
                                $$ = calculate($1,$3,"<=");
                            }	
| expression GT expression			
                            { 
                                $$ = calculate($1,$3,">");
                            }	
| expression GE expression			
                            { 
                                $$ = calculate($1,$3,">=");
                            }	
| '(' expression ')'				
                            { 
                                $$ = new_op();
                                $$->tac = copy_tac($2->tac);
                                $$->addr = $2->addr;
                            }
| '-' expression  %prec NEGATIVE
                            {
                                $$ = new_op();
                                int t = new_temp();
                                BUF_ALLOC(buf);
                                sprintf(buf,"%s = -%s \n",identifiers[t].name,identifiers[$2->addr].name);
                                $$->tac = cat_tac($2->tac,buf);
                                $$->addr = t;
                            }
| call_statement
                            {
                                $$ = new_op();
                                $$->tac = copy_tac($1->tac);
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


