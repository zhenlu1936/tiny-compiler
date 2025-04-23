#include "e_exp.h"

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
	struct id *var = add_identifier(buf, INT_NUM);
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
	struct id *num = add_identifier("1", INT_NUM);
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
	struct id *num = add_identifier("1", INT_NUM);
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