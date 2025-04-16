#include "e_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct id identifiers[MAX];
int identifiers_amount;
int num_amount;
int temp_amount;
int label_amount;

struct id* find_identifier(const char* name, int add, int type) {
	int has_added = 0, id = 0;
	for (id = 0; id < MAX; id++) {
		if (identifiers[id].name && !strcmp(name, identifiers[id].name)) {
			has_added = 1;
			break;
		}
	}
	if (!has_added && add) {
		id = identifiers_amount++;
		char* id_name = (char*)malloc(sizeof(char) * strlen(name));
		strcpy(id_name, name);
		identifiers[id].name = id_name;
		identifiers[id].type = type;
		identifiers[id].addr = id;
	}
	if (id == MAX) {
		perror("identifier not found");
	}
	return &identifiers[id];
}

void cat_tac(struct tac* dest, struct tac* src) {
	struct tac* t = dest;
	while (t->next != NULL) t = t->next;
	t->next = src;
}

struct op* cat_list(struct op* exp_1, struct op* exp_2) {
	struct op* stat_list = new_op();

	cat_tac(stat_list->code, exp_1->code);
	cat_tac(stat_list->code, exp_2->code);

	return stat_list;
}

struct op* cpy_op(const struct op* src) {
	struct op* nop = new_op();
	cat_tac(nop->code, src->code);
	if (src->addr != NO_ADDR) {
		nop->addr = src->addr;
	}
	return nop;
}

struct op* new_op() {
	struct op* nop;
	MALLOC_AND_SET(nop, 1, struct op);
	MALLOC_AND_SET(nop->code, 1, struct tac);
	return nop;
}

struct tac* new_tac(int type, struct id* id_1, struct id* id_2,
					struct id* id_3) {
	struct tac* ntac = (struct tac*)malloc(sizeof(struct tac));

	ntac->type = type;
	ntac->next = NULL;
	ntac->prev = NULL;
	ntac->id_1 = id_1;
	ntac->id_2 = id_2;
	ntac->id_3 = id_3;

	return ntac;
}

struct id* new_temp() {
	BUF_ALLOC(buf);
	sprintf(buf, "t%d", temp_amount++);
	return find_identifier(buf, ADD, INT_TEMP);
}

const char* to_str(struct id* id) {
	if (id == NULL) return "NULL";

	switch (id->type) {
		case INT_FUNC:
		case INT_VAR:
		case LABEL_IFZ:
		case INT_NUM:
			return id->name;

		default:
			perror("unknown TAC arg type");
			return "?";
	}
}