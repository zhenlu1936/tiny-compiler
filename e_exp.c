#include "e_exp.h"

#include <stdio.h>
#include <stdlib.h>

#include "e_helper.h"

extern struct id identifiers[MAX];
extern int identifiers_amount;
extern int num_amount;
extern int temp_amount;
extern int label_amount;

struct op* process_calculate(struct op* exp_1, struct op* exp_2, int cal) {
	struct op* exp = new_op();

	cat_tac(exp, exp_1->code);
	cat_tac(exp, exp_2->code);
	struct id* t = new_temp();
	exp->addr = t->addr;
	NEW_TAC_3(code, cal, &identifiers[exp->addr], &identifiers[exp_1->addr],
			  &identifiers[exp_2->addr]);
	cat_tac(exp, code);

	free(exp_1);
	free(exp_2);

	return exp;
}

struct op* process_negative(struct op* exp_1) {
	struct op* neg_exp = new_op();

	struct id* t = new_temp();
	NEW_TAC_2(code, TAC_NEGATIVE, t, &identifiers[exp_1->addr]);
	cat_tac(neg_exp, exp_1->code);
	cat_tac(neg_exp, code);
	neg_exp->addr = t->addr;

	free(exp_1);

	return neg_exp;
}

struct op* process_integer(int integer) {
	struct op* int_exp = new_op();

	BUF_ALLOC(buf);
	sprintf(buf, "%d", integer);
	struct id* var = find_identifier(buf, ADD, INT_NUM);
	int_exp->addr = var->addr;

	return int_exp;
}

struct op* process_identifier(char* name) {
	struct op* id_exp = new_op();

	struct id* var = find_identifier(name, NOT_ADD, INT_VAR);
	id_exp->addr = var->addr;

	return id_exp;
}

struct op* process_inc(char* name, int pos) {
	struct op* inc_exp = new_op();

	struct id* t = new_temp();
	struct id* var = find_identifier(name, NOT_ADD, INT_VAR);
	struct id* num = find_identifier("1", ADD, INT_NUM);
	if (pos == INC_HEAD) {
		NEW_TAC_3(code_1, TAC_PLUS, t, var, num);
		cat_tac(inc_exp, code_1);
	} else {
		NEW_TAC_2(code_1, TAC_ASSIGN, t, var);
		cat_tac(inc_exp, code_1);
	}
	NEW_TAC_3(code_2, TAC_PLUS, var, t, num);
	cat_tac(inc_exp, code_2);
	inc_exp->addr = t->addr;

	return inc_exp;
}

struct op* process_dec(char* name, int pos) {
	struct op* inc_exp = new_op();

	struct id* t = new_temp();
	struct id* var = find_identifier(name, NOT_ADD, INT_VAR);
	struct id* num = find_identifier("1", ADD, INT_NUM);
	if (pos == INC_HEAD) {
		NEW_TAC_3(code_1, TAC_MINUS, t, var, num);
		cat_tac(inc_exp, code_1);
	} else {
		NEW_TAC_2(code_1, TAC_ASSIGN, t, var);
		cat_tac(inc_exp, code_1);
	}
	NEW_TAC_3(code_2, TAC_MINUS, var, t, num);
	cat_tac(inc_exp, code_2);
	inc_exp->addr = t->addr;

	return inc_exp;
}

struct op* process_expression_list_end(struct op* exp_1) {
	struct op* exp = new_op();

	NEW_TAC_1(code, TAC_ARG, &identifiers[exp_1->addr]);
	cat_tac(exp, exp_1->code);
	cat_tac(exp, code);

	return exp;
}

struct op* process_expression_list(struct op* exp_1, struct op* exp_2) {
	struct op* exp_list = new_op();

	NEW_TAC_1(code, TAC_ARG, &identifiers[exp_2->addr]);
	cat_tac(exp_list, exp_1->code);
	cat_tac(exp_list, exp_2->code);
	cat_tac(exp_list, code);

	free(exp_1);
	free(exp_2);

	return exp_list;
}