section .text
    global grey_scale:

; Arguments:
;   rdi - address of source matrix
;   rsi - address of results matrix
;   rdx - number of columns
;   rcx - number of lines
;   r8  - address to [3] color variables to determine the weighted average

grey_scale:
    ; Determine number of iterations = columns * lines.
    ; Store the result in the rcx.
    mov rax, rdx
    mul rcx
    mov rcx, rax

    ; Loop rcx times.
compute:
    ; Red.
    mov rax, 0
    mov al, BYTE[rdi]
    mul BYTE[r8]
    mov rdx, rax
    inc rdi

    ; Green.
    mov rax, 0
    mov al, BYTE[rdi]
    mul BYTE[r8+1]
    mov r9, rax
    inc rdi

    ; Blue.
    mov rax, 0
    mov al, BYTE[rdi]
    mul BYTE[r8+2]
    mov r10, rax
    inc rdi

    ; Add three colors ...
    mov rax, rdx
    add rax, r9
    add rax, r10
    ; Divide the sum by 0x100.
    mov r10, 0x100
    mov rdx, 0
    div r10d

    ; Store the color in the result array.
    mov WORD[rsi], ax
    inc rsi

    loop compute
    ret

