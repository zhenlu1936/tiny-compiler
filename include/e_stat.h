struct op *process_variable_list_end(char*name);

struct op *process_variable_list(struct op *exp_1, char *name);

struct op *process_assign(char *name, struct op *exp_1);

struct op *process_input(char *name);

struct op *process_output(char *name);

struct op *process_call(char *name, struct op *exp_1);

struct op *process_return(struct op *exp_1);

struct op *process_for(struct op *exp_1, struct op *exp_2, struct op *exp_3, struct op *exp_4);

struct op *process_while(struct op *exp_1, struct op *exp_2);

struct op *process_if_only(struct op *exp_1, struct op *exp_2);

struct op *process_if_else(struct op *exp_1, struct op *exp_2,
	struct op *exp_3);