# Hoperun：最新 GoldieOS 组件架构

两个入口是 src/platforms/goldieos/targets/ws63 与 src/platforms/goldieos/targets/windows；src/boards/ws63/hoperun 提供板级，src/platforms/goldieos/components 提供平台组件。不要构建原 BSP 的桌面/设置应用。

本地 BspRoot 参数必须指向**完整 ai-hardware-agent-examples 仓库根目录**，而不是 examples/goldieos 或某个 build 目录。必须包含上级 cmake/convai-*.cmake、cmake/toolchains、examples/goldieos 的源代码、头文件、libs/ws63/board、libs/win10 及 skills/_shared/tools。build-hoperun-app/ws63-link-root 只是链接用的部分副本，不能替代完整 BSP。

```powershell
# 用相同新入口构建；可以先 PrepareOnly 核对依赖。
& "<repo>/skills/_shared/scripts/build.ps1" -Target hoperun-app -RepoRoot "<unified-app>" -BspRoot "<examples 仓库>"
& "<repo>/skills/_shared/scripts/build.ps1" -Target hoperun-win -RepoRoot "<unified-app>" -BspRoot "<examples 仓库>" -MinGWRoot "<mingw64>"
```

默认输出 build/skills/hoperun-app/out/unified_ai_hoperun.fwpkg 与 build/skills/hoperun-win/unified_ai_simulator.exe。本地指定工具可传 CMakePath、NinjaPath；无 MinGWRoot 时按清单准备 GCC 13.2.0 x64 POSIX/MSVCRT。RISC-V GCC 由完整 BSP 携带；不混用 Xtensa 或 MinGW。SDK 使用 26.8.0，当前项目自带 LVGL 归档由 CMake 校验。

构建不要求云端凭据。需要实际对话时，LocalConfig 指向用户私有头文件，默认 config/ws63.local.h。没有凭据时项目使用空模板，不能声称对话可用。convai.cfg 是厂商复制的模板，本应用不读取其中的鉴权。

Windows 输出会检查 PE 导入并复制 MinGW bin 中的直接运行库 DLL；pe-dependencies.log 可用于核对其他运行依赖。功能验收必须单独运行，使用默认麦克风/扬声器；无 Wi-Fi 配网。不自动启动。

WS63 打包器在 cmd.exe 中有命令行长度限制；当前入口已启用 Ninja response file，不回退旧架构。遇到长路径错误优先使用较短的源码/缓存路径（例如 D:/ua、D:/ua-cache），不修改系统策略。离线 BSP 包尚无项目确认的发布地址，用户提供 URL/local_path、固定版本与 SHA-256 后使用相同依赖加载器。
