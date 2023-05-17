.ORIG x3000 ; start of user space
; вывели приглашение на ввод
LEA R0, INPUT_PROMPT
PUTS
; считаем адрес, в котором будет располагаться строка
LD R1, STRING_PTR
; считаем терминирующий символ
LD R2 NEWLINE_NEG
; "считаем" из ввода "конец строки"
ADD R1, R1, #1
AND R0, R0, #0
; читаем символы пользователя, пока не будет введен enter (контроль длины строки)
; ЦИКЛ продолжается пока не будет введен ВВОД (символ CR)
; по итогу ввода 
READ_SYM
ADD R1, R1, #-1
STR R0, R1, #0
GETC
OUT
; сравниваем с CR+LF
ADD R3, R0, R2 
BRnp READ_SYM
; выведем приглашение на вывод
LEA R0, OUTPUT_PROMPT
PUTS
; выведем введенные символы в обратном порядке
AND R0, R0, #0
ADD R0, R0, R1
PUTS

HALT    ; Всегда должно быть при завершении программы

INPUT_PROMPT    .STRINGZ    "ENTER BINARY VALUE: "  ; метка, с записанным приглашением на ввод
OUTPUT_PROMPT   .STRINGZ    "REVERSED: "
STRING_PTR      .FILL       xF000                   ; метка, с указателем на область в которой будет
                                                    ; размещаться строка
NEWLINE_NEG     .FILL       x-000A                  ; -"символ конца строки"

.END
