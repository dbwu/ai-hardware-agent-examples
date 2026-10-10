# 上库清单与逐文件说明

更新日期：2026-10-10。以当前 unified-app 实际文件为准，路径相对于该目录；本清单重整了此前分散的新增文件附录，不包含已经删除的临时诊断源码。尚未执行暂存、提交或推送。

## 当前验证状态与提交范围

| 目标 | 用户最新验收 |
|---|---|
| Hoperun WS63 单板 | 显示、配网和对话正常 |
| 利尔达 ESP32-S3 | 显示、配网和对话正常 |
| 实战派 ESP32-S3 | 显示、配网和对话正常 |
| Hoperun Windows 模拟器 | 界面和对话正常；不提供 Wi-Fi 配网 |

四目标编译通过，三块板烧写成功，9 项自动化测试此前通过。此结论不涵盖长期压力、异常断电、所有手机或其他 BSP；详见 VALIDATION.md 顶部的当前结论。

提交下面列出的源码、配置模板、构建入口、测试、文档和必要依赖。SDK 三个平台库及 LVGL 源码归档为构建依赖；原始 SDK tar 可选归档。外部 ai-hardware-agent-examples/GoldieOS BSP、工具链和厂商打包工具不在本清单内，Hoperun 构建须另行提供完整依赖。

## 不提交的本地文件

- 所有 build*/：固件、EXE、合并镜像、生成头文件、CMake 缓存、烧写工具副本和日志。build-host 与 build-verification 已删除，再生成时仍忽略。
- managed_components/、__pycache__/：下载或运行缓存。
- config/local.defaults、config/ws63.local.h、sdkconfig、sdkconfig.old：实际鉴权与本地配置；只提交模板。
- 含本地鉴权的固件和 EXE 不作为普通源码上库。

## 平台与板级边界

ws63 表示芯片平台、工具链及 SDK ABI；hoperun 表示润和板型。boards/hoperun 保存电源/引脚、显示传输、音频布局、BSP 路径及服务获取；ports/ws63 保存当前 GoldieOS 后端运行时、网络、TLS/SDK、存储和应用接入。WS63_BOARD 选择板级实现；新板使用兼容服务 ABI 时主要扩展 boards，不同 RTOS/驱动体系仍需后端适配。

boards/common 为两块 ESP 板的共用实现，不代表 WS63 也使用它。Windows 模拟器使用 Windows SDK 与虚拟设备，复用公共应用和页面，不执行 WS63 固件。

## 行数统计口径与目录汇总

文本物理行数包含空行、注释、配置及文档；二进制/归档 .a、.tar、.tar.gz 不计行数。目录递归统计，父子目录有重叠，不可相加。只统计本清单候选文件。

| 目录 | 文本文件数（递归） | 行数（递归） |
|---|---:|---:|
| `./（项目合计）` | 137 | 11449 |
| `boards/` | 27 | 1552 |
| `boards/common/` | 4 | 459 |
| `boards/hoperun/` | 4 | 103 |
| `boards/include/` | 3 | 46 |
| `boards/lrd/` | 8 | 493 |
| `boards/szpi/` | 8 | 451 |
| `components/` | 20 | 2170 |
| `components/ai_cloud/` | 1 | 2 |
| `components/ai_core/` | 1 | 1 |
| `components/ai_media/` | 1 | 2 |
| `components/ai_network/` | 1 | 1 |
| `components/ai_ui/` | 1 | 2 |
| `components/board/` | 1 | 14 |
| `components/convai_sdk/` | 1 | 12 |
| `components/esp-wifi-connect/` | 12 | 2128 |
| `components/esp-wifi-connect/include/` | 5 | 375 |
| `components/esp_services/` | 1 | 8 |
| `config/` | 7 | 66 |
| `core/` | 2 | 153 |
| `core/include/` | 1 | 61 |
| `main/` | 4 | 144 |
| `ports/` | 36 | 4248 |
| `ports/esp_idf/` | 11 | 1388 |
| `ports/esp_idf/include/` | 1 | 22 |
| `ports/esp_idf/sdk/` | 7 | 1086 |
| `ports/ws63/` | 25 | 2860 |
| `ports/ws63/firmware/` | 17 | 1850 |
| `ports/ws63/windows/` | 4 | 845 |
| `services/` | 10 | 434 |
| `services/cloud/` | 2 | 176 |
| `services/cloud/include/` | 1 | 36 |
| `services/media/` | 4 | 123 |
| `services/media/include/` | 2 | 39 |
| `services/network/` | 4 | 135 |
| `services/network/include/` | 2 | 46 |
| `tests/` | 10 | 521 |
| `third_party/` | 5 | 625 |
| `third_party/convai-sdk-26.8.0/` | 4 | 602 |
| `third_party/convai-sdk-26.8.0/include/` | 4 | 602 |
| `third_party/convai-sdk-26.8.0/include/convai/` | 4 | 602 |
| `tools/` | 2 | 54 |
| `ui/` | 7 | 296 |
| `ui/include/` | 3 | 42 |
| `ui/web/` | 2 | 104 |

根目录直属文本文件：7 个，共 1186 行；已包含在项目合计中。

## 根目录与应用入口

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `.gitignore` | 7 | 排除构建目录、下载缓存和本地鉴权配置 |
| `BOARD_COMPARISON.md` | 310 | 芯片平台、三款板级实现和 Hoperun Windows 模拟器对比及当前验收状态 |
| `CMakeLists.txt` | 25 | ESP-IDF 工程入口；选择 szpi/lrd、设置独立 sdkconfig 和默认配置、传递 SDK 路径、拒绝在同一构建目录切换板型 |
| `README.md` | 166 | 架构、板级差异、构建烧写、配置及线程约束 |
| `REPOSITORY_GUIDE.md` | 318 | 当前源码提交范围、逐文件说明、目录及文件行数和排除项 |
| `VALIDATION.md` | 320 | 四目标最新验收结论与历史排查记录；历史待验证状态不代表当前状态 |
| `dependencies.lock` | 40 | 锁定组件版本和内容校验值；应与 manifest 一起提交 |
| `main/CMakeLists.txt` | 2 | 注册应用入口组件及其依赖 |
| `main/Kconfig.projbuild` | 28 | menuconfig 中的 Agent、鉴权、服务地址、音量与播放缓冲选项；默认鉴权值为空 |
| `main/app_main.c` | 108 | 应用组装入口；初始化板卡/UI/媒体/SDK/网络，串行处理事件、按键、配网与连接超时 |
| `main/idf_component.yml` | 6 | 声明 ESP-IDF 版本范围及 cJSON、esp_codec_dev、LVGL 依赖 |

## 配置模板与板型配置

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `config/common.defaults` | 21 | 两块 ESP32-S3 共用的 IDF、内存、网络、LVGL 等默认配置 |
| `config/local.defaults.example` | 8 | 本地鉴权配置模板；复制为 `local.defaults` 后填写，真实文件不提交 |
| `config/lrd.defaults` | 3 | 利尔达的 8 MB Flash、Quad PSRAM 和分区表选择 |
| `config/lrd.partitions.csv` | 10 | 利尔达分区布局 |
| `config/szpi.defaults` | 3 | 实战派的 16 MB Flash、Octal PSRAM 和分区表选择 |
| `config/szpi.partitions.csv` | 10 | 实战派 NVS、OTA 数据、两个应用槽和 assets 分区布局 |
| `config/ws63.local.h.example` | 11 | WS63 私有鉴权、服务地址、CA 和播放缓冲模板 |

## 公共业务与服务

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `core/ai_app.c` | 92 | 对话状态机：启停、联网/断线、连接超时、错误及旧会话事件过滤 |
| `core/include/ai_app.h` | 61 | 对话状态、事件、界面快照、应用实例与注入操作的公共定义 |
| `services/cloud/ai_cloud.c` | 140 | ConvAI SDK 生命周期、状态/音频回调翻译、打断处理与停止同步 |
| `services/cloud/include/ai_cloud.h` | 36 | 云接入配置、上行发送函数、媒体操作和事件投递接口 |
| `services/media/ai_audio.c` | 48 | 音频格式转换与不同板型的 TDM 麦克风/参考声道选择 |
| `services/media/ai_playback.c` | 36 | 环形缓冲、预缓冲、短回复收尾、溢出检查与 reset 代号 |
| `services/media/include/ai_audio.h` | 20 | G711A 编解码、输入声道映射和输出双声道转换接口 |
| `services/media/include/ai_playback.h` | 19 | 调用者提供存储的有界播放缓冲接口 |
| `services/network/ai_portal.c` | 29 | 三板共用的 DNS 报文解析、边界校验与应答生成。 |
| `services/network/ai_provision.c` | 60 | 纯 C 配网参数/索引、HTTP 请求头和存储 CRC/边界校验 |
| `services/network/include/ai_portal.h` | 19 | 无平台依赖的 C/C++ 接口，以及 HTTP 跳转状态和缓存策略。 |
| `services/network/include/ai_provision.h` | 27 | 跨平台配网校验 API 与保存网络快照结构 |

## ESP 板级接口及共用驱动

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `boards/common/audio_codec.h` | 15 | ESP 音频 Codec 操作接口；属于 ESP 板级代码，并非 WS63 公共抽象 |
| `boards/common/audio_codec_lckfb.c` | 372 | I2S、ES8311 DAC、ES7210 ADC 初始化、音量及 PCM 读写；名称沿用原代码，实际两块板共用 |
| `boards/common/audio_codec_lckfb.h` | 9 | 共用 ES8311/ES7210 Codec 实例声明 |
| `boards/common/board.c` | 63 | 公共板级组装、Codec 调用、触摸读取/坐标映射和按键读取 |
| `boards/include/board.h` | 21 | ESP 板型描述、初始化、音频读写及输入接口；不是跨平台统一 HAL |
| `boards/include/board_display.h` | 10 | ESP 屏幕句柄及显示分辨率接口，供 LVGL 端口使用 |

## 实战派板级

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `boards/szpi/board_init.c` | 49 | 实战派 I2C 与 PCA9557 初始化 |
| `boards/szpi/board_init.h` | 10 | 实战派初始化入口与板级设备句柄声明 |
| `boards/szpi/board_pins.h` | 114 | 实战派引脚、I2C 地址、音频及屏幕硬件常量 |
| `boards/szpi/lcd_init.c` | 112 | 实战派 ST7789 SPI 初始化、寄存器序列、方向和背光控制 |
| `boards/szpi/lcd_init.h` | 9 | 实战派屏幕初始化、背光和句柄声明 |
| `boards/szpi/pca9557.c` | 78 | PCA9557 寄存器与输出控制实现 |
| `boards/szpi/pca9557.h` | 69 | PCA9557 类型、引脚操作及控制接口声明 |
| `boards/szpi/profile.c` | 10 | 实战派名称、麦克风/参考 slot0/slot1、BOOT 按键能力和功放控制 |

## 利尔达板级

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `boards/lrd/board_init.c` | 49 | 利尔达 I2C 与 TPT29555A 初始化 |
| `boards/lrd/board_init.h` | 10 | 利尔达初始化入口与板级设备句柄声明 |
| `boards/lrd/board_pins.h` | 121 | 利尔达引脚、I2C 地址、音频及屏幕硬件常量 |
| `boards/lrd/lcd_init.c` | 119 | 利尔达 ST7789 初始化、寄存器序列、方向和背光控制 |
| `boards/lrd/lcd_init.h` | 9 | 利尔达屏幕初始化、背光和句柄声明 |
| `boards/lrd/profile.c` | 19 | 利尔达名称、麦克风/参考 slot2/slot1、功放控制和触摸复位 |
| `boards/lrd/tpt29555a.c` | 92 | TPT29555A 寄存器、方向与输出控制实现 |
| `boards/lrd/tpt29555a.h` | 74 | TPT29555A 类型、端口与控制接口声明 |

## Hoperun 板级及 WS63 板级接口

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `boards/hoperun/README.md` | 12 | 新增同类 WS63 板的步骤与 BSP 兼容边界。 |
| `boards/hoperun/board.c` | 68 | Hoperun 外设、电源、服务获取和音频布局适配。 |
| `boards/hoperun/board.cmake` | 3 | Hoperun BSP 默认路径和板级源文件清单。 |
| `boards/hoperun/display.c` | 20 | 显示字节序与分块写入。 |
| `boards/include/ws_board.h` | 15 | WS63 板级接口、板名和采集布局；服务句柄须兼容当前 GoldieOS 后端 |

## ESP-IDF 平台端口

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `ports/esp_idf/config.c` | 38 | 从构建配置读取鉴权参数，构造 SDK JSON；API Key 优先，否则使用产品三元组 |
| `ports/esp_idf/include/esp_services.h` | 22 | 向组装层公开 ESP 网络、媒体、时间和云配置接口 |
| `ports/esp_idf/media.c` | 174 | FreeRTOS 录放音、发送停止同步与统计；队列锁/输出锁分离，reset 与 DMA 收尾 |
| `ports/esp_idf/network.cpp` | 68 | Wi-Fi 管理与性能模式、配网切换、连接状态、SNTP 和网络状态文字 |
| `ports/esp_idf/sdk/convai_platform_esp32.c` | 84 | 将 ESP 平台函数组装为 ConvAI HAL 并注册 |
| `ports/esp_idf/sdk/convai_platform_esp32.h` | 5 | SDK 平台注册入口声明 |
| `ports/esp_idf/sdk/convai_platform_esp32_internal.h` | 136 | HAL 私有结构和各适配函数声明 |
| `ports/esp_idf/sdk/esp32_misc.c` | 141 | 日志、设备标识等辅助 HAL 实现 |
| `ports/esp_idf/sdk/esp32_netal.c` | 251 | Socket 网络适配，非阻塞检查与 TCP_NODELAY |
| `ports/esp_idf/sdk/esp32_osal.c` | 223 | 内存、时钟、线程创建/join/destroy、互斥、随机数及字符串接口 |
| `ports/esp_idf/sdk/esp32_tlsal.c` | 246 | mbedTLS 连接、收发和证书校验适配 |

## WS63 平台、选板及固件端口

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `ports/ws63/CMakeLists.txt` | 19 | 将相同的 core/cloud/audio/playback 源码构建为公共静态库，并传递 WS63 SDK 链接依赖 |
| `ports/ws63/README.md` | 86 | WS63 独立应用架构、BSP 依赖、配置、固件构建及验收边界 |
| `ports/ws63/board.cmake` | 16 | WS63_BOARD 选择、合法性校验及禁止跨板复用构建目录。 |
| `ports/ws63/common.cmake` | 44 | 按选定板型注册源文件、私有配置、LVGL 和公共应用/媒体/UI，供固件及模拟器共用 |
| `ports/ws63/firmware/CMakeLists.txt` | 75 | WS63 固件构建入口；选板、BSP 裁剪、TLS/网页/网关适配链接，按 WS63_BOARD 导出镜像 |
| `ports/ws63/firmware/app.c` | 123 | 独立应用组装与事件循环、网络/配网命令、NTP 和云会话初始化；硬件初始化委托板级 |
| `ports/ws63/firmware/config.c` | 39 | 从私有头文件读取鉴权，构造并校验 SDK JSON |
| `ports/ws63/firmware/dhcp_gateway.c` | 41 | 当前 Hoperun BSP 缺少 DHCP 网关选项的兼容处理，校验边界并重算 UDP 校验和 |
| `ports/ws63/firmware/dhcp_gateway.h` | 8 | 报文补全接口与长度约束。 |
| `ports/ws63/firmware/lv_conf.h` | 22 | WS63 的 LVGL 9.5.0 配置，统一字体、主题、色深及刷新参数 |
| `ports/ws63/firmware/media.c` | 141 | GoldieOS AudioService 录放音端口，使用板级采集布局并调用公共 G711A/播放缓冲 |
| `ports/ws63/firmware/network.c` | 338 | WS63 网络任务、热点/HTTP、共享 DNS 接入、DHCP 网关适配生命周期、扫描连接及 NV 配置保存 |
| `ports/ws63/firmware/portal_gateway.c` | 42 | AP IPv4 输出适配；复制 DHCP 报文后补全，退出配网恢复原输出函数。 |
| `ports/ws63/firmware/runtime.c` | 13 | 委托通用板级初始化，提供 NTP、SDK 初始化与设备标识适配 |
| `ports/ws63/firmware/runtime.h` | 10 | BSP 就绪、运行环境初始化、时钟、设备标识和 SDK 适配接口 |
| `ports/ws63/firmware/sdk_platform.c` | 811 | WS63 SDK HAL；Goldie OSAL、网络、TLS/CA、线程退出等待、日志与设备信息 |
| `ports/ws63/firmware/ui.c` | 90 | 共用 LVGL 页面、BSP 输入事件、快照邮箱及 UI 任务；显示传输委托板级接口 |
| `ports/ws63/firmware/wifi_store.c` | 40 | 内部 NV 读取、校验回退、写入读回与外来键保护；不依赖 SD |
| `ports/ws63/firmware/wifi_store.h` | 10 | NV 应用键预留及双快照存储接口 |
| `ports/ws63/firmware/ws_services.h` | 25 | WS63 组装、媒体、配置、UI 与网络命令的内部接口 |
| `ports/ws63/firmware/ws_wifi_abi.h` | 22 | 当前润和 BSP Wi-Fi 热点最小 ABI，包含结构尺寸和偏移断言 |

## Windows 模拟器端口

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `ports/ws63/windows/CMakeLists.txt` | 30 | MinGW 模拟器目标、Windows SDK/HAL 及厂商虚拟设备链接 |
| `ports/ws63/windows/README.md` | 26 | 模拟器依赖、构建运行、配置来源与验证边界 |
| `ports/ws63/windows/runtime.c` | 28 | Windows 时间、网络可用性和设备适配；不启用配网服务 |
| `ports/ws63/windows/sdk_platform.c` | 761 | Windows SDK 平台实现，包含线程、网络、TLS 证书校验与日志 |

## 公共页面及 UI 端口

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `ui/ai_lvgl.c` | 55 | 三板唯一 LVGL 页面实现，创建控件、处理按钮和渲染状态快照 |
| `ui/ai_ui.c` | 95 | ESP LVGL 运行端口、触摸任务及显示 DMA 完成通知；页面布局在 ai_lvgl.c |
| `ui/include/ai_lvgl.h` | 8 | 公共 LVGL 页面创建、对话状态和网络文本接口 |
| `ui/include/ai_screen.h` | 23 | ESP32 与 WS63 共用的页面尺寸、配色、按钮文案及状态格式 |
| `ui/include/ai_ui.h` | 11 | UI 初始化、状态快照、网络文字及启停/配网命令接口 |
| `ui/web/wifi_configuration.html` | 101 | 三板共用最小配网页面：扫描、连接保存、删除、默认及退出；安全显示 SSID |
| `ui/web/wifi_configuration_done.html` | 3 | 共用配网完成提示页面，兼容 done.html 入口 |

## ESP 组件注册及 Wi-Fi 组件

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `components/ai_cloud/CMakeLists.txt` | 2 | 注册云服务适配组件及 SDK/core 依赖 |
| `components/ai_core/CMakeLists.txt` | 1 | 注册公共状态机组件 |
| `components/ai_media/CMakeLists.txt` | 2 | 注册公共音频转换和播放缓冲组件 |
| `components/ai_network/CMakeLists.txt` | 1 | 注册跨平台配网校验及共享 DNS/门户协议组件，供 ESP 网络适配调用 |
| `components/ai_ui/CMakeLists.txt` | 2 | 注册 UI 组件及 LVGL、显示、系统依赖 |
| `components/board/CMakeLists.txt` | 14 | 按 UNIFIED_BOARD 选择板型源码并加入公共驱动 |
| `components/convai_sdk/CMakeLists.txt` | 12 | 检查 SDK 路径并导入 ESP32-S3 预编译库，公开头文件和 cJSON 链接依赖 |
| `components/esp-wifi-connect/CMakeLists.txt` | 40 | 注册 C++ 源码、依赖并把两份 HTML 嵌入固件 |
| `components/esp-wifi-connect/dns_server.cc` | 103 | ESP UDP socket/任务适配；调用共享 ai_portal DNS 应答逻辑 |
| `components/esp-wifi-connect/idf_component.yml` | 18 | 组件版本、来源、MIT 许可标识及依赖声明 |
| `components/esp-wifi-connect/include/dns_server.h` | 27 | 配网 DNS 服务声明 |
| `components/esp-wifi-connect/include/ssid_manager.h` | 35 | 已保存网络配置管理接口 |
| `components/esp-wifi-connect/include/wifi_configuration_ap.h` | 85 | 配网 AP、Web 服务接口及内部状态声明 |
| `components/esp-wifi-connect/include/wifi_manager.h` | 128 | Wi-Fi 管理器接口、配置和事件定义 |
| `components/esp-wifi-connect/include/wifi_station.h` | 100 | Station 扫描、连接与状态接口 |
| `components/esp-wifi-connect/ssid_manager.cc` | 125 | Wi-Fi 配置的 NVS 读写与列表管理 |
| `components/esp-wifi-connect/wifi_configuration_ap.cc` | 753 | ESP 配网热点、HTTP 接口、配置处理及未知 GET 的 302/no-store 门户跳转 |
| `components/esp-wifi-connect/wifi_manager.cc` | 313 | Station 与配网 AP 的生命周期管理和事件转发 |
| `components/esp-wifi-connect/wifi_station.cc` | 401 | 已保存网络扫描、连接、重连与获取 IP |
| `components/esp_services/CMakeLists.txt` | 8 | 注册 ESP 网络、媒体、配置与 SDK HAL 源码 |

## 测试与维护工具

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `tests/CMakeLists.txt` | 53 | 独立于 ESP-IDF 的主机 CMake/CTest 工程 |
| `tests/test_audio.c` | 58 | G711A、两种声道映射、缓冲边界/回绕/短回复与重置测试 |
| `tests/test_cloud.c` | 85 | 使用假 SDK 测试云适配的创建/启动失败、回调、打断和停止行为 |
| `tests/test_core.c` | 61 | 状态机启停、断线、迟到事件、错误、超时和重复会话测试 |
| `tests/test_dhcp_gateway.c` | 48 | 网关补全、重复选项、异常/截断报文、容量和 UDP 校验和测试。 |
| `tests/test_portal.c` | 31 | DNS A/AAAA、截断报文、容量限制和附加记录处理回归测试。 |
| `tests/test_provision.c` | 44 | 异常 HTTP 长度/请求、网络参数与索引、损坏存储快照测试 |
| `tests/test_wifi_web.cjs` | 75 | 执行真实共用页面脚本，验证扫描去重、特殊名称、保存/默认/删除、退出及错误；需 Node |
| `tests/test_ws_config.c` | 26 | 测试真实云配置生成、设备标识失败、内存失败恢复及 JSON 数字类型 |
| `tests/test_ws_wifi_store.c` | 40 | 模拟 NV 的重启加载、读写失败、冲突保护、校验回退和删除回归 |
| `tools/check_architecture.py` | 16 | 检查 core/services 不直接引用平台、板卡或 GUI 头文件 |
| `tools/update_repository_guide.py` | 38 | 从 Git 候选文件集合检查清单覆盖率、更新逐文件及目录行数；与执行目录无关，不读取鉴权内容。 |

## 第三方依赖

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `third_party/README.md` | 23 | SDK 下载来源、版本、SHA-256 和依赖说明；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_api.h` | 162 | 引擎创建、启停、收发等公共 API；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_event.h` | 124 | 事件类型与回调定义；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_platform.h` | 148 | 平台 HAL ABI 与接口；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_types.h` | 168 | SDK 通用类型、音频格式与选项；提交 |
| `third_party/convai-sdk-26.8.0/libs/esp32s3/libconvai_sdk.a` | — | 两块 ESP32-S3 必需的真实 SDK 静态库；提交 |
| `third_party/convai-sdk-26.8.0/libs/windows/libconvai_sdk.a` | — | Windows 模拟器必需的 SDK 静态库；提交，不计算行数 |
| `third_party/convai-sdk-26.8.0/libs/ws63/libconvai_sdk.a` | — | WS63 目标 SDK 库；独立固件必须提供 |
| `third_party/convai_sdk_26.8.0.tar` | — | 原始 SDK 发布包，可选归档；不要只提交它而漏掉构建实际使用的解压目录 |
| `third_party/lvgl-9.5.0-source.tar.gz` | — | LVGL 9.5.0 源码及 MIT 许可归档；WS63 构建必需；提交，不计压缩文件行数 |

## 当前验证产物（不上库）

| 目标 | 产物 | 使用方式 |
|---|---|---|
| 利尔达 | build-lrd/unified_ai_lrd_merged.bin | ESP32-S3，8 MB Flash，合并镜像烧写到 0x0 |
| 实战派 | build-szpi/unified_ai_szpi_merged.bin | ESP32-S3，16 MB Flash，合并镜像烧写到 0x0 |
| Hoperun 单板 | build-hoperun-app/out/unified_ai_hoperun.fwpkg | 使用匹配板卡的厂商工具，不推测裸 BIN 烧写地址 |
| Hoperun 模拟器 | build-hoperun-win/unified_ai_simulator.exe | 在所在目录运行，使用 Windows 网络和音频 |

## 提交前检查

在 unified-app 仓库根目录运行：

```sh
python tools/update_repository_guide.py
python tools/check_architecture.py
git status --short
git diff --check
git add --dry-run -- .
```

确认新增文件、删除旧网页文件的变更和忽略范围后再执行暂存/提交。不要只提交已跟踪文件而遗漏 boards/hoperun、services/network、WS63/Windows 源码及测试。实际内容以当前 Git 差异为准，模板中的鉴权保持为空。
