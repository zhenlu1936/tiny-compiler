struct op* process_expression_list_end(struct op* exp_1);

struct op* process_expression_list(struct op* exp_1, struct op* exp_2);

struct op* process_inc(char* name, int pos);

struct op* process_dec(char* name, int pos);

struct op* process_calculate(struct op* exp_1, struct op* exp_2, int cal);

struct op* process_negative(struct op* exp_1);

struct op* process_integer(int integer);

struct op* process_identifier(char* name);
