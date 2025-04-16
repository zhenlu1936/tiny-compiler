#include "e_stat.h"

#include <stdio.h>
#include <stdlib.h>

#include "e_helper.h"

extern struct id identifiers[MAX];
extern int identifiers_amount;
extern int num_amount;
extern int temp_amount;
extern int label_amount;

struct op* process_variable_list_end(char* name) {
	struct op* variable = new_op();

	struct id* var = find_identifier(name, ADD, INT_VAR);
	NEW_TAC_1(buf,TAC_VAR,var);
	cat_tac(variable, buf);

	return variable;
}

struct op* process_variable_list(struct op* exp_1, char* name) {
	struct op* variable_list = new_op();

	struct id* var = find_identifier(name, ADD, INT_VAR);
	NEW_TAC_1(buf,TAC_VAR,var);
	cat_tac(variable_list, exp_1->code);
	cat_tac(variable_list, buf);

	free(exp_1);

	return variable_list;
}

struct op* process_for(struct op* exp_1, struct op* exp_2, struct op* exp_3,
					   struct op* exp_4) {
	struct op* for_stat = new_op();

	NAME_ALLOC(label_name_1);
	sprintf(label_name_1, "label_%d", label_amount++);
	struct id* label_1 = find_identifier(label_name_1, ADD, LABEL_IFZ);
	NAME_ALLOC(label_name_2);
	sprintf(label_name_2, "label_%d", label_amount++);
	struct id* label_2 = find_identifier(label_name_2, ADD, LABEL_IFZ);

	NEW_TAC_1(buf_1,TAC_LABEL,label_1);
	cat_tac(for_stat, exp_1->code);
	cat_tac(for_stat, buf_1);
	cat_tac(for_stat, exp_2->code);
	NEW_TAC_2(buf_2,TAC_IFZ,&identifiers[exp_2->addr],label_2);
	cat_tac(for_stat, buf_2);
	cat_tac(for_stat, exp_4->code);
	cat_tac(for_stat, exp_3->code);

	NEW_TAC_1(buf_3,TAC_GOTO,label_1);
	cat_tac(for_stat, buf_3);
	NEW_TAC_1(buf_4,TAC_LABEL,label_2);
	cat_tac(for_stat, buf_4);

	free(exp_1);
	free(exp_2);
	free(exp_3);
	free(exp_4);

	return for_stat;
}

struct op* process_while(struct op* exp_1, struct op* exp_2) {
	struct op* while_stat = new_op();

	NAME_ALLOC(label_name_1);
	sprintf(label_name_1, "label_%d", label_amount++);
	struct id* label_1 = find_identifier(label_name_1, ADD, LABEL_IFZ);
	NAME_ALLOC(label_name_2);
	sprintf(label_name_2, "label_%d", label_amount++);
	struct id* label_2 = find_identifier(label_name_2, ADD, LABEL_IFZ);

	NEW_TAC_1(buf_1,TAC_LABEL,label_1);
	cat_tac(while_stat, buf_1);
	cat_tac(while_stat, exp_1->code);
	NEW_TAC_2(buf_2,TAC_IFZ,&identifiers[exp_1->addr],label_2);
	cat_tac(while_stat, buf_2);
	cat_tac(while_stat, exp_2->code);

	NEW_TAC_1(buf_3,TAC_GOTO,label_1);
	cat_tac(while_stat, buf_3);
	NEW_TAC_1(buf_4,TAC_LABEL,label_2);
	cat_tac(while_stat, buf_4);

	free(exp_1);
	free(exp_2);

	return while_stat;
}

struct op* process_if_only(struct op* exp_1, struct op* exp_2) {
	struct op* if_only_stat = new_op();

	NAME_ALLOC(label_name);
	sprintf(label_name, "label_%d", label_amount++);
	struct id* label = find_identifier(label_name, ADD, LABEL_IFZ);
	NEW_TAC_2(buf_1,TAC_IFZ,&identifiers[exp_1->addr],label);
	cat_tac(if_only_stat, exp_1->code);
	cat_tac(if_only_stat, buf_1);
	cat_tac(if_only_stat, exp_2->code);
	NEW_TAC_1(buf_2,TAC_LABEL,label);
	cat_tac(if_only_stat, buf_2);

	free(exp_1);
	free(exp_2);

	return if_only_stat;
}

struct op* process_if_else(struct op* exp_1, struct op* exp_2,
						   struct op* exp_3) {
	struct op* if_else_stat = new_op();

	NAME_ALLOC(label_name_1);
	sprintf(label_name_1, "label_%d", label_amount++);
	struct id* label_1 = find_identifier(label_name_1, ADD, LABEL_IFZ);
	NEW_TAC_2(buf_1,TAC_IFZ,&identifiers[exp_1->addr],label_1);
	cat_tac(if_else_stat, exp_1->code);
	cat_tac(if_else_stat, buf_1);
	cat_tac(if_else_stat, exp_2->code);

	NAME_ALLOC(label_name_2);
	sprintf(label_name_2, "label_%d", label_amount++);
	struct id* label_2 = find_identifier(label_name_2, ADD, LABEL_IFZ);
	NEW_TAC_1(buf_2,TAC_GOTO,label_2);
	cat_tac(if_else_stat, buf_2);
	NEW_TAC_1(buf_3,TAC_LABEL,label_1);
	cat_tac(if_else_stat, buf_3);

	cat_tac(if_else_stat, exp_3->code);
	NEW_TAC_1(buf_4,TAC_LABEL,label_2);
	cat_tac(if_else_stat, buf_4);

	free(exp_1);
	free(exp_2);
	free(exp_3);

	return if_else_stat;
}

struct op* process_call(char* name, struct op* exp_1) {
	struct op* call_stat = new_op();

	struct id* func = find_identifier(name, NOT_ADD, INT_FUNC);
	struct id* t = new_temp();
	NEW_TAC_2(buf,TAC_CALL,t,func);
	cat_tac(call_stat, exp_1->code);
	cat_tac(call_stat, buf);
	call_stat->addr = t->addr;

	free(exp_1);

	return call_stat;
}

struct op* process_return(struct op* exp_1) {
	struct op* return_stat = new_op();

	NEW_TAC_1(buf,TAC_RETURN,&identifiers[exp_1->addr]);
	cat_tac(return_stat, exp_1->code);
	cat_tac(return_stat, buf);

	free(exp_1);

	return return_stat;
}

struct op* process_output(char* name) {
	struct op* output_stat = new_op();

	struct id* var = find_identifier(name, NOT_ADD, INT_VAR);
	NEW_TAC_1(buf,TAC_OUTPUT,var);
	cat_tac(output_stat, buf);

	return output_stat;
}

struct op* process_input(char* name) {
	struct op* input_stat = new_op();

	struct id* var = find_identifier(name, NOT_ADD, INT_VAR);
	NEW_TAC_1(buf,TAC_INPUT,var);
	cat_tac(input_stat, buf);

	return input_stat;
}

struct op* process_assign(char* name, struct op* exp_1) {
	struct op* assign_stat = new_op();

	struct id* var = find_identifier(name, NOT_ADD, INT_VAR);
	NEW_TAC_2(buf,TAC_ASSIGN,var,&identifiers[exp_1->addr]);
	cat_tac(assign_stat, exp_1->code);
	cat_tac(assign_stat, buf);
	assign_stat->addr = exp_1->addr;

	free(exp_1);

	return assign_stat;
}
