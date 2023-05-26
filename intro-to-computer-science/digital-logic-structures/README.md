
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

### Задача N2.

$Y = \overline{A} * \overline{B} + \overline{A} * B * \overline{C} + \overline{A + \overline{C}} =$  
$= \overline{A} * \overline{B} + \overline{A} * \overline{C} * B + \overline{A} * \overline{\overline{C}} =$  
$= \overline{A} * \overline{B} + \overline{A} * \overline{C} * B + \overline{A} * C =$  
$= \overline{A} * ( \overline{B} + \overline{C} * B + C ) =$  
$= \overline{A} * ( \overline{B} + \overline{C} * B + 1 * C ) =$  
$= \overline{A} * ( \overline{B} + \overline{C} * B + ( 1 + B ) * C ) =$  
$= \overline{A} * ( \overline{B} + \overline{C} * B + C + C * B ) =$  
$= \overline{A} * ( \overline{B} + C + C * B + \overline{C} * B ) =$  
$= \overline{A} * ( \overline{B} + C + B ) = \overline{A} * ( ( B + \overline{B} ) + C ) =$  
$= \overline{A} * ( C + 1 ) = \overline{A} * 1 = \overline{A}$

<details>

<summary>Таблица истинности</summary>

| $A$ | $B$ | $C$ | $Y$ |
| :---: | :---: | :---: | :---: |
| 0 | x | x | 1 |
| 1 | x | x | 0 |

x - любое значение

</details>

### Задача N3.

$Y = \overline{A * B * C} + A * \overline{B} =$  
$= \overline{A} + \overline{B} + \overline{C} + A * \overline{B} =$  
$= \overline{B} * ( 1 + A ) + \overline{A} + \overline{C} =$  
$= \overline{A} + \overline{B} + \overline{C}$

<details>

<summary>Таблица истинности</summary>

| $A$ | $B$ | $C$ | $Y$ |
| :---: | :---: | :---: | :---: |
| 0 | x | x | 1 |
| x | 0 | x | 1 |
| x | x | 0 | 1 |
| 1 | 1 | 1 | 0 |

x - любое значение

</details>

### Задача N4.

$Y = \overline{A} * B * C + \overline{A} * B * \overline{C} =$  
$= \overline{A} * ( B * C + B * \overline{C} ) = \overline{A} * B$  

<details>

<summary>Таблица истинности</summary>

| $A$ | $B$ | $C$ | $Y$ |
| :---: | :---: | :---: | :---: |
| x | 0 | x | 0 |
| 1 | x | x | 0 |
| 0 | 1 | x | 1 |

x - любое значение

</details>

### Задача N5.

$Y = \overline{A + \overline{A} * B + \overline{A} * \overline{B}} + \overline{A + \overline{B}} =$  
$= \overline{A + \overline{A} * \overline{\overline{B}} + \overline{A} * \overline{B}} + \overline{A + \overline{B}}=$  
$= \overline{A + \overline{A}} + \overline{A + \overline{B}} =$  
$= \overline{1} + \overline{A + \overline{B}} =$  
$= \overline{A} * \overline{\overline{B}} = \overline{A} * B$  

<details>

<summary>Таблица истинности</summary>

**см. задачу N4**

</details>
