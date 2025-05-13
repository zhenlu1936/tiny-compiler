// hjj: tbd, float num
#include "e_obj.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "e_tac.h"

/* global var */
int tos; /* top of static */
int tof; /* top of frame */
int oof; /* offset of formal */
int oon; /* offset of next frame */
struct rdesc rdesc[R_NUM];

// 清除某变量对应的所有寄存器描述符
static void rdesc_clear_all(int r) {
	rdesc[r].var = NULL;
	rdesc[r].mod = 0;
	rdesc[r].next = NULL;
	if (rdesc[r].prev != NULL) {
		int pre = RDESC_NUM(rdesc[r].prev);
		rdesc[r].prev = NULL;
		rdesc_clear_all(pre);
	}
}

// 清除某变量对应的所有非最新的寄存器描述符
static void rdesc_clear_prev(int r) {
	if (rdesc[r].prev != NULL) {
		int pre = RDESC_NUM(rdesc[r].prev);
		rdesc[r].prev = NULL;
		rdesc_clear_all(pre);
	}
}

// 清除临时的寄存器描述符
static void rdesc_clear_temp(int r) {
	rdesc[r].var = NULL;
	rdesc[r].mod = 0;
	rdesc[r].prev->next = NULL;
	rdesc[r].prev = NULL;
}

// 填充寄存器描述符
static void rdesc_fill(int r, struct id *s, int mod) {
	// hjj: 需要允许同时有数个寄存器存储同一符号的情况，在asm_cmp会用到
	// hjj: 用链表存储某个var对应的rdesc的先后次序
	int first_appear;
	for (first_appear = R_GEN; first_appear < R_NUM; first_appear++) {
		if (rdesc[first_appear].var == s) {
			FIND_LATEST_RDESC(first_appear, latest_appear);
			if (r != latest_appear) {
				rdesc[latest_appear].next = &rdesc[r];
				rdesc[r].prev = &rdesc[latest_appear];
			}
			break;	// hjj: 对每个var只有一条链表及头结点，也就是最小的rdesc
		}
	}
	rdesc[r].var = s;
	rdesc[r].mod = mod;
}

// 写回寄存器内容至内存
static void asm_write_back(int r) {
	if ((rdesc[r].var != NULL) && rdesc[r].mod) {
		if (rdesc[r].var->scope == 1) /* local var */
		{
			input_str(obj_file, "	STO (R%u+%u),R%u\n", R_BP,
					  rdesc[r].var->offset, r);
		} else /* global var */
		{
			input_str(obj_file, "	LOD R%u,STATIC\n", R_TP);
			input_str(obj_file, "	STO (R%u+%u),R%u\n", R_TP,
					  rdesc[r].var->offset, r);
		}
		rdesc[r].mod = UNMODIFIED;
	}
}

// 加载符号到寄存器
static void asm_load(int r, struct id *s) {
	/* already in a reg */
	for (int first_appear = R_GEN; first_appear < R_NUM; first_appear++) {
		if (rdesc[first_appear].var == s) {
			// hjj: 应该找最近被修改的寄存器，找下一个
			FIND_LATEST_RDESC(first_appear, latest_appear);

			/* load from the reg */
			if (r != latest_appear) {
				input_str(obj_file, "	LOD R%u,R%u\n", r, latest_appear);
				rdesc[latest_appear].next = &rdesc[r];
				rdesc[r].prev = &rdesc[latest_appear];
			}
			return;
		}
	}

	/* not in a reg */
	switch (s->id_type) {
		case ID_NUM:
			input_str(obj_file, "	LOD R%u,%u\n", r, s->num);
			break;

		case ID_TEMP:
		case ID_VAR:
			if (s->scope == 1) /* local var */
			{
				if ((s->offset) >= 0)
					input_str(obj_file, "	LOD R%u,(R%u+%d)\n", r, R_BP,
							  s->offset);
				else
					input_str(obj_file, "	LOD R%u,(R%u-%d)\n", r, R_BP,
							  -(s->offset));
			} else /* global var */
			{
				input_str(obj_file, "	LOD R%u,STATIC\n", R_TP);
				input_str(obj_file, "	LOD R%u,(R%u+%d)\n", r, R_TP,
						  s->offset);
			}
			break;

		case ID_STRING:
			input_str(obj_file, "	LOD R%u,L%u\n", r, s->label);
			break;
	}

	// rdesc_fill(r, s, UNMODIFIED);
}

// 为符号分配寄存器
static int reg_alloc(struct id *s) {
	int r;

	/* empty register */
	for (r = R_GEN; r < R_NUM; r++) {
		if (rdesc[r].var == NULL) {
			asm_load(r, s);
			rdesc_fill(r, s, UNMODIFIED);
			return r;
		}
	}

	/* unmodifed register */
	for (r = R_GEN; r < R_NUM; r++) {
		if (!rdesc[r].mod) {
			asm_load(r, s);
			rdesc_fill(r, s, UNMODIFIED);
			return r;
		}
	}

	/* random register */
	srand(time(NULL));
	int random = (rand() % (R_NUM - R_GEN)) + R_GEN;
	asm_write_back(random);
	asm_load(random, s);
	rdesc_fill(random, s, UNMODIFIED);
	return random;
}

// 寻找符号对应的最晚被修改的寄存器
static int reg_find(struct id *s) {
	int first_appear;

	/* already in a register */
	for (first_appear = R_GEN; first_appear < R_NUM; first_appear++) {
		if (rdesc[first_appear].var == s) {
			FIND_LATEST_RDESC(first_appear, latest_appear);
			if (!strcmp(s->name, "a")) {
				printf("goint to find latest a: %d\n", latest_appear);
			}
			return latest_appear;
		}
	}

	return reg_alloc(s);
}

// 生成二元运算对应的汇编代码
static void asm_bin(char *op, struct id *a, struct id *b, struct id *c) {
	int reg_b_new = -1, reg_c = -1;

	while (reg_b_new == reg_c) {
		reg_b_new = reg_alloc(b);
		reg_c = reg_find(c);
	}

	input_str(obj_file, "	%s R%u,R%u\n", op, reg_b_new, reg_c);
	rdesc_clear_prev(reg_b_new);
	rdesc_fill(reg_b_new, a, MODIFIED);
}

// 生成比较运算对应的汇编代码
static void asm_cmp(int op, struct id *a, struct id *b, struct id *c) {
	int reg_temp = -1, reg_c = -1;

	while (reg_temp == reg_c) {
		reg_temp = reg_alloc(b);
		reg_c = reg_find(c);
	}

	input_str(obj_file, "	SUB R%u,R%u\n", reg_temp, reg_c);
	input_str(obj_file, "	TST R%u\n", reg_temp);

	switch (op) {
		case TAC_EQ:
			input_str(obj_file, "	LOD R3,R1+40\n");
			input_str(obj_file, "	JEZ R3\n");
			input_str(obj_file, "	LOD R%u,0\n", reg_temp);
			input_str(obj_file, "	LOD R3,R1+24\n");
			input_str(obj_file, "	JMP R3\n");
			input_str(obj_file, "	LOD R%u,1\n", reg_temp);
			break;

		case TAC_NE:
			input_str(obj_file, "	LOD R3,R1+40\n");
			input_str(obj_file, "	JEZ R3\n");
			input_str(obj_file, "	LOD R%u,1\n", reg_temp);
			input_str(obj_file, "	LOD R3,R1+24\n");
			input_str(obj_file, "	JMP R3\n");
			input_str(obj_file, "	LOD R%u,0\n", reg_temp);
			break;

		case TAC_LT:
			input_str(obj_file, "	LOD R3,R1+40\n");
			input_str(obj_file, "	JLZ R3\n");
			input_str(obj_file, "	LOD R%u,0\n", reg_temp);
			input_str(obj_file, "	LOD R3,R1+24\n");
			input_str(obj_file, "	JMP R3\n");
			input_str(obj_file, "	LOD R%u,1\n", reg_temp);
			break;

		case TAC_LE:
			input_str(obj_file, "	LOD R3,R1+40\n");
			input_str(obj_file, "	JGZ R3\n");
			input_str(obj_file, "	LOD R%u,1\n", reg_temp);
			input_str(obj_file, "	LOD R3,R1+24\n");
			input_str(obj_file, "	JMP R3\n");
			input_str(obj_file, "	LOD R%u,0\n", reg_temp);
			break;

		case TAC_GT:
			input_str(obj_file, "	LOD R3,R1+40\n");
			input_str(obj_file, "	JGZ R3\n");
			input_str(obj_file, "	LOD R%u,0\n", reg_temp);
			input_str(obj_file, "	LOD R3,R1+24\n");
			input_str(obj_file, "	JMP R3\n");
			input_str(obj_file, "	LOD R%u,1\n", reg_temp);
			break;

		case TAC_GE:
			input_str(obj_file, "	LOD R3,R1+40\n");
			input_str(obj_file, "	JLZ R3\n");
			input_str(obj_file, "	LOD R%u,1\n", reg_temp);
			input_str(obj_file, "	LOD R3,R1+24\n");
			input_str(obj_file, "	JMP R3\n");
			input_str(obj_file, "	LOD R%u,0\n", reg_temp);
			break;
	}

	/* Delete c from the descriptors and insert a */
	rdesc_clear_temp(reg_temp);
	rdesc_fill(reg_temp, a, MODIFIED);
}

// 生成条件跳转(ifz)对应的汇编代码
static void asm_cond(char *op, struct id *a, const char *l) {
	for (int r = R_GEN; r < R_NUM; r++) asm_write_back(r);

	if (a != NULL) {
		int r;

		for (r = R_GEN; r < R_NUM; r++) /* Is it in reg? */
		{
			if (rdesc[r].var == a) break;
		}

		if (r < R_NUM)
			input_str(obj_file, "	TST R%u\n", r);
		else
			input_str(obj_file, "	TST R%u\n",
					  reg_find(a)); /* Load into new register */
	}

	input_str(obj_file, "	%s %s\n", op, l);
}

// 生成函数调用对应的汇编代码
static void asm_call(struct id *a, struct id *b) {
	int r;
	for (int r = R_GEN; r < R_NUM; r++) asm_write_back(r);
	for (int r = R_GEN; r < R_NUM; r++) rdesc_clear_all(r);
	input_str(obj_file, "	STO (R2+%d),R2\n", tof + oon); /* store old bp */
	oon += 4;
	input_str(obj_file, "	LOD R4,R1+32\n"); /* return addr: 4*8=32 */
	input_str(obj_file, "	STO (R2+%d),R4\n",
			  tof + oon); /* store return addr */
	oon += 4;
	input_str(obj_file, "	LOD R2,R2+%d\n", tof + oon - 8); /* load new bp */
	input_str(obj_file, "	JMP %s\n", b->name); /* jump to new func */
	if (a != NULL) {
		r = reg_find(a);
		input_str(obj_file, "	LOD R%u,R%u\n", r, R_TP);
		rdesc[r].mod = MODIFIED;
	}
	oon = 0;
}

// 生成函数返回对应的汇编代码
static void asm_return(struct id *a) {
	for (int r = R_GEN; r < R_NUM; r++) asm_write_back(r);
	for (int r = R_GEN; r < R_NUM; r++) rdesc_clear_all(r);

	if (a != NULL) /* return value */
	{
		asm_load(R_TP, a);
	}

	input_str(obj_file, "	LOD R3,(R2+4)\n"); /* return address */
	input_str(obj_file, "	LOD R2,(R2)\n");   /* restore bp */
	input_str(obj_file, "	JMP R3\n");		   /* return */
}

// 生成开始段，初始化栈和出口
static void asm_head() {
	char head[] =
		"	# head\n"
		"	LOD R2,STACK\n"
		"	STO (R2),0\n"
		"	LOD R4,EXIT\n"
		"	STO (R2+4),R4\n"
		"	JMP main\n";

	input_str(obj_file, "%s", head);
}

// 生成结束段
static void asm_tail() {
	char tail[] =
		"\n	# tail\n"
		"EXIT:\n"
		"	END\n";

	input_str(obj_file, "%s", tail);
}

// 生成字符串数据段
static void asm_str(struct id *s) {
	const char *t = s->name; /* The text */
	int i;

	input_str(obj_file, "L%u:\n", s->label); /* Label for the string */
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

// 生成静态数据段
static void asm_static(void) {
	int i;

	struct id *sl;

	for (sl = id_global; sl != NULL; sl = sl->next) {
		if (sl->id_type == ID_STRING) asm_str(sl);
	}

	input_str(obj_file, "STATIC:\n");
	input_str(obj_file, "	DBN 0,%u\n", tos);
	input_str(obj_file, "STACK:\n");
}

// 根据单条三地址码，生成汇编代码
static void asm_code(struct tac *code) {
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

		case TAC_EQ:
		case TAC_NE:
		case TAC_LT:
		case TAC_LE:
		case TAC_GT:
		case TAC_GE:
			asm_cmp(code->type, code->id_1, code->id_2, code->id_3);
			return;

		case TAC_ASSIGN:
			r = reg_find(code->id_2);
			rdesc_fill(r, code->id_1, MODIFIED);
			return;

		case TAC_INPUT:
			r = reg_find(code->id_1);
			input_str(obj_file, "	IN\n");
			input_str(obj_file, "	LOD R%u,R15\n", r);
			rdesc[r].mod = MODIFIED;
			return;

		case TAC_OUTPUT:
			if (code->id_1->id_type == ID_VAR) {
				r = reg_find(code->id_1);
				input_str(obj_file, "	LOD R15,R%u\n", r);
				input_str(obj_file, "	OUTN\n");
			} else if (code->id_1->id_type == ID_STRING) {
				r = reg_find(code->id_1);
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
			for (int r = R_GEN; r < R_NUM; r++) rdesc_clear_all(r);
			input_str(obj_file, "%s:\n", code->id_1->name);
			return;

		case TAC_ARG:
			r = reg_find(code->id_1);
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

// 根据source_to_tac生成的三地址码，生成汇编代码
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
