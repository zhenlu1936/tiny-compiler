#include "e_func.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_helper.h"

extern struct id identifiers[MAX];
extern int identifiers_amount;
extern int num_amount;
extern int temp_amount;
extern int label_amount;

struct op* process_program(struct op* exp_1) {
	struct op* program = cpy_op(exp_1);

	printf("program compiled to tac!\n\n");

	FILE* fp = fopen("tac.txt", "w");
	if (fp == NULL) {
		perror("failed to open file");
	}
	fputs(program->tac, fp);

	puts(program->tac);

	free(exp_1);

	return program;
}

struct op* process_function(struct op* exp_1, struct op* exp_2,
							struct op* exp_3) {
	struct op* function = new_op();

	cat_tac(function->tac, exp_1->tac);
	cat_tac(function->tac, exp_2->tac);
	cat_tac(function->tac, exp_3->tac);
	BUF_ALLOC(buf);
	sprintf(buf, "end\n");
	cat_tac(function->tac, buf);

	free(exp_1);
	free(exp_2);
	free(exp_3);

	return function;
}

struct op* process_function_head(char* name) {
	struct op* function_head = new_op();

	find_identifier(name, ADD, INT_FUNC);
	BUF_ALLOC(buf_1);
	sprintf(buf_1, "label %s\n", name);
	BUF_ALLOC(buf_2);
	sprintf(buf_2, "start\n");
	cat_tac(function_head->tac, buf_1);
	cat_tac(function_head->tac, buf_2);

	return function_head;
}

struct op* process_parameter_list_end(char* name) {
	struct op* parameter = new_op();

	find_identifier(name, ADD, INT_VAR);  // 可能要改成局部
	BUF_ALLOC(buf);
	sprintf(buf, "param %s\n", name);
	strcpy(parameter->tac, buf);

	return parameter;
}

struct op* process_parameter_list(struct op* exp_1, char* name) {
	struct op* parameter_list = new_op();

	find_identifier(name, ADD, INT_VAR);
	BUF_ALLOC(buf);
	sprintf(buf, "param %s\n", name);
	cat_tac(parameter_list->tac, exp_1->tac);
	cat_tac(parameter_list->tac, buf);

	free(exp_1);
	
	return parameter_list;
}