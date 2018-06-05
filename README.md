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
- [X] 语法分析，语法树生成			完成: 2018.05.27
- [X] 语义分析，类型检查			完成: 2018.05.31
- [ ] 中间代码生成，目标代码优化    开始: 2018.06.04
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

init-declarator-list --> init-declarator {"," init-declarator} ";"

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

unary-expr --> {"+" | "-" | "!"} primary-expr

primary-expr --> identifier args-list? | identifier "[" expr "]" | "(" expr ")" | **INTLITERAL** | FLOATLITERAL | BOOLLITERAL | STRINGLITERAL

// parameters

para-list --> "(" proper-para-list? ")"

proper-para-list --> para-decl ( "," para-decl )*

para-decl --> type declarator

arg-list --> "(" proper-args-list? ")"

proper-arg-list --> args ( "," arg )*

arg --> expr

### 更新5，5.24 重新构建抽象语法树结构，决定更换语法分析方法

更换为LR(1)方法

简要笔记

使用LR(0)语法分析时，移进归约问题可以首先计算状态的Follow集合确定会触发归约条件的集合，然后再分析触发移进的符号集，即下一个移进的符号的First集合，当两个集合不相交时可以简单地依照两个集合来重写相应的动作。这样的，在发生冲突时才多看一位的算法称为SLR(1)，即简单的LR(1)。

在构造DFA时就考虑向右一个字符的方法，称为LR(1)

LR(1) 方法分析表状态数过大，分析效率降低，使用LALR(1)方案构建语法树

### 更新6，5.28 重新上线，更换语法分析方法

考虑到初次接触，编写时间较为紧迫，换回递归下降法。而且，无论使用LALR(1)或者递归下降都需要改写文法规则，相较之下，选择较为简单，并且有编写经验的递归下降较为稳妥

因此现在首先是对语法文法规则进行改写，将函数定义与变量定义区分开

### 更新7，5.29 大致完成语法分析器，正在测试

### 更新8，5.30 语法分析器一切正常

### 更新9，5.31 语义分析编写结束
简要笔记

使用哈希表作为符号表数据结构，然后使用方案2建表，即为每一个作用域建立一个符号表，每一个函数就是一个新的作用域，每一个compound_stmt也是一个新的作用域，使用一个变量保存当前符号表，当前符号表包含一个指针指向上级符号表，所以可以使用这个指针来搜索最近的变量，也可以使用这个指针来模拟退出作用域的过程。

语义分析我的实现流程使用了语法树与符号表实现，VC作为强类型语言，变量必须先声明后使用，不仅如此，而且因为存在块作用域，变量的生命周期存在限制，这个进入作用域又退出的流程我选择择使用方案二实现，即不删除符号表变量，而是为每一个作用域建立一张表，使用额外的连接属性将各个符号表联系到一起。然后使用一个全局的状态记忆当前进入的符号表使用一个属性记录当前进入的函数用于检查标识符存在问题，再设置一个全局状态用来检查Break与Continue语句，接下来只需要定义对不同结点的操作就可以对所有结点进行操作了。

### 更新10，6.04 语义分析编写结束
1. 针对声明结点:
	1. 如果是函数声明结点则首先根据子树确定类型，然后递归计算各子树，计算了形式参数列表之后记得将函数个信息存入符号表，注意计算子树之前需要新建符号表，函数声明的形参列表与后面的语句可以认为已经进入新的作用域了，创建新的作用域时记得附上函数名，便于作用域中可能出现的return语句进行语义分析
	2. 如果是变量声明结点，则保存声明类型到全局变量，然后计算所有子树语义
	3. 如果是形式参数声明结点，则根据根结点确定子树数据类型，保存到全局变量，然后计算子树
2. 针对初始化式(initialiser) 
	不管是变量初始化还是列表初始化，都认为相同，然后之直接计算第一个子树就好
3. 针对声明式(declarator)
	1. 如果是数组类型声明，首先根据全局类型设置结点数据类型，然后插入当前符号表，并计算子树类型，检查是否为整型字面值，并检查有无初始化表达式，有的话计算下一个子树。
	2. 如果是变量类型声明，首先根据全局类型设置结点数据类型，然后插入当前符号表，并检查有无初始化表达式，有的话计算下一个子树
	3. 检查有无兄弟节点，有的话就计算下一个兄弟结点
4. 针对语句(statement)
	1. 如果是复合语句(compund-stmt)，则创建新的符号表以模拟进入新的状态，然后计算所有子树，计算结束时候退出当前作用域，回到上级作用域
	2. 如果是break语句或者continue语句，则检查当前所处语句位置，如果既不处于for语句也不处于while语句内部，则报出错误
	3. 如果是return语句，则根据返回表达式类型确定语句类型，并从符号表中找到当前所处函数的信息，保证return语句返回值与函数类型一致
	4. 如果是if，while，for语句，则首先更新全局状态，告知当前所处语句，然后依次计算所有子结点，
	5. 如果是表达式语句，那么计算子结点
	6. 如果存在兄弟结点，则计算下一个兄弟结点
5. 针对表达式(expr)
	1. 如果是字面值节点，那么直接根据字面值类型确定节点类型
	2. 如果是比较计算符，那么计算两个子树类型，然后根据定义判断二者能否进行计算，如果可以计算，则置当前结点为布尔型
	3. 如果是赋值和计算节点，那么同样的计算子树类型，然后根据运算符定义判断能否计算，根据运算规则可能会进行类型转换
	4. 如果是基本表达式(primitive)，那么需要进一步判断
		- 如果是一个标识符节点，则从符号表中检索此标识符，如果有且标识符类型匹配(标识符是一个函数或者变量)，就根据符号表的信息为此结点设置类型
		- 如果是一个数组结点，那么操作和标识符相同，只不过需要把符号表中的数组类型转换为对应的基本类型。
		- 如果是一个函数结点，首先和标识符相同，先从按照名字检索，然后根据符号表的信息调用函数时传递的实参类型是否与形参列表匹配

### 更新11，6.04 开始进经中间代码生成

### 更新12，6.05 中间代码模块完成，暂未测试

### 更新13，6.05 中间代码编码完成，测试完成
(TODO)
