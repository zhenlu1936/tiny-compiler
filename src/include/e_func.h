struct op *process_program(struct op *exp_1);
struct op *process_function(struct op *exp_1, struct op *exp_2,
							struct op *exp_3);
struct op *process_function_head(int data_type, char *name);
struct op *process_parameter_list_end(int data_type, char *name);
struct op *process_parameter_list(struct op *exp_1, int data_type, char *name);