#include "e_func.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_tac.h"

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