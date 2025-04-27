## 语法规则

和semantic一致，但多出了
1. for循环，且循环各条件表达式可为空。
1. 自增和自减，位置可在变量之前或之后。
1. 类型int，long，float和double。

## 已实现的部分

实现了以上语法规则下的三地址码生成。

## 问题说明

### 区分全局变量和局部变量

为了区分全局变量和局部变量，语义分析器加入了scope变量。当分析器进入函数内部时，scope置为1；当分析器离开函数时，scope置为0。

### 变量定义的函数调用链

yacc中，定义变量的产生式在variable_list，调用了declare_var函数。该函数再调用mk_tac函数以生成三地址码，其中有mk_var(name)作为参数。mk_var函数根据scope，调用lookup_sym查找全局符号表或局部符号表，若符号表中已存在以name为名的变量则报错；然后调用mk_sym函数。分配新的符号，并为符号各属性赋值；最后根据scope，调用insert_sym向全局符号表或局部符号表插入符号。

### 参数列表的翻译

为了生成形参列表，产生式parameter_list调用了declare_para函数。该函数再调用mk_tac函数以生成三地址码，其中有mk_var(name)作为参数。

为了生成实参列表，产生式call_statement和call_expression会分别调用do_call函数和do_call_ret函数。两个函数均会整理arglist包含的三地址码，并生成arg指令，然后拼接在一起。不同之处在于，do_call函数最后会直接返回三地址码code；do_call_ret函数会生成储存返回值的ret符号，最后返回以ret为返回值、以code为三地址码的expression。

ppt生成形参的方式和semantic基本一致。ppt生成实参的方式并不区分statement和expression，而是直接生成一个临时变量t作为函数返回值的地址。

## 分工

黄家俊完成e_proc.c。
连怡超完成e_tac.c。
黄顺阳完成e.l，e.y，e_main.c。