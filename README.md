# VarianC-Compiler

---

**简介**:

VC是一门类C的迷你语言，VC同时包括有C语言的一个子集和一些来自于Java语言的特性

VC程序包含了零行或者多行函数声明实现在一个文件中，所以需要说明的是，VC不支持多文件编译

VC中的两个函数通过传入形式参数与返回值进行沟通

VC程序字符集由一系列ASCII字符集构成。在VC中将空格，制表，换页，回车，换行符看作空白字符。程序使用换行，回车或者回车组合换行符作为单行终止符，即单行行尾的终结符为\n或者\r或者\r\n。

VC同时包含单行和多行注释，单行注释使用//，多行注释使用/**/包含代码块，注释不允许嵌套

### 开发日志待办事项

- [X] 词法分析，符号序列生成		完成: 2018.05.21
- [ ] 语法分析，语法树生成			开始: 2018.05.22
- [ ] 语义分析，类型检查
- [ ] 中间代码生成，目标代码优化
- [ ] 目标代码生成 

### 更新1，2018.05.21 词法分析开始

**词法规则如下**

identifier -> [_a-zA-Z][_a-zA-Z0-9]*

关键字: boolean,break,continue,else,for,float,if,int,return,void,while

运算符:

Name         |Tokens     
------------|----------
ArithMetic Op| + - * /   
Relational Op| < <= > >= 
Equality Op  | == !=     
Logical Op   | || && !   
Assignment Op| =         
注意:其中"!"是一个取反单目运算符，"+"与"-"同时也被重载为正负标识符号

分隔符: { } ( ) [ ] ; ,

字面量:

int_literal -> digit(digit)*

上述字面量是一个**int**类型

float_literal ->  digit* fraction exponent?
				| digit+ .
				| digit+ .? exponent

digit -> [0-9]

fraction -> . digit+

exponent -> (E|e)(+|-)?digit+

举几个例子: 1.2 1. .1 1e2 1.2E+2 1.2e-2 .1E2

上述字面量是一个**float**类型

一个boolean型字面量包括false和true，二者都是ascii序列，因此二者不可作为标识符，属于boolean类型

一个string字面量包括零个或者多个字符与前后两个双引号，且包含如下转义字符，\b \f \n \r \t \' \" \\，string型字面量只能被内建函数putString()和putStringLn()使用。

类型系统说明:

VC 是一门强类型语言，因此所有函数和变量必须先定义后使用，不同类型之间只有在特定情况下才能互相转换与运算。

VC 的类型可以分为基本类型和数组类型;基本类型包括了**void**
 **int** **boolean** **float**;数组类型包括了**boolean[]**
 **int[]** **float[]** 也就是数组中每个元素的类型分别为各自

void说明：void是一个表示空集的类型标识符，它只被用于表达函数无返回值的情况

boolean说明：**boolean**是一个包含了**false**和**true**的逻辑值。VC中有5种逻辑运算符== != && || ! 逻辑表达式可以控制**if**和**while**结构的控制流，它也可以控制for语句

int说明: **int**是一个32位有符号整形类型，也就是大小范围为-2147483648 --- 2147483647，并且有如下几个运算符可以用在**int**中，+ - * / < <= > >= == !=，头四个运算符的结果是int，后6个运算符的结果是boolea类型。注意&& ||以及!不可用于**int**.

float说明: **float**是一个单精度浮点类型，有关于它的运算符和规则与**int**相同

### 更新2，2018.5.21 词法分析开发结束，相关代码冻结

### 更新3，2018.5.21 解冻词法分析代码，修复bug

**bug说明:**

1. 不识别任何注释(fixed)
2. 浮点数识别会提前终止文件(fixed)

### 更新4，2018.5.21 语法分析开始

**语法规则如下**

program --> (func-decl|var-decl)*

// declarations

func-decl --> type identifier para-list compound-stmt

var-decl --> type init-declarator-list ";"

init-declarator-list --> init-declarator ";"

init-declarator --> declarator ( "=" initialiser ) ?

declarator --> identifier | identifier "[" **INTLITERAL** ? "]"

initialiser --> expr | "{" expr ( "," expr )* "}"

// primitive types

type --> void | boolean | int | float

// identifier 

identifier --> **ID**

// statement 

compound-stmt --> "{" var-decl* stmt* "}"

stmt --> compound-stmt | if-stmt | for-stmt | while-stmt | break-stmt | continue-stmt | return-stmt | expr-stmt

if-stmt --> if "(" expr ")" stmt ( else stmt ) ?

for-stmt --> for "(" expr? ";" expr? ";" expr? ")" stmt 

while-stmt --> while "(" expr ")" stmt

break-stmt --> break ";"

continue-stmt --> continue ";"

return-stmt --> return expr? ";"

expr-stmt --> expr? ";"

//exporessions 

expr --> assignment-expr

assignment-expr --> ( cond-or-expr "=" )* cond-or-expr

cond-or-expr --> cond-and-expr { "||" cond-and-expr }

cond-and-expr --> equlity-expr { "&&" equlity-expr }

equlity-expr --> rel-expr { "==" | "!=" rel-expr }

rel-expr --> additive-expr { "<" | "<=" | ">" | ">=" additive-expr }

additive-expr --> multiplicative-expr { "+" | "-" multiplicative-expr }

multiplicative-expr --> unary-expr { "*" | "/" unary-expr}

unary-expr --> "+" | "-" | "!" primary-expr

primary-expr --> identifier args-list? | identifier "[" expr "]" | "(" expr ")" | **INTLITERAL** | FLOATLITERAL | BOOLLITERAL | STRINGLITERAL

// parameters

para-list --> "(" proper-para-list? ")"

proper-para-list --> para-decl ( "," para-decl )*

para-decl --> type declarator

arg-list --> "(" proper-args-list? ")"

proper-arg-list --> args ( "," arg )*

arg --> expr

(TODO)
