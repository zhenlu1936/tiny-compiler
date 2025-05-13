/* register */
#ifndef E_OBJ_H
#define E_OBJ_H

#define R_UNDEF -1
#define R_FLAG 0
#define R_IP 1
#define R_BP 2
#define R_JP 3
#define R_TP 4
#define R_GEN 5
#define R_NUM 14

/* frame */
#define FORMAL_OFF -4 /* first formal parameter */
#define OBP_OFF 0	  /* dynamic chain */
#define RET_OFF 4	  /* ret address */
#define LOCAL_OFF 8	  /* local var */

#define MODIFIED 1
#define UNMODIFIED 0

#define SEARCHED 1
#define NOT_SEARCHED 0

struct rdesc /* register descriptor */
{
	struct id *var;
	int mod;
	struct rdesc *next;
	struct rdesc *prev;
};

extern int tos; /* top of static */
extern int tof; /* top of frame */
extern int oof; /* offset of formal */
extern int oon; /* offset of next frame */
extern struct rdesc rdesc[];

#define RDESC_NUM(p_rdesc) (p_rdesc - rdesc)
#define FIND_LATEST_RDESC(first_appear,latest_appear) \
	int latest_appear = first_appear; \
	while (rdesc[latest_appear].next) { \
		latest_appear = RDESC_NUM(rdesc[latest_appear].next); \
	}

void tac_to_obj();

#endif