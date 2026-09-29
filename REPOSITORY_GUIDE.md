# 上库清单与文件说明

以 2026-09-29 的目录及行数统计为准。本文中的路径均相对于 `unified-app/`。建议把该目录作为独立项目根目录提交；也可作为现有仓库的一个子目录。本文只整理清单，没有执行 Git 初始化、暂存、提交或推送。

## 行数统计口径与目录汇总

统计日期：2026-09-29。行数采用**文本物理行数，包含空行和注释**，不是去除注释后的有效代码行数；源码、构建脚本、配置、HTML 和 Markdown 均按同一口径统计。二进制 `.a` 和 `.tar` 不计行数，逐文件表以“—”表示。

只统计本清单内的上库文本文件，不包含构建目录、下载缓存、Git 元数据或本地鉴权配置。目录行数递归包含所有子目录；父子目录存在重叠，不可重复相加。仅含二进制的 `third_party/convai-sdk-26.8.0/libs/` 及其子目录不列入统计。根目录合计包含根目录文件和本文最终版本。

| 目录 | 文本文件数（递归） | 行数（递归） |
|---|---:|---:|
| `./（项目合计）` | 92 | 9371 |
| `boards/` | 22 | 1434 |
| `boards/common/` | 4 | 459 |
| `boards/include/` | 2 | 31 |
| `boards/lrd/` | 8 | 493 |
| `boards/szpi/` | 8 | 451 |
| `components/` | 21 | 4378 |
| `components/ai_cloud/` | 1 | 2 |
| `components/ai_core/` | 1 | 1 |
| `components/ai_media/` | 1 | 2 |
| `components/ai_ui/` | 1 | 2 |
| `components/board/` | 1 | 14 |
| `components/convai_sdk/` | 1 | 12 |
| `components/esp-wifi-connect/` | 14 | 4337 |
| `components/esp-wifi-connect/assets/` | 2 | 1975 |
| `components/esp-wifi-connect/include/` | 5 | 390 |
| `components/esp_services/` | 1 | 8 |
| `config/` | 6 | 55 |
| `core/` | 2 | 153 |
| `core/include/` | 1 | 61 |
| `main/` | 4 | 144 |
| `ports/` | 13 | 1400 |
| `ports/esp_idf/` | 11 | 1346 |
| `ports/esp_idf/include/` | 1 | 22 |
| `ports/esp_idf/sdk/` | 7 | 1071 |
| `ports/ws63/` | 2 | 54 |
| `services/` | 6 | 293 |
| `services/cloud/` | 2 | 176 |
| `services/cloud/include/` | 1 | 36 |
| `services/media/` | 4 | 117 |
| `services/media/include/` | 2 | 35 |
| `tests/` | 4 | 212 |
| `third_party/` | 5 | 615 |
| `third_party/convai-sdk-26.8.0/` | 4 | 602 |
| `third_party/convai-sdk-26.8.0/include/` | 4 | 602 |
| `third_party/convai-sdk-26.8.0/include/convai/` | 4 | 602 |
| `tools/` | 1 | 16 |
| `ui/` | 2 | 142 |
| `ui/include/` | 1 | 11 |

根目录直属文本文件：6 个，共 529 行；已包含在项目合计中。

## 一、提交范围

| 分类 | 路径 | 建议 |
|---|---|---|
| 工程入口、配置与说明 | 根目录的 `.gitignore`、`CMakeLists.txt`、`dependencies.lock`、`README.md`、`VALIDATION.md`、`REPOSITORY_GUIDE.md` | 全部提交 |
| 自有代码与组件封装 | `main/`、`core/`、`services/`、`ports/`、`boards/`、`ui/`、`components/` | 全部提交，逐文件用途见下文 |
| 可共享配置 | `config/common.defaults`、两个板型的 defaults/分区表、`config/local.defaults.example` | 全部提交，实际鉴权配置除外 |
| 测试与检查工具 | `tests/`、`tools/` | 全部提交，便于修改后验证 |
| SDK 说明和头文件 | `third_party/README.md`、`third_party/convai-sdk-26.8.0/include/convai/` | 提交 |
| SDK ESP32-S3 库 | `third_party/convai-sdk-26.8.0/libs/esp32s3/libconvai_sdk.a` | 必须提供；当前默认构建直接链接此文件 |
| SDK WS63 库 | `third_party/convai-sdk-26.8.0/libs/ws63/libconvai_sdk.a` | 建议提交，保留项目的 WS63 复用目标 |
| SDK Windows 库 | `third_party/convai-sdk-26.8.0/libs/windows/libconvai_sdk.a` | 可选；当前主机测试用假 SDK，并不链接此库 |
| SDK 原始发布包 | `third_party/convai_sdk_26.8.0.tar` | 可选归档；当前构建不解压此文件，单独提交 tar 不能替代解压后的头文件和库 |

建议本次保留 SDK 头文件及 ESP32-S3、WS63 两个平台库。原始 tar 和 Windows 库可继续随仓库存档，也可存放到团队制品库；如果调整存放方式，同步修改 `third_party/README.md`。当前工程没有自动下载 ConvAI SDK 的步骤，因此不能只保留下载链接后就删除必需的库。

## 二、不要提交的内容

| 路径/文件 | 原因 |
|---|---|
| `build-szpi/`、`build-lrd/` | CMake/Ninja 缓存、对象文件、ELF、BIN、MAP、生成头文件及当前包含鉴权值的 sdkconfig；均可重新生成 |
| `build-host/`、`build-ws63-core/` | 主机测试和 WS63 公共库的本地构建结果 |
| `build-verification/` | 本机编译/烧写/串口日志和临时 Python 脚本，不是项目构建所需源码 |
| 其他 `build*/` | 当前 `.gitignore` 已统一排除 |
| `managed_components/` | 由 ESP-IDF 组件管理器按 manifest 与 lock 文件下载，不需要重复提交 |
| `sdkconfig`、`sdkconfig.old` | 本地生成配置；当前两个板型的实际文件分别位于 `build-szpi/`、`build-lrd/` |
| `config/local.defaults` | 开发者真实鉴权配置；只提交无真实凭据的 `.example` 模板 |
| `__pycache__/` | Python 缓存 |

当前 `.gitignore` 已包含上述规则。不要用整个目录的 `git add -f` 绕过它。若父仓库另有 `*.a` 规则，应只为确定需要的第三方 SDK 库设置例外，不能放开所有构建静态库。已被 Git 跟踪的文件不受新增 ignore 规则保护，需要单独检查暂存清单。

固件 BIN 适合作为版本发布附件或 CI 制品保存；当前固件已包含鉴权配置，不应当作普通源码一并提交。

## 三、根目录和工程入口

| 文件 | 行数 | 用途 |
|---|---:|---|
| `.gitignore` | 6 | 排除构建目录、下载缓存和本地鉴权配置 |
| `CMakeLists.txt` | 25 | ESP-IDF 工程入口；选择 szpi/lrd、设置独立 sdkconfig 和默认配置、传递 SDK 路径、拒绝在同一构建目录切换板型 |
| `dependencies.lock` | 40 | 锁定组件版本和内容校验值；应与 manifest 一起提交 |
| `README.md` | 133 | 架构、板级差异、构建烧写、配置及线程约束 |
| `VALIDATION.md` | 64 | 编译、测试、烧写及实板观察记录，区分已验证结果与未验证范围 |
| `REPOSITORY_GUIDE.md` | 261 | 本上库清单与逐文件说明 |
| `main/app_main.c` | 108 | 应用组装入口；初始化板卡/UI/媒体/SDK/网络，串行处理事件、按键、配网与连接超时 |
| `main/CMakeLists.txt` | 2 | 注册应用入口组件及其依赖 |
| `main/idf_component.yml` | 6 | 声明 ESP-IDF 版本范围及 cJSON、esp_codec_dev、LVGL 依赖 |
| `main/Kconfig.projbuild` | 28 | menuconfig 中的 Agent、鉴权、服务地址、音量与播放缓冲选项；默认鉴权值为空 |

## 四、可共享构建配置

| 文件 | 行数 | 用途 |
|---|---:|---|
| `config/common.defaults` | 21 | 两块 ESP32-S3 共用的 IDF、内存、网络、LVGL 等默认配置 |
| `config/szpi.defaults` | 3 | 实战派的 16 MB Flash、Octal PSRAM 和分区表选择 |
| `config/szpi.partitions.csv` | 10 | 实战派 NVS、OTA 数据、两个应用槽和 assets 分区布局 |
| `config/lrd.defaults` | 3 | 利尔达的 8 MB Flash、Quad PSRAM 和分区表选择 |
| `config/lrd.partitions.csv` | 10 | 利尔达分区布局 |
| `config/local.defaults.example` | 8 | 本地鉴权配置模板；复制为 `local.defaults` 后填写，真实文件不提交 |

新 checkout 用 defaults 生成配置；已有 sdkconfig 时 defaults 不覆盖原值，应使用对应板型的 menuconfig。不要为了便于他人构建，把实际 Agent/密钥回填到公共 defaults 或 Kconfig 默认值中。

## 五、跨平台公共层

| 文件 | 行数 | 用途 |
|---|---:|---|
| `core/include/ai_app.h` | 61 | 对话状态、事件、界面快照、应用实例与注入操作的公共定义 |
| `core/ai_app.c` | 92 | 对话状态机：启停、联网/断线、连接超时、错误及旧会话事件过滤 |
| `services/cloud/include/ai_cloud.h` | 36 | 云接入配置、上行发送函数、媒体操作和事件投递接口 |
| `services/cloud/ai_cloud.c` | 140 | ConvAI SDK 生命周期、状态/音频回调翻译、打断处理与停止同步 |
| `services/media/include/ai_audio.h` | 16 | G711A 编解码、输入声道映射和输出双声道转换接口 |
| `services/media/ai_audio.c` | 46 | 音频格式转换与不同板型的 TDM 麦克风/参考声道选择 |
| `services/media/include/ai_playback.h` | 19 | 调用者提供存储的有界播放缓冲接口 |
| `services/media/ai_playback.c` | 36 | 环形缓冲、预缓冲、短回复收尾、溢出检查与 reset 代号 |

这些文件直接复用于 WS63，不依赖 ESP-IDF、FreeRTOS 或 LVGL。锁、工作线程、音频设备和 UI 更新由平台端提供。

## 六、ESP-IDF 平台与 SDK HAL

| 文件 | 行数 | 用途 |
|---|---:|---|
| `ports/esp_idf/include/esp_services.h` | 22 | 向组装层公开 ESP 网络、媒体、时间和云配置接口 |
| `ports/esp_idf/config.c` | 38 | 从构建配置读取鉴权参数，构造 SDK JSON；API Key 优先，否则使用产品三元组 |
| `ports/esp_idf/network.cpp` | 64 | Wi-Fi 管理、配网切换、连接状态、SNTP 与网络状态文字 |
| `ports/esp_idf/media.c` | 151 | FreeRTOS 录放音任务、发送停止同步、播放队列互斥及播放结束通知 |
| `ports/esp_idf/sdk/convai_platform_esp32.h` | 5 | SDK 平台注册入口声明 |
| `ports/esp_idf/sdk/convai_platform_esp32.c` | 84 | 将 ESP 平台函数组装为 ConvAI HAL 并注册 |
| `ports/esp_idf/sdk/convai_platform_esp32_internal.h` | 136 | HAL 私有结构和各适配函数声明 |
| `ports/esp_idf/sdk/esp32_osal.c` | 223 | 内存、时钟、线程创建/join/destroy、互斥、随机数及字符串接口 |
| `ports/esp_idf/sdk/esp32_netal.c` | 236 | Socket 网络适配 |
| `ports/esp_idf/sdk/esp32_tlsal.c` | 246 | mbedTLS 连接、收发和证书校验适配 |
| `ports/esp_idf/sdk/esp32_misc.c` | 141 | 日志、设备标识等辅助 HAL 实现 |

## 七、板级接口与两块板的驱动

| 文件 | 行数 | 用途 |
|---|---:|---|
| `boards/include/board.h` | 21 | 板型描述、初始化、音频读写、按键与触摸接口 |
| `boards/include/board_display.h` | 10 | ESP 屏幕句柄及显示分辨率接口，供 LVGL 端口使用 |
| `boards/common/board.c` | 63 | 公共板级组装、Codec 调用、触摸读取/坐标映射和按键读取 |
| `boards/common/audio_codec.h` | 15 | ESP 音频 Codec 操作接口；属于 ESP 板级代码，并非 WS63 公共抽象 |
| `boards/common/audio_codec_lckfb.h` | 9 | 共用 ES8311/ES7210 Codec 实例声明 |
| `boards/common/audio_codec_lckfb.c` | 372 | I2S、ES8311 DAC、ES7210 ADC 初始化、音量及 PCM 读写；名称沿用原代码，实际两块板共用 |
| `boards/szpi/board_pins.h` | 114 | 实战派引脚、I2C 地址、音频及屏幕硬件常量 |
| `boards/szpi/board_init.h` | 10 | 实战派初始化入口与板级设备句柄声明 |
| `boards/szpi/board_init.c` | 49 | 实战派 I2C 与 PCA9557 初始化 |
| `boards/szpi/pca9557.h` | 69 | PCA9557 类型、引脚操作及控制接口声明 |
| `boards/szpi/pca9557.c` | 78 | PCA9557 寄存器与输出控制实现 |
| `boards/szpi/lcd_init.h` | 9 | 实战派屏幕初始化、背光和句柄声明 |
| `boards/szpi/lcd_init.c` | 112 | 实战派 ST7789 SPI 初始化、寄存器序列、方向和背光控制 |
| `boards/szpi/profile.c` | 10 | 实战派名称、麦克风/参考 slot0/slot1、BOOT 按键能力和功放控制 |
| `boards/lrd/board_pins.h` | 121 | 利尔达引脚、I2C 地址、音频及屏幕硬件常量 |
| `boards/lrd/board_init.h` | 10 | 利尔达初始化入口与板级设备句柄声明 |
| `boards/lrd/board_init.c` | 49 | 利尔达 I2C 与 TPT29555A 初始化 |
| `boards/lrd/tpt29555a.h` | 74 | TPT29555A 类型、端口与控制接口声明 |
| `boards/lrd/tpt29555a.c` | 92 | TPT29555A 寄存器、方向与输出控制实现 |
| `boards/lrd/lcd_init.h` | 9 | 利尔达屏幕初始化、背光和句柄声明 |
| `boards/lrd/lcd_init.c` | 119 | 利尔达 ST7789 初始化、寄存器序列、方向和背光控制 |
| `boards/lrd/profile.c` | 19 | 利尔达名称、麦克风/参考 slot2/slot1、功放控制和触摸复位 |

## 八、界面与 WS63 接入

| 文件 | 行数 | 用途 |
|---|---:|---|
| `ui/include/ai_ui.h` | 11 | UI 初始化、状态快照、网络文字及启停/配网命令接口 |
| `ui/ai_ui.c` | 131 | 最小 LVGL 页面、单 UI 任务、触摸任务、显示 DMA 完成通知 |
| `ports/ws63/CMakeLists.txt` | 18 | 将相同的 core/cloud/audio/playback 源码构建为公共静态库，并传递 WS63 SDK 链接依赖 |
| `ports/ws63/README.md` | 36 | WS63 媒体/线程接口契约、与 GoldieOS 现有“设置 → AI 设置”页面的连接方法 |

WS63 目录目前不是完整板卡工程。独立仓库中编译该公共库仍需提供 WS63 交叉工具链；README 中引用的原工程工具链文件不在本项目内，换环境时传入实际可用的 `CMAKE_TOOLCHAIN_FILE`。ESP32 两个板型的构建不依赖原示例目录。

## 九、ESP-IDF 组件注册文件

这些小文件是实际构建入口，不能因为源码在其他目录就删掉。

| 文件 | 行数 | 用途 |
|---|---:|---|
| `components/ai_core/CMakeLists.txt` | 1 | 注册公共状态机组件 |
| `components/ai_cloud/CMakeLists.txt` | 2 | 注册云服务适配组件及 SDK/core 依赖 |
| `components/ai_media/CMakeLists.txt` | 2 | 注册公共音频转换和播放缓冲组件 |
| `components/ai_ui/CMakeLists.txt` | 2 | 注册 UI 组件及 LVGL、显示、系统依赖 |
| `components/board/CMakeLists.txt` | 14 | 按 UNIFIED_BOARD 选择板型源码并加入公共驱动 |
| `components/esp_services/CMakeLists.txt` | 8 | 注册 ESP 网络、媒体、配置与 SDK HAL 源码 |
| `components/convai_sdk/CMakeLists.txt` | 12 | 检查 SDK 路径并导入 ESP32-S3 预编译库，公开头文件和 cJSON 链接依赖 |

## 十、随源码维护的 Wi-Fi 配网组件

`components/esp-wifi-connect/` 是项目直接包含的组件，不在 `managed_components/` 内。当前构建依赖其中源码和网页，必须一起提交。

| 文件 | 行数 | 用途 |
|---|---:|---|
| `components/esp-wifi-connect/CMakeLists.txt` | 40 | 注册 C++ 源码、依赖并把两份 HTML 嵌入固件 |
| `components/esp-wifi-connect/idf_component.yml` | 18 | 组件版本、来源、MIT 许可标识及依赖声明 |
| `components/esp-wifi-connect/include/wifi_manager.h` | 133 | Wi-Fi 管理器接口、配置和事件定义 |
| `components/esp-wifi-connect/wifi_manager.cc` | 315 | Station 与配网 AP 的生命周期管理和事件转发 |
| `components/esp-wifi-connect/include/wifi_station.h` | 100 | Station 扫描、连接与状态接口 |
| `components/esp-wifi-connect/wifi_station.cc` | 401 | 已保存网络扫描、连接、重连与获取 IP |
| `components/esp-wifi-connect/include/wifi_configuration_ap.h` | 95 | 配网 AP、Web 服务接口及内部状态声明 |
| `components/esp-wifi-connect/wifi_configuration_ap.cc` | 961 | 配网热点、HTTP 接口及配置处理 |
| `components/esp-wifi-connect/include/ssid_manager.h` | 35 | 已保存网络配置管理接口 |
| `components/esp-wifi-connect/ssid_manager.cc` | 125 | Wi-Fi 配置的 NVS 读写与列表管理 |
| `components/esp-wifi-connect/include/dns_server.h` | 27 | 配网 DNS 服务声明 |
| `components/esp-wifi-connect/dns_server.cc` | 112 | 热点配网域名解析服务 |
| `components/esp-wifi-connect/assets/wifi_configuration.html` | 1832 | 实际配网页面资源，属于源码依赖 |
| `components/esp-wifi-connect/assets/wifi_configuration_done.html` | 143 | 配网完成页面资源 |

## 十一、ConvAI SDK 文件

| 文件 | 行数 | 用途及提交要求 |
|---|---:|---|
| `third_party/README.md` | 13 | SDK 下载来源、版本、SHA-256 和依赖说明；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_api.h` | 162 | 引擎创建、启停、收发等公共 API；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_event.h` | 124 | 事件类型与回调定义；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_platform.h` | 148 | 平台 HAL ABI 与接口；提交 |
| `third_party/convai-sdk-26.8.0/include/convai/convai_types.h` | 168 | SDK 通用类型、音频格式与选项；提交 |
| `third_party/convai-sdk-26.8.0/libs/esp32s3/libconvai_sdk.a` | — | 两块 ESP32-S3 必需的真实 SDK 静态库；提交 |
| `third_party/convai-sdk-26.8.0/libs/ws63/libconvai_sdk.a` | — | WS63 目标 SDK 库；本项目保留 WS63 复用能力，建议提交 |
| `third_party/convai-sdk-26.8.0/libs/windows/libconvai_sdk.a` | — | Windows SDK 库；当前构建不用，可选归档 |
| `third_party/convai_sdk_26.8.0.tar` | — | 原始 SDK 发布包，可选归档；不要只提交它而漏掉构建实际使用的解压目录 |

SDK 是预编译外部依赖，没有 SDK 内部实现源码。更新时应保持头文件与目标静态库来自同一发布版本。

## 十二、测试与检查

| 文件 | 行数 | 用途 |
|---|---:|---|
| `tests/CMakeLists.txt` | 17 | 独立于 ESP-IDF 的主机 CMake/CTest 工程 |
| `tests/test_core.c` | 61 | 状态机启停、断线、迟到事件、错误、超时和重复会话测试 |
| `tests/test_audio.c` | 49 | G711A、两种声道映射、缓冲边界/回绕/短回复与重置测试 |
| `tests/test_cloud.c` | 85 | 使用假 SDK 测试云适配的创建/启动失败、回调、打断和停止行为 |
| `tools/check_architecture.py` | 16 | 检查 core/services 不直接引用平台、板卡或 GUI 头文件 |

## 十三、提交前如何查看清单

现有 `.gitignore` 可以保留不变。在目标 Git 仓库中先预览，再暂存；不要自动把父目录的原示例一起加入。

若 `unified-app` 是仓库中的子目录，在仓库根目录执行：

```sh
git add --dry-run -- unified-app/
git add -- unified-app/
git diff --cached --name-status
git diff --cached --stat
```

若把 `unified-app` 本身作为仓库根目录，在该目录执行相同命令，将 `unified-app/` 替换为 `.`。上述命令默认会包含可选的 tar 和 Windows 库；若决定不归档它们，应先添加对应的精确 ignore 规则。

当前清单中的待提交文本文件已与两块板当前配置的 Agent/鉴权值做过匹配检查，未发现匹配。该检查只覆盖已知配置值，不代表扫描了任意来源的全部秘密；重点是始终排除实际 sdkconfig、local.defaults 和含配置的固件产物。
