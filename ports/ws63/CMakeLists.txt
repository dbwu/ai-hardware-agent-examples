cmake_minimum_required(VERSION 3.20)
project(unified_ai_ws63_core C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(root "${CMAKE_CURRENT_LIST_DIR}/../..")
set(CONVAI_SDK_ROOT "${root}/third_party/convai-sdk-26.8.0" CACHE PATH "ConvAI SDK 26.8.0")
# This builds only the reusable library. The host application supplies LiteOS
# event dispatch, media workers, SDK HAL and its own application presenter.
add_library(unified_ai_portable STATIC
    "${root}/core/ai_app.c"
    "${root}/services/cloud/ai_cloud.c"
    "${root}/services/media/ai_audio.c"
    "${root}/services/media/ai_playback.c"
    "${root}/services/network/ai_provision.c")
target_include_directories(unified_ai_portable PUBLIC
    "${root}/core/include" "${root}/services/cloud/include" "${root}/services/media/include"
    "${CONVAI_SDK_ROOT}/include/convai" "${root}/services/network/include")
target_compile_options(unified_ai_portable PRIVATE -Wall -Wextra -Werror)
target_link_libraries(unified_ai_portable INTERFACE "${CONVAI_SDK_ROOT}/libs/ws63/libconvai_sdk.a")
