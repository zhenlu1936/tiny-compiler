#include "e_stat.h"

#include <stdio.h>
#include <stdlib.h>

#include "e_tac.h"

struct op *process_variable_list_end(char *name) {
	struct op *variable = new_op();

	struct id *var = add_identifier(name, INT_VAR);
	NEW_TAC_1(code,TAC_VAR,var);
	cat_tac(variable, code);

	return variable;
}

struct op *process_variable_list(struct op *exp_1, char *name) {
	struct op *variable_list = new_op();

	struct id *var = add_identifier(name, INT_VAR);
	NEW_TAC_1(code,TAC_VAR,var);
	cat_tac(variable_list, exp_1->code);
	cat_tac(variable_list, code);

	free(exp_1);

	return variable_list;
}

struct op *process_for(struct op *exp_1, struct op *exp_2, struct op *exp_3,
					   struct op *exp_4) {
	struct op *for_stat = new_op();

	struct id *label_1=new_label();
	struct id *label_2=new_label();

	NEW_TAC_1(code_1,TAC_LABEL,label_1);
	cat_tac(for_stat, exp_1->code);
	cat_tac(for_stat, code_1);
	cat_tac(for_stat, exp_2->code);
	NEW_TAC_2(code_2,TAC_IFZ,exp_2->addr,label_2);
	cat_tac(for_stat, code_2);
	cat_tac(for_stat, exp_4->code);
	cat_tac(for_stat, exp_3->code);

	NEW_TAC_1(code_3,TAC_GOTO,label_1);
	cat_tac(for_stat, code_3);
	NEW_TAC_1(code_4,TAC_LABEL,label_2);
	cat_tac(for_stat, code_4);

	free(exp_1);
	free(exp_2);
	free(exp_3);
	free(exp_4);

	return for_stat;
}

struct op *process_while(struct op *exp_1, struct op *exp_2) {
	struct op *while_stat = new_op();

	struct id *label_1=new_label();
	struct id *label_2=new_label();

	NEW_TAC_1(code_1,TAC_LABEL,label_1);
	cat_tac(while_stat, code_1);
	cat_tac(while_stat, exp_1->code);
	NEW_TAC_2(code_2,TAC_IFZ,exp_1->addr,label_2);
	cat_tac(while_stat, code_2);
	cat_tac(while_stat, exp_2->code);

	NEW_TAC_1(code_3,TAC_GOTO,label_1);
	cat_tac(while_stat, code_3);
	NEW_TAC_1(code_4,TAC_LABEL,label_2);
	cat_tac(while_stat, code_4);

	free(exp_1);
	free(exp_2);

	return while_stat;
}

struct op *process_if_only(struct op *exp_1, struct op *exp_2) {
	struct op *if_only_stat = new_op();

	struct id *label=new_label();

	NEW_TAC_2(code_1,TAC_IFZ,exp_1->addr,label);
	cat_tac(if_only_stat, exp_1->code);
	cat_tac(if_only_stat, code_1);
	cat_tac(if_only_stat, exp_2->code);
	NEW_TAC_1(code_2,TAC_LABEL,label);
	cat_tac(if_only_stat, code_2);

	free(exp_1);
	free(exp_2);

	return if_only_stat;
}

struct op *process_if_else(struct op *exp_1, struct op *exp_2,
						   struct op *exp_3) {
	struct op *if_else_stat = new_op();

	struct id *label_1=new_label();
	NEW_TAC_2(code_1,TAC_IFZ,exp_1->addr,label_1);
	cat_tac(if_else_stat, exp_1->code);
	cat_tac(if_else_stat, code_1);
	cat_tac(if_else_stat, exp_2->code);

	struct id *label_2=new_label();
	NEW_TAC_1(code_2,TAC_GOTO,label_2);
	cat_tac(if_else_stat, code_2);
	NEW_TAC_1(code_3,TAC_LABEL,label_1);
	cat_tac(if_else_stat, code_3);

	cat_tac(if_else_stat, exp_3->code);
	NEW_TAC_1(code_4,TAC_LABEL,label_2);
	cat_tac(if_else_stat, code_4);

	free(exp_1);
	free(exp_2);
	free(exp_3);

	return if_else_stat;
}

struct op *process_call(char *name, struct op *exp_1) {
	struct op *call_stat = new_op();

	struct id *func = find_func(name);
	struct id *t = new_temp();
	NEW_TAC_2(code,TAC_CALL,t,func);
	cat_tac(call_stat, exp_1->code);
	cat_tac(call_stat, code);
	call_stat->addr = t;

	free(exp_1);

	return call_stat;
}

struct op *process_return(struct op *exp_1) {
	struct op *return_stat = new_op();

	NEW_TAC_1(code,TAC_RETURN,exp_1->addr);
	cat_tac(return_stat, exp_1->code);
	cat_tac(return_stat, code);

	free(exp_1);

	return return_stat;
}

struct op *process_output(char *name) {
	struct op *output_stat = new_op();

	struct id *var = find_identifier(name);
	NEW_TAC_1(code,TAC_OUTPUT,var);
	cat_tac(output_stat, code);

	return output_stat;
}

struct op *process_input(char *name) {
	struct op *input_stat = new_op();

	struct id *var = find_identifier(name);
	NEW_TAC_1(code,TAC_INPUT,var);
	cat_tac(input_stat, code);

	return input_stat;
}

struct op *process_assign(char *name, struct op *exp_1) {
	struct op *assign_stat = new_op();

	struct id *var = find_identifier(name);
	NEW_TAC_2(code,TAC_ASSIGN,var,exp_1->addr);
	cat_tac(assign_stat, exp_1->code);
	cat_tac(assign_stat, code);
	assign_stat->addr = exp_1->addr;

	free(exp_1);

	return assign_stat;
}
