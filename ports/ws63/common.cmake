# Shared application, configuration and LVGL sources for board and simulator.
function(unified_ws63_common target)
if(EXISTS "${WS63_LOCAL_CONFIG}")
    configure_file("${WS63_LOCAL_CONFIG}" "${CMAKE_CURRENT_BINARY_DIR}/ws63_config.h" COPYONLY)
else()
    configure_file("${app_root}/config/ws63.local.h.example" "${CMAKE_CURRENT_BINARY_DIR}/ws63_config.h" COPYONLY)
    message(STATUS "WS63 credentials not configured; AI start will report error -7")
endif()
# Use the same LVGL release as ESP-IDF, from a checked-in source-only archive.
set(lvgl_archive "${app_root}/third_party/lvgl-9.5.0-source.tar.gz")
file(SHA256 "${lvgl_archive}" lvgl_sha)
if(NOT lvgl_sha STREQUAL "acf605dc3b67f030eb3bd3f34967ff5ac115264b82fc03799f45231664387d9f")
    message(FATAL_ERROR "Unexpected LVGL source archive checksum")
endif()
set(lvgl_root "${CMAKE_CURRENT_BINARY_DIR}/deps/lvgl-9.5.0")
if(NOT EXISTS "${lvgl_root}/${lvgl_sha}.stamp")
    file(ARCHIVE_EXTRACT INPUT "${lvgl_archive}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/deps")
    file(WRITE "${lvgl_root}/${lvgl_sha}.stamp" "verified")
endif()
file(GLOB_RECURSE lvgl_sources "${lvgl_root}/src/*.c")
target_sources(${target} PRIVATE ${lvgl_sources})
target_include_directories(${target} BEFORE PRIVATE "${lvgl_root}")
target_compile_definitions(${target} PRIVATE LV_CONF_INCLUDE_SIMPLE LV_KCONFIG_IGNORE)

target_sources(${target} PRIVATE
    ${WS63_BOARD_SOURCES}
    "${app_root}/ports/ws63/firmware/app.c"
    "${app_root}/ports/ws63/firmware/ui.c"
    "${app_root}/ports/ws63/firmware/media.c"
    "${app_root}/ports/ws63/firmware/config.c"
    "${app_root}/ui/ai_lvgl.c"
    "${app_root}/services/network/ai_provision.c"
    "${app_root}/services/network/ai_portal.c"
    "${app_root}/core/ai_app.c"
    "${app_root}/services/cloud/ai_cloud.c"
    "${app_root}/services/media/ai_audio.c"
    "${app_root}/services/media/ai_playback.c")
target_include_directories(${target} BEFORE PRIVATE
    "${app_root}/boards/include" "${ws_board_dir}"
    "${sdk_include}" "${sdk_include}/convai" "${app_root}/ports/ws63/firmware" "${CMAKE_CURRENT_BINARY_DIR}"
    "${app_root}/core/include" "${app_root}/services/cloud/include"
    "${app_root}/services/media/include" "${app_root}/services/network/include" "${app_root}/ui/include" "${GOLDIEOS_ROOT}/platform")
set_target_properties(${target} PROPERTIES C_STANDARD 11)
endfunction()
