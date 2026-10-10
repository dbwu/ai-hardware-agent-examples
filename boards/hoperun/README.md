# Hoperun WS63 板级适配

构建选择 `-DWS63_BOARD=hoperun`（默认）。固件与 Windows 模拟器均由 `ports/ws63/board.cmake` 选择此目录，不要在同一构建目录切换板型。

- `board.cmake`：BSP 默认路径及板级源文件列表，允许通过 GOLDIEOS_ROOT 指定兼容 BSP。
- `board.c`：外设等待、AW9523 电源保持引脚、键盘初始化、音频/WLAN 服务获取、麦克风与参考通道约定、显示名称。
- `display.c`：屏幕 RGB565 字节序、2040 像素分块和 BSP 显示调用。
- 接口：`boards/include/ws_board.h`，不暴露引脚或具体厂商名。

新增同类 WS63 板：在 boards/<vendor_board>/ 实现相同接口及 board.cmake，使用独立 build 目录传入 WS63_BOARD。通常只需调整此层的 BSP 路径、硬件初始化、显示及音频布局。网络、TLS、SDK、NV、业务和共用界面继续复用。

当前 ports/ws63 的运行环境仍为 GoldieOS OSAL、AudioService、Wlan_Drv、WifiService 和应用管理 ABI。不同厂商 BSP 需要在板级提供兼容服务；若连 RTOS/网络栈接口也不同，需要新增平台后端，不能仅改板名。Windows 目标还要求 BSP 提供兼容的模拟器服务。厂商底层驱动仍在外部 BSP 中，不复制到本目录。
