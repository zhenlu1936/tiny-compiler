#include "e_proc.h"

#include <stdio.h>
#include <stdlib.h>

#include "e_tac.h"

struct op *process_calculate(struct op *exp_1, struct op *exp_2, int cal) {
	struct op *exp = new_op();

	struct id *t = new_temp();
	exp->addr = t;
	struct id *exp_temp_1 = exp_1->addr;
	struct id *exp_temp_2 = exp_2->addr;

	cat_op_and_free(exp, exp_1);
	cat_op_and_free(exp, exp_2);
	cat_tac(exp, NEW_TAC_3(cal, exp->addr, exp_temp_1, exp_temp_2));

	return exp;
}

struct op *process_negative(struct op *exp_1) {
	struct op *neg_exp = new_op();

	struct id *t = new_temp();
	struct id *exp_temp = exp_1->addr;
	neg_exp->addr = t;

	cat_op_and_free(neg_exp, exp_1);
	cat_tac(neg_exp, NEW_TAC_2(TAC_NEGATIVE, t, exp_temp));

	return neg_exp;
}

struct op *process_integer(int integer) {
	struct op *int_exp = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "%d", integer);
	struct id *var = add_identifier(buf, ID_NUM, NO_DATA);
	int_exp->addr = var;

	return int_exp;
}

struct op *process_identifier(char *name) {
	struct op *id_exp = new_op();

	struct id *var = find_identifier(name);
	id_exp->addr = var;

	return id_exp;
}

struct op *process_inc(char *name, int pos) {
	struct op *inc_exp = new_op();

	struct id *t = new_temp();
	struct id *var = find_identifier(name);
	struct id *num = add_identifier("1", ID_NUM, NO_DATA);
	inc_exp->addr = t;

	if (pos == INC_HEAD) {
		cat_tac(inc_exp, NEW_TAC_3(TAC_PLUS, t, var, num));
	} else {
		cat_tac(inc_exp, NEW_TAC_2(TAC_ASSIGN, t, var));
	}
	cat_tac(inc_exp, NEW_TAC_3(TAC_PLUS, var, t, num));

	return inc_exp;
}

struct op *process_dec(char *name, int pos) {
	struct op *dec_exp = new_op();

	struct id *t = new_temp();
	struct id *var = find_identifier(name);
	struct id *num = add_identifier("1", ID_NUM, NO_DATA);
	dec_exp->addr = t;

	if (pos == INC_HEAD) {
		cat_tac(dec_exp, NEW_TAC_3(TAC_MINUS, t, var, num));
	} else {
		cat_tac(dec_exp, NEW_TAC_2(TAC_ASSIGN, t, var));
	}
	cat_tac(dec_exp, NEW_TAC_3(TAC_MINUS, var, t, num));

	return dec_exp;
}

struct op *process_expression_list_end(struct op *exp_1) {
	struct op *exp = new_op();

	struct id *exp_temp = exp_1->addr;

	cat_op_and_free(exp, exp_1);
	cat_tac(exp, NEW_TAC_1(TAC_ARG, exp_temp));

	return exp;
}

struct op *process_expression_list(struct op *exp_1, struct op *exp_2) {
	struct op *exp_list = new_op();

	struct id *exp_temp = exp_2->addr;

	cat_op_and_free(exp_list, exp_1);
	cat_op_and_free(exp_list, exp_2);
	cat_tac(exp_list, NEW_TAC_1(TAC_ARG, exp_temp));

	return exp_list;
}

/**********/
struct op *process_declaration(int data_type, struct op *exp_1) {
	struct op *declaration = new_op();

	struct tac *head = exp_1->code;
	while (head) {
		head->id_1->data_type = data_type;
		head = head->next;
	}
	cat_op_and_free(declaration, exp_1);

	return declaration;
}

struct op *process_variable_list_end(char *name) {
	struct op *variable = new_op();

	struct id *var = add_identifier(name, ID_VAR, NO_TYPE);

	cat_tac(variable, NEW_TAC_1(TAC_VAR, var));

	return variable;
}

struct op *process_variable_list(struct op *exp_1, char *name) {
	struct op *variable_list = new_op();

	struct id *var = add_identifier(name, ID_VAR, NO_TYPE);

	cat_op_and_free(variable_list, exp_1);
	cat_tac(variable_list, NEW_TAC_1(TAC_VAR, var));

	return variable_list;
}

struct op *process_for(struct op *exp_1, struct op *exp_2, struct op *exp_3,
					   struct op *exp_4) {
	struct op *for_stat = new_op();

	struct id *label_1 = new_label();
	struct id *label_2 = new_label();
	struct id *exp_temp = exp_2->addr;

	cat_op_and_free(for_stat, exp_1);
	cat_tac(for_stat, NEW_TAC_1(TAC_LABEL, label_1));
	cat_op_and_free(for_stat, exp_2);
	if (exp_temp) {
		cat_tac(for_stat, NEW_TAC_2(TAC_IFZ, exp_temp, label_2));
	}
	cat_op_and_free(for_stat, exp_4);
	cat_op_and_free(for_stat, exp_3);
	cat_tac(for_stat, NEW_TAC_1(TAC_GOTO, label_1));
	if (exp_temp) {
		cat_tac(for_stat, NEW_TAC_1(TAC_LABEL, label_2));
	}

	return for_stat;
}

struct op *process_while(struct op *exp_1, struct op *exp_2) {
	struct op *while_stat = new_op();

	struct id *label_1 = new_label();
	struct id *label_2 = new_label();
	struct id *exp_temp = exp_1->addr;

	cat_tac(while_stat, NEW_TAC_1(TAC_LABEL, label_1));
	cat_op_and_free(while_stat, exp_1);
	cat_tac(while_stat, NEW_TAC_2(TAC_IFZ, exp_temp, label_2));
	cat_op_and_free(while_stat, exp_2);
	cat_tac(while_stat, NEW_TAC_1(TAC_GOTO, label_1));
	cat_tac(while_stat, NEW_TAC_1(TAC_LABEL, label_2));

	return while_stat;
}

struct op *process_if_only(struct op *exp_1, struct op *exp_2) {
	struct op *if_only_stat = new_op();

	struct id *label = new_label();
	struct id *exp_temp = exp_1->addr;

	cat_op_and_free(if_only_stat, exp_1);
	cat_tac(if_only_stat, NEW_TAC_2(TAC_IFZ, exp_temp, label));
	cat_op_and_free(if_only_stat, exp_2);
	cat_tac(if_only_stat, NEW_TAC_1(TAC_LABEL, label));

	return if_only_stat;
}

struct op *process_if_else(struct op *exp_1, struct op *exp_2,
						   struct op *exp_3) {
	struct op *if_else_stat = new_op();

	struct id *label_1 = new_label();
	struct id *label_2 = new_label();
	struct id *exp_temp = exp_1->addr;

	cat_op_and_free(if_else_stat, exp_1);
	cat_tac(if_else_stat, NEW_TAC_2(TAC_IFZ, exp_temp, label_1));
	cat_op_and_free(if_else_stat, exp_2);
	cat_tac(if_else_stat, NEW_TAC_1(TAC_GOTO, label_2));
	cat_tac(if_else_stat, NEW_TAC_1(TAC_LABEL, label_1));
	cat_op_and_free(if_else_stat, exp_3);
	cat_tac(if_else_stat, NEW_TAC_1(TAC_LABEL, label_2));

	return if_else_stat;
}

struct op *process_call(char *name, struct op *exp_1) {
	struct op *call_stat = new_op();

	struct id *func = find_func(name);
	struct id *t = new_temp();
	call_stat->addr = t;

	cat_op_and_free(call_stat, exp_1);
	cat_tac(call_stat, NEW_TAC_2(TAC_CALL, t, func));

	return call_stat;
}

struct op *process_return(struct op *exp_1) {
	struct op *return_stat = new_op();

	struct id *exp_temp = exp_1->addr;

	cat_op_and_free(return_stat, exp_1);
	cat_tac(return_stat, NEW_TAC_1(TAC_RETURN, exp_temp));

	return return_stat;
}

struct op *process_output_variable(char *name) {
	struct op *output_stat = new_op();

	struct id *var = find_identifier(name);

	cat_tac(output_stat, NEW_TAC_1(TAC_OUTPUT, var));

	return output_stat;
}

struct op *process_output_text(char *string) {
	struct op *output_stat = new_op();

	struct id *str = add_identifier(string, ID_STRING, NO_DATA);

	cat_tac(output_stat, NEW_TAC_1(TAC_OUTPUT, str));

	return output_stat;
}

struct op *process_input(char *name) {
	struct op *input_stat = new_op();

	struct id *var = find_identifier(name);

	cat_tac(input_stat, NEW_TAC_1(TAC_INPUT, var));

	return input_stat;
}

struct op *process_assign(char *name, struct op *exp_1) {
	struct op *assign_stat = new_op();

	struct id *var = find_identifier(name);
	struct id *exp_temp = exp_1->addr;
	assign_stat->addr = exp_temp;

	cat_op_and_free(assign_stat, exp_1);
	cat_tac(assign_stat, NEW_TAC_2(TAC_ASSIGN, var, exp_temp));

	return assign_stat;
}

/**********/
struct op *process_program(struct op *exp_1) {
	struct op *program = cpy_op_and_free(exp_1);

	printf("program compiled to tac!\n");

	FILE *f = fopen("tac.txt", "w");
	if (f == NULL) {
		perror("failed to open file");
	}
	output_tac(f, program->code);

	// clear_table(GLOBAL_TABLE);
	// clear_table(LOCAL_TABLE);

	return program;
}

struct op *process_function(struct op *exp_1, struct op *exp_2,
							struct op *exp_3) {
	struct op *function = new_op();

	cat_op_and_free(function, exp_1);
	cat_op_and_free(function, exp_2);
	cat_op_and_free(function, exp_3);
	cat_tac(function, NEW_TAC_0(TAC_END));

	return function;
}

struct op *process_function_head(int data_type, char *name) {
	struct op *function_head = new_op();

	struct id *func = add_identifier(name, ID_FUNC, data_type);
	cat_tac(function_head, NEW_TAC_1(TAC_LABEL, func));
	cat_tac(function_head, NEW_TAC_0(TAC_BEGIN));

	return function_head;
}

struct op *process_parameter_list_end(int data_type, char *name) {
	struct op *parameter = new_op();

	struct id *var = add_identifier(name, ID_VAR, data_type);
	cat_tac(parameter, NEW_TAC_1(TAC_PARAM, var));

	return parameter;
}

struct op *process_parameter_list(struct op *exp_1, int data_type, char *name) {
	struct op *parameter_list = new_op();

	struct id *var = add_identifier(name, ID_VAR, data_type);
	cat_op_and_free(parameter_list, exp_1);
	cat_tac(parameter_list, NEW_TAC_1(TAC_PARAM, var));

	return parameter_list;
}