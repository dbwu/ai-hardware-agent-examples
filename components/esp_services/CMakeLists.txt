set(port "../../ports/esp_idf")
idf_component_register(SRCS "${port}/media.c" "${port}/network.cpp" "${port}/config.c"
    "${port}/sdk/convai_platform_esp32.c" "${port}/sdk/esp32_osal.c"
    "${port}/sdk/esp32_netal.c" "${port}/sdk/esp32_tlsal.c" "${port}/sdk/esp32_misc.c"
    INCLUDE_DIRS "${port}/include" "${port}/sdk"
    REQUIRES ai_cloud convai_sdk
    PRIV_REQUIRES ai_media board esp-wifi-connect esp_timer esp_netif esp_wifi
    mbedtls lwip cjson nvs_flash)
