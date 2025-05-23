#include "e_proc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_tac.h"

struct tac *tac_head;
static struct tac *arg_list_head;
static struct block *block_top;

/*****expression*****/
// 处理形如"a=a op b"的表达式
struct op *process_calculate(struct op *exp_l, struct op *exp_r, int cal) {
	struct op *exp = new_op();

	struct id *t = new_temp(exp_l->addr->data_type);	// 分配临时变量
	exp->addr = t;
	struct id *exp_l_addr = exp_l->addr;
	struct id *exp_r_addr = exp_r->addr;

	if (exp_l_addr->data_type != exp_r_addr->data_type) {
		perror("wrong type");
	}
	cat_tac(exp, NEW_TAC_1(TAC_VAR, t));
	cat_op(exp, exp_l);	 // 拼接exp和exp_l的code
	cat_op(exp, exp_r);	 // 拼接exp和exp_r的code
	cat_tac(
		exp,
		NEW_TAC_3(
			cal, exp->addr, exp_l_addr,
			exp_r_addr));  // 生成代表目标表达式的三地址码，并拼接至exp的code末尾

	return exp;
}

// 处理形如"a=-b"的表达式
struct op *process_negative(struct op *exp) {
	struct op *neg_exp = new_op();

	struct id *t = new_temp(exp->addr->data_type);
	struct id *exp_addr = exp->addr;
	neg_exp->addr = t;

	cat_op(neg_exp, exp);
	cat_tac(neg_exp, NEW_TAC_2(TAC_NEGATIVE, t, exp_addr));

	return neg_exp;
}

// 分配整数型数字符号
struct op *process_int(int integer) {
	struct op *int_exp = new_op();

	BUF_ALLOC(buf);	 // 声明一个char数组变量buf，储存符号名
	sprintf(buf, "%d", integer);
	struct id *var =
		add_identifier(buf, ID_NUM, DATA_INT);	// 向符号表添加以buf为名的符号
	var->num.num_int = integer;
	int_exp->addr = var;

	return int_exp;
}

// 分配浮点型数字符号
struct op *process_float(double floatnum) {
	struct op *float_exp = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "%f", floatnum);
	struct id *var = add_identifier(buf, ID_NUM, DATA_FLOAT);
	var->num.num_float = floatnum;
	float_exp->addr = var;

	return float_exp;
}

// 分配标识符
struct op *process_identifier(char *name) {
	struct op *id_exp = new_op();

	struct id *var = find_identifier(name);
	id_exp->addr = var;

	return id_exp;
}

// 处理形如"a++"和"++a"的表达式
struct op *process_inc(char *name, int pos) {
	struct op *inc_exp = new_op();

	struct id *var = find_identifier(name);
	struct id *t = new_temp(var->data_type);
	struct id *num = add_identifier("1", ID_NUM, NO_DATA);
	inc_exp->addr = t;

	if (var->data_type != DATA_INT) {
		perror("wrong type");
	}
	cat_tac(inc_exp, NEW_TAC_1(TAC_VAR, t));
	if (pos == INC_HEAD) {
		cat_tac(inc_exp, NEW_TAC_3(TAC_PLUS, t, var, num));
		cat_tac(inc_exp, NEW_TAC_2(TAC_ASSIGN, var, t));
	} else {
		cat_tac(inc_exp, NEW_TAC_2(TAC_ASSIGN, t, var));
		cat_tac(inc_exp, NEW_TAC_3(TAC_PLUS, var, t, num));
	}

	return inc_exp;
}

// 处理形如"a--"和"--a"的表达式
struct op *process_dec(char *name, int pos) {
	struct op *dec_exp = new_op();

	struct id *var = find_identifier(name);
	struct id *t = new_temp(var->data_type);
	struct id *num = add_identifier("1", ID_NUM, NO_DATA);
	dec_exp->addr = t;

	if (var->data_type != DATA_INT) {
		perror("wrong type");
	}
	cat_tac(dec_exp, NEW_TAC_1(TAC_VAR, t));
	if (pos == INC_HEAD) {
		cat_tac(dec_exp, NEW_TAC_3(TAC_MINUS, t, var, num));
		cat_tac(dec_exp, NEW_TAC_2(TAC_ASSIGN, var, t));
	} else {
		cat_tac(dec_exp, NEW_TAC_2(TAC_ASSIGN, t, var));
		cat_tac(dec_exp, NEW_TAC_3(TAC_MINUS, var, t, num));
	}

	return dec_exp;
}

// 处理实参列表
struct op *process_argument_list(struct op *raw_exp_list) {
	struct op *argument_list = new_op();

	cat_op(argument_list, raw_exp_list);
	cat_tac(argument_list, arg_list_head);

	return argument_list;
}

// 处理表达式列表的开端，在调用函数时生成实参
struct op *process_expression_list_head(struct op *arg_exp) {
	struct op *exp = new_op();

	struct id *exp_temp = arg_exp->addr;  // not *temp var*
	struct tac *arg = NEW_TAC_1(TAC_ARG, exp_temp);
	arg->next = NULL;
	arg_list_head = arg;

	cat_op(exp, arg_exp);

	return exp;
}

// 处理表达式列表，在调用函数时生成实参
struct op *process_expression_list(struct op *arg_list_pre,
								   struct op *arg_exp) {
	struct op *exp_list = new_op();

	struct id *exp_temp = arg_exp->addr;  // not *temp var*
	struct tac *arg = NEW_TAC_1(TAC_ARG, exp_temp);
	arg->next = arg_list_head;
	arg_list_head = arg;

	cat_op(exp_list, arg_exp);
	cat_op(exp_list, arg_list_pre);

	return exp_list;
}

/*****statement*****/
// 处理变量声明，为process_variable函数声明的变量加上类型
struct op *process_declaration(int data_type, struct op *declaration_exp) {
	struct op *declaration = new_op();

	struct tac *head = declaration_exp->code;
	while (
		head) {	 // 逐个修改包含已声明变量的declaration_exp表达式所含变量的类型
		head->id_1->data_type = data_type;
		head = head->next;
	}
	cat_op(declaration, declaration_exp);

	return declaration;
}

// 处理变量声明的末尾，向符号表加入尚未初始化类型的变量
struct op *process_variable_list_end(char *name) {
	struct op *variable = new_op();

	struct id *var = add_identifier(name, ID_VAR, NO_TYPE);

	cat_tac(variable, NEW_TAC_1(TAC_VAR, var));

	return variable;
}

// 处理变量声明，向符号表加入尚未初始化类型的变量
struct op *process_variable_list(struct op *var_list_pre, char *name) {
	struct op *variable_list = new_op();

	struct id *var = add_identifier(name, ID_VAR, NO_TYPE);

	cat_op(variable_list, var_list_pre);
	cat_tac(variable_list, NEW_TAC_1(TAC_VAR, var));

	return variable_list;
}

static void push_block_stack(struct id *label_begin, struct id *label_end) {
	struct block *block_pushed = new_block(label_begin, label_end);
	block_pushed->prev = block_top;
	block_top = block_pushed;
}

static void pop_block_stack() {
	struct block *block_poped = block_top;
	if (block_top == NULL) {
		perror("stack is empty");
	}
	block_top = block_top->prev;
	free(block_poped);
}

static void parse_labels() {
	while (block_top->continue_stat_head) {
		block_top->continue_stat_head->code->id_1 = block_top->label_begin;
		block_top->continue_stat_head = block_top->continue_stat_head->next;
	}
	while (block_top->break_stat_head) {
		block_top->break_stat_head->code->id_1 = block_top->label_end;
		block_top->break_stat_head = block_top->break_stat_head->next;
	}
}

void block_initialize() {
	struct id *label_begin = new_label();
	struct id *label_end = new_label();
	push_block_stack(label_begin, label_end);
}

// 处理for语句块
struct op *process_for(struct op *initialization_exp, struct op *condition_exp,
					   struct op *iteration_exp, struct op *block) {
	struct op *for_stat = new_op();

	struct id *exp_temp = condition_exp->addr;

	parse_labels();
	cat_op(for_stat, initialization_exp);
	cat_tac(for_stat, NEW_TAC_1(TAC_LABEL, block_top->label_begin));
	cat_op(for_stat, condition_exp);
	if (exp_temp) {	 // 如果condition_exp不为空，则拼接label_2
		cat_tac(for_stat, NEW_TAC_2(TAC_IFZ, exp_temp, block_top->label_end));
	}
	cat_op(for_stat, block);
	cat_op(for_stat, iteration_exp);
	cat_tac(for_stat, NEW_TAC_1(TAC_GOTO, block_top->label_begin));
	if (exp_temp) {	 // 如果condition_exp不为空，则拼接label_2
		cat_tac(for_stat, NEW_TAC_1(TAC_LABEL, block_top->label_end));
	}

	pop_block_stack();

	return for_stat;
}

// 处理while语句块
struct op *process_while(struct op *condition_exp, struct op *block) {
	struct op *while_stat = new_op();

	struct id *exp_temp = condition_exp->addr;

	parse_labels();
	cat_tac(while_stat, NEW_TAC_1(TAC_LABEL, block_top->label_begin));
	cat_op(while_stat, condition_exp);
	cat_tac(while_stat, NEW_TAC_2(TAC_IFZ, exp_temp, block_top->label_end));
	cat_op(while_stat, block);
	cat_tac(while_stat, NEW_TAC_1(TAC_GOTO, block_top->label_begin));
	cat_tac(while_stat, NEW_TAC_1(TAC_LABEL, block_top->label_end));

	pop_block_stack();

	return while_stat;
}

// 处理break语句
struct op *process_break() {
	struct op *break_stat = new_op();

	struct id *dummy_label = NULL;
	cat_tac(break_stat, NEW_TAC_1(TAC_GOTO, dummy_label));

	if(block_top==NULL){
		perror("continue not in a loop");
	}
	break_stat->next = block_top->break_stat_head;
	block_top->break_stat_head = break_stat;

	return break_stat;
}

// 处理continue语句
struct op *process_continue() {
	struct op *continue_stat = new_op();

	struct id *dummy_label = NULL;
	cat_tac(continue_stat, NEW_TAC_1(TAC_GOTO, dummy_label));

	if(block_top==NULL){
		perror("continue not in a loop");
	}
	continue_stat->next = block_top->continue_stat_head;
	block_top->continue_stat_head = continue_stat;

	return continue_stat;
}

struct op *process_if_only(struct op *condition_exp, struct op *block) {
	struct op *if_only_stat = new_op();

	struct id *label = new_label();
	struct id *exp_temp = condition_exp->addr;

	cat_op(if_only_stat, condition_exp);
	cat_tac(if_only_stat, NEW_TAC_2(TAC_IFZ, exp_temp, label));
	cat_op(if_only_stat, block);
	cat_tac(if_only_stat, NEW_TAC_1(TAC_LABEL, label));

	return if_only_stat;
}

// 处理if else语句块
struct op *process_if_else(struct op *condition_exp, struct op *if_block,
						   struct op *else_block) {
	struct op *if_else_stat = new_op();

	struct id *label_1 = new_label();
	struct id *label_2 = new_label();
	struct id *exp_temp = condition_exp->addr;

	cat_op(if_else_stat, condition_exp);
	cat_tac(if_else_stat, NEW_TAC_2(TAC_IFZ, exp_temp, label_1));
	cat_op(if_else_stat, if_block);
	cat_tac(if_else_stat, NEW_TAC_1(TAC_GOTO, label_2));
	cat_tac(if_else_stat, NEW_TAC_1(TAC_LABEL, label_1));
	cat_op(if_else_stat, else_block);
	cat_tac(if_else_stat, NEW_TAC_1(TAC_LABEL, label_2));

	return if_else_stat;
}

// 处理call表达式
struct op *process_call(char *name, struct op *arg_list) {
	struct op *call_stat = new_op();

	struct id *func = find_func(name);
	struct id *t = new_temp(func->data_type);
	call_stat->addr = t;

	cat_tac(call_stat, NEW_TAC_1(TAC_VAR, t));
	cat_op(call_stat, arg_list);
	cat_tac(call_stat, NEW_TAC_2(TAC_CALL, t, func));

	return call_stat;
}

// 处理return表达式
struct op *process_return(struct op *ret_exp) {
	struct op *return_stat = new_op();

	struct id *exp_temp = ret_exp->addr;

	cat_op(return_stat, ret_exp);
	cat_tac(return_stat, NEW_TAC_1(TAC_RETURN, exp_temp));

	return return_stat;
}

// 处理output变量的表达式
struct op *process_output_variable(char *name) {
	struct op *output_stat = new_op();

	struct id *var = find_identifier(name);

	cat_tac(output_stat, NEW_TAC_1(TAC_OUTPUT, var));

	return output_stat;
}

// 处理output文本的表达式
struct op *process_output_text(char *string) {
	struct op *output_stat = new_op();

	struct id *str = add_identifier(string, ID_STRING, NO_DATA);

	cat_tac(output_stat, NEW_TAC_1(TAC_OUTPUT, str));

	return output_stat;
}

// 处理input表达式
struct op *process_input(char *name) {
	struct op *input_stat = new_op();

	struct id *var = find_identifier(name);

	cat_tac(input_stat, NEW_TAC_1(TAC_INPUT, var));

	return input_stat;
}

// 处理赋值表达式
struct op *process_assign(char *name, struct op *exp) {
	struct op *assign_stat = new_op();

	struct id *var = find_identifier(name);
	struct id *exp_temp = exp->addr;
	assign_stat->addr = exp_temp;

	if (var->data_type != exp_temp->data_type) {
		perror("wrong type");
		printf("var type: %d\tvar name: %s\n", var->data_type, var->name);
		printf("exp type: %d\texp name: %s\n", exp_temp->data_type,
			   exp_temp->name);
	}
	cat_op(assign_stat, exp);
	cat_tac(assign_stat, NEW_TAC_2(TAC_ASSIGN, var, exp_temp));

	return assign_stat;
}

/*****function&program*****/
// 处理整个程序，输出code
struct op *process_program(struct op *program) {
	// printf("program compiled to tac!\n");

	tac_head = program->code;

	// clear_table(GLOBAL_TABLE);
	// clear_table(LOCAL_TABLE);

	return program;
}

// 处理函数
struct op *process_function(struct op *function_head, struct op *parameter_list,
							struct op *block) {
	struct op *function = new_op();

	cat_op(function, function_head);
	cat_op(function, parameter_list);
	cat_op(function, block);
	cat_tac(function, NEW_TAC_0(TAC_END));

	return function;
}

// 处理函数头
struct op *process_function_head(int data_type, char *name) {
	struct op *function_head = new_op();

	struct id *func = add_identifier(
		name, ID_FUNC, data_type);	// 向符号表添加类型为函数的符号
	cat_tac(function_head, NEW_TAC_1(TAC_LABEL, func));
	cat_tac(function_head, NEW_TAC_0(TAC_BEGIN));

	return function_head;
}

// 处理函数参数列表的开端，加入标识符
struct op *process_parameter_list_head(int data_type, char *name) {
	struct op *parameter = new_op();

	struct id *var = add_identifier(name, ID_VAR, data_type);
	cat_tac(parameter, NEW_TAC_1(TAC_PARAM, var));

	return parameter;
}

// 处理函数参数列表，加入标识符
struct op *process_parameter_list(struct op *param_list_pre, int data_type,
								  char *name) {
	struct op *parameter_list = new_op();

	struct id *var = add_identifier(name, ID_VAR, data_type);
	cat_op(parameter_list, param_list_pre);
	cat_tac(parameter_list, NEW_TAC_1(TAC_PARAM, var));

	return parameter_list;
}