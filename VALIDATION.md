# 验证记录

## 当前验收结论（2026-10-10）

以下为用户对最新清理、分层版本的明确反馈，优先于后文历史排查中的“待验证”状态。

| 目标 | 用户确认结果 | 构建/部署证据 |
|---|---|---|
| Hoperun WS63 单板 | 显示、配网和对话正常 | 编译、FWPKG 烧写成功；自动门户网关修复此前已验证 |
| 利尔达 ESP32-S3 | 显示、配网和对话正常 | 编译、COM6 烧写及分区哈希校验成功 |
| 实战派 ESP32-S3 | 显示、配网和对话正常 | 编译、COM8 烧写及分区哈希校验成功 |
| Hoperun Windows 模拟器 | 界面和对话正常 | Windows EXE 编译、启动成功，用户完成运行验证 |

本轮四个目标的基础功能验收完成。模拟器不提供 Wi-Fi 配网，不能替代实体板硬件验证。9 项自动化测试此前通过；长时间压力、异常断电、跨手机兼容性和其他厂商 BSP 不属于本次反馈所确认的范围。

已清理临时诊断代码，保留共享门户、WS63 DHCP 网关补全、TLS 时间、NV 配置存储和播放同步修复。尚未执行 Git 暂存、提交或推送。

## 历史排查与逐次验证

以下按工作过程保留；旧日志/构建目录部分已按用户要求删除，文中的旧路径和“待验证”仅代表当时状态。

## 2026-10-10 同手机利尔达可弹窗、润和不可弹窗

域名诊断版反馈仍不弹窗。日志中 DNS 查询域名为 szlong.weixin.qq.com，不能把这些业务查询误认为系统联网探测。电脑临时连接热点实测得到 192.168.4.3，DNS 和默认网关均为 192.168.4.1，HTTP /generate_204 返回 302、/ 返回 200；电脑已恢复 Huawei-Guest。电脑 UDP DNS 探测超时且单板未记录对应查询，电脑存在 singbox_tun，不能直接以此判定单板 DNS 应答丢失。

发现并修正启动竞态：原 WS63 DHCP 先于 DNS 监听就绪，手机拿到租约后首轮探测可能失败。现先准备 HTTP，等待 DNS 非阻塞监听就绪后才启动 DHCP，退出时等待 DNS 线程释放端口。修复版编译通过，是否为当前手机问题根因仍待实测，不宣称已解决。

启动顺序修复版已烧写成功，首次握手超时后重试成功，全部下载项成功，Burned successfully，退出码 0。成功日志：build-verification/ws63-portal-startup-flash-retry.log。已开启 ws63-portal-startup-capture.log 采集，待手机复测。

用户复测启动顺序版仍不弹窗，故不能把启动竞态认定为当前根因。日志仍只有微信域名 DNS 查询，未见系统 HTTP 探测。电脑第二次测试明确指定 Wi-Fi 出接口后 DNS 仍超时、单板未记录电脑 DNS 查询；HTTP 302/200 正常，电脑已恢复 Huawei-Guest。代理隧道仍可能拦截 DNS，需要无代理干扰的测试才能解释这一结果，暂停进一步猜测性固件修改。

域名访问结果更正：用户首次输入有误，随后确认 http://example.com/generate_204 可打开配网页面。ws63-portal-confirmed-hotspot.log 记录 HTTP GET /generate_204、HTTP 302 sent=1 及后续根页面请求，证明手动域名解析与 302 链路可用；系统自动探测仍未成功。新增前 20 次 DNS 查询的域名日志，并把 A 应答 TTL 从 0 调为与利尔达一致的 28 秒，以缩小实现差异；此为诊断/兼容性验证改动，不等于已确认根因。新诊断镜像编译通过。

域名诊断镜像已烧写：首次握手超时，重试成功，所有下载项成功，Burned successfully，退出码 0。日志：build-verification/ws63-portal-domain-flash-retry.log。已开启 ws63-portal-domain-capture.log 采集，等待新一轮自动探测验证。

进一步实测：确认手机连接润和 xxx1758，等待 20 秒显示“不可上网”，利尔达则显示“需要验证”。润和日志记录多次 DNS A/AAAA 查询应答，自动探测阶段未记录到有效 HTTP 请求。用户手动访问 192.168.4.1 可打开页面，日志确认 HTTP GET /、/saved/list、/scan。因此页面服务可用；尚需验证按域名访问时 DNS 应答是否被手机接受以及 302 跳转链路，不能仅凭 sendto 成功判定解析有效。

实板反馈更正：用户随后确认刚才连的是利尔达热点，润和仍不能自动弹窗。撤回润和自动弹窗验证通过的结论，也不能据此归因于旧网络缓存。本次润和采集仅记录到 DNS A 查询成功应答（40/40 字节），未捕获完整 HTTP 302 链路。后续须核对手机连接的热点名称与润和屏幕一致，再复现排查；润和自动弹窗问题仍未解决。

诊断镜像已通过 COM4（CH340K）、115200 烧写成功：所有下载项成功，Burned successfully，退出码 0。日志：build-verification/ws63-portal-diagnostic-flash.log。串口采集已开启，待手机重新连接热点复现；采集文件为 build-verification/ws63-portal-diagnostic-capture.log。

用户确认同手机连接利尔达可以自动弹出配网页面，润和不行。此前 DNS/302 改进不能视为已解决问题，排查优先比较平台实现。WS63 增加 DNS 配置返回值检查及 AP HTTP 就绪、DNS bind/查询应答、HTTP 请求路径和 302 发送结果诊断，不记录 POST 密码、请求头和查询参数。日志用于区分服务启动失败、DNS 未到达和 HTTP 探测未到达；实际根因仍待润和连接后采集确认。

## 2026-10-10 最新利尔达镜像构建与烧写

基于最新代码使用 ESP-IDF 6.1 构建 build-lrd（UNIFIED_BOARD=lrd）成功，应用大小 0x173920，应用分区剩余 58%。COM6 识别为 USB-SERIAL CH340；通过 idf.py flash 烧写成功，数据 Hash 校验通过，退出码 0，并经 RTS 自动复位。沿用 build-lrd 中已有鉴权配置，未输出凭据。日志：build-verification/lrd-ui-regression.log 与 lrd-ui-regression-flash.log。本次包含共享配网页面改进；WS63 专用 DNS/结果查询后端不适用于 ESP 平台。利尔达本轮配网和对话功能待用户实板验证。

## 2026-10-10 配网自动弹窗与提交响应丢失改进

本轮镜像已通过 COM4、115200 烧写成功：所有下载项成功，Burned successfully，退出码 0。烧写日志：build-verification/ws63-portal-flash.log。用户将验证手机自动弹出配网页面及连接网络功能，结果待反馈；本次包含 NV 分区，可能需要重新配网。

WS63 配网期间将系统 DNS 指向 192.168.4.1（底层 dhcp_option_dns_server 从 dns_getserver 生成 DHCP option 6，已核对固件反汇编），退出恢复原 DNS，STA DHCP 随后刷新正常 DNS。手机 HTTP 探测和未知 GET 路径返回 302 到配网根页面；根页面与配网 API 保持直接响应。DNS 接收失败时主动休眠，避免错误忙循环。

共享网页为提交生成 request_id；WS63 保存最近一次提交结果，提供 /connection/status?id= 查询。网络传输错误时只查询同次结果，不自动重复提交密码；已确认保存成功后的列表刷新失败不再被误报为连接失败。ESP 原同步提交仍兼容，若传输失败且不支持结果查询，则明确提示结果未知。此改进不保证消除 APSTA 信道切换本身的短暂断连。

WS63 镜像编译通过，7 项测试通过，网页测试新增响应丢失恢复、不重复提交、保存失败传播及列表刷新失败场景。尚未烧写本轮镜像，手机自动弹窗、断连恢复和退出配网后的 DNS/AI 对话需实板验证。建议手机先忽略旧热点再重连，以获取新的 DHCP DNS 设置；自动弹窗仍受手机系统探测策略影响。

## 2026-10-10 TLS 校验时间修复

实板验证结果：用户在烧写 TLS 时间修复镜像并重启测试后反馈“功能已正常”。串口 ws63-tls-clock-capture.log 同时确认 TLS handshake complete 和 WebSocket handshake complete。本轮 Error -7 与 Start 操作阻塞已按用户反馈恢复；尚未单独验证长时间运行及多次冷启动稳定性。Wi-Fi 按钮和自动配网继续保留。

修复镜像已烧写成功：首次等待握手超时（退出码 4），重试所有下载项成功，Burned successfully，退出码 0。成功日志：build-verification/ws63-tls-clock-flash-retry.log。已重新开启串口采集，实际 TLS 握手及基础对话待用户重启后验证。

用户反馈 Ready 时 Start/Wi-Fi 均无按下反馈，暂未确定触摸原因。重新采集 ws63-ready-capture.log 后捕获到实际 Start：TLS 返回 -0x2700，verify flags=0x00000200，明确为证书尚未生效；libc time 异常且与 SDK 的 NTP 时间不一致。WS63 TLS 与平台适配统一启用 MBEDTLS_PLATFORM_TIME_ALT，并在 SDK 初始化前注册 NTP-backed 时间回调，保持 VERIFY_REQUIRED。修复镜像编译通过，现有 7 项回归测试通过（不覆盖硬件 TLS 握手）。烧写及握手结果待实板确认。用户说明当前板无电源拨动开关，不能以开关检查作为本板触摸故障结论。

## 2026-10-10 Error -7 定位到 TLS 证书校验

诊断镜像已经 COM4、115200 烧写成功，所有下载项成功，Burned successfully，退出码 0。烧写日志：build-verification/ws63-tls-diagnostic-flash.log。已启动新一轮串口采集，等待重启联网后复现；诊断结果以下续记为准。Wi-Fi 按钮及自动配网保留。

用户在最新镜像再次复现 Error -7。COM4 日志 ws63-start-network-capture.log 显示 DNS 解析成功、TCP 连接发起成功，TLS handshake 返回 -0x2700（证书校验失败）。SDK 的 -7 同时代表网络错误，此次不能归因为配置 JSON。新增失败时输出 verify flags、校验原因及 libc/SDK 时间对比，诊断镜像编译通过；尚未烧写，具体证书链或时间原因待确认。保持 VERIFY_REQUIRED，不绕过证书校验。

按用户最新决定，保留启动无保存网络/离线超时自动配网，撤回本轮手动触发配网调整；保留 Wi-Fi 按钮作为已有连接时重新配网的入口。

## 2026-10-10 联网后 Error -7 排查

Agent 和产品鉴权均非空、已编入 ELF，未输出凭据。原 -7 混合多个错误分支，尚未确认实板根因。现区分 -7 缺少鉴权、-71 设备标识、-72 JSON 分配、-73 字段创建、-74 序列化失败；Start 重新生成配置，允许启动时临时失败后恢复。固定 codec 改为 raw JSON 数字 0，避开浮点格式化依赖，但未确认此为根因。WS63 与 Windows 编译通过，新增配置生成测试后 7 项测试通过。

本轮已通过 COM4、115200 烧写最新 unified_ai_ws63.fwpkg，所有下载项成功，最终 Burned successfully，退出码 0。日志：build-verification/ws63-unified-2026-10-10-cloud-config-flash.log。此次包含 NV 分区，可能需要重新配网；Start 后云端连接与实际对话仍待用户实板验证，尚不能确认 Error -7 已解决。


## 2026-10-10 板内 NV 与配网忙循环修复版烧写成功

通过 COM4、115200 烧写当前 unified_ai_ws63.fwpkg，所有下载项成功，最终 Burned successfully，退出码 0。日志：build-verification/ws63-unified-2026-10-10-nv-wifi-flash.log。本次已写入内部 NV 保存网络与非阻塞 accept/主动休眠修复。无卡保存、重启后自动重连、配网长时间运行及基础对话尚待实板验证；手机自动弹窗未确认。

## 2026-10-10 无 SD 卡配网存储修复

用户确认未插卡。Wi-Fi 保存已从 FATFS 迁移为内部 NV 双快照（0x5100/0x5101），写入后读回核对，拒绝覆盖其他格式及忽略读错误。WS63 镜像编译通过，含新增 NV 故障/损坏回退测试的 6 项测试全部通过。仍包含此前非阻塞 accept + 每轮休眠的配网忙循环修复。此版本尚未烧写：无卡保存/断电重连、重启是否消失、手机自动弹窗仍需实板验证。


## 2026-10-10 配网重启与保存失败排查

- 用户确认手机可手动打开 http://192.168.4.1，HTTP 页面可用；未自动弹窗不等同 HTTP 服务失败。自动弹窗仍需结合手机探测及 DHCP/DNS 验证。
- 串口日志 ws63-reboot-capture.log 两次记录 ai_wifi 任务 NMI，任务 CPU 占用约 96%；回溯含 network_task 与 lwip_selscan。配网 select 循环没有显式让出执行时间，现改为非阻塞 accept，每轮（包括成功请求和出错）休眠 20 ms；已接受连接显式恢复阻塞模式并保留收发超时。此修改尚未实板确认消除重启。
- 同一日志显示 mount failed 3、SD card not available。当前 FATFS 配置保存依赖 microSD；网页原通用错误不能区分连接失败与存储失败，现拆分参数校验、连接失败、连接成功但保存失败三种提示。未添加静默忽略保存失败的行为。
- WS63 修复镜像编译成功，公共 5 项测试通过；未烧写本轮镜像。首次无保存网络自动进入热点是三板共有行为。

## 2026-10-10 media(-16) 修复镜像烧写成功

用户确认单板已进入下载模式后，经 COM4、115200 烧写 2,228,872 字节的 unified_ai_ws63.fwpkg。所有下载项及主应用均成功，工具最终返回 Burned successfully，退出码 0。日志：build-verification/ws63-unified-2026-10-10-media-fix-download-mode.log。本次已写入允许省略 audio_output_config 回调的修复；启动、Wi-Fi 配网与实际对话仍待实板复测。

## 2026-10-10 WS63 media(-16) 初始化修复

用户在最新固件启动时观察到 media(-16)。对应检查为 AudioService.audio_output_config 缺失；对照原示例 convai_audio_downlink.c，该回调本来就是可选的。此前只对 Windows 放宽检查，遗漏 WS63。现已统一允许省略回调，播放时仍仅在回调存在时调用，采集/播放/缓冲等必需接口检查保留。

WS63 镜像与 Windows EXE 编译通过，公共 5 项测试通过（不等同实板音频验收）。修复镜像 2,228,872 字节。首次烧写尝试未能打开 COM4，退出码 3；该次未写入固件，待串口释放后重试。

## 2026-10-10 更换润和单板后最新镜像烧写成功

- 用户确认更换为同型号润和 WS63 单板。前期原 GoldieOS 桌面的观察来自更换后的板，不能据此否定此前另一块板的烧写结果。
- 本次经 COM4、115200 使用 BurnToolCmd 烧写最新 unified_ai_ws63.fwpkg（2,228,888 字节，包含取消业务层仅充电阻塞的修改）。
- 引导、参数、NV、主应用均报告下载成功，最终 Burned successfully，退出码 0。日志：build-verification/ws63-unified-2026-10-10-new-board-continue.log。
- 最新版本已写入；重新启动后的界面、Wi-Fi 触摸及配网、基础对话仍待用户实板验证。

## 2026-10-10 统一上电业务体验

移除 WS63 应用 owner 的开关检查、仅充电等待和关闭网络分支，删除公共 runtime 的电源状态/保持接口。板级 runtime 私有任务独立管理 AW9523B 电源保持，USB 维持供电时业务持续运行。WS63 固件与 Windows EXE 重新编译通过；此版本尚未烧写，USB 下 OFF 启动、开关切换期间配网/对话与纯电池关机仍需验证。


## 2026-10-10 润和 WS63 烧写成功

- 使用官方 BurnToolCmd，经 COM4（USB-SERIAL CH340K）、115200 波特率烧写 build-ws63-app/out/unified_ai_ws63.fwpkg，文件大小 2,229,016 字节。
- 工具等待握手时，用户关闭再打开单板电源，握手成功；引导、参数、NV 与主应用均报告下载成功。
- 最终输出 Burned successfully，退出码 0。日志：build-verification/ws63-unified-2026-10-10-flash.log（本地忽略目录）。
- 烧写成功不代表应用功能验收；本次未采集复位后的启动日志，显示/触摸、Wi-Fi 配网与基础对话待实板验证。

当前状态（2026-10-10）：用户已反馈利尔达最小配网和基础对话验证正常。WS63 独立固件编译打包通过，尚未收到实板验收结果。新增 Windows 模拟器编译链接通过；不包含 Wi-Fi 配网，使用电脑网络。模拟器窗口交互和实际语音对话仍待验证。详细历史过程见下文。

## 2026-10-10 润和固件与 Windows 模拟器双目标

### 模拟器 Listening 但无语音识别排查

用户反馈云端已进入 Listening，但说话无识别。对照原示例 convai_audio_uplink.c，发现 Windows 原实现将参考声道强制置零；统一版本错误沿用了 WS63 的右声道回声参考映射。Windows 已改为左声道麦克风、参考声道 G711A 静音（0xd5），不再把电脑的第二麦克风声道当作播放参考；硬件映射保持不变。

新增无参考声道编码测试，覆盖大幅度右声道输入、左声道保留、静音编码、非法索引和输出容量。重新构建后 5 项测试通过。该差异已修复，但实际麦克风输入与云端识别效果仍需复测，不能仅凭编码测试判定语音识别恢复。

### 模拟器 Start 禁用修复

实测复现界面显示 Offline 与 Initialization failed。增加具体阶段错误后定位为 media (-16)：Windows BSP 的 AudioService.audio_output_config 为空。Windows 目标允许该可选回调缺失，播放前仅在提供回调时配置采样率；WS63 仍保留必需接口检查。初始化失败现在显示阶段与错误码，便于区分音频、网络和 SDK 问题。

重新编译并打开窗口验证，界面已显示 Ready、Windows network: available，Start 恢复蓝色启用状态。本次未点击 Start 发起云端对话，实际音频与连续会话仍待用户验证。

- 提取共用构建脚本和 runtime 接口，复用应用 owner、LVGL 页面、云接入、音频转换和缓冲；固件与 Windows 分别实现电源、网络、时钟和 SDK HAL。
- WS63 固件重新编译打包通过，未执行烧写。Windows 使用 MinGW GCC/G++ 13.2、真实 SDK 26.8.0 Windows 库编译链接成功。
- 模拟器进程运行 8 秒未退出，随后关闭；未据此认定窗口显示、录放音或云端对话通过。
- 5 项公共测试全部通过，core/services 架构依赖检查通过。模拟器编译定义 AI_UI_NO_PROVISION，不创建 Wi-Fi 按钮，不构建板级配网 HTTP/DNS 服务；硬件目标保留配网。

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

实板需验证两种触摸控制器、麦克风/参考通道、配网、TLS 和音频双向传输、打断尾音、断网恢复、反复启停及长时间运行。硬件 DMA 的约 180 ms 尾音预算尚未实测。该日期 WS63 仅验证了公共库；后续独立固件进度见下文。

## 2026-09-29：WS63 独立应用与 ESP32 编译回归

- 新增 ports/ws63/firmware，排除旧 apps、sdk_integration、system_init 和 aud_algo 应用服务，开机入口为 standalone_entry，直接启动 Unified AI。
- WS63 的 CMake/Ninja 编译、厂商链接、签名及 fwpkg 打包成功。最终固件符号表含 standalone_entry、ws_ui_init、ai_app_dispatch；检索未发现 convai_talk_page/setting_app/convai_bridge 匹配符号。SDK 链接目标为本项目 26.8.0 的 WS63 库。
- 产物：build-ws63-app/out/unified_ai_ws63.fwpkg 和 unified_ai_ws63.bin；日志位于 build-verification/ws63-build-final.log。
- 共用 ai_screen.h 后，实战派与利尔达均重新编译成功，应用分区检查通过；利尔达随后经用户授权烧写 COM6，写入校验通过，实战派本次未烧写。
- 主机 CTest 3/3 通过，架构依赖检查通过。公共业务实现未改变，测试不代表 WS63 的真实 OS/GUI/驱动运行验证。
- WS63 使用空鉴权模板构建，未配置真实凭据，未烧写、未进行端云对话。BSP 头文件仍存在历史告警。触摸、配网、音频通道、DMA 收尾、TLS/CA、开关机和资源余量均待润和实板验收。

## 2026-09-29～30：利尔达卡顿与 Error -5，尚未闭环

1. 共享 UI 版本烧写 COM6，写入校验通过。启动日志显示 8 MB Flash/PSRAM、ST7789、ES8311/ES7210 正常初始化，Wi-Fi 获取 IP，证书包验证通过，收到 session.created 与语音开始事件。用户反馈播报卡顿，最后 Error -5；日志有持续 send queue full 和停止超时。
2. 关闭 ESP Wi-Fi 省电并设置 TCP_NODELAY，增加每 5 秒上行统计。重新编译烧写后，统计多数区间约 250 帧/5 秒、无拒绝，观察到完整 response.done；但用户再次反馈相同错误。这些网络调整不能作为问题已解决的证据。
3. 检查发现播放任务持有音频队列锁时等待 I2S 写入，SDK 入队回调也需要该锁。已将队列锁与 output_lock 分开：入队仅锁短时内存操作，播放写入和 reset 仍受输出锁保护，避免旧音频跨 reset 写回。增加 SDK 上行真实返回码和连续 30 帧失败的日志，保留原失败处理阈值。
4. 最新利尔达版本已编译并烧写，写入哈希校验通过。随后 COM6 不存在，串口重新枚举请求被拒绝；未能取得该版本对话日志，卡顿/报错修复效果仍未确认。用户下一次复测需覆盖两轮长回复、播报中停止和重新 Start。
5. 公共层 CTest 3/3 与架构检查通过；它们不覆盖 ESP 实际线程调度、I2S 或网络质量。需要结合真实 SDK 返回码继续定位，不能仅凭 -5 断言所有失败都来自队列满。

本地日志：lrd-ui-regression-boot.log、lrd-ui-regression-conversation.log、lrd-audio-fix-conversation.log、lrd-ui-regression-flash.log，均在 build-verification 下。最后一个文件记录最新烧写；早先两次烧写日志被该临时脚本覆盖。新版本诊断日志文件 lrd-playback-lock-conversation.log 未产生，因为串口打开失败。

## 2026-09-30：三板共用 LVGL 页面与最小配网

- 三板共用 ui/ai_lvgl.c 的控件/交互、ai_screen.h 的布局和状态文案；WS63 改用 LVGL 9.5.0。ui/web 两份资源同时嵌入 ESP 和 WS63 固件；移除旧高级配网页和 WS63 设备端键盘。
- WS63 新增独立网络任务、开放热点 AI-Setup-XXXX、192.168.4.1 的 HTTP/DNS、扫描连接、最多 10 个保存网络、删除/设默认和退出配网。FATFS 两个快照使用序号和 CRC，最新无效时读取上一份。旧 wificfg.txt 保留，首次使用独立应用重新配网。
- 公共 services/network 模块进行输入/索引、请求头长度与存储校验。ESP 和 WS63 在配网期间均禁止启动 AI；退出后才恢复 Station 对话入口。
- 三板完整编译成功：利尔达应用 1,520,688 字节（0x173430），实战派应用 1,527,376 字节（0x174e50）；IDF 分区容量检查通过。WS63 bin 2,066,788 字节，fwpkg 2,227,208 字节，完成厂商链接、签名和打包。WS63 仍使用空鉴权模板。
- WS63 公共静态库重新交叉编译通过；最终固件不依赖新增原子 exchange 运行库，保留平台互斥锁用于命令交换。BSP 旧头文件仍有编译告警。
- CTest 5/5 通过：原有 3 项，加 provision_validation 和 wifi_web_contract。后者用 Node 执行真实 HTML 内脚本与模拟 DOM/HTTP，覆盖扫描去重、特殊 SSID 安全显示、中文名称、连接/保存、默认/删除、退出和请求失败；不代表浏览器像素检查或真实射频验证。没有 Node 时只运行 4 项 C 测试。
- 本轮尚未烧写润和 WS63；屏幕颜色/触摸、热点与 STA 共存、DHCP、音频、TLS、堆栈余量及连续对话仍待实板验证。此前利尔达播报卡顿/Error -5 的修订保留，不能凭本次编译和主机测试判定已解决。

构建日志：build-verification/ws63-network-build.log、lrd-ui-regression.log、szpi-auth-build.log。上库清单已更新共用页面、网络适配、测试、LVGL 源码归档及逐文件行数。

### 利尔达统一配网页面实板验收

- 使用 COM6（CH340）执行 IDF 构建和完整 flash，工具报告 Hash of data verified、Done；未执行整片擦除，原有 NVS 保留。日志：build-verification/lrd-unified-wifi-2026-09-30-flash.log。
- 后续串口日志采集中 COM6 消失，重新枚举为润和 COM4（CH340K），因此本次没有保存完整启动日志。
- 用户随后明确反馈“利尔达验证正常”，并切换到润和单板；本轮 Wi-Fi 和基础对话由用户验收通过。此前卡顿/Error -5 记录保留为历史，不以本次反馈推断所有长回复压力场景均已覆盖。

### 润和鉴权镜像与 COM4 烧写尝试

- 经用户授权，将已验证的利尔达 Agent、鉴权参数和服务 URL 复制到被 Git 忽略的 config/ws63.local.h；设备名仍取润和自身标识。通过本机默认信任库完成服务端证书链验证，将已验证根证书配置为 WS63 私有信任锚，保持 TLS 证书校验开启。
- 显式重新运行 CMake 配置与编译，确认生成头文件与本地私有配置一致，最终 bin 中包含所需 Agent/鉴权与 CA；没有打印配置值。fwpkg 为 2,228,904 字节，含实际鉴权，不上库。
- 官方 BurnToolCmd 在 COM4 使用 921600 波特率尝试及重试，均停在复位握手阶段，返回 Ack receiving timeout / Shakehand failed。尚未确认写入成功。使用隔离副本启用 resetBeforeBurn，并按工具 WS63 示例使用 115200 波特率重试，仍握手超时，退出码 4；未进入镜像写入阶段。
- 日志：ws63-unified-2026-09-30-flash.log、ws63-unified-2026-09-30-flash-retry.log、ws63-unified-2026-09-30-flash-soft-reset.log，均在 build-verification/。

## 2026-10-10 三板共用门户协议

新增 services/network/ai_portal.c，ESP32-S3 两板与 WS63 均调用同一 DNS 应答生成器。统一 A/AAAA 语义、TTL、长度检查和附加记录处理；ESP 不再无条件对 AAAA 回 A。HTTP 统一 302/no-store，并在 ESP 应用路由后增加未知 GET 兜底跳转。保留 ESP 的语言参数和各平台 DHCP、线程、HTTP 框架及存储适配。

验证：host 8/8 测试通过（新增 DNS 截断、容量、响应循环、AAAA、附加记录测试）；架构依赖检查通过；WS63、利尔达和实战派镜像均编译成功。本轮没有烧写，手机自动弹窗仍待硬件验证；不能把代码共用视为此前故障已修复。TUN 保持原状。

## 2026-10-10 共用协议镜像实测与 DHCP 诊断

共用协议镜像经 COM4 烧写成功，工具退出码 0。用户反馈仍显示不可上网、不弹配网页面。重新采集只看到微信域名 A/AAAA 查询与 sendto 成功，未见 HTTP 系统探测，不能推断手机已接收到 DNS 应答。

新增可选 WS63_PORTAL_DIAGNOSTICS（默认 OFF）：在 TCP/IP 线程安装 AP IPv4 output 观察器，原样调用原 output；只观察前 8 条 DHCP 应答的地址与选项类型，不记录客户端名称、标识或密码。用于核对发给手机的 DHCP 参数，不是已确认的功能修复。退出 AP 前恢复原 output。

## 2026-10-10 确认 DHCP 缺少 Option 3

DHCP 诊断镜像烧写成功。手机重连日志确认多轮 Offer(message=2)/ACK(message=5) 包含地址 192.168.4.2、掩码 255.255.255.0、DNS 192.168.4.1，但没有路由器选项 3。SDK 头文件 LWIP_DHCPS_GW 默认 0，实际 DHCP 实现在预编译库内，仅修改应用宏不能改变它。先前电脑有网关的结果不能代表本次手机收到的 DHCP 应答。

新增 WS63 AP 输出兼容适配：对 DHCP Offer/ACK 私有副本，在已有 END 后的余量内补齐 Option 3，保持 IP/UDP 长度不变并重算 UDP 校验和；已有网关、NAK、截断、分片、选项重载或余量不足则不改。仅影响 AP 服务端端口 67→68，退出 AP 恢复原 output。诊断观察器位于补全后，便于验证实际发出的网关。长期可用开启 LWIP_DHCPS_GW 的厂商库替代此兼容适配。

9/9 主机测试通过，WS63 镜像编译通过。自动弹窗的最终修复效果仍待新镜像实机验证。

## 2026-10-10 自动配网页面实机验证通过

网关补全镜像烧写成功（Burned successfully，退出码 0），用户正常重启、忽略热点后重连，明确反馈“已修复”。日志确认 DHCP Offer 与 ACK 均新增 Option 3=192.168.4.1，随后出现 /generate204 与 /generate_204 探测、HTTP 302、页面根路径及 /saved/list、/scan 请求。结合修复前缺失网关且没有系统 HTTP 探测的日志，确认本次自动弹窗问题由润和 DHCP 未下发默认网关导致。

本轮结论限定为自动门户弹窗已验证；尚未单独确认此版本提交 Wi-Fi 密码后的联网和 AI 对话回归。COM4 诊断采集已停止。WS63_PORTAL_DIAGNOSTICS 仍为构建选项（默认 OFF），当前烧写的诊断构建为 ON；网关补全不依赖此诊断选项。未再更换用户验证通过的镜像。

## 2026-10-10 提交前清理

删除 portal_trace.c、WS63_PORTAL_DIAGNOSTICS 构建选项、DHCP/DNS/HTTP 逐请求诊断日志及仅用于打印域名的重复解析。保留 DHCP Option 3 补全与 UDP 校验和、共享 DNS、TLS 时间、NV 存储、网络线程让出 CPU 和网页请求结果恢复等必要修复。历史诊断日志说明仅作排查记录，不再是当前构建选项。保留有用的失败错误码与 TLS 证书校验失败原因。

将清单重算脚本整理为 tools/update_repository_guide.py，去掉固定工作目录；清理 build-verification 中旧串口/烧写日志、反汇编和一次性探测脚本。保留忽略的构建缓存、烧写工具、本地鉴权和用于当前编译的辅助脚本。未执行提交、暂存、推送或自动烧写。当前 boards 仅含乐鑫板级实现，润和仍由 ports/ws63/firmware 接入 BSP。

随后按用户要求增加 boards/hoperun 和通用 ws_board 接口、WS63_BOARD 构建选择，将板级电源、显示、音频布局及 BSP 路径分离。此调整取代上节“不执行目录迁移”的初始清理范围。

清理与 Hoperun 分层后的最终 WS63 固件、Windows 模拟器构建通过；9/9 主机测试和架构检查通过。移除 122 个旧临时文件。清单覆盖所有 Git 候选文件，鉴权/镜像/日志忽略规则有效。未做本轮硬件烧写，屏幕和音频分层仍需实机回归。

## 2026-10-10 清理版烧写与构建目录迁移

清理和 Hoperun 板级分层后的镜像通过 COM4 烧写成功（Burned successfully，退出码 0），用户待验证显示、配网和对话。

按板型区分输出：新建 build-hoperun-app 与 build-hoperun-win，沿用原工具链、Release 和 Hoperun BSP 配置重新生成并完整编译，两者成功。Windows 本地 convai.cfg 已迁移并核对哈希一致。随后删除旧 build-ws63-app、build-ws63-win；更早的可选公共库 build-ws63-core 也已按用户要求删除。历史记录中的旧路径仅用于历史追溯，当前命令与交付路径见 README 及 ports/ws63 文档。

## 2026-10-10 平台/板型命名复核

Hoperun 固件导出名改为 unified_ai_${WS63_BOARD}.fwpkg/.bin，当前为 unified_ai_hoperun；重新构建成功，新旧导出文件哈希一致后清除旧别名。内部 WS63 SDK 库、工具链、平台目录和通用配置接口保留芯片名称。修正文档中把 Hoperun 显示、电源、音频和 DHCP BSP 特性泛化为 WS63 平台特性的说明，并更新已烧写状态。

按用户要求删除 build-host 与 build-verification（包括日志、辅助脚本和烧写工具副本）。测试源码仍在 tests，可重新生成 build-host；固件及模拟器产物保留。此前测试结果仍见历史记录，本轮仅调整导出命名与文档，不改变固件内容。

## 2026-10-10 四目标编译供用户逐一验证

最新源码分别完成利尔达、实战派、Hoperun 固件和 Hoperun Windows 模拟器编译，全部退出码 0。ESP 合并镜像由各自 build 目录的 flash_args 经 esptool merge-bin 生成，利尔达 8 MB Flash、实战派 16 MB Flash，烧写偏移均为 0x0。Hoperun 使用 fwpkg。此轮未烧写/启动程序，显示、配网和对话验收等待用户逐一确认；模拟器不覆盖实体板 Wi-Fi/触摸/Codec 验证。

## 2026-10-10 Hoperun 清理分层版用户验收

用户明确确认润和显示、配网和对话功能均正常。此结论覆盖当前清理、板级分层及 DHCP 网关修复版本。随后用户切换利尔达单板继续验证。

利尔达最新镜像通过 COM6、460800 baud 烧写成功：bootloader、分区表、OTA 初始数据及应用均校验 Hash of data verified，esptool 退出码 0，已 RTS 自动复位。当前这轮利尔达功能验收待用户反馈。

## 2026-10-10 利尔达最新版本用户验收

用户确认利尔达显示、配网和对话功能均正常。随后换接实战派进行同项验证。

实战派最新 16 MB Flash 配置镜像通过 COM8、460800 baud 烧写成功：引导、分区表、OTA 初始数据和应用均完成哈希校验，esptool 退出码 0，已 RTS 自动复位。实战派当前轮功能验证待用户反馈。

## 2026-10-10 实战派验收与 Windows 模拟器启动

用户确认实战派显示、配网和对话正常；至此三块单板当前版本上述功能均得到用户确认。已启动 build-hoperun-win/unified_ai_simulator.exe，进程保持运行且窗口响应正常。模拟器界面和云端对话仍待用户验收。
