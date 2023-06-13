%include 'library.asm'

section .data
    arr dd  0, 0, 1, 0, 1, 1, 1, 0
    n   dd  8

section .text
    global _start
    
_start:

    ; Ваш код

    call print_number
    call exit