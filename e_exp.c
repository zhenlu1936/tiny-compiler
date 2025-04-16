#include "e_exp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_helper.h"

extern struct id identifiers[MAX];
extern int identifiers_amount;
extern int num_amount;
extern int temp_amount;
extern int label_amount;

struct op* process_calculate(struct op* exp_1, struct op* exp_2,
							 const char* cal) {
	struct op* exp = new_op();

	cat_tac(exp->tac, exp_1->tac);
	cat_tac(exp->tac, exp_2->tac);
	exp->addr = new_temp();
	BUF_ALLOC(buf);
	sprintf(buf, "%s = %s %s %s\n", identifiers[exp->addr].name,
			identifiers[exp_1->addr].name, cal, identifiers[exp_2->addr].name);
	cat_tac(exp->tac, buf);

	free(exp_1);
	free(exp_2);

	return exp;
}

struct op* process_negative(struct op* exp_1) {
	struct op* neg_exp = new_op();

	int t = new_temp();
	BUF_ALLOC(buf);
	sprintf(buf, "%s = -%s\n", identifiers[t].name,
			identifiers[exp_1->addr].name);
	cat_tac(neg_exp->tac, exp_1->tac);
	cat_tac(neg_exp->tac, buf);
	neg_exp->addr = t;

	free(exp_1);

	return neg_exp;
}

struct op* process_integer(int integer) {
	struct op* int_exp = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "%d", integer);
	int_exp->addr = find_identifier(buf, ADD, INT_NUM);

	return int_exp;
}

struct op* process_identifier(char* name) {
	struct op* id_exp = new_op();

	id_exp->addr = find_identifier(name, NOT_ADD, INT_VAR);

	return id_exp;
}

struct op* process_inc(char* name, int pos) {
	struct op* inc_exp = new_op();

	int t = new_temp();
	BUF_ALLOC(buf);
	if (pos == INC_HEAD) {
		sprintf(buf, "%s = %s + 1\n", identifiers[t].name, name);
	} else {
		sprintf(buf, "%s = %s\n", identifiers[t].name, name);
	}
	strcpy(inc_exp->tac, buf);
	sprintf(buf, "%s = %s + 1\n", name, identifiers[t].name);
	cat_tac(inc_exp->tac, buf);
	inc_exp->addr = t;

	return inc_exp;
}

struct op* process_dec(char* name, int pos) {
	struct op* inc_exp = new_op();

	int t = new_temp();
	BUF_ALLOC(buf);
	if (pos == INC_HEAD) {
		sprintf(buf, "%s = %s - 1\n", identifiers[t].name, name);
	} else {
		sprintf(buf, "%s = %s\n", identifiers[t].name, name);
	}
	strcpy(inc_exp->tac, buf);
	sprintf(buf, "%s = %s - 1\n", name, identifiers[t].name);
	cat_tac(inc_exp->tac, buf);
	inc_exp->addr = t;

	return inc_exp;
}

struct op* process_expression_list_end(struct op* exp_1) {
	struct op* exp = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "arg %s\n", identifiers[exp_1->addr].name);
	cat_tac(exp->tac, exp_1->tac);
	cat_tac(exp->tac, buf);

	return exp;
}

struct op* process_expression_list(struct op* exp_1, struct op* exp_2) {
	struct op* exp_list = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "arg %s\n", identifiers[exp_2->addr].name);
	cat_tac(exp_list->tac, exp_1->tac);
	cat_tac(exp_list->tac, exp_2->tac);
	cat_tac(exp_list->tac, buf);

	free(exp_1);
	free(exp_2);

	return exp_list;
}