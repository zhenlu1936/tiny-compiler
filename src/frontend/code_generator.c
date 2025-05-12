#include "code_generator.h"
#include "asm_generator.h"
#include "reg_manager.h"
#include <stdio.h>

// 将三地址码翻译为目标代码

void tac_to_obj() {
	tof = LOCAL_OFF; /* TOS allows space for link info */
	oof = FORMAL_OFF;
	oon = 0;

	for (int r = 0; r < R_NUM; r++) rdesc[r].var = NULL;

	asm_head();

	struct tac *cur;
	for (cur = tac_head; cur != NULL; cur = cur->next) {
		input_str(obj_file, "\n\t# ");
		output_tac(obj_file, cur);
		// input_str(obj_file, "\n");
		asm_code(cur);
	}

	asm_tail();
	asm_static();
}

void asm_code(struct tac *code) {
	int r;

	switch (code->type) {
		case TAC_UNDEF:
			perror("cannot translate TAC_UNDEF");
			return;

		case TAC_PLUS:
			asm_bin("ADD", code->id_1, code->id_2, code->id_3);
			return;

		case TAC_MINUS:
			asm_bin("SUB", code->id_1, code->id_2, code->id_3);
			return;

		case TAC_MULTIPLY:
			asm_bin("MUL", code->id_1, code->id_2, code->id_3);
			return;

		case TAC_DIVIDE:
			asm_bin("DIV", code->id_1, code->id_2, code->id_3);
			return;

		// hjj
		// case TAC_EQ:
		// 	asm_cmp("JE", code->id_1, code->id_2, code->id_3);
		// 	return;
		// case TAC_NE:
		// 	asm_cmp("JNE", code->id_1, code->id_2, code->id_3);
		// 	return;
		// case TAC_LT:
		// 	asm_cmp("JL", code->id_1, code->id_2, code->id_3);
		// 	return;
		// case TAC_LE:
		// 	asm_cmp("JLE", code->id_1, code->id_2, code->id_3);
		// 	return;
		// case TAC_GT:
		// 	asm_cmp("JG", code->id_1, code->id_2, code->id_3);
		// 	return;
		// case TAC_GE:
		// 	asm_cmp("JGE", code->id_1, code->id_2, code->id_3);
		// 	return;
		case TAC_EQ:
		case TAC_NE:
		case TAC_LT:
		case TAC_LE:
		case TAC_GT:
		case TAC_GE:
			asm_cmp(code->type, code->id_1, code->id_2, code->id_3);
			return;

		case TAC_ASSIGN:
			r = reg_alloc(code->id_2);
			rdesc_fill(r, code->id_1, MODIFIED);
			return;

		case TAC_INPUT:
			r = reg_alloc(code->id_1);
			input_str(obj_file, "	IN\n");
			input_str(obj_file, "	LOD R%u,R15\n", r);
			rdesc[r].mod = MODIFIED;
			return;

		case TAC_OUTPUT:
			if (code->id_1->id_type == ID_VAR) {
				r = reg_alloc(code->id_1);
				input_str(obj_file, "	LOD R15,R%u\n", r);
				input_str(obj_file, "	OUTN\n");
			} else if (code->id_1->id_type == ID_STRING) {
				r = reg_alloc(code->id_1);
				input_str(obj_file, "	LOD R15,R%u\n", r);
				input_str(obj_file, "	OUTS\n");
			}
			return;

		case TAC_GOTO:
			asm_cond("JMP", NULL, code->id_1->name);
			return;

		case TAC_IFZ:
			asm_cond("JEZ", code->id_1, code->id_2->name);
			return;

		case TAC_LABEL:
			for (int r = R_GEN; r < R_NUM; r++) asm_write_back(r);
			for (int r = R_GEN; r < R_NUM; r++) rdesc_clear(r);
			input_str(obj_file, "%s:\n", code->id_1->name);
			return;

		case TAC_ARG:
			r = reg_alloc(code->id_1);
			input_str(obj_file, "	STO (R2+%d),R%u\n", tof + oon, r);
			oon += 4;
			return;

		case TAC_PARAM:
			code->id_1->scope = 1; /* parameter is special local var */
			code->id_1->offset = oof;
			oof -= 4;
			return;

		case TAC_CALL:
			asm_call(code->id_1, code->id_2);
			return;

		case TAC_BEGIN:
			/* We reset the top of stack, since it is currently empty apart from
			 * the link information. */
			scope = 1;
			tof = LOCAL_OFF;
			oof = FORMAL_OFF;
			oon = 0;
			return;

		case TAC_END:
			asm_return(NULL);
			scope = 0;
			return;

		case TAC_VAR:
			if (scope) {
				code->id_1->scope = 1; /* local var */
				code->id_1->offset = tof;
				tof += 4;
			} else {
				code->id_1->scope = 0; /* global var */
				code->id_1->offset = tos;
				tos += 4;
			}
			return;

		case TAC_RETURN:
			asm_return(code->id_1);
			return;

		default:
			/* Don't know what this one is */
			perror("unknown TAC opcode to translate");
			printf("type: %d\n", code->type);
			return;
	}
}

void asm_head() {
	char head[] =
		"	# head\n"
		"	LOD R2,STACK\n"
		"	STO (R2),0\n"
		"	LOD R4,EXIT\n"
		"	STO (R2+4),R4\n";

	input_str(obj_file, "%s", head);
}

void asm_tail() {
	char tail[] =
		"\n	# tail\n"
		"EXIT:\n"
		"	END\n";

	input_str(obj_file, "%s", tail);
}

void asm_str(struct id *s) {
	const char *t = s->name; /* The text */
	int i;

	input_str(obj_file, "label_%u:\n", s->label); /* Label for the string */
	input_str(obj_file, "	DBS ");			 /* Label for the string */

	for (i = 1; t[i + 1] != 0; i++) {
		if (t[i] == '\\') {
			switch (t[++i]) {
				case 'n':
					input_str(obj_file, "%u,", '\n');
					break;

				case '\"':
					input_str(obj_file, "%u,", '\"');
					break;
			}
		} else
			input_str(obj_file, "%u,", t[i]);
	}

	input_str(obj_file, "0\n"); /* End of string */
}

void asm_static(void) {
	int i;

	struct id *sl;

	for (sl = id_global; sl != NULL; sl = sl->next) {
		if (sl->id_type == ID_STRING) asm_str(sl);
	}

	input_str(obj_file, "STATIC:\n");
	input_str(obj_file, "	DBN 0,%u\n", tos);
	input_str(obj_file, "STACK:\n");
}
