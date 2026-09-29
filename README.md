# Unified AI App

从立创实战派和利尔达 ESP32-S3 示例提取的最小 AI 语音对话应用。两块板使用同一套应用、云接入、媒体处理和界面，通过构建时选择板级适配。原示例未修改。

固定使用 **ConvAI SDK 26.8.0**，随工程保留发布包、头文件及 ESP32-S3 / WS63 / Windows 静态库。来源与校验值见 [third_party/README.md](third_party/README.md)。

代码上库范围、排除项和逐文件用途见 [REPOSITORY_GUIDE.md](REPOSITORY_GUIDE.md)。

## 使用范围

- 屏幕仅显示板卡名称、网络/配网信息、会话状态，以及 **Start/Stop**、**Wi-Fi** 两个按钮。
- 立创板 BOOT 短按也可启停；利尔达板使用触摸按钮。
- 开始后连续采集，保留原有 8 kHz、双声道平面排列的 G711A 上行，以及云端打断事件处理。没有增加本地唤醒/AEC 算法。
- 断线后结束当前会话，网络恢复后由用户重新点击 Start。最小实现不自动重开对话。
- 字体使用 LVGL 内置英文字体，删除 Node.js、npm、Windows 字体文件和运行时生成字库的依赖。
- 移除音色选择、音量设置页、聊天记录、浮球、波形/动画、滑动手势、触摸诊断点、旧 LCD UI、面板工厂、功能调用控制和高频诊断日志。音量通过构建配置设置，Agent 的音色/角色使用云端配置。
- Wi-Fi 配网网页沿用已有组件；不包含 OTA、应用商店或动态应用运行时。

## 分层

```mermaid
flowchart TD
    Main[main: 组装与事件循环] --> Core[core: 对话状态机]
    Main --> Cloud[services/cloud: ConvAI 会话适配]
    Main --> Port[ports/esp_idf: 媒体任务、网络、配置、SDK HAL]
    Main --> UI[ui: 最小 LVGL 页面]
    Cloud --> SDK[ConvAI SDK 26.8.0]
    Cloud --> MediaOps[ai_media_ops_t 接口]
    Port --> MediaOps
    Port --> Media[services/media: G711A、声道映射、有界缓冲]
    Port --> Board[boards: SZPI / LRD]
    UI --> Board
```

| 目录 | 边界与复用方式 |
|---|---|
| `core/` | 只依赖标准 C；同一个 owner 线程处理输入、网络和云事件，输出 `ai_view_t` |
| `services/cloud/` | 只依赖标准 C、SDK 公共头文件和媒体接口；不调用 LVGL、GPIO、FreeRTOS |
| `services/media/` | 无 OS 依赖的采样转换和缓冲；外部提供内存、锁与工作线程 |
| `ports/esp_idf/` | ESP-IDF 线程、同步、网络、时间、鉴权配置与 SDK HAL |
| `boards/` | 引脚、IO 扩展器、屏幕初始化、触摸和实际录放音；音频 Codec 驱动共用 |
| `ui/` | 仅消费状态快照、产生启停/配网命令；不依赖 ConvAI SDK |
| `main/` | 静态组装依赖并管理应用生命周期，不实现硬件驱动 |
| `ports/ws63/` | 可复用静态库构建入口与接入现有“设置 → AI 设置”页面的说明 |

参考 [ESP-Brookesia](https://github.com/espressif/esp-brookesia) 的硬件适配、服务、应用与表现层分离。当前功能规模使用静态依赖注入，不引入运行时服务注册、动态插件或多语言运行时。

## 板级差异

| 项目 | `szpi` 立创实战派 | `lrd` 利尔达 |
|---|---|---|
| Flash / PSRAM | 16 MB / Octal | 8 MB / Quad |
| IO 扩展器 | PCA9557，0x19 | TPT29555A，0x22 |
| 触摸 | FT6336，0x38 | CST836U，0x15 |
| ES7210 地址 | 0x41 | 0x40 |
| 上行麦克风 / 参考 | TDM slot0 / slot1 | TDM slot2 / slot1 |
| 按键 | GPIO0 BOOT | 无物理按键依赖 |

I2S 时序、Codec 初始化与 ST7789 寄存器序列来自对应原工程。不要仅修改板名切换硬件；Flash、PSRAM 与分区配置也必须一起切换。

## 构建与烧录

使用已激活的 ESP-IDF **6.1** 环境。组件清单允许 6.0–6.1，当前验证版本为 6.1.0。首次构建需要联网获取锁定版本的 LVGL、esp_codec_dev 和 cJSON。

在本目录执行，始终为两块板使用不同构建目录：

```sh
idf.py -B build-szpi -DUNIFIED_BOARD=szpi build
idf.py -B build-lrd -DUNIFIED_BOARD=lrd build
```

每个构建目录独立保存 sdkconfig。工程会拒绝在一个已配置的构建目录中切换板型。

配置云接入参数：

```sh
idf.py -B build-szpi -DUNIFIED_BOARD=szpi menuconfig
```

进入 `Unified AI application`，填写 Agent ID，以及 API key 或完整产品三元组。API key 非空时优先使用。WebSocket URL 留空使用 SDK 默认地址。利尔达板使用相同命令并替换目录和板型。

也可以在**首次配置之前**复制 `config/local.defaults.example` 为 `config/local.defaults` 并填写；此文件已被忽略。已有 sdkconfig 时，用 menuconfig 修改，defaults 不会覆盖已有值。

```sh
idf.py -B build-szpi -DUNIFIED_BOARD=szpi -p COMx flash monitor
idf.py -B build-lrd -DUNIFIED_BOARD=lrd -p COMx flash monitor
```

如需替换 SDK 存放路径，增加 `-DCONVAI_SDK_ROOT=/path/to/convai-sdk-26.8.0`，该目录必须同时提供 `include/convai` 和 `libs/esp32s3/libconvai_sdk.a`。缺库直接报错，不链接占位实现。

首次启动无已保存 Wi-Fi 时，屏幕显示 `AI-Setup-…` 热点名和配网页地址。连接热点完成配网；联网并完成时间同步后点击 Start。点击 Wi-Fi 会先停止会话，再进入配网。TLS 使用自带证书包或 SDK 提供的 CA，保留服务器证书校验。

## 生命周期与线程约束

- 所有核心状态变更在 `app_main` 的事件循环串行执行。SDK 回调只发送拷贝后的事件，不在回调中 stop/join。
- LVGL 只有 UI 任务操作。状态和网络文本经单元素快照队列传递；I2C 触摸读取在独立任务执行。
- 显示缓冲在 SPI DMA 完成回调后才归还 LVGL。删除旧实现“入队即 flush ready”的提前复用行为。
- 录音、播放任务启动一次。停止会话通过上行 barrier 等待发送结束，清空播放缓冲，再停止/销毁 SDK 引擎，不靠固定 sleep 猜测任务退出。
- 每次会话使用新的 session 代号，过滤已入队的旧回调。播放 reset 使用独立 epoch，防止已经取出的旧数据在 reset 后重新写入。
- G711A 下行缓冲默认 256 KiB，约 32 秒编码音频，使用 PSRAM；容量可配置。超过容量明确终止本次会话，避免静默截断或无限占用内存。
- 初始预缓冲 200 ms；短回复收到 finish 后立即排空；界面在缓冲及硬件尾部播完后返回 Listening。
- 软件 reset 后硬件 DMA 仍可能有最多约 180 ms 尾音，这是保留的 I2S DMA 配置预算，尚需实板测量。不是硬件级即时静音保证。
- SDK 26.8.0 音频回调只提供格式，未暴露响应 ID。当前依赖 SDK 有序交付状态/音频事件；不能识别跨越新一轮 ANSWERING 才到达的旧音频包。

## 测试

本次执行结果及固件产物见 [VALIDATION.md](VALIDATION.md)。

主机测试不依赖 ESP-IDF，也不连接云服务。需要 CMake 和本机 C11 编译器：

```sh
cmake -S tests -B build-host
cmake --build build-host
ctest --test-dir build-host --output-on-failure
python tools/check_architecture.py
```

Windows 使用 MinGW 时，可以在其环境中添加 `-G Ninja -DCMAKE_C_COMPILER=gcc`。测试覆盖会话关闭/重启、迟到事件、网络断开、连接超时与时间回绕、100 次启停、G711 向量、两种声道映射、缓冲回绕/溢出/短音频、SDK 创建失败与启动失败，以及回调中的打断和断线处理。

WS63 公共库可使用原工程的 RISC-V 工具链构建：

```sh
cmake -S ports/ws63 -B build-ws63-core -G Ninja -DCMAKE_TOOLCHAIN_FILE=../ai-hardware-agent-examples/cmake/toolchains/ws63-riscv-gcc.cmake
cmake --build build-ws63-core
```

这只生成公共静态库，不生成润和板可烧录固件；完整接入说明见 [ports/ws63/README.md](ports/ws63/README.md)。

## 实板验收

本次没有连接开发板，也没有使用真实云凭据发起会话。编译通过不等于完成实板验收。两块板均需检查：首次配网/重新配网、触摸坐标与按键、麦克风/参考声道、短回复和长回复、连续启停、播报中停止、云端打断、Wi-Fi 断开恢复、重新连接后是否串入旧音频，以及持续运行的内存与线程栈余量。

默认没有恢复旧工程的看门狗关闭配置。若实板触发看门狗，应根据回溯定位阻塞或高优先级忙循环，避免通过关闭看门狗掩盖问题。
