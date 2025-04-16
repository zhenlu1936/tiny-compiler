#include "e_stat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_helper.h"

extern struct id identifiers[MAX];
extern int identifiers_amount;
extern int num_amount;
extern int temp_amount;
extern int label_amount;

struct op* process_variable_list_end(char* name) {
	struct op* variable = new_op();

	find_identifier(name, ADD, INT_VAR);
	BUF_ALLOC(buf);
	sprintf(buf, "var %s\n", name);
	strcpy(variable->tac, buf);

	return variable;
}

struct op* process_variable_list(struct op* exp_1, char* name) {
	struct op* variable_list = new_op();

	find_identifier(name, ADD, INT_VAR);
	BUF_ALLOC(buf);
	sprintf(buf, "var %s\n", name);
	cat_tac(variable_list->tac, exp_1->tac);
	cat_tac(variable_list->tac, buf);

	free(exp_1);
	
	return variable_list;
}

struct op* process_for(struct op* exp_1, struct op* exp_2, struct op* exp_3,
					   struct op* exp_4) {
	struct op* for_stat = new_op();

	NAME_ALLOC(label_name_1);
	sprintf(label_name_1, "label_%d", label_amount++);
	find_identifier(label_name_1, ADD, LABEL_IFZ);
	NAME_ALLOC(label_name_2);
	sprintf(label_name_2, "label_%d", label_amount++);
	find_identifier(label_name_2, ADD, LABEL_IFZ);

	BUF_ALLOC(buf);
	sprintf(buf, "label %s\n", label_name_1);
	cat_tac(for_stat->tac, exp_1->tac);
	cat_tac(for_stat->tac, buf);
	cat_tac(for_stat->tac, exp_2->tac);
	sprintf(buf, "ifz %s goto %s\n", identifiers[exp_2->addr].name,
			label_name_2);
	cat_tac(for_stat->tac, buf);
	cat_tac(for_stat->tac, exp_4->tac);
	cat_tac(for_stat->tac, exp_3->tac);

	sprintf(buf, "goto %s\n", label_name_1);
	cat_tac(for_stat->tac, buf);
	sprintf(buf, "label %s\n", label_name_2);
	cat_tac(for_stat->tac, buf);

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
	find_identifier(label_name_1, ADD, LABEL_IFZ);
	NAME_ALLOC(label_name_2);
	sprintf(label_name_2, "label_%d", label_amount++);
	find_identifier(label_name_2, ADD, LABEL_IFZ);

	BUF_ALLOC(buf);
	sprintf(buf, "label %s\n", label_name_1);
	cat_tac(while_stat->tac, buf);
	cat_tac(while_stat->tac, exp_1->tac);
	sprintf(buf, "ifz %s goto %s\n", identifiers[exp_1->addr].name,
			label_name_2);
	cat_tac(while_stat->tac, buf);
	cat_tac(while_stat->tac, exp_2->tac);

	sprintf(buf, "goto %s\n", label_name_1);
	cat_tac(while_stat->tac, buf);
	sprintf(buf, "label %s\n", label_name_2);
	cat_tac(while_stat->tac, buf);

	free(exp_1);
	free(exp_2);

	return while_stat;
}

struct op* process_if_only(struct op* exp_1, struct op* exp_2) {
	struct op* if_only_stat = new_op();

	NAME_ALLOC(label_name);
	sprintf(label_name, "label_%d", label_amount++);
	find_identifier(label_name, ADD, LABEL_IFZ);
	BUF_ALLOC(buf);
	sprintf(buf, "ifz %s goto %s\n", identifiers[exp_1->addr].name, label_name);
	cat_tac(if_only_stat->tac, exp_1->tac);
	cat_tac(if_only_stat->tac, buf);
	cat_tac(if_only_stat->tac, exp_2->tac);
	sprintf(buf, "label %s\n", label_name);
	cat_tac(if_only_stat->tac, buf);

	free(exp_1);
	free(exp_2);

	return if_only_stat;
}

struct op* process_if_else(struct op* exp_1, struct op* exp_2,
						   struct op* exp_3) {
	struct op* if_else_stat = new_op();

	NAME_ALLOC(label_name_1);
	sprintf(label_name_1, "label_%d", label_amount++);
	find_identifier(label_name_1, ADD, LABEL_IFZ);
	BUF_ALLOC(buf);
	sprintf(buf, "ifz %s goto %s\n", identifiers[exp_1->addr].name,
			label_name_1);
	cat_tac(if_else_stat->tac, exp_1->tac);
	cat_tac(if_else_stat->tac, buf);
	cat_tac(if_else_stat->tac, exp_2->tac);

	NAME_ALLOC(label_name_2);
	sprintf(label_name_2, "label_%d", label_amount++);
	find_identifier(label_name_2, ADD, LABEL_IFZ);
	sprintf(buf, "goto %s\n", label_name_2);
	cat_tac(if_else_stat->tac, buf);
	sprintf(buf, "label %s\n", label_name_1);
	cat_tac(if_else_stat->tac, buf);

	cat_tac(if_else_stat->tac, exp_3->tac);
	sprintf(buf, "label %s\n", label_name_2);
	cat_tac(if_else_stat->tac, buf);

	free(exp_1);
	free(exp_2);
	free(exp_3);

	return if_else_stat;
}

struct op* process_call(char* name, struct op* exp_1) {
	struct op* call_stat = new_op();

	int t = new_temp();
	BUF_ALLOC(buf);
	sprintf(buf, "%s = call %s\n", identifiers[t].name, name);
	cat_tac(call_stat->tac, exp_1->tac);
	cat_tac(call_stat->tac, buf);
	call_stat->addr = t;

	free(exp_1);

	return call_stat;
}

struct op* process_return(struct op* exp_1) {
	struct op* return_stat = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "return %s\n", identifiers[exp_1->addr].name);
	cat_tac(return_stat->tac, exp_1->tac);
	cat_tac(return_stat->tac, buf);

	free(exp_1);

	return return_stat;
}

struct op* process_output(char* name) {
	struct op* output_stat = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "output %s\n", name);
	strcpy(output_stat->tac, buf);

	return output_stat;
}

struct op* process_input(char* name) {
	struct op* input_stat = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "input %s\n", name);
	strcpy(input_stat->tac, buf);

	return input_stat;
}

struct op* process_assign(char* name, struct op* exp_1) {
	struct op* assign_stat = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "%s = %s\n", name, identifiers[exp_1->addr].name);
	cat_tac(assign_stat->tac, exp_1->tac);
	cat_tac(assign_stat->tac, buf);
	assign_stat->addr = exp_1->addr;

	free(exp_1);

	return assign_stat;
}
