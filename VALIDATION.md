# 验证记录

验证日期：2026-09-24。原有两份示例代码未修改。

## 2026-09-28 实战派实板烧写与启动检查

- 用户反馈利尔达验证通过后，切换到实战派；COM8 为 CH340K UART，COM7 为原生 USB 串口。实测 ESP32-S3 revision v0.2、16 MB Flash、8 MB Octal PSRAM。
- 使用实战派已配置的 Agent ID 与产品三元组构建，镜像大小为 1,608,048 字节（`0x188970`）。通过 COM8 烧写 bootloader、分区表、OTA 初始数据和应用，哈希校验通过。
- 启动确认新应用编译时间为 2026-09-28 17:00:33；PSRAM 内存测试、PCA9557、ST7789、ES8311/ES7210 初始化完成。初始化时出现两条对尚未启用的 I2S 通道调用 disable 的错误日志，随后 Codec open 与音频初始化返回成功；实际录放音质量仍需实板验收。
- 40 秒观察窗口内没有崩溃或重复重启，Wi-Fi 连接成功并取得 IP。观察到会话被启动，证书包校验成功、TLS/WebSocket 握手完成、`session.created` 和 `conversation.created` 到达，随后收到 `input_audio_buffer.speech_started`。这确认云会话已建立，但不能替代完整双向语音效果验证。
- SDK 对自定义地址打印了证书校验关闭提示，但平台 TLS 实现使用证书包，随后实际打印 `Certificate validated`；本次记录以实际 TLS 平台校验结果为准。
- 已脱敏构建日志：`build-verification/szpi-auth-build.log`；烧写日志：`build-verification/szpi-auth-flash.log`；已脱敏启动日志：`build-verification/szpi-boot-2026-09-28.log`。

## 2026-09-28 利尔达实板烧写与启动检查

- 在当前源码上再次执行 `idf.py -B build-lrd -DUNIFIED_BOARD=lrd build`，构建通过，应用镜像仍为 1,600,880 字节。
- 通过 COM6（CH340）确认目标为 ESP32-S3 revision v0.2、8 MB Flash；以 460800 波特率写入 bootloader、分区表、OTA 初始数据和应用，所有镜像哈希校验通过。
- 复位后读取 45 秒日志：8 MB PSRAM 内存测试通过，TPT29555A、ST7789、ES8311/ES7210 与 SDK HAL 初始化完成，观察期间未出现 panic、看门狗超时或重复重启。
- 已保存的 Wi-Fi 未找到匹配 AP，约 30 秒后自动进入配网，热点为 `AI-Setup-AE29`，配网页地址为 `http://192.168.4.1`，DNS、DHCP 与 Web 服务启动完成。
- 首次烧写时 Agent ID 和鉴权参数为空，尚不能发起云端对话。触摸交互、实际录放音、云端会话及长时间稳定性仍待验证。立创板与 WS63 的实板验证状态不变。
- 启动日志保存在本地忽略目录：`build-verification/lrd-boot-2026-09-28.log`。

### 同日更新鉴权配置后再次烧写

用户配置 Agent ID 与产品三元组后重新构建，应用镜像为 1,601,376 字节（`0x186f60`）。通过 COM6 烧写完成并通过哈希校验；启动日志确认运行新镜像（应用编译时间 2026-09-28 16:31:37），不再出现鉴权参数缺失提示。

复位后观察 40 秒，PSRAM、屏幕、音频与 Wi-Fi 初始化完成，未见崩溃或重复重启。设备处于 `AI-Setup-AE29` 配网模式，并有客户端连接；尚未观察到路由器联网成功或云端鉴权结果。构建与启动日志已过滤配置中的鉴权值，分别保存在 `build-verification/lrd-auth-build.log` 和 `build-verification/lrd-auth-boot-2026-09-28.log`，烧写日志为 `build-verification/lrd-auth-flash.log`。

## 构建结果

| 目标 | 工具链与配置 | 结果 |
|---|---|---|
| 立创实战派 `szpi` | ESP-IDF 6.1.0、Xtensa GCC 15.2.0，16 MB Flash / Octal PSRAM | 完整固件编译与链接通过；应用二进制 1,607,552 字节（`0x188780`） |
| 利尔达 `lrd` | ESP-IDF 6.1.0、Xtensa GCC 15.2.0，8 MB Flash / Quad PSRAM | 完整固件编译与链接通过；应用二进制 1,600,880 字节（`0x186d70`） |
| WS63 公共库 | 原 GoldieOS 工具链 GCC 7.3，`rv32imfc / ilp32f / --short-enums` | 同一份 core、cloud、audio、playback 源码交叉编译通过，生成 `libunified_ai_portable.a`；对象文件未引入额外原子操作运行库依赖 |

ESP32-S3 使用发布包原版 SDK 26.8.0 静态库，不使用符号重写、占位 SDK 或允许重复定义的链接选项。两套最终配置均启用两个 SNTP 服务器、关闭 LVGL 示例构建。

本地构建产物：

- `build-szpi/unified_ai_app.bin`、`build-lrd/unified_ai_app.bin`：应用分区镜像。
- 两个构建目录还包含 bootloader、分区表、OTA 初始数据与 `flash_args`。首次烧录使用 README 中的 `idf.py ... flash` 命令，由 IDF 按正确偏移写入完整镜像。
- `build-verification/szpi.log`、`build-verification/lrd.log`：最终构建日志。
- `build-ws63-core/libunified_ai_portable.a`：公共静态库，**不是 WS63 完整固件**。

构建目录和日志不纳入源码管理，重新构建可生成。

## 主机测试

MinGW GCC 13.2，C11，启用 `-Wall -Wextra -Werror`，断言保持启用。CTest **3/3 通过**：

| 测试 | 覆盖内容 |
|---|---|
| `core_lifecycle` | 启停、迟到事件过滤、网络断开、启动失败、连接超时与计时回绕、100 次重启 |
| `audio_mapping_buffer` | G711A 向量与边界、两块板的实际声道映射、缓冲回绕/溢出、短回复及 reset |
| `cloud_lifecycle` | 假 SDK 注入创建/启动失败、同步 CONNECTED、重复 ANSWERING、finish 与播放结束分离、打断、断线后拒绝自动重连回调、幂等停止和格式拒绝 |

`tools/check_architecture.py` 检查通过：core/services 没有直接引用 ESP-IDF、FreeRTOS、LVGL 或板级头文件。它只检查源码依赖边界，不替代运行时验证。

## 2026-09-24 验证边界（后续实板结果见上文）

本次未连接真实开发板、未烧录、未配置真实云凭据，也未发起云端会话。主机测试中的云回调由假 SDK 驱动；ESP32 固件则已链接真实发布库。

实板需验证两种触摸控制器、麦克风/参考通道、配网、TLS 和音频双向传输、打断尾音、断网恢复、反复启停及长时间运行。硬件 DMA 的约 180 ms 尾音预算尚未实测。WS63 仍需按 `ports/ws63/README.md` 实现平台接口，并接入现有设置页后验证完整固件。
