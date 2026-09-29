# WS63 复用接入说明

这里提供移植边界和接入顺序，不声称已完成润和板固件移植。ESP-IDF 的 `main/`、`boards/` 和 `ui/` 不进入 WS63 构建。

## 直接复用

将以下文件加入现有 GoldieOS 构建，并链接本发布包的 `libs/ws63/libconvai_sdk.a`：

- `core/ai_app.c`：会话策略、错误处理、连接超时、会话代号校验。
- `services/cloud/ai_cloud.c`：SDK 生命周期与事件翻译。使用 C11 原子布尔量；已用现有 WS63 GCC 7.3 工具链编译，生成对象没有额外原子运行库的外部引用。
- `services/media/ai_audio.c`：G711A 与 PCM 转换。8 kHz 的格式约定保持一致。
- `services/media/ai_playback.c`：使用调用方提供的内存；由 WS63 的锁保护，缓冲大小按设备预算设置。

## 在现有工程中的连接点

| 现有代码 | 新职责 |
|---|---|
| `platform/convai_platform_ws63.c` | 继续注册 `convai_platform_t`，核实线程 join/destroy 真正等待 SDK 回调退出 |
| `sdk_integration/convai_audio_uplink.c` | 用实际采集接口生成云端要求的 G711A 平面声道数据，再调用 `ai_uplink_fn` |
| `sdk_integration/convai_audio_downlink.c` | 实现 `ai_media_ops_t` 的 push/reset/finish；不要把旧 bridge 的状态控制同时保留 |
| `apps/settings/convai_talk_page.cpp` | 保留“设置 → AI 设置”页面，按钮产生 `AI_TOGGLE`/`AI_STOP`，渲染 `ai_view_t` |

由应用服务持有一个 `ai_app_t`。UI、网络和云回调把事件复制到同一个队列，服务线程串行调用 `ai_app_dispatch()`；定期调用 `ai_app_tick()`，时间必须来自单调时钟。页面显示状态通过自己的 UI 消息机制更新，不能从 SDK 线程直接访问页面控件。

## 必须满足的接口语义

1. `start(session)` 先设置媒体会话代号，再启动云服务；返回 0 只表示启动请求受理，`AI_CONNECTED` 才表示已连接。
2. `enable(false)` 等待正在执行的上行回调完成，之后不得再调用发送函数。`send` 返回 +1 表示暂未连接，不能按连续发送失败处理。
3. `push` 在返回前复制 SDK 音频；缓冲满必须报告错误。`reset` 清空缓冲并增加播放代号，禁止之前取出的数据在 reset 后写入播放设备。
4. `finish` 标记云端发送完毕；在播放缓冲和硬件播放尾部结束后投递 `AI_TURN_FINISHED`，不能收到最后一包就显示监听状态。
5. `post` 只能复制/入队，不可同步调用核心，否则 SDK 回调中执行 stop/join 会死锁。停止后旧事件通过 session 代号被丢弃。
6. SDK destroy 返回时必须已结束回调；媒体 barrier 在 destroy 前完成，避免上行任务访问已释放的引擎。

WS63 的采集布局、参考声道、采样率和播放声道数必须按它的现有音频服务配置，不能照搬 ESP32 的 TDM slot0/slot2 映射。若当前板型使用单声道或不同采样率，需要在媒体端口完成相应转换，并与云端协商格式。

AI 服务生命周期与页面生命周期分开：离开设置页时，可以投递 `AI_STOP`，也可以保留后台会话；若保留后台会话，render 回调需写入长期存在的状态快照，不能持有已销毁的控件指针。
