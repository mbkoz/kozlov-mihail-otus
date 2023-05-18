; R0 - регистр для возврата значений (или указателя на значение)
; R1...R5 - регистры для хранения данных вызова
; R6 - указатель на вершину стека
; R7 - хранит адрес возврата
;!!!!! симулятор не сбрасывает память .BLKW. Для сброса необходимо пересобрать

.ORIG x3000
MAIN
    LD R6, STACK_PTR        ; сохраним вершину стека
    LEA R0, INPUT_PROMPT    ; вывели приглашение на ввод
    PUTS
    ; считаем набор цифр
    ; конвертируем в число (* 10 + мл разряд) *10 подфункция
    ; convert to bin string (place in stragith o reverse order)
    ; trim lead zeroes
    ; print
    ; провести оптимизацию с моментальной конвертацией в число (а не записью в буфер)
    

    JSR READ_STR_FUNC
    PUTS
    JSR STR_TO_NUM
    JSR NUM_TO_BIN_STR_FUNC
    JSR TRIM_LEAD_0
    PUTS
    HALT

NEG_LFEED   .FILL   x-000A  ; -'\n'
NEG_0       .FILL   x-30    ; -'0'
NEG_9       .FILL   x-39    ; -'9'

READ_DIGIT_FUNC     ; функция для чтения 0, 1 или LF в R0
    STR R7, R6, #0  ; сначала надо сохранить адрес возврата
    ADD R6, R6, #1
    JSR PUSH_STACK  ; т.к. данный вызов его перепишет
    LD R1, NEG_0
    LD R2, NEG_9
    LD R3, NEG_LFEED
READ_CHAR
    GETC 
    ADD R4, R0, R3
    BRz END_READ_CHAR   ; считали LF
    ADD R4, R0, R1
    BRn READ_CHAR
    ADD R4, R0, R2
    BRp READ_CHAR
    OUT
END_READ_CHAR
    JSR POP_STACK
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

STR_LEN         .FILL       #6              ; длина буффера для хранения строки
STR_BUF_PTR     .BLKW       #6              ; сброшеный в 0 буфер для размещения строки

READ_STR_FUNC    ; функция для чтения строки (не более пяти символов) с возвратом адреса в R0
    STR R7, R6, #0  ; сначала надо сохранить адрес возврата
    ADD R6, R6, #1
    JSR PUSH_STACK  ; т.к. данный вызов его перепишет
    LEA R1, STR_BUF_PTR
    LD R2, STR_LEN
    LD R3, NEG_LFEED
    ADD R2, R2, #-1 ; сократили для хранения терминирующего символа
READ_NEXT_DIGIT
    JSR READ_DIGIT_FUNC
    ADD R4, R0, R3
    BRz READ_NEXT_DIGIT_END
    STR R0, R1, #0
    ADD R1, R1, #1
    ADD R2, R2, #-1
    BRp READ_NEXT_DIGIT
READ_NEXT_DIGIT_END
    AND R0, R0, #0
    STR R0, R1, #0  ; запишем еще 0, to make simulator happy
    LEA R0, STR_BUF_PTR ; возвращаем через r0 указатель на строку
    JSR POP_STACK
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

MULTx10_FUNC ; увеличивает в 10 раз R0 (R0->R0)
    STR R7, R6, #0  ; сначала надо сохранить адрес возврата
    ADD R6, R6, #1
    JSR PUSH_STACK  ; т.к. данный вызов его перепишет
    ADD R0, R0, R0; R0=2x
    ADD R1, R0, R0; R1 = 4x
    ADD R1, R1, R1; R1 = 8x
    ADD R0, R1, R0; R0 = 10x
    JSR POP_STACK
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

STR_TO_NUM ; принимает строку через R0 и конвертирует в число в R0
    STR R7, R6, #0  ; сначала надо сохранить адрес возврата
    ADD R6, R6, #1
    JSR PUSH_STACK  ; т.к. данный вызов его перепишет
    LD R3, NEG_0
    AND R1, R1, x0  ; перенесли строку в R1
    ADD R1, R0, #0
    AND R0, R0, x0  ; в R0 будет копиться результат
    LDR R2, R1, #0
    BRz NUM_CVN_LOOP_END
NUM_CVN_LOOP
    JSR MULTx10_FUNC
    ADD R2, R2, R3  ; выстем '0' чтобы получить число
    ADD R0, R0, R2
    ADD R1, R1, #1
    LDR R2, R1, #0
    BRnp NUM_CVN_LOOP
NUM_CVN_LOOP_END
    JSR POP_STACK
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

BIN_STR_LEN     .FILL       #16              ; длина строки для хранения 16 разрядов регистра
BIN_STR_BUF_PTR .BLKW       #17              ; сброшеный в 0 буфер для двоичной размещения строки + терминирующий 0
POS_0           .FILL       x30

NUM_TO_BIN_STR_FUNC      ; вернет строку с двоичным представлением числа в R0
    STR R7, R6, #0  ; сначала надо сохранить адрес возврата
    ADD R6, R6, #1
    JSR PUSH_STACK  ; т.к. данный вызов его перепишет

    LEA R1, BIN_STR_BUF_PTR
    LD R2, BIN_STR_LEN
    ADD R1, R1, R2  ; указываем на конец строки
    
    AND R5, R5, #0  ; загрузили '\0' в конец строки
    STR R5, R1, #0
    ADD R1, R1, #-1

    AND R3, R3, #0  ; маска
    ADD R3, R3, #1

NUM_TO_BIN_STR_CVN
    LD R5, POS_0

    AND R4, R3, R0
    BRz SKIP_INC
    
    ADD R5, R5, #1
SKIP_INC
    STR R5, R1, #0
    AND R5, R5, #0

    ADD R3, R3, R3
    ADD R1, R1, #-1
    ADD R2, R2, #-1
    BRnp NUM_TO_BIN_STR_CVN
NUM_TO_BIN_STR_CVN_END

    LEA R0, BIN_STR_BUF_PTR

    JSR POP_STACK
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

TRIM_LEAD_0 ; убирает из начала строки '0' (в R0)
    STR R7, R6, #0  ; сначала надо сохранить адрес возврата
    ADD R6, R6, #1
    JSR PUSH_STACK  ; т.к. данный вызов его перепишет

    LD R1, NEG_0

TRIM_0_LOOP
    LDR R2, R0, #0
    ADD R2, R2, R1
    BRz TRIM_0_LOOP
TRIM_0_LOOP_END

    JSR POP_STACK
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

PUSH_STACK  ; функция для сохранения состояния рабочих регистров
    ;STR R0, R6, #0
    STR R1, R6, #1
    STR R2, R6, #2
    STR R3, R6, #3
    STR R4, R6, #4
    STR R5, R6, #5
    STR R6, R6, #6  ; так же будет сохраняться адрес предыдущего стек фрейма
    ADD R6, R6, #7  ; указатель стека указывает на следующую ячейку, которая не занята
    RET

POP_STACK   ; восстановление состояния рабочих регистров
    ;LDR  R0, R6, #-7
    LDR  R1, R6, #-6
    LDR  R2, R6, #-5
    LDR  R3, R6, #-4
    LDR  R4, R6, #-3
    LDR  R5, R6, #-2
    LDR  R6, R6, #-1    ; теперь R6 хранит указатель предыдущего состояния вершины
    RET

STACK_PTR       .FILL       xF000           ; метка, с указателем на область в которой будет
                                            ; размещаться стек для регистров ядра

INPUT_PROMPT    .STRINGZ    "ENTER NUMBER: "; метка, с записанным приглашением на ввод
OUTPUT_PROMPT   .STRINGZ    "\nBINARY: "
.END
