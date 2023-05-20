; Распроеделение регистров:
;   R0 - регистр для возврата значений/передачи аргументов
;   R1...R5 - регистры для хранения данных вызова
;   R6 - указатель на вершину стека
;   R7 - хранит адрес возврата (PC вызвавшей процедуры)

; программа принимает ввод - десятичное число.
; ввод осуществляется до ввода ENTER
; ввод фильтруется (символы не 0..9 не принимаются)
; переполнение не контролируется. Числа больше 65535 не будут корректно обработаны
; первые 0 двоичного представления отсекаются
; пустой ввод == 0

.ORIG x3000
MAIN
    LD R6, STACK_PTR        ; сохраним вершину стека
    LEA R0, INPUT_PROMPT    ; загрузили приглашение на ввод
    PUTS                    ; вывели приглашение на ввод
    JSR READ_NUM_FUNC       ; считаем десятичное число в R0
    ADD R1, R0, #0          ; временно перенесли результат в R1
    LEA R0, OUTPUT_PROMPT
    PUTS                    ; вывели приглашение на вывод
    ADD R0, R1, #0          ; вернули результат в R0
    JSR PRINT_BIN_FUNC      ; вывели двоичное представление числа (R0 изменен)
    ADD R0, R1, #0          ; восстановили результат в R0
    JSR REVERSE_BIN_FUNC    ; развернули довичное представление числа
    ADD R1, R0, #0          ; вывели прилашение на вывод 2 и вывели развернутое число
    LEA R0, OUTPUT_PROMPT_2
    PUTS
    ADD R0, R1, #0
    JSR PRINT_BIN_FUNC
    HALT

STACK_PTR       .FILL       xF000                   ; указатель стека
INPUT_PROMPT    .STRINGZ    "ENTER DEC NUMBER: "    ; метка, с записанным приглашением на ввод
OUTPUT_PROMPT   .STRINGZ    "\nBINARY: "            ; метка, с записанным приглашением на вывод
OUTPUT_PROMPT_2 .STRINGZ    "\nREVERSED BINARY: "

; символы, с которыми работают процедуры
NEG_LFEED   .FILL   x-000A  ; -'\n'
POS_LFEED   .FILL   x000A   ; '\n'
NEG_0       .FILL   x-30    ; -'0'
POS_0       .FILL   x30     ; '0'
NEG_9       .FILL   x-39    ; -'9'

; функция, которая читает ожидает ввода цифр или LF. Возвращает считанную цифру или LF в R0
READ_DIGIT_FUNC
    STR R7, R6, #0
    ADD R6, R6, #1
    JSR PUSH_STACK  ; сохраняем состояние регистров в стек

    LD R1, NEG_0
    LD R2, NEG_9
    LD R3, NEG_LFEED

READ_CHAR
    GETC
    ADD R4, R0, R3
    BRz END_READ_CHAR   ; если считали LF, то цикл завершен
    ADD R4, R0, R1
    BRn READ_CHAR       ; если код символа <'0', то введи не цифру
    ADD R4, R0, R2
    BRp READ_CHAR       ; если код символа >'9', то введи не цифру
    OUT
    ADD R0, R0, R1      ; вычитаем смещение, чтобы вернуть в R0 считанную цифру
END_READ_CHAR

    JSR POP_STACK       ; возвращаем состояние стека
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

; увеличивает R0 в 10 раз
MULTx10_FUNC 
    STR R7, R6, #0
    ADD R6, R6, #1
    JSR PUSH_STACK  ; сохраняем состояние регистров в стек
    ADD R0, R0, R0; R0=2x
    ADD R1, R0, R0; R1 = 4x
    ADD R1, R1, R1; R1 = 8x
    ADD R0, R1, R0; R0 = 10x
    JSR POP_STACK   ; возвращаем состояние регистров в стек
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

; функция последовательно читает цифры из ввода до нажатия '\n'
; после завершения выполнения в R0 возвращает введенное dec число
; R0 - будет использоваться для приема символа от GETC, для умножения в MULTx10 и для взврата результата
; R1 - число, к котое после очередного ввода цифры будет x10 и прибавляться ввод
; R2 - хранит -'\n' для сравнения
; R3 - временное хранилище

READ_NUM_FUNC
    STR R7, R6, #0
    ADD R6, R6, #1
    JSR PUSH_STACK  ; сохраняем состояние регистров в стек

    AND R1, R1, #0  ; R1 будет хранить конечный результат
    LD R2, NEG_LFEED

READ_NEX_DIGIT_BEGIN
    JSR READ_DIGIT_FUNC
    ADD R4, R0, R2  ; проверили, что ввод не '\n'
    BRz GET_NEXT_DIGIT_END
    ADD R3, R0, #0 ; перенести считанный символ R0 в R3
    ADD R0, R1, #0 ; перенесли число в R0 для умножения
    JSR MULTx10_FUNC
    ADD R0, R0, R3 ; прибавили то, что считали
    ADD R1, R0, #0 ; перенесли результат в R1
    BRnzp READ_NEX_DIGIT_BEGIN
GET_NEXT_DIGIT_END

    ADD R0, R1, #0 ; перенос в R0 для возврата значения

    JSR POP_STACK   ; возвращаем состояние регистров в стек
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

MSB_MASK    .FILL x8000 ; маска для самого старшего разряда
ITER_NUM    .FILL x10   ; 16 итераций на 16 разрядов числа

; функция выводит число R0 в двоичной форме, пропуская первые 0'и
; R1 - число для вывода
; R2 - хранит общее количество оставшихся итераций (всего 16)
; R3 - маска для старшего бита
; R4 - temp
PRINT_BIN_FUNC
    STR R7, R6, #0
    ADD R6, R6, #1
    JSR PUSH_STACK  ; сохраняем состояние регистров в стек

    ADD R1, R0, #0 ; перенесли выводимое число в R1

    LD R2, ITER_NUM
    LD R3, MSB_MASK

SKIP_LEAD_ZEROES
    ADD R4, R2, #-1             ; проверяем количество итераций 
    BRz SKIP_LEAD_ZEROES_END    ; хотя бы 1 '0' надо отобразить
    AND R4, R1, R3
    BRnp SKIP_LEAD_ZEROES_END   ; если старший бит 0, то пора R1 выводить
    ADD R1, R1, R1              ; число сместили на разряд влево
    ADD R2, R2, #-1             ; уменьшили количество итераций
    BRnzp SKIP_LEAD_ZEROES
SKIP_LEAD_ZEROES_END

PRINT_MSB
    LD R0, POS_0
    AND R4, R1, R3
    BRz SKIP_INC
    ADD R0, R0, #1
SKIP_INC
    OUT
    ADD R1, R1, R1
    ADD R2, R2, #-1
    BRnp PRINT_MSB  ;проверяем количество оставшихся итераций
PRINT_MSB_END

    JSR POP_STACK   ; возвращаем состояние регистров в стек
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

LSB_MASK .FILL x01
; развернет значение регистра R0 в обратном направлении
; R0 - регистр, в котором будет формироваться развернутое значение
; R1 - копия входного параметра (будет исследоваться <-)
; R2 - маска (b001), будет сдвигаться <- каждую итерацию
; R3 - счетчик циклов (всего 16)
; R4 - temp
; R5 - temp
REVERSE_BIN_FUNC
    STR R7, R6, #0
    ADD R6, R6, #1
    JSR PUSH_STACK  ; сохраняем состояние регистров в стек

    ADD R1, R0, #0  ; сохранили копию входного параметра
    LD R2, MSB_MASK
    LD R3, ITER_NUM
    AND R4, R4, #0  ; счетчик нулей в начале числа

; сначала посчитаем количество старших нулевых разрядов
COUNT_LEAD_ZEROES
    ADD R3, R3, #-1             ; -1 чтобы отобразить хотя бы 1 '0'
    BRz COUNT_LEAD_ZEROES_END
    AND R5, R1, R2
    BRnp COUNT_LEAD_ZEROES_END
    ADD R4, R4, #1
    ADD R1, R1, R1
    BRnzp COUNT_LEAD_ZEROES
COUNT_LEAD_ZEROES_END
; вычислим количество итераций для разворота числа
    LD R3, ITER_NUM
    NOT R4, R4
    ADD R4, R4, #1
    ADD R3, R3, R4

    LD R2, LSB_MASK ; теперь загрузим в R2 LSB
    ADD R1, R0, #0  ; восстановим в R1 параметр
    AND R0, R0, #0  ; подготовим R0 под ответ

REVERSE_LOOP
    ADD R3, R3, #0
    BRz REVERSE_LOOP_END    ; проверяем счетчик циклов
    ADD R0, R0, R0,         ; смещаем R0 влево
    AND R5, R1, R2          ; применыем маску к копии
    BRz SKIP_INC_REV
    ADD R0, R0, #1
SKIP_INC_REV
    ADD R3, R3, #-1
    ADD R2, R2, R2          ; смещаем маску влево
BRnzp REVERSE_LOOP          ; повторяем пока всей маской не пройдемся по вх. числу
REVERSE_LOOP_END

    JSR POP_STACK   ; возвращаем состояние регистров в стек
    ADD R6, R6, #-1
    LDR R7, R6, #0
    RET

PUSH_STACK  ; функция для сохранения состояния рабочих регистров (R1...R5)
    ;STR R0, R6, #0
    STR R1, R6, #1
    STR R2, R6, #2
    STR R3, R6, #3
    STR R4, R6, #4
    STR R5, R6, #5
    STR R6, R6, #6
    ADD R6, R6, #7  ; указатель стека указывает на следующую ячейку, которая не занята
    RET

POP_STACK   ; восстановление состояния рабочих регистров  (R1...R5)
    ;LDR  R0, R6, #-7
    LDR  R1, R6, #-6
    LDR  R2, R6, #-5
    LDR  R3, R6, #-4
    LDR  R4, R6, #-3
    LDR  R5, R6, #-2
    LDR  R6, R6, #-1    ; теперь R6 хранит указатель предыдущего состояния вершины
    RET

.END
