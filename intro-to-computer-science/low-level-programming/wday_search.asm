%include 'library.asm'

section .data
    arr dd  17, 21, 13, 17, 17, 12
    n   equ ($-arr) / 4

section .bss
align       4
stack       resb 1024   ; для опытов со стеком
stack_top   equ $

section .text
    global _start
    
_start:
    ; инициализация регистров и стека
    xor eax, eax
    xor edx, edx
    mov ecx, n
    mov ebx, arr
    ; инициализация стека
    mov esp, stack_top
    push -1;    получаетя, что -1 - самая жаркая погода SE(0xFF)

    cmp ecx, 0x00
    je final

; отматываем с вершины стека значения температур, пока не найлем более теплый день
search_wd:
    pop eax
    cmp eax, [ebx]
    ja print_temp  ; используем "беззнаквый" режим сравнения (if above)
    jmp search_wd
; выводим найденный "более теплый" день и сохраняем новое значение температуры в стек
print_temp:
    call print_number
    push eax
    mov eax, [ebx]
    push eax
    add ebx, 4
    loop search_wd
final:
    call exit