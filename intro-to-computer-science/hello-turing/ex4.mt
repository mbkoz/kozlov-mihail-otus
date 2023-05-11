; Ex4
; Напишите программу, которая удваивает число в унарной кодировке.
; qi - начальное состояние (необходимо задать в advanced options)

qi _ _ r qi
qi 1 1 r q1

q1 _ _ l q2
q1 1 1 r q1

q2 _ b l q3
q2 1 f r q2
q2 f f r q2
q2 b b r q2

q3 _ _ r q4
q3 1 1 * q2
q3 f f l q3
q3 b b l q3

q4 _ _ l q5
q4 f 1 r q4
q4 b 1 r q4

q5 _ _ * halt
q5 1 1 l q5
