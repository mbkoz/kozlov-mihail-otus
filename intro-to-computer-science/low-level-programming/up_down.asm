%include 'library.asm'

section .data
    arr dd  'u', 'd', 'd', 'u', 'u', 'd', 'd', 'u'
    n   dd  8

section .text
    global _start
    
_start:

    ; Ваш код

    call print_number
    call exit