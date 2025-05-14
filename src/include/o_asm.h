#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H
#include <stdio.h>
#include <stdlib.h>

#include "o_reg.h"
/* register */
extern int tos;	 // 栈顶偏移
extern int tof;	 // 栈帧偏移
extern int oof;	 // 参数偏移
extern int oon;	 // 临时偏移
// 函数声明
void asm_bin(char *op, struct id *a, struct id *b, struct id *c);
// hjj
// void asm_cmp(char *op, struct id *a, struct id *b, struct id *c);
void asm_cmp(int op, struct id *a, struct id *b, struct id *c);
void asm_cond(char *op, struct id *a, const char *l);
void asm_call(struct id *a, struct id *b);
void asm_return(struct id *a);

#endif	// ASM_GENERATOR_H