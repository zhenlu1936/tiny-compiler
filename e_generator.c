#include "e_generator.h"

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
	struct op *if_else_stat = new_op();

	NAME_ALLOC(label_name_1);
	sprintf(label_name_1, "label_%d", label_amount++);
	find_identifier(label_name_1, ADD, LABEL_IFZ);
	BUF_ALLOC(buf);
	sprintf(buf, "ifz %s goto %s\n", identifiers[exp_1->addr].name, label_name_1);
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