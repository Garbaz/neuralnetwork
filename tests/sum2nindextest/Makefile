asm_sum2nindextest:
	gcc -O2 -g -c sum2nindextest.c
	objdump -S sum2nindextest.o

run_sum2nindextest:
	gcc -O2 -o sum2nindextest sum2nindextest.c
	./sum2nindextest 1 2 3  # 3 args => argc=4
	printf "$?\n" # Print return code
