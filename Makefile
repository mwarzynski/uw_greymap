compile:
	nasm -f elf64 grey-scale.asm
	gcc -Wall -Wextra grey-scale.c grey-scale.o -o grey-scale

clean:
	rm grey-scale.o grey-scale

