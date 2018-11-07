compile:
	nasm -f elf64 grey-scale.asm
	ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o grey-scale -lc grey-scale.o

clean:
	rm grey-scale.o grey-scale

