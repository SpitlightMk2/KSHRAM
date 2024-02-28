# KSHRAM

KSHRAM是一个命令式的K-Shoot Mania谱面处理工具包。

KSHRAM: A command-style K-Shoot Mania chart editing toolpack.

KSHRAM = K-Shoot Mania chart in RAM.

KSHRAM以GNU GPL v3许可证发布。具体请参考GPL许可的官方文档。

KSHRAM is licensed under GNU GPL v3.

尽管这不是必须的，如果您愿意的话，请在使用本工具处理过的谱面的effector中以你喜欢的方式加上KSHRAM的名字来表示它参与了谱面的制作过程。比如您可以写：

```
[你的名字] feat. KSHRAM
```

Although not necessary, I would appreciate if you could add the name "KSHRAM" into your chart when this software is used in the chart's production. For example:

```
[your name] feat. KSHRAM
```


## 使用指南

在ksm editor当中有一项功能是在谱面当中插入注释。<br>
插入注释时写入特定的命令，使用KSHRAM处理，即可对谱面执行特定的修改操作。<br>
具体的命令书写方式请见[这里](doc/KSHRAM命令说明_CN.md)。

本程序有命令行式和由Qt实现的图形交互界面两种形式。

使用命令行式KSHRAM程序时，可以直接点击打开交互模式，或是按照

```
KSHRAM.exe 输入谱面路径 输出位置
```

的格式调用该程序。输出位置不写的话会自动输出到输入谱面同一路径下，文件名末尾会追加一个`_out`，不用担心会覆盖原有文件。

使用Qt图形界面时只需要选定输入输出位置，点击Run即可。

> 使用命令行模式的时候只能支持ASCII路径。


## How to use

The K-Shoot Mania editor has a function to insert comments into a chart.<br>
By writing commands in these comments and calling KSHRAM, these commands will be translated to certain manipulations on the charts.

The document of all available commands is [here](doc/KSHRAM命令说明_CN.md).

The program comes either in console style or with a GUI supported by Qt.

If using the console style program, You can directly call `KSHRAM.exe` and enter interactive mode, or call with args:

```
KSHRAM.exe [input chart path] [output path]
```

Output path is optional. If not given, the output chart will be placed in the same directory as the input, with `_out` inserted into the original file name. The program will not overwrite the input file unless it's explicitly told to do so.

If using the GUI style program, just pick the input and output and click "Run".

## 构建

目前没有任何额外的依赖。

直接在根目录使用CMake构建，即可构建命令行版本；

用Qt Creator(Qt6.6)打开根目录的CMake，即可构建带有图形界面的版本。


## How to build

No dependencies required(for now).

Run CMake in the root directory to build as a console-style program,

or open with Qt Creator(Qt6.6) to build as a Qt Widget program.

## 以后可能会有什么？

batch会支持输入参数，并且可以在内部对参数进行简单计算后再传到对应命令上。

lisp风格的条件分支语句。


## Possible to-do features

batch: will support parsing arguments, and support basic evaluation before sending to a certain command.

lisp-style conditional scripts.


## 请帮帮我

如果您愿意翻译命令说明文档，或者美化其格式/文件结构，我将不胜感激。

如果文档当中有任何错误（特别是笔误），请在issue中告知。


## Contribute to this project

Translations of the command document in need!