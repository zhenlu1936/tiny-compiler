#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_tac.h"

extern int yyparse();
extern FILE *yyin;

// void tac_list() {
// 	out_str(file_x, "\n# tac list\n\n");

// 	struct tac *cur;
// 	for (cur = tac_head; cur != NULL; cur = cur->next) {
// 		out_str(file_x, "%p\t", cur);
// 		out_tac(file_x, cur);
// 		out_str(file_x, "\n");
// 	}
// }

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s filename\n", argv[0]);
		exit(0);
	}

	if ((yyin = fopen(argv[1], "r")) == NULL) {
		printf("open file %s failed\n", argv[1]);
		exit(0);
	}

	FILE *tac_file = fopen("tac.txt", "w");
	if (tac_file == NULL) {
		perror("failed to open file");
	}

	tac_init();
	yyparse();
	tac_output(tac_file, tac_head);

	fclose(yyin);

	return 0;
}