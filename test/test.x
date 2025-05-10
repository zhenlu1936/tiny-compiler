var int global
label add
begin
param x
param y
t1 = x + y
return t1
end

label main
begin
var int a
var int b
var int i
var float c
a = 2
b = 1
c = 0.100000
i = 0
label label_1
t2 = (i < 5)
ifz t2 goto label_2
t4 = a
a = t4 - 1
arg t4
arg b
t5 = call add
b = t5
t3 = i
i = t3 + 1
goto label_1
label label_2
label label_3
t6 = (a == 1)
ifz t6 goto label_4
a = 1
goto label_3
label label_4
t7 = (c == b)
ifz t7 goto label_6
t8 = (a != b)
ifz t8 goto label_5
c = 5
label label_5
label label_6
return c
end

