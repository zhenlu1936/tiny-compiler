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
				fprintf(f, "%s = %s + %s", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_MINUS:
				fprintf(f, "%s = %s - %s", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_MULTIPLY:
				fprintf(f, "%s = %s * %s", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_DIVIDE:
				fprintf(f, "%s = %s / %s", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_EQ:
				fprintf(f, "%s = (%s == %s)", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_NE:
				fprintf(f, "%s = (%s != %s)", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_LT:
				fprintf(f, "%s = (%s < %s)", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_LE:
				fprintf(f, "%s = (%s <= %s)", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_GT:
				fprintf(f, "%s = (%s > %s)", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_GE:
				fprintf(f, "%s = (%s >= %s)", to_str(code->id_1),
						to_str(code->id_2), to_str(code->id_3));
				break;

			case TAC_NEGATIVE:
				fprintf(f, "%s = - %s", to_str(code->id_1), to_str(code->id_2));
				break;

			case TAC_ASSIGN:
				fprintf(f, "%s = %s", to_str(code->id_1), to_str(code->id_2));
				break;

			case TAC_GOTO:
				fprintf(f, "goto %s", code->id_1->name);
				break;

			case TAC_IFZ:
				fprintf(f, "ifz %s goto %s", to_str(code->id_2),
						code->id_1->name);
				break;

			case TAC_ARG:
				fprintf(f, "arg %s", to_str(code->id_1));
				break;

			case TAC_PARAM:
				fprintf(f, "param %s", to_str(code->id_1));
				break;

			case TAC_CALL:
				if (code->id_1 == NULL)
					fprintf(f, "call %s", (char*)code->id_2);
				else
					fprintf(f, "%s = call %s", to_str(code->id_1),
							(char*)code->id_2);
				break;

			case TAC_INPUT:
				fprintf(f, "input %s", to_str(code->id_1));
				break;

			case TAC_OUTPUT:
				fprintf(f, "output %s", to_str(code->id_1));
				break;

			case TAC_RETURN:
				fprintf(f, "return %s", to_str(code->id_1));
				break;

			case TAC_LABEL:
				fprintf(f, "label %s", code->id_1->name);
				break;

			case TAC_VAR:
				fprintf(f, "var %s", to_str(code->id_1));
				break;

			case TAC_BEGIN:
				fprintf(f, "begin");
				break;

			case TAC_END:
				fprintf(f, "end");
				break;

			default:
				perror("unknown TAC opcode");
				break;
		}
		code = code->next;
	}
	// fputs(program->code, fp);
	// puts(program->code);

	free(exp_1);

	return program;
}

struct op* process_function(struct op* exp_1, struct op* exp_2,
							struct op* exp_3) {
	struct op* function = new_op();

	cat_tac(function->code, exp_1->code);
	cat_tac(function->code, exp_2->code);
	cat_tac(function->code, exp_3->code);
	// BUF_ALLOC(buf);
	// sprintf(buf, "end\n");
	NEW_TAC_0(buf, TAC_END);
	cat_tac(function->code, buf);

	free(exp_1);
	free(exp_2);
	free(exp_3);

	return function;
}

struct op* process_function_head(char* name) {
	struct op* function_head = new_op();

	struct id* func = find_identifier(name, ADD, INT_FUNC);
	// BUF_ALLOC(buf_1);
	// sprintf(buf_1, "label %s\n", name);
	// BUF_ALLOC(buf_2);
	// sprintf(buf_2, "start\n");
	NEW_TAC_1(buf_1, TAC_LABEL, func);
	NEW_TAC_0(buf_2, TAC_BEGIN);
	cat_tac(function_head->code, buf_1);
	cat_tac(function_head->code, buf_2);

	return function_head;
}

struct op* process_parameter_list_end(char* name) {
	struct op* parameter = new_op();

	struct id* var = find_identifier(name, ADD, INT_VAR);  // 可能要改成局部
	// BUF_ALLOC(buf);
	// sprintf(buf, "param %s\n", name);
	NEW_TAC_1(buf, TAC_PARAM, var);
	cat_tac(parameter->code, buf);

	return parameter;
}

struct op* process_parameter_list(struct op* exp_1, char* name) {
	struct op* parameter_list = new_op();

	struct id* var = find_identifier(name, ADD, INT_VAR);
	// BUF_ALLOC(buf);
	// sprintf(buf, "param %s\n", name);
	NEW_TAC_1(buf, TAC_PARAM, var);
	cat_tac(parameter_list->code, exp_1->code);
	cat_tac(parameter_list->code, buf);

	free(exp_1);

	return parameter_list;
}