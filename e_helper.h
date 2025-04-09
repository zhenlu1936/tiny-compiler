#define MAX 100
#define BUF_SIZE 2048
#define NAME_SIZE 64

#define ADD 1
#define NOT_ADD 0

#define INT_VAR 0
#define INT_FUNC 1
#define INT_TEMP 2
#define INT_NUM 3
#define LABEL_IFZ 2

#define BUF_ALLOC(buf)                            \
	char buf[BUF_SIZE] = {0};                       \

#define NAME_ALLOC(name)                            \
	char name[NAME_SIZE] = {0};                       \

#define MALLOC_AND_SET(pointer,len,type)                        \
	pointer = (type*)malloc((len)*sizeof(type));    \
    memset(pointer,0,(len)*sizeof(type));                    \


struct id{
    const char* name;
    int addr;
    int type;
    int num;
};

struct op{
    char* tac;
    int addr;
};

int find_identifier(const char* name,int add,int type);

char* cat_tac(char* src_1, const char* src_2);

char* cpy_tac(char* dest, const char* src);

struct op* new_op();

int new_temp();

struct op* calculate(struct op* exp_1, struct op* exp_2, const char* cal);

char* format_string(const char* input);