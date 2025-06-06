#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "e_tac.h"
#include "o_asm.h"
#include "o_reg.h"

// 函数声明
void tac_to_obj();
void asm_code(struct tac *code);
void asm_head();
void asm_tail();
void asm_static(void);
void asm_str(struct id *s);
#endif	// CODE_GENERATOR_H