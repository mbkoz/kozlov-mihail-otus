
# LANGUAGES AND PROGRAMMING

### Грамматика двоичного числа

**Задание**  
Опишите в форме Backus–Naur (BNF) грамматику xx, где x - любая цепочка из нулей и единиц  

**Комментарии:** 
- Представлено 2 решения - в BNF и EBNF
- В решении дополнительно приведена грамматика двоичного числа (либо 0, либо не может начинаться с последовательности 0ей)

**вариант BNF**
```BNF
<bin_digit> ::= "0" | "1"
/* bin_seq - последовательность 0 и 1 */
<bin_seq> ::= <bin_digit> | <bin_digit> <bin_seq>
/* bin_number - двоичное число, либо 0, либо последовательность 0 и 1 начиная с 1 */
<bin_number> ::= <bin_digit> | "1" <bin_seq>
```
[**ссылка на реализацию**](https://bnfplayground.pauliankline.com/?bnf=%3Cbin_digit%3E%20%3A%3A%3D%20%220%22%20%7C%20%221%22%0A%2F*%20bin_seq%20-%20%D0%BF%D0%BE%D1%81%D0%BB%D0%B5%D0%B4%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D1%8C%200%20%D0%B8%201%20*%2F%0A%3Cbin_seq%3E%20%3A%3A%3D%20%3Cbin_digit%3E%20%7C%20%3Cbin_digit%3E%20%3Cbin_seq%3E%0A%2F*%20bin_number%20-%20%D0%B4%D0%B2%D0%BE%D0%B8%D1%87%D0%BD%D0%BE%D0%B5%20%D1%87%D0%B8%D1%81%D0%BB%D0%BE%2C%20%D0%BB%D0%B8%D0%B1%D0%BE%200%2C%20%D0%BB%D0%B8%D0%B1%D0%BE%20%D0%BF%D0%BE%D1%81%D0%BB%D0%B5%D0%B4%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D1%8C%200%20%D0%B8%201%20%D0%BD%D0%B0%D1%87%D0%B8%D0%BD%D0%B0%D1%8F%20%D1%81%201%20*%2F%0A%3Cbin_number%3E%20%3A%3A%3D%20%3Cbin_digit%3E%20%7C%20%221%22%20%3Cbin_seq%3E&name=)


**вариант EBNF**
```BNF
/* bin_seq - последовательность 0 и 1 */
<bin_seq> ::= ("0" | "1")+
/* bin_number - двоичное число, либо 0, либо последовательность 0 и 1 начиная с 1 */
<bin_number> ::= "0" | "1" <bin_seq>?
```

[**ссылка на реализацию**](https://bnfplayground.pauliankline.com/?bnf=%2F*%20bin_seq%20-%20%D0%BF%D0%BE%D1%81%D0%BB%D0%B5%D0%B4%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D1%8C%200%20%D0%B8%201%20*%2F%0A%3Cbin_seq%3E%20%3A%3A%3D%20(%220%22%20%7C%20%221%22)%2B%0A%2F*%20bin_number%20-%20%D0%B4%D0%B2%D0%BE%D0%B8%D1%87%D0%BD%D0%BE%D0%B5%20%D1%87%D0%B8%D1%81%D0%BB%D0%BE%2C%20%D0%BB%D0%B8%D0%B1%D0%BE%200%2C%20%D0%BB%D0%B8%D0%B1%D0%BE%20%D0%BF%D0%BE%D1%81%D0%BB%D0%B5%D0%B4%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D1%8C%200%20%D0%B8%201%20%D0%BD%D0%B0%D1%87%D0%B8%D0%BD%D0%B0%D1%8F%20%D1%81%201%20*%2F%0A%3Cbin_number%3E%20%3A%3A%3D%20%220%22%20%7C%20%221%22%20%3Cbin_seq%3E%3F&name=)

### Грамматика последовательности скобок

**Задание**  
Опишите в форме Backus–Naur (BNF) грамматику цепочки открывающихся и закрывающихся круглых скобок. Раскрытые скобки должны быть сбалансированы закрытыми
Правильно - (), (()), ()(), (((()())()()))
Неправильно - ())(, (, )(, ))))))))))(

**Комментарии:**  
- Представлено 2 решения - в BNF и EBNF
- Решение дополнено переменной `<brackets_ts>`, чтобы можно было загогнять тесты череp разделитель ", "

**вариант BNF**
```BNF
<brackets> ::= "()" | "(" <brackets> ")" | <brackets> <brackets>
/* brackets_ts для более удобного тестирования корректных случаев */
<brackets_ts> ::= <brackets> | <brackets> ", " <brackets_ts>
```

[**ссылка на реализацию**](https://bnfplayground.pauliankline.com/?bnf=%3Cbrackets%3E%20%3A%3A%3D%20%22()%22%20%7C%20%22(%22%20%3Cbrackets%3E%20%22)%22%20%7C%20%3Cbrackets%3E%20%3Cbrackets%3E%0A%3Cbrackets_ts%3E%20%3A%3A%3D%20%3Cbrackets%3E%20%7C%20%3Cbrackets%3E%20%22%2C%20%22%20%3Cbrackets_ts%3E&name=)

**вариант EBNF**
```BNF
<brackets> ::= "(" <brackets>? ")" <brackets>?
/* brackets_ts для более удобного тестирования корректных случаев */
<brackets_ts> ::= <brackets> (", " <brackets_ts>)*
```

[**ссылка на реализацию**](https://bnfplayground.pauliankline.com/?bnf=%3Cbrackets%3E%20%3A%3A%3D%20%22(%22%20%3Cbrackets%3E%3F%20%22)%22%20%3Cbrackets%3E%3F%0A%2F*%20brackets_ts%20%D0%B4%D0%BB%D1%8F%20%D0%B1%D0%BE%D0%BB%D0%B5%D0%B5%20%D1%83%D0%B4%D0%BE%D0%B1%D0%BD%D0%BE%D0%B3%D0%BE%20%D1%82%D0%B5%D1%81%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D1%8F%20%D0%BA%D0%BE%D1%80%D1%80%D0%B5%D0%BA%D1%82%D0%BD%D1%8B%D1%85%20%D1%81%D0%BB%D1%83%D1%87%D0%B0%D0%B5%D0%B2%20*%2F%0A%3Cbrackets_ts%3E%20%3A%3A%3D%20%3Cbrackets%3E%20(%22%2C%20%22%20%3Cbrackets_ts%3E)*&name=)


