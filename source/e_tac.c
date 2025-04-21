#include "e_tac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct id *identifiers;
struct id *id_global;
int scope;

int identifiers_amount;
int num_amount;
int temp_amount;
int label_amount;

void tac_init() {
	scope = 0;
	MALLOC_AND_SET_ZERO(id_global, MAX, struct id);
	identifiers = id_global;
	temp_amount = 0;
	label_amount = 1;
}

static struct id *_find_identifier(const char *name, int add, int type) {
	struct id *id_table;
	id_table = identifiers;
	int has_added = 0, id = 0;
	for (id = 0; id < MAX; id++) {
		if (id_table[id].name && !strcmp(name, id_table[id].name)) {
			has_added = 1;
			break;
		}
	}
	if (!has_added && add) {
		id = identifiers_amount++;
		char *id_name = (char*)malloc(sizeof(char)  *strlen(name));
		strcpy(id_name, name);
		id_table[id].name = id_name;
		id_table[id].type = type;
		id_table[id].addr = id;
	}
	if (id == MAX) {
		perror("identifier not found");
	}
	return &id_table[id];
}

struct id *find_identifier(const char *name) {
	return _find_identifier(name, NOT_ADD, NO_TYPE);
}

struct id *add_identifier(const char *name, int type) {
	return _find_identifier(name, ADD, type);
}

void cat_tac(struct op *dest, struct tac *src) {
	struct tac *t = dest->code;
	if (t == NULL) {
		dest->code = src;
		return;
	}
	while (t->next != NULL) t = t->next;
	t->next = src;
}

struct op *cat_list(struct op *exp_1, struct op *exp_2) {
	struct op *stat_list = new_op();

	cat_tac(stat_list, exp_1->code);
	cat_tac(stat_list, exp_2->code);

	return stat_list;
}

struct op *cpy_op(const struct op *src) {
	struct op *nop = new_op();
	cat_tac(nop, src->code);
	if (src->addr != NULL) {
		nop->addr = src->addr;
	}
	return nop;
}

struct op *new_op() {
	struct op *nop;
	MALLOC_AND_SET_ZERO(nop, 1, struct op);
	return nop;
}

struct tac *new_tac(int type, struct id *id_1, struct id *id_2,
					struct id *id_3) {
	struct tac *ntac = (struct tac*)malloc(sizeof(struct tac));

	ntac->type = type;
	ntac->next = NULL;
	ntac->prev = NULL;
	ntac->id_1 = id_1;
	ntac->id_2 = id_2;
	ntac->id_3 = id_3;

	return ntac;
}

struct id *new_temp() {
	BUF_ALLOC(buf);
	sprintf(buf, "t%d", temp_amount++);
	return add_identifier(buf, INT_TEMP);
}

const char *to_str(struct id *id) {
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