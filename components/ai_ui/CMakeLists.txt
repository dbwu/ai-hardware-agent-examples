idf_component_register(SRCS "../../ui/ai_ui.c" "../../ui/ai_lvgl.c" INCLUDE_DIRS "../../ui/include"
    REQUIRES ai_core PRIV_REQUIRES board lvgl esp_timer)
