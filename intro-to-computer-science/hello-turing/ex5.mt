; Ex5
; Напишите программу, которая разворачивает строку 
; (натуральное число в двоичном представлении).
; qi - начальное состояние (необходимо задать в advanced options)

qi _ _ r qi
qi 0 0 r q1
qi 1 1 r q1

q1 _ _ l q2
q1 0 0 r q1
q1 1 1 r q1

q2 _ _ r q6
q2 0 p r q3
q2 1 p r q4
q2 p p l q2

q3 _ 0 l q5
q3 0 0 r q3
q3 1 1 r q3
q3 p p r q3

q4 _ 1 l q5
q4 0 0 r q4
q4 1 1 r q4
q4 p p r q4

q5 0 0 l q5
q5 1 1 l q5
q5 p p l q2

q6 0 0 l halt
q6 1 1 l halt
q6 p _ r q6
