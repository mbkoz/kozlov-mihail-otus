
# INTRO TO COMPUTER SIENCE 3

### Задача N1.

$Y = A * C + \overline{A} * \overline{B} * C  =$  
$= C * ( A +  \overline{A} * \overline{B} ) =$  
$= C * ( A * 1 + \overline{A} * \overline{B} ) =$  
$= C * ( A * ( 1 + \overline{B} ) + \overline{A} * \overline{B} ) =$  
$= C * ( A + A * \overline{B} + \overline{A} * \overline{B} ) =$  
$= C * ( A + \overline{\overline{A}} * \overline{B} + \overline{A} * \overline{B} ) =$  
$= C * ( A + \overline{B} ) =$  
$= C * ( B \rightarrow A )$  

<details>

<summary>Таблица истинности</summary>

| $A$ | $B$ | $C$ | $Y$ |
| :---: | :---: | :---: | :---: |
| x | x | 0 | 0 |
| 0 | 0 | 1 | 1 |
| 0 | 1 | 1 | 0 |
| 1 | 0 | 1 | 1 |
| 1 | 1 | 1 | 1 |

x - любое значение

</details>