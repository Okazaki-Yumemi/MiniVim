#### 光标移动

1. `h,j,k,l` (已完成)
2. `0,$,^` (0: 移动到行首， $: 移动到行尾， ^: 移动到第一个非空白字符)

0: Motion (拓展Motion) [已完成]
\$: 拓展 () [已完成]
^: 拓展Motion [已经完成]

3. `gg,G`:

gg: 移动到文件首行,落在该行第一个非空白字符 (耦合一个MoveToFirstNoneEmpty)

**Notice**: 这个地方要注意，开始双字符输入。

[完成]



G: 移动到文件末行,落在该行第一个非空白字符 (耦合一个MoveToFirstNoneEmpty) [完成]
4. `w,e,b,ge,W,E,B,GE`//写错了，这个地方是gE  gE ? or GE?

- w/W 跳到下一个 word / WORD 的开头
- e/E 跳到当前或下一个 word / WORD 的结尾
- b/B 跳到前一个 word / WORD 的开头
- ge/gE 跳到前一个 word / WORD 的结尾

vim中： word - 字母数字/下划线这类字符 标点符号 分成不同的块

例子:
```
hello,world foo_bar
```

->
```
hello
,
world
foo_bar
``` 


WORD 只看空白
```
hello,world foo_bar
```
->
```
hello,world
foo_bar
```
设计:

这个地方，他的行为主要是

-  词尾，词头
-  大写，小写
-  前 / 后


先写 前 / 后 的判断

然后 找词头/词尾的函数

里面耦合一个判断方式的大小写区别, word 找非字母，用isalpha，WORD 找空格，用isspace

FindNextStart -> w/W
FindNextEnd -> e/E
FindPrevStart -> b/B
FindPrevEnd -> ge/gE

先用FindNextStart来耦合word于WORD， 把这个拆进去，传入buffer 和 bool.
true: word
false: WORD

-> w -> Window::FindNextStart(buffer, true)

最好再写一个Classify函数吧？

Classify(characeter, mode) 

mode = word的时候，返回 WordChar,Punctuation, Blank

mode = WORD的时候，返回 NoneBlank, Blank

// 判断写好了，看看怎么分类:

用一个统一的函数

第一层 if/ else 分 大小写

第二层 分为四个函数:

NextStart
NextEnd
PrevStart
PrevEnd

换行咋整。？

【已完成】

5. `%`
6. ``m,` ``
7. `\,?,n,N`

#### 文本编辑

1. `i,I,a,A,o,O,Del`

i: 在光标前插入文本，进入插入模式  [ok]
I: 在行首插入文本，进入插入模式  [ok]
A: 在行尾插入文本，进入插入模式  [ok]



2. `x,X`
3. `d,dd,D,x,X`
4. `c,C`
5. `r`
6. `<,>,<<,>>`
7. `y,p,P`
8. `",q,@`
9. `~`
10. `J`
11. `.`
12. `u, C-r`

#### 模式切换

1. `:`
2. `v`
3. `ESC`

#### 文件操作

1. `:w`
2. `:q`
3. `:wq`
4. `:edit`