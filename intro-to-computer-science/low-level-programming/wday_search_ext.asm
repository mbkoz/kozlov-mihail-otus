; передача аргументов в функции упорядочена edi, esi, edx, ecx, r8, r9

%include 'library.asm'

%macro _push_abcd 0
    push eax
    push ebx
    push ecx
    push edx
%endmacro

%macro _pop_abcd 0
    pop edx
    pop ecx
    pop ebx
    pop eax
%endmacro


%macro _print_string_macro 1
    mov edi, %1
    mov esi, %1 %+ _sz
    call _print_string
%endmacro

%macro _read_into_string_macro 1
    mov edi, %1
    mov esi, %1 %+ _sz
    call _read_string
%endmacro

%macro _atoi_macro 1
    mov edi, %1
    call _atoi
%endmacro

section .data
    promt db 'Enter number of days: '
    promt_sz equ $-promt

    next_inp db 'Enter next T value: '
    next_inp_sz equ $-next_inp

    result_promt db 'nearest warm T: '
    result_promt_sz equ $-result_promt

section .bss
    align       4
    read_buffer resb 12 ;буффер длясчитанных данных
    read_buffer_sz equ $ - read_buffer - 1 ; -1 для терминирующего символа

section .text
    global _start
    
_start:
    ; сохраним количество итераций в задаче
    _print_string_macro    promt
    call _read_int
    mov ecx, edi
    ; сохраним в вершину стека самую горячую xD температуру (se0xFF)
    push -1

    cmp ecx, 0
    je final

gen_next_temp:
    _print_string_macro     next_inp
    call _read_int

search_wd:
    pop eax
    cmp eax, edi
    ja print_temp  ; используем "беззнаквый" режим сравнения (if above)
    jmp search_wd

print_temp:
    mov edx, edi
    _print_string_macro     result_promt
    call print_number
    push eax
    push edx
    loop gen_next_temp

final:
    call exit

; функция, печатает строку по адресу edi, длиной esi
_print_string:
    _push_abcd
    mov ecx, edi
    mov edx, esi
    mov ebx, 1
    mov eax, 4
    int 80h
    _pop_abcd
    ret

; функция, читает строку и плмещает по адресу edi, длиной не более esi (с учетом LF)
_read_string:
    _push_abcd
    mov ecx, edi
    mov edx, esi
    mov ebx, 0
    mov eax, 3
    int 80h
    _pop_abcd
    ret

; ковертирует строку по адресу edi в число в esi до символа не цифры (не работает для "-" чисел)
_atoi:
    push eax
    xor esi, esi

cvn_start:
    movzx eax, byte [edi]
    cmp al, '0'
    jl cvn_end
    cmp al, '9'
    jg cvn_end
    imul esi, 10
    sub eax, '0'
    add esi, eax
    inc edi
    jmp cvn_start
cvn_end:

    pop eax
    ret

; функция использует внешний статический буфер для чтения строки и потом конвертирует ее в число в edi
_read_int:
    push esi
    _read_into_string_macro read_buffer
    call _atoi
    mov edi, esi
    pop esi
    ret


