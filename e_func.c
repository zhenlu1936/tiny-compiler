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

	FILE* f = fopen("tac.txt", "w");
	if (f == NULL) {
		perror("failed to open file");
	}

	struct tac* code = program->code;
	while (code) {
		switch (code->type) {
			case TAC_PLUS:
				fprintf(f, "%s = %s + %s\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_MINUS:
				fprintf(f, "%s = %s - %s\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_MULTIPLY:
				fprintf(f, "%s = %s * %s\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_DIVIDE:
				fprintf(f, "%s = %s / %s\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_EQ:
				fprintf(f, "%s = (%s == %s)\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_NE:
				fprintf(f, "%s = (%s != %s)\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_LT:
				fprintf(f, "%s = (%s < %s)\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_LE:
				fprintf(f, "%s = (%s <= %s)\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_GT:
				fprintf(f, "%s = (%s > %s)\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_GE:
				fprintf(f, "%s = (%s >= %s)\n", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_NEGATIVE:
				fprintf(f, "%s = - %s\n", to_str(code->id_1), to_str(code->id_2));
				break;

			case TAC_ASSIGN:
				fprintf(f, "%s = %s\n", to_str(code->id_1), to_str(code->id_2));
				break;

			case TAC_GOTO:
				fprintf(f, "goto %s\n", code->id_1->name);
				break;

			case TAC_IFZ:
				fprintf(f, "ifz %s goto %s\n", to_str(code->id_1),
						code->id_2->name);
				break;

			case TAC_ARG:
				fprintf(f, "arg %s\n", to_str(code->id_1));
				break;

			case TAC_PARAM:
				fprintf(f, "param %s\n", to_str(code->id_1));
				break;

			case TAC_CALL:
				if (code->id_1 == NULL)
					fprintf(f, "call %s\n", (char*)code->id_2);
				else
					fprintf(f, "%s = call %s\n", to_str(code->id_1),
							to_str(code->id_2));
				break;

			case TAC_INPUT:
				fprintf(f, "input %s\n", to_str(code->id_1));
				break;

			case TAC_OUTPUT:
				fprintf(f, "output %s\n", to_str(code->id_1));
				break;

			case TAC_RETURN:
				fprintf(f, "return %s\n", to_str(code->id_1));
				break;

			case TAC_LABEL:
				fprintf(f, "label %s\n", to_str(code->id_1));
				break;

			case TAC_VAR:
				fprintf(f, "var %s\n", to_str(code->id_1));
				break;

			case TAC_BEGIN:
				fprintf(f, "begin\n");
				break;

			case TAC_END:
				fprintf(f, "end\n");
				break;

			default:
				perror("unknown TAC opcode");
				break;
		}
		code = code->next;
	}

	free(exp_1);

	return program;
}

struct op* process_function(struct op* exp_1, struct op* exp_2,
							struct op* exp_3) {
	struct op* function = new_op();

	cat_tac(function, exp_1->code);
	cat_tac(function, exp_2->code);
	cat_tac(function, exp_3->code);
	NEW_TAC_0(buf, TAC_END);
	cat_tac(function, buf);

	free(exp_1);
	free(exp_2);
	free(exp_3);

	return function;
}

struct op* process_function_head(char* name) {
	struct op* function_head = new_op();

	struct id* func = find_identifier(name, ADD, INT_FUNC);
	NEW_TAC_1(buf_1, TAC_LABEL, func);
	NEW_TAC_0(buf_2, TAC_BEGIN);
	cat_tac(function_head, buf_1);
	cat_tac(function_head, buf_2);

	return function_head;
}

struct op* process_parameter_list_end(char* name) {
	struct op* parameter = new_op();

	struct id* var = find_identifier(name, ADD, INT_VAR);  // 可能要改成局部
	NEW_TAC_1(buf, TAC_PARAM, var);
	cat_tac(parameter, buf);

	return parameter;
}

struct op* process_parameter_list(struct op* exp_1, char* name) {
	struct op* parameter_list = new_op();

	struct id* var = find_identifier(name, ADD, INT_VAR);
	NEW_TAC_1(buf, TAC_PARAM, var);
	cat_tac(parameter_list, exp_1->code);
	cat_tac(parameter_list, buf);

	free(exp_1);

	return parameter_list;
}