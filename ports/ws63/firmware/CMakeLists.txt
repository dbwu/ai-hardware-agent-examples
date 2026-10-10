cmake_minimum_required(VERSION 3.21)
project(unified_ai_ws63 C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
get_filename_component(app_root "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
include("${CMAKE_CURRENT_LIST_DIR}/../board.cmake")
set(CONVAI_SDK_ROOT "${app_root}/third_party/convai-sdk-26.8.0" CACHE PATH "ConvAI SDK distribution")
set(WS63_LOCAL_CONFIG "${app_root}/config/ws63.local.h" CACHE FILEPATH "Private WS63 credentials")
if(NOT EXISTS "${GOLDIEOS_ROOT}/cmake/goldieos-ws63.cmake")
    message(FATAL_ERROR "Set GOLDIEOS_ROOT to the selected board BSP")
endif()
set(sdk_include "${CONVAI_SDK_ROOT}/include")
if(NOT EXISTS "${CONVAI_SDK_ROOT}/libs/ws63/libconvai_sdk.a")
    message(FATAL_ERROR "Missing WS63 SDK binary")
endif()
get_filename_component(examples_root "${GOLDIEOS_ROOT}/../.." ABSOLUTE)
include("${examples_root}/cmake/convai-build-options.cmake")
include("${examples_root}/cmake/convai-artifacts.cmake")
include("${GOLDIEOS_ROOT}/cmake/goldieos-common.cmake")
include("${GOLDIEOS_ROOT}/cmake/goldieos-ws63.cmake")
include("${GOLDIEOS_ROOT}/cmake/ws63-mbedtls.cmake")
include("${GOLDIEOS_ROOT}/cmake/ws63-firmware.cmake")
add_library(convai_sdk STATIC IMPORTED GLOBAL)
set_target_properties(convai_sdk PROPERTIES
    IMPORTED_LOCATION "${CONVAI_SDK_ROOT}/libs/ws63/libconvai_sdk.a"
    INTERFACE_INCLUDE_DIRECTORIES "${sdk_include};${sdk_include}/convai")
goldieos_add_opus(PLATFORM ws63 SOURCE_DIR "${GOLDIEOS_ROOT}")
convai_target_enable_ws63_options(opus)
goldieos_add_ws63_target(TARGET unified_ws63 PLATFORM_TARGET ws63_platform SOURCE_DIR "${GOLDIEOS_ROOT}")

# Keep the board/services libraries; exclude the old desktop, settings and AI bridge.
get_target_property(bsp_sources unified_ws63 SOURCES)
list(FILTER bsp_sources EXCLUDE REGEX "/apps/")
list(FILTER bsp_sources EXCLUDE REGEX "/sdk_integration/")
list(FILTER bsp_sources EXCLUDE REGEX "/init/system_init\\.c$")
list(FILTER bsp_sources EXCLUDE REGEX "/platform/convai_platform_ws63\\.c$")
list(FILTER bsp_sources EXCLUDE REGEX "/services/aud_algo/")
set_property(TARGET unified_ws63 PROPERTY SOURCES "${bsp_sources}")
include("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
unified_ws63_common(unified_ws63)
# Embed exactly the same web assets used by the ESP-IDF component.
set(web_assets "${CMAKE_CURRENT_BINARY_DIR}/wifi_assets.c")
file(WRITE "${web_assets}" "/* Generated from ui/web; do not edit. */\n")
foreach(page IN ITEMS wifi_configuration wifi_configuration_done)
    set(source "${app_root}/ui/web/${page}.html")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
    file(READ "${source}" bytes HEX)
    string(REGEX REPLACE "(..)" "0x\\1," bytes "${bytes}")
    if(page STREQUAL "wifi_configuration")
        set(symbol ws_wifi_html)
    else()
        set(symbol ws_wifi_done_html)
    endif()
    file(APPEND "${web_assets}" "const char ${symbol}[] = {${bytes}0};\n")
endforeach()

target_sources(unified_ws63 PRIVATE sdk_platform.c network.c wifi_store.c runtime.c
    dhcp_gateway.c portal_gateway.c "${web_assets}")

goldieos_add_ws63_mbedtls(TARGET ws63_tls SOURCE_DIR "${GOLDIEOS_ROOT}"
    SDK_INCLUDE_DIR "${sdk_include}" PLATFORM_TARGET ws63_platform)
# The BSP's libc clock is not synchronized by its NTP service. Use the same
# NTP-backed clock for certificate validity checks and SDK authentication.
target_compile_definitions(ws63_tls PRIVATE MBEDTLS_PLATFORM_TIME_ALT)
target_compile_definitions(unified_ws63 PRIVATE MBEDTLS_PLATFORM_TIME_ALT)
goldieos_add_ws63_firmware(TARGET ws63_firmware APP_TARGET unified_ws63 OPUS_TARGET opus
    SDK_TARGET convai_sdk MBEDTLS_TARGET ws63_tls DEFAULT_BOARD_ROOT "${GOLDIEOS_ROOT}"
    BASENAME goldieos VERSION "0.1.0")
# The vendor packager hard-codes goldieos.*. Publish a distinct application name.
add_custom_target(unified_ws63_image ALL
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/out/goldieos.fwpkg"
        "${CMAKE_CURRENT_BINARY_DIR}/out/unified_ai_${WS63_BOARD}.fwpkg"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/out/goldieos.bin"
        "${CMAKE_CURRENT_BINARY_DIR}/out/unified_ai_${WS63_BOARD}.bin"
    DEPENDS ws63_firmware)
