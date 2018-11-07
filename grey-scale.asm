SYS_WRITE equ 1
SYS_EXIT  equ 60

        section .data
output  db `Hello World!\n`
output_len equ $-output
        
        section .text
        global _start
_start:
        mov rdi,output
        mov [rdi+28],ebx
        mov [rdi+32],edx
        mov [rdi+36],ecx
        mov eax,SYS_WRITE
        mov rdi,1               ;stdout
        mov rsi,output
        mov rdx,output_len
        syscall
        mov eax,SYS_EXIT
        mov rdi,0
        syscall

