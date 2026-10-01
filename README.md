# MiniVim

## 项目初衷

> 一个面向大一学生的 C++17 课程项目: 从零实现一个可以在终端中运行的, 具有基本 Vim(version 9.1) 操作方式的文本编辑器.

完成项目后, 你应该能够:

1. 熟悉相对现代 C++ 的基本语法与 STL, 如 类, std::filesystem, std::optional, std::vector;(Basic)
2. 掌握基本的 **模块化设计** 思想, 将一个复杂问题拆分成若干职责明确, 相互协作的模块;
3. 理解 **封装** 的意义, 通过类的公开接口隐藏内部状态和实现细节;(Advanced, 你极有可能在重构代码的时候真正体会到以上两点)
4. 对 Vim 之类的文本编辑器的底层设计实现有新的了解.(Just For Fun)

## 项目架构

### 框架与目录结构

MiniVim 使用 C++17 ,希望实现一个行为基于Vim9.1的终端文本编辑器。项目为 Basic 部分提供了教学框架，帮助大家把文本存储、光标移动、命令解析和终端显示拆分为职责明确的模块。当然你也可以从零设计实现，只要满足项目规定的外部行为与评测要求；对于第一次写项目的同学，请优先沿用框架。

项目中的主要文件如下。

```text
MiniVim/
|-- Makefile
|-- *this file
|-- src/
|   |-- Types.hpp, Key.hpp, TextLayout.hpp
|   |-- Buffer.hpp, Buffer.cpp
|   |-- Window.hpp, Window.cpp
|   |-- Command.hpp, Command.cpp
|   |-- Renderer.hpp, Renderer.cpp
|   |-- Editor.hpp, Editor.cpp
|   |-- Terminal.hpp, Terminal.cpp
|   `-- main.cpp
|-- test/basic/
|       |-- Note.md
|       |-- 1.in ... 28.in
|       `-- 1.ans ... 28.ans
-- docs/images/key-processing-flow.png
```

**框架中直接抛出"Not implemented"和有注释要求填写的地方都是你需要补全的函数**

### src内各文件简要介绍

| 文件                   | 模块职责                                      |
| ---------------------- | --------------------------------------------- |
| `Types.hpp`、`Key.hpp` | 定义了一些基本类型。                          |
| `TextLayout.hpp`       | 辅助函数头文件,帮助处理文本显示。             |
| `Buffer.hpp/.cpp`      | 按行保存文件内容, 处理底层的文本修改。        |
| `Window.hpp/.cpp`      | 作为视口，限制合法位置并确保光标可见。        |
| `Command.hpp/.cpp`     | 把 `NormalMode` 的按键解析为动作。            |
| `Renderer.hpp/.cpp`    | 根据Buffer,Window的状态生成一帧的完整字符串。 |
| `Editor.hpp/.cpp`      | 协调链接各模块成完整编辑器。                  |
| `Terminal.hpp/.cpp`    | 已提供的终端基础逻辑。                        |
| `main.cpp`             | 已提供的程序入口。                            |

对于这些文件更加具体的描述我们作为注释写在了这些文件里,请不要忽视这些注释

### 建议的阅读与实现顺序

阅读代码时,建议先分文件略读,了解各文件内接口;再从main函数中的run(),自顶向下过一遍一次按键流入的逻辑。

实现时建议分阶段推进：

1. **基础数据** 从简单的部分开始, 完成按键辅助函数，实现 Buffer 来储存实际文件内容。
2. **光标与解析** 实现 Window 的位置限制和移动，以及 NormalModeParser 的 Basic 单键解析。
3. **画面生成** 实现 Renderer 的帧生成逻辑。
4. **完整Basic** 实现 Editor, 此时可以在终端里尝试使用你的MiniVim,使用的过程本身就是一种debug。
5. **Advanced** 此时你已经完全理解了框架,可以自己在框架之上修改来实现Advanced部分的功能了

### 项目的运作流程

![](figure/key-processing-flow.png)

`Editor::ProcessKey()` 是按键分发唯一的入口。Basic 中只有 Normal 模式先经过 `NormalModeParser::Feed()` 生成 `EditorAction`，再由 `Editor::Execute()` 执行动作。Insert 与 Command-line 分别交给 `HandleInsert()` 和 `HandleCommandLine()`。

## 项目开发常识

### 编译与运行

下面以 Ubuntu 24.04 环境为例：

```text
sudo apt install build-essential
g++ --version
make --version
```

所有未另行说明的项目命令均在 MiniVim 根目录执行。

**当前项目的 Makefile 默认设置是 `TARGET := MiniVim`，但是提交的时候ACMOJ只能识别叫做"code"的二进制,所以如果提交,你需要改成TARGET := code** 接下来的讲解我们默认名字为MiniVim,你可以使用以下命令编译并运行：

```text
make    //编译
./MiniVim   //在当前根目录内运行MiniVim
./MiniVim example.txt
./MiniVim "notes with spaces.txt"
```

程序只接收零个或一个文件路径。没有路径时创建无名缓冲区；有路径时由 Buffer 按规则加载或初始化对应内容。

当你修改了src下文件的任何代码,你都需要重新编译生成新的二进制

请执行:

```text
make clean //清除当前的编译产物
make //重新编译
```

### Basic部分的Debug

完成 Basic 后，可按以下顺序进行一次人工检查：

1. 启动程序, 就像使用一个真正的文本编辑器一样自由使用你已经实现过的Basic功能。

如果你对于你的MiniVim在这些操作都很满意,那你就应该有自信认为你的程序正确
在你有自信的情况下,可以使用我们下发的basic部分测试点进行最终确认,我们保证这些测试点和ACMOJ上的Basic测试点要求是完全相同的

## 项目测评

- **下发测试** Basic部分的全部测试点,具体使用说明在test/basic/Note.md里
- **在线评分** Basic 最终得分以 ACMOJ 评测为准。Advanced部分测试点我们不会下发,但我们保证测评内容不会超出spec的要求。

### 项目测评工具vtemu的使用与介绍

#### 工具作用与准备

vtemu 是终端交互程序的脚本驱动器和屏幕采样器。它通过伪终端（PTY）运行 MiniVim，将.in文件的输入序列转换为按键传给MiniVim，再不断将虚拟终端的截图转化成文本序列，输出到.ans文件中。详情请见: [vtemu GitHub 仓库](https://github.com/c-w-Inf/vtemu)

这样,我们就可以利用重定向输入输出流来自动化评测你的MiniVim了!

具体使用的方式请见 basic/test/Note.md

但在使用之前,我们需要先编译它,先切换到vtemu的根目录

```bash
make all # 编译vtemu
```

你应该在/vtemu/bin下发现一个叫做vtemu的二进制文件

#### 脚本记号与采样

| 记号            | 含义                                                 |
| --------------- | ---------------------------------------------------- |
| `<SP>`、`<TAB>` | 空格、Tab                                            |
| `<CR>`、`<BS>`  | Enter、Backspace。                                   |
| `<ESC>`         | Esc                                                  |
| `<X>`、`<X300>` | 等待默认或指定时间                                   |
| `<EX>`          | Esc再等待默认时间。                                  |
| `<P-0;0;1;80>`  | 输出屏幕矩形[top, left, bottom, right]区域的文本快照 |
| `<CURP>`        | 输出光标坐标                                         |
| `<E>`           | 结束驱动器并清理子进程                               |

希望这些内容可以帮助你理解我们的下发测试点
(你会发现我们的测试点都是很仁慈的!)

## 项目实现要求

从此开始就进入了我们对于项目实现的指导，本项目 Basic 部分上限 60 pts, Advanced 上限 40pts, Extra 上限 10pts, 总分上限为100pts, 不溢出; Code Review 期间如果发现你对自己的代码了解不足，甚至不清楚自己在写什么，会对该对应部分功能代码的分数采取清零措施.

### Basic部分要求

你必须实现 Basic 部分才能获得基础分数并继续完成 Advanced 部分与 Extra 部分. Basic 部分的最终得分即为 ACMOJ
上评测得到的分数.

# 按键记号约定

文档中常使用 `$<>$` 记法标注按键。有时代表实际输入键 (为清晰起见)，多数场景下代表按本义输入序列, 用 `<key>` 标记输入的特殊键。对应下表的按键名称。

更具体的, 我们给出本项目中可能用到的按键名称与含义对照表:

| 按键名称  | 含义                      |
| :-------- | :------------------------ |
| `<BS>`    | 退格键                    |
| `<Tab>`   | 制表符                    |
| `<CR>`    | 回车键                    |
| `<Esc>`   | 转义键                    |
| `<Space>` | 空格键                    |
| `<Del>`   | 删除                      |
| `<S-...>` | 组合 Shift 键, shift+...  |
| `<C-...>` | 组合 Control 键, ctrl+... |
| `<C-G>`   | 组合 Control 键, ctrl+G   |

#### Basic 概述

MiniVim 的哲学是模态编辑, 高效的编辑操作依托各模式间的切换完成. 简而言之, 在 Basic 部分中, 你需要为 MiniVim 实现最基础的
3 种模式:

- **Normal**: 移动光标, 执行编辑动作或进入其他模式;
- **Insert**: 插入和删除文本;
- **Command-line**: 输入保存, 退出等以 `:` 开始的命令;

并为每个模式实现一些最基础的功能.

- 在 Basic 部分, 你可以简单地认为屏幕由两部分构成: 屏幕最下面一行的保留部分 (用于输入命令, 显示信息等), 以及上面其余的编辑部分, 即缓冲区.
- Normal 模式与 Insert 模式主要操作缓冲区, 而 Command-line 模式则会将保留部分当成一块用于输入命令的命令缓冲区来操作.
- 在 Basic 部分中, 所有要求的功能在终端上打印的字符都以默认颜色与属性呈现 (具体来说, 都使用默认前景色与背景色; 没有额外终端字符属性, 如粗体). 也就是说, 你不需要操作任何颜色与属性即可通过 Basic 评测.
- `<C-q>` 在该项目中是未被绑定的保留键位. 你可以将其实现为紧急退出键, 按下后在任意模式下结束程序, 以便方便调试 Basic 及之后的部分. 我们不会对该键行为进行评测.

#### Normal 模式 (Basic)

在 Normal 模式中, 光标总会停留在某一个字符上 (对于空行, 则只能停留在不存在字符的位置) .

在 Basic 部分, 你需要为 Normal 模式实现以下的功能键.

1. 光标移动: `h,j,k,l`, 且不要求实现其他额外功能
   具体的, 你需要实现:

##### 上下左右移动:hjkl

- `h`: 光标向左移动一个字符；如果光标在行首则不变。
- `l`: 光标向右移动一个字符；如果光标在行末则不变。
- `j`: 光标向下移动一条文本行；如果光标在最后一行则不变。目标行过短时停在其最后字符，空行停在第 0 列。
- `k`: 光标向上移动一条文本行；如果光标在第一行则不变。列的钳制规则与 `j` 相同。

2. 文本的修改：实现`i,a`

具体地,

- `i` 在光标所选字符之前进入 Insert 模式。若光标处在空行，则在该空行的唯一位置进入 Insert 模式。

- `a` 在光标所选字符之后进入 Insert 模式。若光标处在空行，则在该空行的唯一位置进入 Insert 模式。

3. Command Mode: 实现 `:`，要求能够进入 Command Mode

- 请注意对于空行, 该行的行末位置不存在实际字符; 而对于非空行, 该行的行末位置是最后一个字符的位置.
- 对于以上的移动的 curswant 功能, 不要求在 Basic 部分中准确实现, 但是在 Advanced 部分中会进行评测.

#### Insert 模式 (Basic)

在 Insert 模式中, 光标表示字符之间的插入位置: 在某字符之前, 或在最后一个字符之后.
进入 `i` 使用当前列; 进入 `a` 时非空行使用 `column + 1`, 空行仍为 0. 不在 Buffer 行内额外存储占位换行符.

在 Basic 部分, 你需要为 Insert 模式实现以下的功能键:

文本的修改：实现ascii码, Backspace, Enter, Esc键位的正常键入:`{printable ascii}`,`<BS>`,`<CR>`,`<Esc>`

具体地:

#### `{printable ascii}`

- 在光标所选字符之前插入输入的字符，并将光标自然移动到下一个位置上。
- 这部分功能键限定为所有 ASCII 码在 `0x20~0x7e` 之间的字符。

#### `<BS>`

- 删除光标所选字符之前的字符，并将光标自然移动到前一个位置上。
- 若光标处在行首（或空行），则拼接该行与上一行，并将光标自然移动到上一行原行末位置上。
- 若光标处在文件首，则无效果。

#### `<CR>`

- 在光标所选字符之前位置切分该行，换行至下一行，并将光标自然移动到下一行的行首。
- 行首、行尾、空行均允许拆行，不自动缩进。更新 Buffer 后设置 `(row + 1, 0)`，不复制原 curswant 到新行。

#### `<Esc>`

- 进入 Normal 模式，并将光标自然移动到前一个位置上。若光标处在行首则不移动光标。
- 有插入 count 时先完成回放；然后仅当列大于 0 才左移一次，不跨行；最后按 Normal 范围钳制并以实际列更新 curswant。`i<Esc>` 可以只改变光标而没有编辑节点，不要因此伪造 modified。历史录制见 [插入重放](MiniVim.md#insert-replay)。

#### Command-line 模式 (Basic)

在 Command-line 模式中：

1. 光标总会停留在窗口底部的命令输入部分，总体操作逻辑类似于 Insert 模式中的一行，并且此时行首必定以 `:` 开头。
2. 相较于 Normal 模式的状态，除了屏幕最下面一行，其余部分显示内容应保持不变。
3. 进入到 Command-line 模式时，初始输入的指令，即命令缓冲区部分为空。
4. 在 Basic 部分中，你只需要考虑光标处在行末位置的情形。也就是说，任何停留在 Command-line 模式的操作最终都会将光标移动到行末位置。

在 Basic 部分, 你需要为 Command-line 模式实现以下的功能键:

命令的键入：实现ascii码, Backspace, :wq, :q!, :quit! , `{printable ascii}`,`<BS>`,`<CR>`,`<Esc>`

- 若当前输入的指令为空字符串, 则不会有任何行为.
- 若当前输入的指令非法, 在 Basic 部分中你可以直接忽略执行它. 在 Basic 部分中, 我们不会对未提及的命令评测.

以下是上述命令实现的细则:

##### `printable ascii` (Basic)

- 在光标所选字符之前（在 Basic 部分中，即行末）插入输入的字符，并将光标自然移动到下一个位置上（在 Basic 部分中，即行末）。
- 这部分功能键限定为所有 ASCII 码在 0x20~0x7e 之间的字符。

##### `<CR>` (Basic)

- 执行当前命令缓冲区中的指令，待执行完毕后进入 Normal 模式。

##### `<Esc>` (Basic)

- 进入 Normal 模式，并将光标移动到进入 Command-line 模式前原来的位置。舍弃在 Command-line 模式中的所有输入。

##### `:wq [file]` (Basic)

1. 将缓冲区内容写入到该路径所在文件。
2. 若文件不存在，则创建新文件并写入。
3. 若文件已存在，则覆盖原有内容。
4. 在 Basic 部分中不需要考虑文件权限等问题。

特别地，任何末尾没有换行的文件，在保存的时候都应该添加一个换行；但是没有任何编辑操作的情况下（不包含移动操作），文件里没有多余的换行。

##### `:q!, :quit!` (Basic)

舍弃缓冲区内容并退出 MiniVim。

##### Command-line Mode 显示要求

不计分，但是建议实现：

- 实现相应模式时显示 `-- INSERT --`、`-- VISUAL --`、`-- REPLACE --` 等提示；命令行输入期间只显示命令行。
- Buffer 之外的填充行以 `~` 标识；Advanced 使用亮蓝色。
- Basic 使用默认前景色、背景色和字符属性，不要求额外颜色。

---

### Advanced部分要求

Advanced 部分包含若干可选功能。它们之间大部分相互独立，但有些功能存在依赖关系，这意味着部分功能的实现需要其他功能来支持；建议按照本 README 的行文顺序来实现或许会更为方便.

对于 Advanced 部分，你可以选择任意一部分功能来实现。

- 每个功能在 ACMOJ 中都有独立测试点计分，ACMOJ 给出的总得分在折算之后作为 Advanced 部分的实际得分。折算公式为 $y = f(x)$，其中 $x$ 为你的 ACMOJ 计分，$y$ 为你的实际得分。
- 如果你使用我们提供的框架，从本部分开始，除 Basic 中已经给出的公共接口与 `Terminal` 外，不再限制 `src/` 中其他模块的接口设计。
- 你可以添加新的类、数据结构、`public` / `private` 接口或源文件，但已经完成的 Basic 行为不得被破坏。
- 除非特别说明，本节中的命令语义按照 Vim 的行为定义。ACMOJ 的评测不会测试未提及的 Vim 行为与特性。若下文明确给出与 Vim 默认行为不同的要求，则以该要求为准。
- 有些功能的实现与大量操作相关联（如 `[count]{operation}`，许多操作都支持加上计数），则该功能的评分会在所有所关联的操作下独立评分，详见给分细则。

在这一部分你需要实现:

#### 光标移动

1. `h,j,k,l`
2. `0,$,^`
3. `gg,G`:
4. `w,e,b,ge,W,E,B,GE`
5. `%`
6. ``m,` ``
7. `\,?,n,N`

#### 文本编辑

1. `i,I,a,A,o,O,Del`
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

#### 窗口操作

`zt,zz,zb,<C-e>,<C-y>,<C-d>,<C-u>`

#### 评分标准

本部分总分共 300 pts, 按照公式换算为至多 40pts. 公式为:

$$
f(x) = \begin{cases}
    \frac{1}{3}x, & 0 \leq x \leq 30, \\
    \frac{1}{6}x + 5, & 30 < x \leq 90, \\
    \frac{1}{9}x + 10, & 90 < x \leq 180, \\
    \frac{1}{12}x + 15, & 180 < x \leq 300.
\end{cases}
$$

<a id="tab:vim_keys"></a>

Vim 按键功能对照及评分表:

| 实现功能                     | 分数及评分细则                                                                                                                                                     |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `h, j, k, l`                 | **6 pts**<br>curswant: 2 pts; [count] `h,l`: 2 pts; [count] `j,k`: 2 pts                                                                                           |
| `0, $, ^`                    | **10 pts**<br>`0`: 2 pts; `$`: 3 pts; [count] `$`: 2 pts; `^`: 3 pts                                                                                               |
| `gg, G`                      | **6 pts**<br>`gg`: 2 pts; `G`: 2 pts; [count] gg / G: 1+1 pts                                                                                                      |
| `w, e, b, ge`                | **12 pts**<br>`w,e,b,ge`: 8 pts; [count] w,e,b,ge: 4 pts;                                                                                                          |
| `W, E, B, gE`                | **8 pts**<br>`W,E,B,gE` with count: 8 pts;                                                                                                                         |
| `%`                          | **5 pts**<br>`%`: 3 pts; [count] `%`: 2 pts                                                                                                                        |
| `m`                          | **7 pts**<br>``m, ` ``: 3 pts; `m` with edits: 4 pts                                                                                                               |
| `\, ?, n, N`                 | **10 pts**<br>`\`, `?`: 3 + 3 pts; `n`, `N`: 1 + 1 pt; [count] `n`/ `N`: 1 + 1pt                                                                                   |
| `i, I, a, A, o, O, Del`      | **20 pts**<br>`i,a` with curswant: 2+2 pts; [count] `i,a,I,A,o,O`: 2+2+2+2+2+2 pts; `Del`: 4 pts                                                                   |
| `x, X`                       | **4 pts**<br>`x`: 2 pts; `X`: 2 pts                                                                                                                                |
| `d, dd, D`                   | **20 pts**<br>`d` charwise: 6 pts; `d` linewise: 4 pts; `dd`, `D`: 2+2 pts; `d, dd, D` with count: 2+2+2 pts                                                       |
| `c, cc, C`                   | **20 pts**<br>`c` charwise: 6 pts; `c` linewise: 4 pts; `cc`, `C`: 2+2 pts; `c, cc, C` with count: 2+2+2 pts                                                       |
| `r`                          | **3 pts**                                                                                                                                                          |
| `<, >, <<, >>`               | **10 pts**<br>`<,>`: 4 pts; `<<,>>`: 2 pts; with count: 2+2pts                                                                                                     |
| `y, p, P`                    | **34 pts**<br>`y` charwise: 6 pts; `y` linewise: 4 pts; `yy`, `Y`: 2+2 pts; `y, yy, Y` with count: 2+2+2 pts<br>`p,P` charwise: 4 + 4 pts; `p,P` linewise: 3+3 pts |
| `" , q, @`                   | **16 pts**<br>`"`: 4 pts; `q`: 6 pts; `Q, @`: 6 pts                                                                                                                |
| `~`                          | **2 pts**                                                                                                                                                          |
| `J`                          | **4 pts**                                                                                                                                                          |
| `.`                          | **7 pts**                                                                                                                                                          |
| `u, Ctrl-r`                  | **16 pts**<br>`u`: 4 pts; `Ctrl-r`: 8 pts; `U`: 4 pts                                                                                                              |
| `Visual Mode`                | **26 pts**<br>basic motions: 10 pts;<br>operators `d,c,y` with count: (2+2+2)*2 = 12 pts<br>`<,>`: 2 pts; `~`: 2 pts;                                              |
| `textobjects`                | **30 pts**                                                                                                                                                         |
| `:w, :q, :wq, :edit`         | **8 pts**<br>`:q, :q!`: 2 pts;<br>`:w, :w!`: 2 pts;<br>`:wq, :wq!`: 2 pts;<br>`:e, :e!`: 2 pts                                                                     |
| `scroll`                     | **6 pts**                                                                                                                                                          |
| `zt, zz, zb`                 | **6 pts**<br>2+2+2 pts                                                                                                                                             |
| `<C-e>, <C-y>, <C-d>, <C-u>` | **4 pts**<br>1+1+1+1 pts                                                                                                                                           |
