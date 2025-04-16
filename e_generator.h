struct op* process_calculate(struct op* exp_1, struct op* exp_2, const char* cal);

struct op* process_for(struct op* exp_1, struct op* exp_2, struct op* exp_3, struct op* exp_4);

struct op* process_while(struct op* exp_1, struct op* exp_2);

struct op* process_if_only(struct op* exp_1, struct op* exp_2);

struct op* process_if_else(struct op* exp_1, struct op* exp_2,
	struct op* exp_3);