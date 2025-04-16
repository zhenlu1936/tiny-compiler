#define MAX 100
#define BUF_SIZE 4096
#define NAME_SIZE 64

#define NO_ADDR -1

#define INC_HEAD 0
#define INC_TAIL 1
#define DEC_HEAD 2
#define DEC_TAIL 3

#define ADD 1
#define NOT_ADD 0

#define INT_VAR 0
#define INT_FUNC 1
#define INT_TEMP 2
#define INT_NUM 3
#define LABEL_IFZ 2

#define TAC_END 0
#define TAC_LABEL 1
#define TAC_BEGIN 2
#define TAC_PARAM 3
#define TAC_VAR 4
#define TAC_IFZ 5
#define TAC_CALL 6
#define TAC_RETURN 7
#define TAC_OUTPUT 8
#define TAC_INPUT 9
#define TAC_ASSIGN 10
#define TAC_NEGATIVE 12
#define TAC_INTEGER 13
#define TAC_IDENTIFIER 14
#define TAC_ARG 15
#define TAC_GOTO 16

#define TAC_PLUS 20
#define TAC_MINUS 21
#define TAC_MULTIPLY 22
#define TAC_DIVIDE 23
#define TAC_EQ 24
#define TAC_NE 25
#define TAC_LT 26
#define TAC_LE 27
#define TAC_GT 28
#define TAC_GE 29

#define BUF_ALLOC(buf) char buf[BUF_SIZE] = {0};

#define NEW_TAC_0(code, type) struct tac* code = new_tac(type, NULL, NULL, NULL)

#define NEW_TAC_1(code, type, id_1) \
	struct tac* code = new_tac(type, id_1, NULL, NULL)

#define NEW_TAC_2(code, type, id_1, id_2) \
	struct tac* code = new_tac(type, id_1, id_2, NULL)

#define NEW_TAC_3(code, type, id_1, id_2, id_3) \
	struct tac* code = new_tac(type, id_1, id_2, id_3)

#define NAME_ALLOC(name) char name[NAME_SIZE] = {0};

#define MALLOC_AND_SET(pointer, len, type)         \
	pointer = (type*)malloc((len) * sizeof(type)); \
	memset(pointer, 0, (len) * sizeof(type));

struct id {
	const char* name;
	int num;
	int addr;
	int type;
};

struct tac {
	int type;
	struct tac* prev;
	struct tac* next;
	struct id* id_1;
	struct id* id_2;
	struct id* id_3;
};

struct op {
	struct tac* code;
	int addr;
};

struct id* find_identifier(const char* name, int add, int type);

void cat_tac(struct op* src_1, struct tac* src_2);

struct op* cat_list(struct op* exp_1, struct op* exp_2);

struct op* cpy_op(const struct op* src);

struct op* new_op();

struct tac* new_tac(int type, struct id* id_1, struct id* id_2,
					struct id* id_3);

struct id* new_temp();

const char* to_str(struct id* id);