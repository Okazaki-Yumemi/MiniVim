# Basic 测试点使用说明

我们提供了一些测试点以便测试你的 Basic 部分.

```bash
diff <(vtemu mvim <test.in 2>/dev/null) test.ans
```

评测时, 请将 `test.in` 和 `test.ans` 替换为给出的实际输入与答案文件. 若你没有将 `vtemu` 所在位置追加到环境变量 `$PATH`
中, 请将上述的 `vtemu` 替换为你本地的 `vtemu` 的实际路径. 若 `diff` 没有输出, 说明你的评测结果与 `ans` 文件给出的一致,
恭喜你通过了该测试点.

对于部分测试点, 还存在 `test.save.ans`, 这说明该测试点还需要测试你的 `mvim` 是否将输入给出的文件内容正确保存到了文件中,
你还需要对比 `save.ans` 文件与保存到当前目录的文件的差异.

```bash
diff save test.save.ans
```
