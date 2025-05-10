	# head
	LOD R2,STACK
	STO (R2),0
	LOD R4,EXIT
	STO (R2+4),R4

	# var int global


	# label add

add:

	# begin


	# param x


	# param y


	# t1 = x + y

	LOD R5,(R2-4)
	LOD R6,(R2-8)
	ADD R5,R6

	# return t1

	LOD R4,STATIC
	STO (R4+4294967295),R5
	LOD R3,(R2+4)
	LOD R2,(R2)
	JMP R3

	# end


	LOD R3,(R2+4)
	LOD R2,(R2)
	JMP R3

	# label main

main:

	# begin


	# var int a


	# var int b


	# var int i


	# var float c


	# a = 2

	LOD R5,2

	# b = 1

	LOD R6,1

	# c = 0.100000

	LOD R7,2576980378

	# i = 0

	LOD R8,0

	# label label_1

	STO (R2+8),R5
	STO (R2+12),R6
	STO (R2+20),R7
	STO (R2+16),R8
label_1:

	# t2 = (i < 5)

	LOD R5,(R2+16)
	LOD R6,5
	SUB R5,R6
	TST R5
	LOD R3,R1+40
	JLZ R3
	LOD R5,0
	LOD R3,R1+24
	JMP R3
	LOD R5,1

	# ifz t2 goto label_2

	LOD R4,STATIC
	STO (R4+4294967295),R5
	TST R7
	JEZ t2

	# t4 = a

	LOD R8,(R2+8)

	# a = t4 - 1

	LOD R9,1
	SUB R8,R9

	# arg t4

	STO (R2+24),R10

	# arg b

	LOD R11,(R2+12)
	STO (R2+28),R11

	# t5 = call add

	STO (R2+8),R8
	STO (R2+32),R2
	LOD R4,R1+32
	STO (R2+36),R4
	LOD R2,R2+32
	JMP 0\à0
	LOD R5,R4

	# b = t5


	# t3 = i

	LOD R6,(R2+16)

	# i = t3 + 1

	LOD R7,1
	ADD R6,R7

	# goto label_1

	STO (R2+12),R5
	STO (R2+16),R6
	JMP label_1

	# label label_2

label_2:

	# label label_3

label_3:

	# t6 = (a == 1)

	LOD R5,(R2+8)
	LOD R6,1
	SUB R5,R6
	TST R5
	LOD R3,R1+40
	JEZ R3
	LOD R5,0
	LOD R3,R1+24
	JMP R3
	LOD R5,1

	# ifz t6 goto label_4

	LOD R4,STATIC
	STO (R4+4294967295),R5
	TST R7
	JEZ t6

	# a = 1


	# goto label_3

	STO (R2+8),R6
	JMP label_3

	# label label_4

label_4:

	# t7 = (c == b)

	LOD R5,(R2+20)
	LOD R6,(R2+12)
	SUB R5,R6
	TST R5
	LOD R3,R1+40
	JEZ R3
	LOD R5,0
	LOD R3,R1+24
	JMP R3
	LOD R5,1

	# ifz t7 goto label_6

	LOD R4,STATIC
	STO (R4+4294967295),R5
	TST R7
	JEZ t7

	# t8 = (a != b)

	LOD R8,(R2+8)
	SUB R8,R6
	TST R8
	LOD R3,R1+40
	JEZ R3
	LOD R8,1
	LOD R3,R1+24
	JMP R3
	LOD R8,0

	# ifz t8 goto label_5

	LOD R4,STATIC
	STO (R4+4294967295),R8
	TST R9
	JEZ t8

	# c = 5

	LOD R10,5

	# label label_5

	STO (R2+20),R10
label_5:

	# label label_6

label_6:

	# return c

	LOD R4,(R2+20)
	LOD R3,(R2+4)
	LOD R2,(R2)
	JMP R3

	# end


	LOD R3,(R2+4)
	LOD R2,(R2)
	JMP R3

	# tail
EXIT:
	END
STATIC:
	DBN 0,4
STACK:
