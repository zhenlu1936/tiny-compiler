// hjj: tbd, float num
#include "e_obj.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "e_tac.h"

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
