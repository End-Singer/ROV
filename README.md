# HDU-ROV Embedded Control System (STM32)

> **项目名称**：杭州电子科技大学水下机器人社团 - 深蓝创客杯 ROV嵌入式电控系统  
> **核心芯片**：STM32F103VCT6 (基于标准库开发)  
> **当前版本**：v1.0 (Release 1226)  
> **维护者**：郭天一

## 📖 项目简介 | Project Overview

本项目为 **HDU 水下机器人社团** 参加 **深蓝创客杯** 比赛所开发的 ROV（水下机器人）嵌入式控制系统源码。

项目包含 STM32 下位机固件与 PC 端上位机控制软件。系统主要基于 **江科大（Jiangsu University of Science and Technology）STM32 教程** 进行二次开发，实现了 ROV 的六自由度运动控制、机械爪作业以及视觉数据的透传通信。

### ✨ 核心功能
* **多维运动控制**：支持 ROV 前进、后退、上浮、下潜、左转、右转等全向运动。
* **作业控制**：通过 PWM 驱动舵机，精确控制机械爪进行水下物品抓取。
* **视觉算法联动**：通过 USART 串口接收树莓派（Raspberry Pi）运行 YOLO 算法的识别结果，并透传至上位机显示。
* **人机交互系统**：
    * 支持 Xbox 手柄遥控（通过 Python 上位机解析）。
    * 实时回传底层传感器及视觉识别状态。

---

## 📂 目录结构说明 | Directory Structure

项目文件结构包含了开发过程中的迭代版本、单元测试模块以及最终交付代码。

```text
End-Singer/
├── 📂 .idea/                   # IDE 配置文件
├── 📂 ROV 1226/                # 【Release】最终版下位机固件 (已验证/实物烧录版本)
├── 📂 ROV 1221 - 1225/         # 【History】历史迭代版本 (按日期归档)
├── 📂 TIM_PWM/                 # [Driver] PWM 输出驱动单元测试 (电机/舵机测试)
├── 📂 SGA_LED/                 # [Driver] GPIO 与指示灯状态机测试
├── 📂 9-2 串口发送+接收/         # [Driver] USART 基础通信实验（江科大）
├── 📂 9-4 串口收发文本数据包/     # [Driver] USART 数据包解析协议实验（江科大）
├── 📄 上位机 1225.py            # 【Host】主要上位机程序 (Xbox控制 + 数据回传显示)
├── 📄 上位机 1224.py            # 【Host】上位机测试版
├── 📂 手柄上位机/               # [Legacy] 往届学长提供的上位机 (不适配当前标准库协议，仅作参考)
└── 📂 串口助手/                 # [Tools] 江科大提供的标准调试工具
```

----------

## 🛠️ 技术架构 | Technical Architecture

### 1. 下位机 (MCU Firmware)

-   **开发环境**：Keil MDK-ARM
    
-   **硬件平台**：STM32F103VCT6
    
-   **软件架构**：
    
    -   基于 **STM32 标准库 (Standard Peripheral Library)** 开发，注重对底层寄存器的理解与控制。
        
    -   **运动学解算**：将上层指令解析为 4~6 个推进器的 PWM 占空比信号。
        
    -   **通信协议**：自定义不定长数据帧，通过 USART 中断方式处理数据包。
        

### 2. 上位机 (Host Computer)

-   **开发语言**：Python 3.x
    
-   **核心依赖**：
    
    -   `pyserial`: 用于串口通信（对应代码中的 `import serial`）。
        
    -   `pygame`: 用于开发上位机界面以及读取 Xbox 手柄数据。
        
    -   _标准库_: `sys`, `threading`, `time` (用于多线程串口收发与系统延时)。
        
-   **功能逻辑**：
    
    -   **多线程架构**：使用 `threading` 创建独立的串口接收线程，确保数据回传不阻塞主界面的渲染。
        
    -   **控制流**：通过 `pygame` 读取 Xbox 手柄模拟量/按键 -> 封装为自定义协议帧 -> 通过串口发送。
        
    -   **数据流**：监听串口 -> 解析 ROV 返回的数据 -> 在控制台中显示。
        

----------

## 🚀 快速开始 | Quick Start

### Step 1: 硬件准备

1.  **通信链路**：使用 USB-TTL 模块连接 PC 与 STM32 的串口引脚。
    
2.  **控制器**：将 Xbox 手柄连接至电脑 USB 接口。
    

### Step 2: 下位机烧录 (Firmware)

1.  使用 Keil 打开 `📂 ROV 1226` 目录下的工程文件。
    
2.  编译工程（Build）并使用下载器（ST-Link/DAP）将代码烧录至 STM32 开发板。
    
    > **Note**: `ROV 1226` 是经过完整水下测试的推荐稳定版本。
    

### Step 3: 上位机运行 (Host)

1.  **安装依赖**：
    
    本项目上位机仅依赖 `pygame` 和 `pyserial`，请在终端执行以下命令：
    
    Bash
    
    ```
    pip install pyserial pygame
    ```
    
2.  **启动程序**：
    
    确保 USB-TTL 已连接，在终端运行：
    
    Bash
    
    ```
    python "上位机 1225.py"
    ```
    
    _程序启动后将自动搜索串口并尝试连接手柄。_
    

----------

## 📡 通信协议摘要 | Protocol Summary

为了确保水下环境通信的稳定性，本项目采用了自定义的数据包结构：

帧头,数据载荷 (Payload),帧尾
[,Data1 ... DataN,]

-   **控制指令 (PC -> ROV)**：包含摇杆 X/Y/Z 轴数据及按键状态，用于控制机器人的运动姿态与机械臂。
    
-   **回传数据 (ROV -> PC)**：包含 YOLO 识别到的目标类别 ID 及置信度信息。
    

----------

## 📝 致谢 & 参考 | Acknowledgements

-   感谢 **江科大 (Jiangsu University of Science and Technology)** 提供的优质 STM32 入门教程，本项目底层驱动大量参考了其代码逻辑。
    
-   感谢 HDU 水下机器人社团学长提供的硬件支持与宝贵的经验指导。
