%include 'library.asm'

section .data
    arr db  'u', 'd', 'd', 'u', 'u', 'd', 'd', 'u'
    n   equ $-arr

section .text
    global _start
    
_start:

    ; Ваш код
    mov eax, n ; если в конце в eax останется число == n, то этаж остался тот же
    mov ebx, arr
    mov ecx, n
    cmp ecx, 0x00       ; если количество итераций =0, то перенесемся к оценке результата
    je after_loop

start_loop:
    cmp byte [ebx + ecx - 1], 'u'
    jne else
    inc eax
    jmp end_loop
else:           ; считаем, что у нас есть только 'u' и 'd' в входной последовательности
    dec eax
end_loop:
    loop start_loop

after_loop:
    cmp eax, n
    je same_floor
    mov eax, 0x00
    jmp output

same_floor:
    mov eax, 0x01

output:
    call print_number
    call exit