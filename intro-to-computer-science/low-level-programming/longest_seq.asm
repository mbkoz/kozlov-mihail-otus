%include 'library.asm'

section .data
    arr db  0, 0, 1, 0, 1, 1, 1, 0
    n   db  8

section .text
    global _start
    
_start:
    xor eax, eax    ; в eax накапливаем данные о длине отрезка
    xor edx, edx    ; в edx храним самый длиный отрезок
    mov ebx, arr
    movsx ecx, byte [n]
    cmp ecx, 0x00   ; если длина списка = 0, то сразу прыгаем в конец программы
    je skip_swap_2:
    ; Ваш код
start_loop:
    cmp byte [ebx + ecx - 1], 0x01  ; SIB адресация???
    jne skip_inc                    ; если 1, то увеличиваем счетчик и начинаем следующую итерацию
    inc eax
    jmp end_loop

skip_inc:
    cmp eax, edx    ; если 0, то проверяем накопленное значение с сохраненным в edx и проматываем 0
    jle skip_swap
    mov edx, eax
skip_swap:
    xor eax, eax

skip_zeroes_loop:
    cmp byte [ebx + ecx - 1], 0x00
    jne start_loop
    loop skip_zeroes_loop

    inc ecx ; если вывалились из цикла сюда, значит кончился счетчик
            ; для корректного завершения прибавим 1
end_loop:
    loop start_loop

    cmp eax, edx    ; eax = max (eax, edx)
    jge skip_swap_2
    mov eax, edx
skip_swap_2:

    call print_number
    call exit