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
    for (int old = R_GEN; old < R_NUM; old++) {
        if (rdesc[old].var == s) {
            rdesc_clear(old);
        }
    }
    rdesc[r].var = s;
    rdesc[r].mod = mod;
}

// 写回寄存器内容到内存
void asm_write_back(int r) {
    if ((rdesc[r].var != NULL) && rdesc[r].mod) {
        if (rdesc[r].var->scope == 1) { // 局部变量
            input_str(obj_file, "    STO (R%u+%u),R%u\n", R_BP, rdesc[r].var->offset, r);
        } else { // 全局变量
            input_str(obj_file, "    LOD R%u,STATIC\n", R_TP);
            input_str(obj_file, "    STO (R%u+%u),R%u\n", R_TP, rdesc[r].var->offset, r);
        }
        rdesc[r].mod = UNMODIFIED;
    }
}

// 加载变量到寄存器
void asm_load(int r, struct id *s) {
    for (int i = R_GEN; i < R_NUM; i++) {
        if (rdesc[i].var == s) {
            input_str(obj_file, "    LOD R%u,R%u\n", r, i);
            return;
        }
    }

    switch (s->id_type) {
        case ID_NUM:
            input_str(obj_file, "    LOD R%u,%u\n", r, s->num);
            break;
        case ID_VAR:
            if (s->scope == 1) {
                input_str(obj_file, "    LOD R%u,(R%u+%d)\n", r, R_BP, s->offset);
            } else {
                input_str(obj_file, "    LOD R%u,STATIC\n", R_TP);
                input_str(obj_file, "    LOD R%u,(R%u+%d)\n", r, R_TP, s->offset);
            }
            break;
        case ID_STRING:
            input_str(obj_file, "    LOD R%u,label_%u\n", r, s->label);
            break;
    }
}

// 分配寄存器
int reg_alloc(struct id *s) {
    for (int r = R_GEN; r < R_NUM; r++) {
        if (rdesc[r].var == s) {
            return r;
        }
    }

    for (int r = R_GEN; r < R_NUM; r++) {
        if (rdesc[r].var == NULL) {
            asm_load(r, s);
            rdesc_fill(r, s, UNMODIFIED);
            return r;
        }
    }

    for (int r = R_GEN; r < R_NUM; r++) {
        if (!rdesc[r].mod) {
            asm_load(r, s);
            rdesc_fill(r, s, UNMODIFIED);
            return r;
        }
    }

    srand(time(NULL));
    int random = (rand() % (R_NUM - R_GEN)) + R_GEN;
    asm_write_back(random);
    asm_load(random, s);
    rdesc_fill(random, s, UNMODIFIED);
    return random;
}