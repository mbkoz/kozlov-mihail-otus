; Ex1
; Напишите программу, которая отвечает является ли длина входной строки 
; (натурального числа в двоичном представлении) четной.
; qi - начальное состояние (необходимо задать в advanced options)

qi _ _ r qi
qi 0 _ r q1
qi 1 _ r q1

q1 _ 0 l halt
q1 0 _ r q2
q1 1 _ r q2

q2 _ 1 l halt
q2 0 _ r q1
q2 1 _ r q1