# ConvAI SDK 26.8.0

本工程固定使用用户指定的 [ConvAI SDK 26.8.0 发布包](https://download.huaweicloud-koophone.com/convai-sdk/convai_sdk_26.8.0.tar)。

- 原始包：`convai_sdk_26.8.0.tar`
- SHA-256：`76ca0bf0e4ebbd352f9eb66c679746101165b0e1e343c8cbbd8ddde1636b1d7a`
- 解压目录：`convai-sdk-26.8.0/`，头文件和静态库必须成套更新。
- ESP32-S3 链接 `libs/esp32s3/libconvai_sdk.a`。
- 同一包包含 `libs/ws63/` 和 `libs/windows/`，供后续平台适配使用。

二进制和头文件保留发布包原样。该版本内部已有 `sdk_cJSON_*` 命名空间，仍有部分 `cJSON_*` 外部引用；工程显式依赖 IDF cJSON 组件，不使用全局 `--allow-multiple-definition`。

Wi-Fi 配网代码来自原示例中的 esp-wifi-connect 3.2.2，其上游地址、版本和 MIT 许可声明保留在组件的 `idf_component.yml` 中。板级驱动与 SDK 平台适配移植自本工作区中的两份 ESP32-S3 示例；Brookesia 仅用作分层设计参考，没有引入其运行时或源码。
