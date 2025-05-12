#include "reg_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 全局变量
struct rdesc rdesc[R_NUM];

// 清除寄存器描述符
void rdesc_clear(int r) {
	rdesc[r].var = NULL;
	rdesc[r].mod = 0;
}

// 填充寄存器描述符
void rdesc_fill(int r, struct id *s, int mod) {
	// hjj: 需要允许同时有数个寄存器存储同一符号的情况，在asm_cmp会用到。
	// for (int old = R_GEN; old < R_NUM; old++) {
	// 	if (rdesc[old].var == s) {	// 若已有寄存器存储该符号，则清除该寄存器
	// 		rdesc_clear(old);
	// 	}
	// }
	rdesc[r].var = s;
	rdesc[r].mod = mod;
}

// 写回寄存器内容到内存
void asm_write_back(int r) {
	if ((rdesc[r].var != NULL) && rdesc[r].mod) {
		if (rdesc[r].var->scope == 1) {	 // 局部变量从栈上加载
			input_str(obj_file, "    STO (R%u+%u),R%u\n", R_BP,
					  rdesc[r].var->offset, r);
		} else {  // 全局变量从静态数据段加载
			input_str(obj_file, "    LOD R%u,STATIC\n", R_TP);
			input_str(obj_file, "    STO (R%u+%u),R%u\n", R_TP,
					  rdesc[r].var->offset, r);
		}
		rdesc[r].mod = UNMODIFIED;
	}
}

// hjj: 有优化余地
// 加载变量到寄存器
void asm_load(int r, struct id *s) {
	for (int i = R_GEN; i < R_NUM; i++) {
		if (rdesc[i].var ==
			s) {  // 若已有寄存器存储目标符号，则直接将该寄存器的值存入寄存器r
			input_str(obj_file, "    LOD R%u,R%u\n", r, i);
			return;
		}
	}

	// 若尚无寄存器存储目标符号
	switch (s->id_type) {
		case ID_NUM:
			input_str(obj_file, "    LOD R%u,%u\n", r, s->num);
			break;
		case ID_TEMP:
		case ID_VAR:
			if (s->scope == 1) {  // 局部变量从栈上加载
				if (s->offset >= 0)
					input_str(obj_file, "    LOD R%u,(R%u+%d)\n", r, R_BP,
							  s->offset);
				else
					input_str(obj_file, "    LOD R%u,(R%u%d)\n", r, R_BP,
							  s->offset);
			} else {  // 全局变量从静态数据段加载
				input_str(obj_file, "    LOD R%u,STATIC\n", R_TP);
				input_str(obj_file, "    LOD R%u,(R%u+%d)\n", r, R_TP,
						  s->offset);
			}
			break;
		case ID_STRING:
			input_str(obj_file, "    LOD R%u,L%u\n", r, s->label);
			break;
		default:
			perror("what type?");
			printf("id type: %d\n", s->id_type);
	}
}

// 寻找存储目标符号的寄存器
int reg_find(struct id *s) {
	for (int r = R_GEN; r < R_NUM; r++) {
		if (rdesc[r].var == s) {  // 若已有寄存器存储该符号
			return r;
		}
	}

	return reg_alloc(s);  // 若无寄存器存储该符号，则分配一个新的
}

// 为目标符号分配寄存器
int reg_alloc(struct id *s) {
	for (int r = R_GEN; r < R_NUM; r++) {
		if (rdesc[r].var == NULL) {	 // 若有寄存器未存储符号
			asm_load(r, s);
			rdesc_fill(r, s, UNMODIFIED);
			return r;
		}
	}

	for (int r = R_GEN; r < R_NUM; r++) {
		if (rdesc[r].mod == UNMODIFIED) {  // 若有寄存器未被修改
			asm_load(r, s);
			rdesc_fill(r, s, UNMODIFIED);
			return r;
		}
	}

	// 若所有寄存器都已被分配且都被修改，则随机挑选寄存器
	srand(time(NULL));
	int random = (rand() % (R_NUM - R_GEN)) + R_GEN;
	asm_write_back(random);
	asm_load(random, s);
	rdesc_fill(random, s, UNMODIFIED);
	return random;
}