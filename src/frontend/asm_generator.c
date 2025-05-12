#include "asm_generator.h"

#include <stdio.h>

#include "reg_manager.h"

// 全局变量定义
int tos = 0;  // 栈顶偏移
int tof = 0;  // 栈帧偏移
int oof = 0;  // 参数偏移
int oon = 0;  // 临时偏移

void asm_bin(char *op, struct id *a, struct id *b, struct id *c) {
	int reg_b = -1, reg_c = -1;
	while (reg_b == reg_c) {
		reg_b = reg_find(b);
		reg_c = reg_find(c);
	}
	// asm_write_back(reg_b);
	input_str(obj_file, "	%s R%u,R%u\n", op, reg_b, reg_c);
	rdesc_fill(reg_b, a, MODIFIED);
}

// hjj
// void asm_cmp(char* op, struct id *a, struct id *b, struct id *c) {
void asm_cmp(int op, struct id *a, struct id *b, struct id *c) {
	int reg_temp = -1, reg_c = -1;

	// hjj
	// 分配寄存器，确保 temp 和 c 不在同一个寄存器中
	while (reg_temp == reg_c) {
		// reg_temp = reg_find(b);
		reg_temp = reg_alloc(b);
		reg_c = reg_find(c);
	}
	// asm_write_back(reg_temp);

	// hjj: 原逻辑有误，不能直接修改存储符号的寄存器，应该分配一个临时符号
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

	// 清除寄存器描述符并更新 a 的描述符
	rdesc_clear(reg_temp);
	rdesc_fill(reg_temp, a, MODIFIED);
}

void asm_cond(char *op, struct id *a, const char *l) {
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

void asm_call(struct id *a, struct id *b) {
	int r;
	for (int r = R_GEN; r < R_NUM; r++) asm_write_back(r);
	for (int r = R_GEN; r < R_NUM; r++) rdesc_clear(r);
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

void asm_return(struct id *a) {
	for (int r = R_GEN; r < R_NUM; r++) asm_write_back(r);
	for (int r = R_GEN; r < R_NUM; r++) rdesc_clear(r);

	if (a != NULL) /* return value */
	{
		asm_load(R_TP, a);
	}

	input_str(obj_file, "	LOD R3,(R2+4)\n"); /* return address */
	input_str(obj_file, "	LOD R2,(R2)\n");   /* restore bp */
	input_str(obj_file, "	JMP R3\n");		   /* return */
}