#include "reg_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局变量
struct rdesc rdesc[R_NUM];

// 清除某变量对应的所有寄存器描述符
void rdesc_clear_all(int r) {
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
void rdesc_clear_prev(int r) {
	if (rdesc[r].prev != NULL) {
		int pre = RDESC_NUM(rdesc[r].prev);
		rdesc[r].prev = NULL;
		rdesc_clear_all(pre);
	}
}

// 清除临时的寄存器描述符
void rdesc_clear_temp(int r) {
	rdesc[r].var = NULL;
	rdesc[r].mod = 0;
	if (rdesc[r].prev != NULL) {
		rdesc[r].prev->next = NULL;
	}
	rdesc[r].prev = NULL;
}

// 填充寄存器描述符
void rdesc_fill(int r, struct id *s, int mod) {
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

// 加载变量到寄存器
void asm_load(int r, struct id *s) {
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

// 寻找符号对应的最晚被修改的寄存器
int reg_find(struct id *s) {
	int first_appear;

	/* already in a register */
	for (first_appear = R_GEN; first_appear < R_NUM; first_appear++) {
		if (rdesc[first_appear].var == s) {
			FIND_LATEST_RDESC(first_appear, latest_appear);
			return latest_appear;
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