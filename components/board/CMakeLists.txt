idf_build_get_property(UNIFIED_BOARD UNIFIED_BOARD)
set(board "../../boards/${UNIFIED_BOARD}")
set(common "../../boards/common")
if(UNIFIED_BOARD STREQUAL "szpi")
    set(expander "${board}/pca9557.c")
else()
    set(expander "${board}/tpt29555a.c")
endif()
idf_component_register(SRCS "${common}/board.c" "${common}/audio_codec_lckfb.c"
    "${board}/board_init.c" "${board}/lcd_init.c" "${board}/profile.c" "${expander}"
    INCLUDE_DIRS "../../boards/include"
    PRIV_INCLUDE_DIRS "${board}" "${common}"
    REQUIRES esp_lcd
    PRIV_REQUIRES esp_driver_gpio esp_driver_i2s esp_driver_i2c esp_driver_spi esp_codec_dev)
