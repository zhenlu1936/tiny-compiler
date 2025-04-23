#include "e_stat.h"

#include <stdio.h>
#include <stdlib.h>

#include "e_tac.h"

struct op *process_variable_list_end(char *name) {
	struct op *variable = new_op();

	struct id *var = add_identifier(name, INT_VAR);
	cat_tac(variable, NEW_TAC_1(TAC_VAR, var));

	return variable;
}

struct op *process_variable_list(struct op *exp_1, char *name) {
	struct op *variable_list = new_op();

	struct id *var = add_identifier(name, INT_VAR);
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
	cat_tac(for_stat, NEW_TAC_2(TAC_IFZ, exp_temp, label_2));
	cat_op_and_free(for_stat, exp_4);
	cat_op_and_free(for_stat, exp_3);
	cat_tac(for_stat, NEW_TAC_1(TAC_GOTO, label_1));
	cat_tac(for_stat, NEW_TAC_1(TAC_LABEL, label_2));

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

struct op *process_output(char *name) {
	struct op *output_stat = new_op();

	struct id *var = find_identifier(name);
	cat_tac(output_stat, NEW_TAC_1(TAC_OUTPUT, var));

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
