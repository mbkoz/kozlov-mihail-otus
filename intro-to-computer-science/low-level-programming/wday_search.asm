%include 'library.asm'

section .data
    arr dd  17, 21, 13, 17, 14, 12
    n   dd  6

section .text
    global _start
    
_start:

    ; Ваш код
    ; Вам нужно будет вызвать print_number n раз

    call exit