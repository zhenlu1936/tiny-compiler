#include "e_tac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int scope;

struct id *id_global, *id_local;
static int temp_amount;
static int label_amount;

static struct id *_find_identifier(const char *name, struct id **id_table,
								   int check) {
	int has_finded = 0;
	struct id *id_wanted = NULL;
	struct id *cur = *id_table;

	while (cur) {
		if (cur->name && !strcmp(name, cur->name)) {
			has_finded = 1;
			id_wanted = cur;
			break;
		}
		cur = cur->next;
	}
	if (!has_finded && check == CHECK_ID_EXIST) {
		perror("identifier not found");
		printf("want name: %s\n", name);
	}
	return id_wanted;
}

static struct id **_choose_id_table(int table) {
	if (table == GLOBAL_TABLE) {
		return &id_global;
	} else {
		return &id_local;
	}
}

static struct id *_collide_identifier(const char *name, int id_type) {
	if (id_type == ID_STRING)
		return _find_identifier(name, _choose_id_table(GLOBAL_TABLE),
								CHECK_ID_NOT_EXIST);
	else
		return _find_identifier(name, _choose_id_table(scope),
								CHECK_ID_NOT_EXIST);
}

static struct id *_add_identifier(const char *name, int id_type, int data_type,
								  struct id **id_table) {
	struct id *id_wanted;

	struct id *id_collision = _collide_identifier(name, id_type);
	if (id_collision) {
		if (ID_IS_CONST(id_collision)) {
			return id_collision;
		} else {
			perror("identifier declared");
			printf("add name: %s\n", name);
			return NULL;
		}
	}

	MALLOC_AND_SET_ZERO(id_wanted, 1, struct id);
	char *id_name = (char *)malloc(sizeof(char) * strlen(name));
	strcpy(id_name, name);
	id_wanted->name = id_name;
	id_wanted->id_type = id_type;
	id_wanted->data_type = data_type;
	id_wanted->next = *id_table;
	*id_table = id_wanted;
	id_wanted->offset = -1; /* Unset address */
	if (id_type == ID_STRING) {
		id_wanted->label = label_amount++;
	}

	return id_wanted;
}

struct id *find_identifier(const char *name) {
	return _find_identifier(name, _choose_id_table(scope), CHECK_ID_EXIST);
}

struct id *find_func(const char *name) {
	return _find_identifier(name, _choose_id_table(GLOBAL_TABLE),
							CHECK_ID_EXIST);
}

struct id *add_identifier(const char *name, int id_type, int data_type) {
	if (id_type == ID_STRING)
		return _add_identifier(name, id_type, data_type,
							   _choose_id_table(GLOBAL_TABLE));
	else
		return _add_identifier(name, id_type, data_type,
							   _choose_id_table(scope));
}

void init_tac() {
	scope = GLOBAL_TABLE;
	id_global = NULL;
	id_local = NULL;
	temp_amount = 1;
	label_amount = 1;
}

void reset_table(int direction) {
	struct id **table = _choose_id_table(scope);
	if (direction == INTO_LOCAL_TABLE) {
		scope = LOCAL_TABLE;
	} else if (direction == OUT_LOCAL_TABLE) {
		*table = NULL;
		scope = GLOBAL_TABLE;
	}
}

// void clear_table(int scope) {
// 	struct id **table = _choose_id_table(scope);
// 	struct id *head = *table;
// 	struct id *cur = head;
// 	while (head) {
// 		head = cur->next;
// 		free(cur);
// 		cur = head;
// 	}
// 	*table = NULL;
// }

void cat_tac(struct op *dest, struct tac *src) {
	struct tac *t = dest->code;
	if (t == NULL) {
		dest->code = src;
	} else {
		while (t->next != NULL) t = t->next;
		t->next = src;
	}
}

// 和cat_tac不同之处在于，释放了作为struct op的src
void cat_op(struct op *dest, struct op *src) {
	cat_tac(dest, src->code);
	// free(src); // hjj: free会导致continue和break出错，无法捕捉需要parse的op
}

struct op *cat_list(struct op *exp_1, struct op *exp_2) {
	struct op *stat_list = new_op();

	cat_op(stat_list, exp_1);
	cat_op(stat_list, exp_2);

	return stat_list;
}

// static struct op *cpy_op(const struct op *src) {
// 	struct op *nop = new_op();
// 	cat_tac(nop, src->code);
// 	if (src->addr != NULL) {
// 		nop->addr = src->addr;
// 	}
// 	return nop;
// }

// 目前来看，并不需要复制再释放的操作，只需要把指针本身复制给dest
struct op *cpy_op(struct op *src) {
	// struct op *nop = cpy_op(src);
	// free(src);
	// return nop;
	return src;
}

struct op *new_op() {
	struct op *nop;
	MALLOC_AND_SET_ZERO(nop, 1, struct op);
	return nop;
}

struct tac *new_tac(int type, struct id *id_1, struct id *id_2,
					struct id *id_3) {
	struct tac *ntac = (struct tac *)malloc(sizeof(struct tac));

	ntac->type = type;
	ntac->next = NULL;
	ntac->prev = NULL;
	ntac->id_1 = id_1;
	ntac->id_2 = id_2;
	ntac->id_3 = id_3;

	return ntac;
}

struct id *new_temp(int data_type) {
	NAME_ALLOC(buf);
	sprintf(buf, "t%d", temp_amount++);
	return add_identifier(buf, ID_TEMP, data_type);
}

struct id *new_label() {
	NAME_ALLOC(label);
	sprintf(label, "label_%d", label_amount++);
	return add_identifier(label, ID_TEMP, NO_DATA);
}

struct block *new_block(struct id *l_begin, struct id *l_end) {
	struct block *nstack;
	MALLOC_AND_SET_ZERO(nstack, 1, struct block);
	nstack->label_begin = l_begin;
	nstack->label_end = l_end;
	return nstack;
}

const char *id_to_str(struct id *id) {
	if (id == NULL) return "NULL";

	switch (id->id_type) {
		case ID_VAR:
		case ID_NUM:
		case ID_FUNC:
		case ID_TEMP:
		case ID_LABEL:
		case ID_STRING:
			return id->name;

		default:
			perror("unknown TAC arg type");
			return "?";
	}
}

const char *data_to_str(int type) {
	if (type == NO_TYPE) return "NULL";

	switch (type) {
		case DATA_INT:
			return "int";
		case DATA_LONG:
			return "long";
		case DATA_FLOAT:
			return "float";
		case DATA_DOUBLE:
			return "double";

		default:
			perror("unknown data type");
			printf("id type: %d\n", type);
			return "?";
	}
}

void output_tac(FILE *f, struct tac *code) {
	// while (code) {
	switch (code->type) {
		case TAC_PLUS:
			fprintf(f, "%s = %s + %s\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_MINUS:
			fprintf(f, "%s = %s - %s\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_MULTIPLY:
			fprintf(f, "%s = %s * %s\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_DIVIDE:
			fprintf(f, "%s = %s / %s\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_EQ:
			fprintf(f, "%s = (%s == %s)\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_NE:
			fprintf(f, "%s = (%s != %s)\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_LT:
			fprintf(f, "%s = (%s < %s)\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_LE:
			fprintf(f, "%s = (%s <= %s)\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_GT:
			fprintf(f, "%s = (%s > %s)\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_GE:
			fprintf(f, "%s = (%s >= %s)\n", id_to_str(code->id_1),
					id_to_str(code->id_2), id_to_str(code->id_3));
			break;

		case TAC_NEGATIVE:
			fprintf(f, "%s = - %s\n", id_to_str(code->id_1),
					id_to_str(code->id_2));
			break;

		case TAC_ASSIGN:
			fprintf(f, "%s = %s\n", id_to_str(code->id_1),
					id_to_str(code->id_2));
			break;

		case TAC_GOTO:
			fprintf(f, "goto %s\n", code->id_1->name);
			break;

		case TAC_IFZ:
			fprintf(f, "ifz %s goto %s\n", id_to_str(code->id_1),
					code->id_2->name);
			break;

		case TAC_ARG:
			fprintf(f, "arg %s\n", id_to_str(code->id_1));
			break;

		case TAC_PARAM:
			fprintf(f, "param %s\n", id_to_str(code->id_1));
			break;

		case TAC_CALL:
			if (code->id_1 == NULL)
				fprintf(f, "call %s\n", (char *)code->id_2);
			else
				fprintf(f, "%s = call %s\n", id_to_str(code->id_1),
						id_to_str(code->id_2));
			break;

		case TAC_INPUT:
			fprintf(f, "input %s\n", id_to_str(code->id_1));
			break;

		case TAC_OUTPUT:
			fprintf(f, "output %s\n", id_to_str(code->id_1));
			break;

		case TAC_RETURN:
			fprintf(f, "return %s\n", id_to_str(code->id_1));
			break;

		case TAC_LABEL:
			fprintf(f, "label %s\n", id_to_str(code->id_1));
			break;

		case TAC_VAR:
			fprintf(f, "var %s %s\n", data_to_str(code->id_1->data_type),
					id_to_str(code->id_1));
			break;

		case TAC_BEGIN:
			fprintf(f, "begin\n");
			break;

		case TAC_END:
			fprintf(f, "end\n\n");
			break;

		default:
			perror("unknown TAC opcode");
			break;
	}
	// fflush(f);
	// code = code->next;
	// }
}

void source_to_tac(FILE *f, struct tac *code) {
	while (code) {
		output_tac(f, code);
		code = code->next;
	}
}

void input_str(FILE *f, const char *format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(f, format, args);
	va_end(args);
}