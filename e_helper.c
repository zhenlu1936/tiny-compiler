#include "e_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct id identifiers[MAX];
int identifiers_amount;
int num_amount;
int temp_amount;
int label_amount;

int find_identifier(const char* name, int add, int type) {
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
	return id;
}

char* cat_tac(char* src_1, const char* src_2) {
	return strcat(src_1, src_2);
}

char* cpy_tac(char* dest, const char* src) {
	return 	strcpy(dest, src);
}

struct op* new_op() {
	struct op* nop;
	MALLOC_AND_SET(nop, 1,struct op);
	MALLOC_AND_SET(nop->tac, BUF_SIZE,char);
	return nop;
}

int new_temp() {
	BUF_ALLOC(buf);
	sprintf(buf, "t%d", temp_amount++);
	return find_identifier(buf, ADD, INT_TEMP);
}

struct op* calculate(struct op* exp_1, struct op* exp_2, const char* cal) {
	struct op* exp = new_op();
	cat_tac(exp->tac, exp_1->tac);
	cat_tac(exp->tac, exp_2->tac);
	exp->addr = new_temp();
	BUF_ALLOC(buf);
	sprintf(buf, "%s = %s %s %s\n", identifiers[exp->addr].name,
			identifiers[exp_1->addr].name, cal, identifiers[exp_2->addr].name);
	cat_tac(exp->tac, buf);
	return exp;
}

char* format_string(const char* input) {
	BUF_ALLOC(buf);
	strcpy(buf, input);
	buf[strlen(buf) - 1] = 0;
	char* src = (char*)buf + 1;

	char* dest = (char*)malloc(MAX * sizeof(char));
	const char* p = src;
	char* q = dest;
	while (*p) {
		if (*p == '\\') {
			p++;
			switch (*p) {
				case 'n':
					*q++ = '\n';
					break;
				case 't':
					*q++ = '\t';
					break;
				case 'r':
					*q++ = '\r';
					break;
				case '\\':
					*q++ = '\\';
					break;
				case '"':
					*q++ = '"';
					break;
				default:
					*q++ = *p;
					break;
			}
			p++;
		} else {
			*q++ = *p++;
		}
	}

	return dest;
}