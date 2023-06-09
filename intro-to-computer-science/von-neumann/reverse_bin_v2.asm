; 1. Каждая новая вызываемая функция может менять рабочие регистры
;   неопределенным образом (для вызывающего кода)
;   например TRAP GETC изменяет R0 и R7.
; 2. Для сохранения состояния регистров вызывающего кода
;   необходимо записать их в отдельный участок памяти.
; 3. Для организации такого хранения можно использовать стек.
; 4. Стек будет размещаться по определенному адресу
;   каждый вызов (или ПЕРЕД каждым вызовом) состояние регистров
;   текущей функции будет сохроняться в стеке.
; 5. Стекинг проще делать в теле вызванной функции. Т.к. в противном случае придется
;   внимательно следить за вызовами функций и предворять их вызовами процедур стекинга
; 7. Если сделать стекинг отдельной процедурой, то в рамках денной процедуры нельзя будет 
;   сохранить R7 т.к. он хранит адрес возврата (теперь уже вызваной процедуры).
; 8. Скорее всего, для работы функций будет хватать только рабочих регистров
;   по этому стек будет использоваться только для хранения рабочих регистров.
; 9. Передача аргументов в функции и прием возвращаемого значения так же будет 
;   через рабочие регистры. Вызывающий код будет готовить значения
;   в зависимости от требований вызываемого кода.
; 10. Возникает противоречие связанное с возвратом значений через регистры.
;   Возврат состояния стека будет просто все перетирать.
;   Заранее договоримся, что возврат значений будет через R0 (как для TRAP)
;   Данный регистр не будет подвергаться стекингу
; 11. Для хранения вершины стека будет использовать R6 (R7 для адреса возврата).

; R0 - регистр для возврата значений (или указателя на значение)
; R1...R5 - регистры для хранения данных вызова
; R6 - указатель на вершину стека
; R7 - хранит адрес возврата

.ORIG x3000
    ; первым делом сохраним вершину стека
    LD R6, STACK_PTR
    ; вывели приглашение на ввод
    LEA R0, INPUT_PROMPT
    PUTS
    ; считаем адрес, в котором будет располагаться строка и длину буфера
    LEA R1, STR_BUF_PTR
    LD R2, STR_LEN
    ; считаем терминирующий символ
    LD R3 NEG_LFEED
    ; передвинем указатель (R1) на конец буфера, т.к. строка будет размещаться из конца в начало
    ADD R1, R1, R2  ;R1 указывает на регистр следующий за буфером
    ; "считаем" из ввода "конец строки"
    AND R0, R0, #0
READ_SYM
    ADD R1, R1, #-1     ; разместим ввод и будем читать символы, пока не введут LF или буфер не кончится
    STR R0, R1, #0
    ADD R2, R2, #-1     ; уменьшаем счетчик оставшегося места
    BRz READ_SYM_END    ; проверяем есть ли место в буфере
    JSR READ_0_OR_1
    ADD R4, R0, R3      ; сравниваем с LF
    BRnp READ_SYM

READ_SYM_END
    ; выведем приглашение на вывод
    LEA R0, OUTPUT_PROMPT
    PUTS
    ; выведем введенные символы в обратном порядке
    AND R0, R0, #0
    ADD R0, R0, R1
    PUTS

    HALT    ; Всегда должно быть при завершении программы



NEG_0       .FILL   x-30    ; -'0'
NEG_1       .FILL   x-31    ; -'1'
NEG_LFEED   .FILL   x-000A  ; -'\n'

READ_0_OR_1     ; функция для чтения 0, 1 или LF в R0
    STR R7, R6, #0  ; сначала надо сохранить адрес возврата
    ADD R6, R6, #1
    JSR PUSH_STACK  ; т.к. данный вызов его перепишет

    LD R1, NEG_0
    LD R2, NEG_1
    LD R3, NEG_LFEED

READ_NEXT_SYM
    GETC
    ADD R4, R0, R1
    BRz PRINT_SYM
    ADD R4, R0, R2
    BRz PRINT_SYM
    ADD R4, R0, R3
    BRz READ_NEXT_SYM_END
    BRnp READ_NEXT_SYM
PRINT_SYM
    OUT
READ_NEXT_SYM_END

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

STACK_PTR       .FILL       xF000                   ; метка, с указателем на область в которой будет
                                                    ; размещаться стек для регистров ядра

STR_LEN         .FILL       #10                    ; длина буффера для хранения строки
STR_BUF_PTR     .BLKW       #10                    ; сброшеный в 0 буфер для размещения строки

INPUT_PROMPT    .STRINGZ    "ENTER BINARY VALUE: "  ; метка, с записанным приглашением на ввод
OUTPUT_PROMPT   .STRINGZ    "\nREVERSED: "
.END
