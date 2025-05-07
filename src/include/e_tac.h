// 常量定义
#define MAX 100          	// 最大值
#define BUF_SIZE 64      	// 缓冲区大小
#define NAME_SIZE 256    	// 名称字符串的最大长度

#define NO_ADDR -1       	// 无地址标识

// 符号表操作方向
#define INC_HEAD 0       	// 增加到表头
#define INC_TAIL 1       	// 增加到表尾
#define DEC_HEAD 2       	// 从表头减少
#define DEC_TAIL 3       	// 从表尾减少

// 符号表范围
#define LOCAL_TABLE 1    	// 局部符号表
#define GLOBAL_TABLE 0   	// 全局符号表
#define INTO_LOCAL_TABLE LOCAL_TABLE  // 进入局部符号表
#define OUT_LOCAL_TABLE GLOBAL_TABLE  // 退出局部符号表

// 符号类型
#define NO_TYPE -1       	// 无类型
#define ID_VAR 0         	// 变量
#define ID_FUNC 1        	// 函数
#define ID_TEMP 2        	// 临时变量
#define ID_NUM 3         	// 数字常量
#define ID_LABEL 4       	// 标签
#define ID_STRING 5      	// 字符串

// 数据类型
#define NO_DATA -1       	// 无数据类型
#define DATA_INT 0       	// 整型
#define DATA_LONG 1      	// 长整型
#define DATA_FLOAT 2     	// 浮点型
#define DATA_DOUBLE 3    	// 双精度浮点型

// 三地址码类型
#define TAC_END 0        	// 结束
#define TAC_LABEL 1      	// 标签
#define TAC_BEGIN 2      	// 函数开始
#define TAC_PARAM 3      	// 参数
#define TAC_VAR 4        	// 变量声明
#define TAC_IFZ 5        	// 条件跳转（if not）
#define TAC_CALL 6       	// 函数调用
#define TAC_RETURN 7     	// 返回
#define TAC_OUTPUT 8     	// 输出
#define TAC_INPUT 9      	// 输入
#define TAC_ASSIGN 10    	// 赋值
#define TAC_NEGATIVE 12  	// 取负
#define TAC_INTEGER 13   	// 整数常量
#define TAC_IDENTIFIER 14	// 标识符
#define TAC_ARG 15		 	// 函数参数
#define TAC_GOTO 16      	// 无条件跳转
#define TAC_PLUS 20      	// 加法
#define TAC_MINUS 21     	// 减法
#define TAC_MULTIPLY 22  	// 乘法
#define TAC_DIVIDE 23    	// 除法
#define TAC_EQ 24        	// 等于
#define TAC_NE 25        	// 不等于
#define TAC_LT 26        	// 小于
#define TAC_LE 27        	// 小于等于
#define TAC_GT 28        	// 大于
#define TAC_GE 29        	// 大于等于

#define BUF_ALLOC(buf) char buf[BUF_SIZE] = {0};

#define NAME_ALLOC(name) char name[NAME_SIZE] = {0};

// #define NEW_LABEL(label) \
// 	NAME_ALLOC(label##_name); \
// 	sprintf(label##_name,"label_%d",label_amount++); \
// 	struct id *label = add_identifier(label##_name, ID_LABEL);

#define NEW_TAC_0(type) new_tac(type, NULL, NULL, NULL)

#define NEW_TAC_1(type, id_1) new_tac(type, id_1, NULL, NULL)

#define NEW_TAC_2(type, id_1, id_2) new_tac(type, id_1, id_2, NULL)

#define NEW_TAC_3(type, id_1, id_2, id_3) new_tac(type, id_1, id_2, id_3)

#define MALLOC_AND_SET_ZERO(pointer, len, type)     \
	pointer = (type *)malloc((len) * sizeof(type)); \
	memset(pointer, 0, (len) * sizeof(type));

#include <stdio.h>

// 符号
struct id {
	const char *name;
	union {
		int num_int;
		double num_float;
	} num;
	int id_type;
	int data_type;
	struct id *next;
};

// 三地址码
struct tac {
	int type;
	struct tac *prev;
	struct tac *next;
	struct id *id_1;
	struct id *id_2;
	struct id *id_3;
};

// 表达式
struct op {
	struct tac *code;
	struct id *addr;
};

// // 变量声明
// struct var_list {
// 	struct id *var;
// 	struct var_list *next;
// };

extern int scope;
extern struct tac *tac_head;
extern struct tac *tac_tail;

// 符号表
void reset_table(int direction);
// void clear_table(int scope);
struct id *find_identifier(const char *name);
struct id *find_func(const char *name);
struct id *add_identifier(const char *name, int id_type, int data_type);

// 三地址码表
void tac_init();
void cat_tac(struct op *dest, struct tac *src);
void cat_op(struct op *dest, struct op *src);			  // 会释放src
struct op *cat_list(struct op *exp_1, struct op *exp_2);  // 会释放exp_2
struct op *cpy_op(struct op *src);

// 初始化
struct op *new_op();
struct tac *new_tac(int type, struct id *id_1, struct id *id_2,
					struct id *id_3);
struct id *new_temp();
struct id *new_label();

// 字符串处理
const char *id_to_str(struct id *id);
const char *data_to_str(int type);
void tac_output(FILE *f, struct tac *code);