cmake_minimum_required(VERSION 3.20)
project(unified_ai_tests C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
enable_testing()
add_executable(test_dhcp_gateway test_dhcp_gateway.c ../ports/ws63/firmware/dhcp_gateway.c)
target_include_directories(test_dhcp_gateway PRIVATE ../ports/ws63/firmware)
add_test(NAME ws63_dhcp_gateway COMMAND test_dhcp_gateway)
add_executable(test_portal test_portal.c ../services/network/ai_portal.c)
target_include_directories(test_portal PRIVATE ../services/network/include)
add_test(NAME portal_dns_protocol COMMAND test_portal)
add_compile_options(-Wall -Wextra -Werror -UNDEBUG)
set(root "${CMAKE_CURRENT_LIST_DIR}/..")
add_executable(test_core test_core.c "${root}/core/ai_app.c")
target_include_directories(test_core PRIVATE "${root}/core/include")
add_test(NAME core_lifecycle COMMAND test_core)
add_executable(test_audio test_audio.c "${root}/services/media/ai_audio.c" "${root}/services/media/ai_playback.c")
target_include_directories(test_audio PRIVATE "${root}/services/media/include")
add_test(NAME audio_mapping_buffer COMMAND test_audio)
add_executable(test_cloud test_cloud.c "${root}/services/cloud/ai_cloud.c")
target_include_directories(test_cloud PRIVATE "${root}/core/include" "${root}/services/cloud/include"
    "${root}/third_party/convai-sdk-26.8.0/include/convai")
add_test(NAME cloud_lifecycle COMMAND test_cloud)

add_executable(test_provision test_provision.c "${root}/services/network/ai_provision.c")
target_include_directories(test_provision PRIVATE "${root}/services/network/include")
add_test(NAME provision_validation COMMAND test_provision)
add_executable(test_ws_wifi_store test_ws_wifi_store.c
    "${root}/ports/ws63/firmware/wifi_store.c" "${root}/services/network/ai_provision.c")
target_include_directories(test_ws_wifi_store PRIVATE "${root}/ports/ws63/firmware"
    "${root}/services/network/include"
    "${root}/../ai-hardware-agent-examples/examples/goldieos/include/platform/ws63")
add_test(NAME ws63_nv_storage COMMAND test_ws_wifi_store)
set(bsp "${root}/../ai-hardware-agent-examples/examples/goldieos")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/config_fixture")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/config_fixture/ws63_config.h" [=[
#define WS_AI_AGENT_ID "test-agent"
#define WS_AI_API_KEY ""
#define WS_AI_PRODUCT_ID "test-product"
#define WS_AI_PRODUCT_KEY "test-key"
#define WS_AI_PRODUCT_SECRET "test-secret"
#define WS_AI_SERVER_URL ""
]=])
add_executable(test_ws_config test_ws_config.c "${root}/ports/ws63/firmware/config.c"
    "${bsp}/third_party/cjson/cJSON.c")
target_include_directories(test_ws_config PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/config_fixture"
    "${root}/ports/ws63/firmware" "${root}/core/include" "${root}/services/cloud/include"
    "${bsp}/include/third_party/cjson")
add_test(NAME ws63_cloud_config COMMAND test_ws_config)
find_program(NODE_EXECUTABLE node)
if(NODE_EXECUTABLE)
    add_test(NAME wifi_web_contract COMMAND "${NODE_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/test_wifi_web.cjs")
endif()
