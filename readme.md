# C Compiler

实现C语言到8086汇编的编译。编译前端采用LR1分析法，使用[ANSI C 文法](http://www.lysator.liu.se/c/ANSI-C-grammar-y.html)。

### build

Require C++14

1. ```cmake```
2. ```make```

### usage

``` Compile file-name [-o]```

file-name：待编译的C语言源程序文件。

-o：开启优化。

程序执行时在控制台输出提示信息，正确执行后在源文件同一目录生成以下文件

1. file-name.asm: 生成的目标代码文件。
2. file-name.tokenlist: 对源程序进行词法分析得到的Token序列。
3. file-name.quatlist: 生成的中间代码文件。
4. file-name.symboltable: 生成的符号表文件。

程序执行后还会在程序目录生成以下文件

1. grammar_tree.json: 对源程序进行LR1语法分析得到的语法树文件
2. grammar_tree.html: 显示语法树的工具程序
3. LR1_table.html: LR1分析表

### reference

+ [C Lex code](http://www.lysator.liu.se/c/ANSI-C-grammar-l.html)
+ [C Yacc code](http://www.lysator.liu.se/c/ANSI-C-grammar-y.html)