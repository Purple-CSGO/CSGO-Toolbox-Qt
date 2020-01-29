# CSGO工具箱 项目说明
> 2020.01.2 21pm last edited by Purp1e

## 一、CSGO工具箱介绍

CSGO工具箱是旨在方便CSGO玩家调整、修改、保存、备份和转移设置等的工具性质的GUI程序，减少玩家在调整设置等事项上花费的时间，从而节省出更多的时间投入游戏本身。

> CSGO Toolbox is dedicated to enabling CSGO players to modify their configs, game settings etc much more swiftly, thus more time is saved to put on game itself.

## 二、项目开发须知

### 1. 开发环境

本项目使用[Qt 5.14.0](http://download.qt.io/archive/qt/5.14/5.14.0/) + [MSVC 2017](ed2k://|file|en_visual_studio_community_2015_x86_dvd_6847364.iso|3965825024|6A7D8489BB2877E6BB8ACB2DD187B637|/) + [qt-vsaddin-msvc2017-2.4.3](http://download.qt.io/archive/vsaddin/2.4.3/) + Git，系统Win10 64位。如果需要参与开发，请配置好这些环境。

当前项目要用到一些外部文件，在`.dll/`中，需要手动将这些文件移动到`debug`或`release`文件夹中。

### 2. 参与开发

可联系邮箱`438518244@qq.com`，或者QQ群`759990621`，开发过程中需要用到[Git操作](https://www.jianshu.com/p/02cf41f38b6a)，请提前备好相关知识。

### 3. 讨论&添加功能|特性须知

使用GitHub的[issues](https://github.com/Purple-CSGO/CSGO-Toolbox/issues)和[milestone](https://github.com/Purple-CSGO/CSGO-Toolbox/milestones)实现：

1. 有任何打算实现或者将要实现或者提议都发布在issues里，标题简述功能，内容详细说明，分成n条，管理者根据情况打上`将要实现`的标签。
2. 某功能相关建议或说明第一次提出时创建`milestione`，并将该issue加入。
3. 当开发者正在实现某功能时，标签改为`正在实现`。
4. 功能已经调试完成之后，标签改为`已实现`。
5. 功能完整加入程序中并确认无误之后，标签改为`已部署`，并关闭issue。
