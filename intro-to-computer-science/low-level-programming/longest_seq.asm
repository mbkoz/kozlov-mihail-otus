%include 'library.asm'

section .data
    arr db  1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
    n   equ $-arr

section .text
    global _start
    
_start:
    xor edx, edx    ; в edx храним самый длиный отрезок
    mov ebx, arr
    mov ecx, n
    cmp ecx, 0x00
    je final

count_ones:
    xor eax, eax    ; в eax накапливаем данные о длине отрезка
count_ones_loop:    ; считаем 1, пока не упремся в 0
    cmp byte [ebx + ecx - 1], 0x01
    jne skip_zeroes
    inc eax
    loop count_ones_loop
    jmp final   ; если вывалились сюда, то массив закончился и пора подвести итоги

skip_zeroes:        ; проверим, как много 1 насчитали
    cmp eax, edx
    cmovg edx, eax

skip_zeroes_loop:    ; пропускаем 0, пока не найдем 1
    cmp byte [ebx + ecx - 1], 0x00
    jne count_ones
    loop skip_zeroes_loop

final:
    cmp eax, edx
    cmovl eax, edx

    call print_number
    call exit