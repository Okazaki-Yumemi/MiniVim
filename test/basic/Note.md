这些是ACMOJ上面的BASIC模块对应的所有测试点
你可以在本地环境运行以确保你的MiniVim输出和我们期望的答案相同
我们的ans均来自Vim的输出
比如说  ./vtemu/bin/vtemu -l 24 -c 80 -x 20 vim < ./test/basic/1.in > ./test/basic/1.ans
该指令生成了1.in测试点对应的1.ans文件
你只需要把指令里面的vim换成你的MiniVim 
即 ./vtemu/bin/vtemu -l 24 -c 80 -x 20 ./MiniVim < ./test/basic/1.in > ./test/basic/1.out
若 diff ./test/basic/1.out ./test/basic/1.ans 没有输出,则你的屏幕输出正确; 第18、20点还需要检查保存的文件

这里的编号和网站一致, 第9点分成9-1、9-2两次运行, 第18点分成18-1到18-4四次运行.


.save.ans是保存文件的答案, 不是屏幕输出. 第18点需要在同一个目录下依次运行18-1到18-4, 每次重新启动MiniVim, 中途保留生成的文件:
- 18-1、18-2、18-3每次运行后, 分别用cmp比较save1与对应的18-1.save.ans、18-2.save.ans、18-3.save.ans. 
- 18-4运行后, 用cmp比较save2与18-4.save.ans; save2应当存在且为空文件.
- 第20点运行后, 执行cmp save ./test/basic/20.save.ans检查保存内容.
