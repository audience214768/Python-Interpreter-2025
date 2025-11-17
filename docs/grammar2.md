> Update date: 2025-11-15
> 
> 相比原版（grammar.md）删除了（原版也写了没用的）下标运算符/数组访问的内容，增加了一些语法案例与提示。
> 
> 本文档有使用 html 语法为部分文字上色的部分，建议使用一个支持相关功能的 Markdown 阅读器进行阅读。

# Simplified Python Reference + Instructions

## I 导言

本文描述 2025 ACM Class: Python Interpreter 工程所描述的 Python 简化版本（下称Simplified Python）语言的相关行为规则，其主要继承标准 Python（可以参考 [The Python Language Reference — Python 3.14.0 documentation](https://docs.python.org/3/reference/)），相比原规则做出了大量的删减，同时也有少量语法规则不同之处。如有重要行为未说明 / 行为与标准 Python 冲突且未特意说明之处，请优先向课程助教询问，一般来说 Simplified Python 行为会与标准 Python 行为相同。

本工程意在使用 C++ 程序模拟 Python 的解释器模式，生成的可执行文件期望输入为一个完整的，在我们规定的语法规则下**保证词法与语法上合法，无未定义行为，不产生运行时错误**且不需要标准输入的 Simplified Python 程序，期望输出为这个程序的标准输出内容。工程的主体部分已经使用 Antlr4 工具完成建构，作为一项工程作业，你被期望填充控制流行为与数据处理部分。

本文档中会对部分文字添加颜色。

- <font color=yellow>黄色</font>部分表示这里提到了一处 Simplified Python 相对于 Python 做出的行为简化，仍然被标准 Python 兼容，在设计上希望用于减少你的实现工作量；

- <font color=yellow>黄色</font>部分也可能是一个简单的行为退化提醒：Simplified Python 由于做出了行为简化，导致其他未修改行为在简化后的语法中也相应退化成了实现难度更低的方式。

- <font color=red>红色</font>部分表示这里提到了一处 Simplified Python 参考 C++ 相关行为模式，相对于 Python 做出的行为修改，它们不被标准 Python 兼容，但是你一般可以从 C++ 中找到这个设计理念的影子，以方便你理解与实现。

- <font color=red>红色</font>部分也可能只是一个警告，标识一处与 C++ 思维直觉不符的语法特性。

颜色高亮仅仅是一种提示，并不是所有的修改都被高亮显示，所以即使你对标准 Python 非常熟悉，也请认真阅读整个文档以确定 Simplified Python 的行为。

## II 控制流结构

你应当按照解释器模式来执行输入程序，即时按照语句顺序逐行推进控制流，直接解析指令，读写并存储数据。遇到跳转语句（分支，循环，函数调用等的开始与结束）时按照要求将控制流跳转到对应的位置。

## III 词法规则

这一部分描述词法分析器（Lexer）期待的合法程序的词法特征。这一部分已经由 Antlr4 工具完成构建，你只需要理解其特征以辅助你进行实现。

Simplified Python 程序的合法字符集是 ASCII 字符集的一部分。

### 1. 预处理

#### 1.1 注释

解释器会忽略程序中的注释。Simplified Python 仅支持单行注释，即从一个字符 `#` 到所在行结束处（一个换行符或文件结束标识符）的内容均会被忽略。

#### 1.2 空白符

空白符包括空格 ` `，制表符 `\t` 与换行符 `\n` ，它们是分割词素的标志。

行首的空白符同时表示为缩进，用以确定代码块的范围。

### 2. 词法分析

词法分析器（Lexer）根据空白符将输入文件分割为词素（token），并根据词素的具体内容进行初步分类，列举如下：

#### 2.1 关键字/保留字（Keywords）

Simplified Python支持的关键字有：

```python
None
True
False
def
return
break
continue
if
elif
else
while
or
and
not
```

关键字的划分优先级高于其他词素，即关键字不会作为标识符。

#### 2.2 标识符 （Identifiers）

标识符的第一个字符必须是英文字符，之后的字符可以是英文字符，数字字符或下划线。

<font color=yellow>尝试让标识符的长度超过64个字符是未定义行为。</font>

#### 2.3 字面量（Literals）

- 逻辑字面量：`True`，`False`

- 整数字面量：十进制高精度整数表示，如 `0`, `-10`, `201`。
  
  - 非零数字字面量的前导零是未定义的（如 `-0021` 未定义）。

- 浮点数字面量：十进制小数表示，如 `-0.0`，`12.34`。
  
  - 不允许省略为零的整数部分（如 `.123`），不允许指数表示法（如 `-1e5`），不允许不必要前导零（如 `00.01`）。以上情形视作未定义行为。

- 字符串字面量：由成对的双引号 `"` 或单引号 `'` 所包围的字符串。
  
  - 允许两个字符串常量的拼接，如 `"123"'456'` 与 `'123456'` 等效。
  
  - 检测到前一个引号后，其到下一个同类型引号之间的内容就是字符串的内容。比如 `"'12''"` 的内容是 `'12''` （或者说 `"\'12\'\'"`）
  
  - <font color=yellow>字符串字面量总是被解释成 ASCII 字符中的可显示字符（printable character）或空白符构成的序列，解释出超过该字符集内容的字符串被视作为产生了未定义行为。</font>具体解释行为可以参考 C++ 的字符串字面量相关规则。

- 格式化字符串字面量（format string literal / f-string literal）：由一个 `f"` 开头，到下一个非 `f"` 里的 `"` 结尾的部分构成一个格式化字符串。
  
  > 格式化字符串在这里就出现不大符合“字面量”这个名字的字面意思...但是标准 Python 说它是字面量，我觉得也是。
  > 
  > 虽然逻辑上它应该在后面被定义，但分类上它很适合出现在这里，那还是放这里得了。
  
  - 在字符串的基础上，格式化字符串内部允许用一对 `{}` 括起一个表达式，在解析格式化字符串时将这个表达式求值，并将其格式化为字符串形式替代这个 `{}` 所占的部分。可以在一个格式化字符串内部格式化多个表达式。
  
  - 格式化规则请参考后面提到的内置函数 `str`，同时保证转换内容也不会超出内置转换函数 `str` 的允许参数范围。
  
  - 格式化字符串最外层解析不允许用单引号代替双引号（`f'xxx'` 不合法），也不允许拼接（`f"a"'b'`，`f"a"f"b"` 不合法）。<font color=yellow>此处弱于标准 Python 语法</font>，但相关实现也不在本项目考察范围之内。
  
  - 在格式化字符串里表示 `{` 与 `}` 本身需要用 `{{` 与 `}}` 代替。比如 `f"{{a}}"` 等效于字符串字面量 `"{a}"`。
  
  - 具体示例：
    
    ```python
    x = 1
    y = 2
    print(f"Value of {'x + y'} is {x + y}")
    # output: Value of x + y is 3
    
    # Notice that f-string literal is also an expression.
    print(f"{f"{True}"} == True")
    # output: True == True.
    
    print(f"The value of f\"{{True}}\" is \"{True}\"")
    # output: The formation of f"{True}" is "True"
    ```
    
    （涉及了一些后面出现的语法，可以看完后回来理解。）

- 空值字面量：`None`
  
  - 是一个单例占位符，用于表示变量不存储任何有效值。

#### 2.4 运算符与分隔符（Operators and delimiters）

- 代数运算符：加号 `+`，减号 `-`，乘号 `*`，浮点除 `/`，整除`//`，取模 `%`
- 比较运算符：大于 `>`，小于 `<`，大于等于 `>=`，小于等于 `<=`，不等于 `!=`，等于 `==`
- 赋值运算符：赋值 `=`
- 复合赋值运算符：加法 `+=`，减法 `-=`，乘法 `*=`，浮点除 `/=`，整除 `//=`，取模`%=`
- 分隔符：括号 `()`，逗号 `,`

## IV 语法规则

这一部分描述语法分析器（Parser）期待的合法程序的语法特征。这一部分已经由 Antlr4 工具完成构建，并将其分拆成表达式树（Abstract Semantic Tree，AST）模式。你需要编写对表达式树的遍历逻辑，同时完成对相关数据的维护。

具体的 Parser 规则你可以参考对应的 g4 文件，这里不再叙述。这里叙述一些相较于 AST 更加抽象的，你实现时更需要关心的部分：

### 1. 类型系统

你（可能）需要支持以下六种数据类型：

- `None`：空值单例。

- `bool`：布尔逻辑类，只允许两个值 `True` 与 `False`。

- `int`：高精度整数。你应该有一个 `int2048` 来模拟它。

- `float`：双精度浮点数，同 C++ 的 `double`。

- `str`：字符串，<font color=red>不可变</font>（没有下标运算符...），类似 C++ 的 `std::string`。

- `tuple`：元组，可以按组存储一系列数据，类似 C++ 的 `std::tuple`。
  
  - 你应当只需要处理函数返回语句与赋值语句中，使用逗号表达式隐式临时产生的元组。例如 `a, b = b, a` 与
    
    ```python
    def f():
        return 1, 2, 3
    p, q, r = f()
    ```
  
  - <font color=yellow>让一个具名变量存储元组是未定义行为，显式构造元组也是未定义行为。</font>
  
  - 不要求同一个元组中的数据类型相同，例如 `(int, str, None, None)` 是合法的。

请注意，Simplified Python 支持的五种可绑定变量的数据类型（`tuple` 不算）全部是不可变数据类型，所以标准 Python 中赋值/函数参数传递等行为中，<font color=yellow>按引用传递退化为按值传递</font>。

### 2. 表达式（expressions）

#### 2.1 基础表达式

即字面量，变量与函数调用，它们都可以代表一个数据对象。

~~也许可以再说的明白一点...~~~

#### 2.2 算术表达式+逻辑表达式

##### 2.2.1 运算符优先级

运算符优先级从高到低（同一行优先级一致）为：

```python
()
* / // %
+ -
< > <= >= == !=
not
and
or
```

优先级一致的从左往右执行。

##### 2.2.2 运算符行为

运算行为基本与标准 Python 一致，可以参考 C++。下面列出一些特殊行为：

- 隐式类型转换
  
  - `bool`，`int`，`float`，三种数值类型之间可以隐式转换，具体行为可以参考 C++ 的 `static_cast`。
  - 在不同数值类型的运算与比较中，较低级的数值类型会隐式转换为较高级的数值类型。
  - 如果期望一个较高级的数值类型但提供的是较低级的数值类型，会发生隐式转换，如下面提到的 `str * int` 表示支持 `str * bool`，但不支持 `str * float`。
  - `None`，`str`，`tuple` 对象不会参与任何隐式类型转换。

- 字符串行为
  
  - 允许 `str + str`，表现为字符串拼接
  
  - 允许 `str * int` 与 `int * str`，表现为字符串复制。
  
  - 允许对两个 `str` 之间应用六种比较运算符，行为同 C++ `std::string` 间的比较。

- 除法与取整除法
  
  - `/` 表示实数除法，结果总是一个 `float`。
  
  - `//` 表示向下取整除法（floor division），完成除法后会将结果向下取整。在参数全为 `bool` 或 `int` 时返回一个 `int` ，在参数有 `float` 时返回一个 `float`。
  
  - 如：`5 // -3 = -2`，`-10.2 // 3 = -4.0`。

- 相等性比较
  
  - 四个偏序比较 `>=`，`<=`，`>`，`<` 要求两个比较对象在允许使用隐式转换的前提下是同一个类型，不然算作出现运行时错误。`==` 与 `!=` 则允许二者即使允许隐式转换也不能归为同一个类型，此时该比较表达式的结果是 `False`。
  
  - 例：`1 == "1"` 等同于 `False`，`1 <= '1'` 会造成一个运行时错误。

- 逻辑运算符
  
  - 你可以将 `not`，`and`，`or` 视作 C++ 中的 `!`，`&&` 与 `||`。
  
  - 它们接受的操作数类型都是 `bool`，允许隐式类型转换。
  
  - `and`，`or` 是短路运算符，即逻辑上不必要的表达式根本不会被求值。比如：
    
    ```python
    sign = 0
    def f():
        sign += 1
        return False
    result1 = False and f()
    result2 = True or f()
    result3 = f() and f() and f()
    # sign = 1. Only the first f() in result3 is called.
    
    def g(i):
        return i <= 1 # will cause a runtime error if i is a string
    result = True or g('1')
    # And nothing happens. g is never called.
    ```

- 连续比较
  
  - 允许链式比较，如 `a <= b == c >= d`。
  - 其语义与行为即为将其拆开为多个二元比较，并用 `and` 链接。比如 `f() < g() < h()` 的行为就类似：
    - 计算 `_f = f()`，`_g = g()`
    - 如果 `_f < _g` 不成立，则表达式值为 `False`。`h()` 不会被调用。
    - 如果 `_f < _g` 成立，则计算 `_h = h()`，比较 `_g` 与 `_h`。
  - 这种行为保证，每个表达式至多被求值一次。

### 3. 语句（statements）

#### 3.1 赋值语句

##### 3.1.1 基本赋值语句

```python
var = Expr
```

其意义是将 `expr` 求值后，作为一个右值赋值给 `var` 对应的变量。

再次强调，在我们的语法中，所有的赋值行为都可以被视作<font color=yellow>按值传递</font>。

如果在当前语境下，没有存活的名称为 `var` 的变量，则该变量会被即刻定义，开始其生命周期。如果存在生命周期未完的 `var` 变量，则它指向的就是这个变量。

赋值语句是函数列表参数以外唯一可以定义变量的操作。<font color=red>它无法遮蔽全局变量。</font>

##### 3.1.2 序列解包赋值语句

本行为是对标准 Python 语法中可迭代对象解包（Iterable unpacking）的简化实现。

在 Simplified Python 中，保证可迭代对象解包只会出现以下两种情形（<font color=yellow>同时也是 `tuple` 数据类型的唯二作用处</font>）：

```python
var1, var2, ..., varN = Expr1, Expr2, ..., ExprN
```

```python
def f(...):
    ...
    return Expr1, Expr2, ..., ExprN
...
var1, var2, ..., varN = f(...)
```

这两类赋值两边序列的长度应当一致，若不一致则产生了运行时错误。

第二类可以约化成第一类。第一类的具体行为可以等效为：

1. 完成所有的 `Expr` 的求值。这一阶段中如果用到了 `var1` 等变量，它们会保持原有的值不变。我们记
   
   ```python
   _expr1_result = Expr1
   _expr2_result = Expr2
   ...
   _exprN_result = ExprN
   ```

2. 从左到右进行赋值，即
   
   ```python
   var1 = _expr1_result
   var2 = _expr2_result
   ...
   varN = _exprN_result
   ```
   
   当然，这些 `_expr_result` 是你需要在解释器中临时存储的数据，它们不应该在你的程序中拥有超出原始赋值代码的生命周期，类似 C++ 的右值语义。
   
   请注意，这与直觉上是求值与赋值交替进行，即仅仅是一系列的 `vari = Expri` 连续执行<font color=red>不一致</font>。比如说，`a, b = b, a` 不表现为 C++ 中 `a = b, b = a` 语句执行后 `a, b` 均拥有操作前 `b` 对象的值这种行为，而是可以完成 C++ 中 `std::swap(a, b)` 的行为。
   
   > 在标准 Python 中，由于存在引用与下标运算符的设计，这一步内部可能出现相互影响，体现它们并非并行进行。比如：
   > 
   > ```python
   > x = [1, 0]
   > i = 1
   > i, x[i] = x[i], i
   > ```
   > 
   > 你可能会期望第三条语句将 `x[1] = 0` 与 `i = 1` 作调换，最终得到 `x = [1, 1]` 与 `i = 0`。但是根据标准 Python 规则，第三条语句实际上的过程：
   > 
   > ```python
   > _1 = x[i] # _1 = x[1] = 0
   > _2 = i    # _2 = 1
   > i = _1    # i = 0
   > x[i] = _2 # x[0] = 1, notice that i = 0 now
   > ```
   > 
   > 最后 `x = [1, 0]`，`i = 0`。这里后续的赋值语句受到了前面赋值的影响。
   > 
   > 不过呢 Simplified Python 里面不涉及这样的复杂设计，不会出现这样的事情。即使出现了也不影响你采用这种两阶段解释方法来解释赋值语句。

##### 3.1.3 连续赋值语句

赋值语句允许连续赋值，表现为：

```python
var1 = var2 = ... = varM = varN = Expr
```

其行为与以下语句相同：

```python
varN = Expr
varM = varN
...
var1 = var2
```

> 请注意，即使在原版 Python 中，赋值语句也仍然不是一个表达式。虽然存在 `a = b = c = None` 这种连续赋值语法，但是整个赋值语句仍然不能被放在另一个需要表达式的位置。（在 Simplified Python 的 Parser 规则中表现为 expr_stmt 的 stmt 后缀，以及 test 不兼容 expr_stmt）
> 
> 在标准 Python 3.8+ 中引入了海象运算符 `:=` ，可以用来解决这个问题。

#### 3.2 增量赋值语句

即为二元算术运算符与赋值运算符的结合，不支持连续增量赋值，支持序列解包增量赋值。

增量赋值语句不能用于定义变量，赋值语句左侧的变量必须已被定义且可见。

具体行为可以参考标准 Python 与 C++。~~这里不想写了~~。

#### 3.3 表达式语句

一个表达式也可以独立成为一个语句，表达式的返回值会被立即丢弃。

#### 3.4 条件判断语句

条件判断语句可以成链，形如：

```python
if expression_1:
    # code block
elif expression_2:
    # code block
else expression_3:
    # code block
```

`elif` 类似 C++ 的 `else if`，它的优势在于可以避免增加缩进层数。`else` 必须在链的最后，`elif` 与 `else` 块可以不存在。具体行为类似 C++。

（短代码段 `simple_stmt` / 长代码块 `suite` 要求的缩进层数差异在这个功能中起到了作用，它具体体现在 Parser 阶段，本次作业不考察相关内容。）

判断条件 `expression` 期望可以隐式转化为 `bool` 的值。

#### 3.5 循环语句

仅支持 `while` 循环：

```python
while expression:
    # code block
```

具体行为类似 C++。

判断条件 `expression` 期望可以隐式转化为 `bool` 的值。

#### 3.6 控制流跳转语句

包括 `break`，`continue` 与 `return`。

- `break` 用于跳出本层 `while` 循环。（保证它在一个 `while` 循环内）

- `continue` 用于跳过本次 `while` 循环剩余的部分，直接开始下一次该 `while` 循环的 `condition` 判定。（保证它在一个 `while` 循环内） 

- `return` 用于结束本层函数调用并给出本次调用的返回值。（保证它出现在一个函数的定义内）
  
  - `return` 可以接一个表达式（可能用逗号组合为元组），比如 `return a` 或 `return 1, 3, b`； 也可以不接表达式，等效于 `return None`。

### 4. 函数（functions）

函数必须在声明时立即定义。

#### 4.1 函数的定义

```python
def func(parameters):
    # code block


# instance:
e0 = "str"
def estr():
    return "estr"
def f(a, b, c, d = 1, e = e0):
    e0 = "e0"
    # The default value for parameter e in function f is still "str".
    if d == 1:
        return a, b, c
    elif d == estr():
        return e

e0 = "other"
# The default value for parameter e in function f is still "str".
```

- 参数列表如 `a, b, c`，变量名之间用逗号分隔，可以为空。

- 有些变量可以有默认值，但是都必须出现在无默认值的变量后面。（这点可以用来消歧义，语法规则文件里没有提但是 Antlr4 应当已经做了检查。）

- 变量默认值在函数定义时立即求值并确定。

- 函数可以有返回值，也可以没有返回值，无需显示声明。同一个函数内部的多个 `return` 返回值类型可以不一样。

- 返回值可以为多变量组成的元组，如 `return a, b`

- 如果执行到了函数尾部却仍然没有遇到一个 `return` 操作，则进行 `return None`。

- 保证不会出现嵌套函数，即函数里面定义函数的情况。

#### 4.2 函数调用

```python
func(parameters)

# instance:
# def estr(): return "estr"
# def f(a, b, c, d = 1, e = e0), e0 was "str" at definiton.
estr()
f(1, 2, None, 2) # calls f(1, 2, None, 1, "str")
f(2, c = "str", b = 1.0, e = estr()) # calls f(2, 1.0, "str", 1, "estr")
```

- 函数调用必须出现在该函数定义后。

- 参数传入有两种形式：关键字参数（keyword argument）和位置参数（positional argument）。我们以 `def foo(a, b)` 为例：
  
  - 关键字参数：即显式指定将参数传递给特定变量。比如 `foo(b = 2, a = 1)` 表示传入参数 `a` 的值为 `1`，`b` 的值为 `2`。
  
  - 位置参数：即按照函数定义时出现的参数顺序传值。比如 `foo(1, 2)` 表示传入第一个参数 `a` 的值为 `1` ，第二个参数 `b` 的值为 `2`。 
  
  - 若一个参数列表中同时有两种参数形式，那么位置参数必须出现在所有关键字参数之前，如 `foo(1, b=2)`。
  
  - 每一个函数定义中未赋默认值的参数都应该以两种参数传入方法之一的形式赋值，未能做到此条的函数调用会产生运行时错误。

- 在以上两种参数传递完成后，函数应当新开一层作用域，并在此作用域中将参数列表中的所有变量完成定义与赋值。赋值的优先级是：参数传递优先于默认赋值。这些函数列表变量的生命周期与该层函数调用相同。

- 函数列表参数是除赋值语句以外唯一可以定义变量的操作。它可以遮蔽全局变量。

### 5. 内置函数（builtin functions）

Simplified Python 有以下五个内置函数，它们不需要输入文件定义即可被使用（甚至如果输入文件尝试重定义它们，反而是一种未定义行为）。

- `print`：向标准输出写数据。
  
  - 可以有任意个参数，逐个输出，中间用空格分隔，输出后换行。
  
  - <font color=yellow>尝试输出 `tuple` 是未定义行为。</font>
  
  - <font color=red>输出 `float` 时强制保留 6 位小数。</font>
  
  - 输出字符串不要输出引号，输出内容即可。
  
  - 例：`print("123", 1.0)` 会输出 `123 1.000000` 并换行。
  
  - 例：`print('AB', "", None, False, -1, -1.0)` 会输出 `AB  None False -1 -1.000000` 并换行。

- `int`：单参数函数，将 `float` 或 `bool` 或 `str` 显示转换为 `int`。保证转换内容合法不溢出。
  
  - 转换 `float, bool` 的行为类似 C++ 的 `static_cast<int>` 。
  
  - 转换 `str` 的行为同 `int2048::read(const std::string &)` 。

- `float`: 单参数函数，将 `int` 或 `bool` 或 `str` 显示转换为 `float`。保证转换内容合法不溢出。
  
  - 转换 `int, bool` 的行为类似 `static_cast<double>`，
  
  - 转换 `str` 的行为同 `std::stod(const std::string &)`。

- `str`：单参数函数，将 `int` 或 `float` 或 `bool` 或 `None` 显示转换为 `str`。保证转换内容合法不溢出。
  
  - 转换 `int` 的行为类似 `int2048::print()` 的行为，但转化结果应当被写入字符串而非输出。
  
  - 转换 `float` 的行为同 `std::to_string(double)`， <font color=red>转换强制保留 6 位小数</font>。
  
  - `str(True) = "True"`，`str(False) = "False"`。 
  
  - `str(None) = "None"`。
  
  - <font color=yellow> 不支持对 `tuple` 的转换。</font>

- `bool`：单参数函数， 将 `int` 或 `float` 或 `str` 或 `None` 转成 `bool`。
  
  - 转换 `int, float` 的行为同 C++ 的 `static_cast<bool>`。
  
  - 对于 `str`，如果对象是 `""` 则转换结果为 `False`，否则为 `True`。
  
  - `bool(None) = False`。
  
  - <font color=yellow>不支持对 `tuple` 的转换。</font>

上述转换函数中，请自行将 C++ `int` 相关转换推广为 `int2048` 相关转换；

### 6. 作用域与生命周期（Scope and lifetime）

<font color=red>定义在最外层的全局变量在全部位置可见，不需要 global 关键字等即可访问。</font>全局变量的生命周期持续到程序结束。

只有函数调用会产生新的作用域，代码块不构成代码块级作用域。定义在函数级作用域中的变量的生命周期从一次函数调用中定义该变量时开始，在该次函数调用结束时结束。

函数作用域应当构成栈形，<font color=yellow>栈深度超过2000层为未定义行为</font>。

> 关于 Simplified Python 作用域规则下的变量可见性：
> 
> 在全局最外层可访问的变量是：所有已定义的全局变量。
> 
> 在函数调用中可访问的变量是：所有已定义的全局变量，函数参数列表定义的局部变量以及所有在**该层**函数调用中已定义的局部变量。
> 
> 赋值语句检测变量是否已定义时会考虑全局变量，所以无法通过赋值语句让局部变量覆盖全局变量。但是函数参数列表引入的局部变量会覆盖全局变量。这点与 C++ 的行为类似。
