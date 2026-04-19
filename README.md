# Robocup2D 赛事自动化管理系统

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-5%2F6-green.svg)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)

基于 Qt 框架开发的 Robocup2D 赛事自动化管理系统，支持分组抽签、循环赛程控制、局域网通信及实时比分与排名统计。

## 功能特性

- **分组抽签**：自动进行队伍分组与抽签，支持自定义队伍数量
- **循环赛程管理**：自动生成并管理循环赛赛程，支持多轮次比赛
- **局域网通信**：基于 Qt Network 模块实现局域网内服务器与客户端通信
- **实时比分统计**：实时监控比赛进程，自动记录并更新比分
- **排名统计**：根据比赛结果自动计算并展示队伍排名
- **比赛回放分析**：支持 RCG 日志文件解析与数据导出
- **自动化启动**：一键启动比赛服务器与多个球队客户端

## 项目结构

```
team_select2d/
├── main.cpp                    # 程序入口
├── mainwindow.h/cpp/ui         # 主窗口界面
├── page.h/cpp/ui               # 页面管理
├── design.qss                  # 样式表
├── main.qrc                    # Qt 资源文件
├── autoplay/                   # 自动比赛模块
│   ├── autoplay.h/cpp/ui       # 自动比赛主界面
│   ├── dialog.h/cpp/ui         # 对话框界面
│   └── server.h/cpp            # 服务器进程管理
├── analysis/                   # 数据分析模块
│   ├── analysis.h/cpp/ui       # 分析界面
│   ├── world_model.h/cpp       # 世界模型
│   ├── physics.h/cpp           # 物理模型
│   └── rcg2csv.h/cpp           # RCG 日志转 CSV 工具
└── CMakeLists.txt              # CMake 构建配置
```

## 环境要求

- **C++ 编译器**：支持 C++17 标准
- **Qt 版本**：Qt 5.15 或 Qt 6.x
- **构建工具**：CMake 3.16 或更高版本
- **操作系统**：Linux

## 编译与运行

### 使用 CMake 构建

```bash
# 克隆项目
git clone https://github.com/your-username/team_select2d.git
cd team_select2d

# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
cmake --build .

# 运行
./team_select2d  # Linux
```

### 使用 Qt Creator

1. 使用 Qt Creator 打开 `CMakeLists.txt`
2. 配置 Kit（选择 Qt 版本与编译器）
3. 点击构建并运行

## 使用说明

### 1. 自动比赛模式

- 点击主界面的 **Autoplay** 按钮进入自动比赛模块
- 设置参赛队伍数量
- 选择球队可执行文件路径
- 点击 **Start** 开始比赛，系统将自动：
  - 生成赛程
  - 启动服务器
  - 按赛程启动对应球队
  - 实时记录比分

### 2. 数据分析模式

- 点击主界面的 **Analysis** 按钮进入数据分析模块
- 选择 RCG 日志文件路径
- 设置输出目录
- 系统将解析日志并导出为 CSV 格式数据

## 技术栈

- **UI 框架**：Qt Widgets
- **网络通信**：Qt Network
- **进程管理**：QProcess / QThread
- **构建系统**：CMake
- **样式设计**：QSS (Qt Style Sheets)

## 核心模块

| 模块               | 说明                      |
| ---------------- | ----------------------- |
| `Autoplay`       | 自动比赛控制，包括赛程生成、队伍匹配、进程管理 |
| `ProcessManager` | 多线程服务器进程管理              |
| `Analysis`       | 比赛日志分析与数据导出             |
| `WorldModel`     | 比赛世界模型解析                |
| `Physics`        | 物理引擎相关计算                |
| `Rcg2Csv`        | RCG 格式转换为 CSV 格式        |

## 贡献指南

欢迎提交 Issue 和 Pull Request！

1. Fork 本仓库
2. 创建你的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交你的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启一个 Pull Request

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

## 联系方式

如有问题或建议，请提交 Issue 或通过以下方式联系：

- 项目地址：[Hugo1127/Robocup2D\_Analyzer](https://github.com/Hugo1127/Robocup2D_Analyzer)

***

⭐ 如果这个项目对你有帮助，请给个 Star！
