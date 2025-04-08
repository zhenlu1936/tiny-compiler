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
		identifiers[id].name = malloc(sizeof(char)*strlen(name));
		strcpy(identifiers[id].name, name);
		identifiers[id].type = type;
		identifiers[id].addr = id;
	}
	if (id == MAX) {
		perror("identifier not found");
	}
	// printf("here2!\n");
	return id;
}

char* cat_tac(const char* src_1, const char* src_2) {
	char* dest;
	if (src_1 == 0 && src_2 != 0) {
		// printf("HEY!\n");
		dest = malloc(strlen(src_2) * sizeof(char));
		strcat(dest, src_2);
	} else if (src_1 != 0 && src_2 == 0) {
		dest = malloc(strlen(src_1) * sizeof(char));
		strcat(dest, src_1);
	} else if (src_1 != 0 && src_2 != 0) {
		dest =
			malloc(strlen(src_1) * sizeof(char) + strlen(src_2) * sizeof(char));
		strcat(dest, src_1);
		strcat(dest, src_2);
	} else {
		dest = malloc(4 * sizeof(char));
	}
	return dest;
}

char* copy_tac(const char* src) {
	char* dest = malloc(strlen(src) * sizeof(char));
	strcpy(dest, src);
	return dest;
}

struct op* new_op() {
	struct op* nop = malloc(sizeof(struct op));
	return nop;
}

int new_temp() {
	BUF_ALLOC(buf);
	sprintf(buf, "t%d", temp_amount++);
	return find_identifier(buf, ADD, INT_TEMP);
}

struct op* calculate(struct op* exp_1, struct op* exp_2, const char* cal) {
	struct op* exp = new_op();
	exp->tac = cat_tac(exp_1->tac, exp_2->tac);
	exp->addr = new_temp();
	char buf[BUF_SIZE];
	sprintf(buf, "%s = %s %s %s\n", identifiers[exp->addr].name,
			identifiers[exp_1->addr].name, cal, identifiers[exp_2->addr].name);
	exp->tac = cat_tac(exp->tac, buf);
}

char* format_string(const char* input){
	BUF_ALLOC(buf);
	strcpy(buf,input);
	buf[strlen(buf)-1] = 0;
	char* src = (char*)buf+1;

	char* dest = malloc(MAX*sizeof(char));
	const char* p = src;
    char* q = dest;
	while (*p) {
        if (*p == '\\') {
            p++;
            switch (*p) {
                case 'n':  *q++ = '\n'; break;
                case 't':  *q++ = '\t'; break;
                case 'r':  *q++ = '\r'; break;
                case '\\': *q++ = '\\'; break;
                case '"':  *q++ = '"';  break;
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