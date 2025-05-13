#ifndef REG_MANAGER_H
#define REG_MANAGER_H

#define R_UNDEF -1
#define R_FLAG 0
#define R_IP 1
#define R_BP 2
#define R_JP 3
#define R_TP 4
#define R_GEN 5
#define R_NUM 14

/* frame */
#define FORMAL_OFF -4 	/* first formal parameter */
#define OBP_OFF 0 		/* dynamic chain */
#define RET_OFF 4 		/* ret address */
#define LOCAL_OFF 8 		/* local var */

#define MODIFIED 1
#define UNMODIFIED 0

#include <stdio.h>
#include "e_tac.h"
// 寄存器描述符结构
struct rdesc {
    struct id *var; // 当前寄存器存储的变量
    int mod;        // 是否被修改
	struct rdesc *next;
	struct rdesc *prev;
};

// 全局变量
extern struct rdesc rdesc[];

#define RDESC_NUM(p_rdesc) (p_rdesc - rdesc)
#define FIND_LATEST_RDESC(first_appear,latest_appear) \
	int latest_appear = first_appear; \
	while (rdesc[latest_appear].next) { \
		latest_appear = RDESC_NUM(rdesc[latest_appear].next); \
	}

// 函数声明
void rdesc_clear_all(int r);
void rdesc_clear_prev(int r);
void rdesc_clear_temp(int r);
void rdesc_fill(int r, struct id *s, int mod);
void asm_write_back(int r);
void asm_load(int r, struct id *s);
int reg_find(struct id *s);
int reg_alloc(struct id *s);

#endif // REG_MANAGER_H