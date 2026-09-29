#include "ai_app.h"
#include "ai_cloud.h"
#include "ai_ui.h"
#include "board.h"
#include "esp_services.h"
#include "convai_platform_esp32.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

typedef struct { ai_event_t event; bool provision; } app_message_t;
static QueueHandle_t events;
static atomic_bool overflow;
static int cloud_config_error;

static void post(void *ctx, const ai_event_t *event) {
    (void)ctx;
    app_message_t message = {.event = *event};
    if (xQueueSend(events, &message, 0) != pdTRUE) atomic_store(&overflow, true);
}
static void ui_command(ai_ui_command_t command) {
    app_message_t message = {.event.type = AI_TOGGLE, .provision = command == AI_UI_PROVISION};
    if (xQueueSend(events, &message, 0) != pdTRUE) atomic_store(&overflow, true);
}
static int start(void *ctx, uint32_t session) {
    (void)ctx;
    if (cloud_config_error) return cloud_config_error;
    if (!esp_network_time_ready()) return -8;
    esp_media_set_session(session);
    return ai_cloud_start(NULL, session);
}
static int stop(void *ctx) {
    (void)ctx;
    return cloud_config_error ? 0 : ai_cloud_stop(NULL);
}
static void require_ok(int rc, const char *step) {
    if (!rc) return;
    ESP_LOGE("app", "%s failed: %d", step, rc);
    /* Hardware/resource failures cannot leave a half-initialized app running. */
    abort();
}

void app_main(void) {
    events = xQueueCreate(32, sizeof(app_message_t));
    require_ok(events ? 0 : -1, "event queue");
    require_ok(board_init(), "board");
    require_ok(ai_ui_init(ui_command), "UI");
    require_ok(esp_media_init(post, NULL), "media");
    require_ok(convai_platform_esp32_init(), "SDK platform");
    ai_cloud_config_t config = {.media = esp_media_ops(), .post = post};
    cloud_config_error = esp_cloud_config(&config);
    if (!cloud_config_error) require_ok(ai_cloud_init(&config), "cloud");
    else ESP_LOGW("app", "Set agent/authentication in menuconfig > Unified AI application");
    ai_app_t app;
    ai_app_ops_t ops = {.start = start, .stop = stop, .render = ai_ui_render};
    require_ok(ai_app_init(&app, &ops), "application");
    require_ok(esp_network_init(), "network");
    bool online = false, was_down = false;
    uint32_t pressed_at = 0, network_update = 0;
    char last_network[128] = {0};
    for (;;) {
        uint32_t now = (uint32_t)(esp_timer_get_time() / 1000);
        esp_network_poll();
        bool connected = esp_network_connected();
        if (connected != online) {
            online = connected;
            ai_event_t e = {.type = online ? AI_NETWORK_UP : AI_NETWORK_DOWN};
            ai_app_dispatch(&app, &e, now);
        }
        if (atomic_exchange(&overflow, false)) {
            /* Never silently lose a stop/disconnect when the queue is saturated. */
            xQueueReset(events);
            ai_event_t e = {AI_FAILED, app.session, -9};
            ai_app_dispatch(&app, &e, now);
        }
        app_message_t message;
        if (xQueueReceive(events, &message, pdMS_TO_TICKS(20)) == pdTRUE) {
            if (message.provision) {
                ai_event_t e = {.type = AI_NETWORK_DOWN};
                ai_app_dispatch(&app, &e, now);
                online = false;
                esp_network_provision();
            } else ai_app_dispatch(&app, &message.event, now);
        }
        bool down = board_button_down();
        if (down && !was_down) pressed_at = now;
        if (!down && was_down && (uint32_t)(now - pressed_at) >= 50) {
            ai_event_t e = {.type = AI_TOGGLE};
            ai_app_dispatch(&app, &e, now);
        }
        was_down = down;
        ai_app_tick(&app, now);
        if ((uint32_t)(now - network_update) >= 500) {
            network_update = now;
            char description[128];
            esp_network_description(description, sizeof(description));
            if (strcmp(description, last_network)) {
                memcpy(last_network, description, sizeof(last_network));
                ai_ui_network(description);
            }
        }
    }
}
