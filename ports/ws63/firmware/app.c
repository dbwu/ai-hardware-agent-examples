#include "ws_services.h"
#include "goldie_osal.h"
#include "ws_board.h"
#include "runtime.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
extern unsigned int sys_now(void);
static goldie_mutex mailbox;
static ai_event_t events[32];
static unsigned head, count;
static bool overflow;
static bool provision_pending;
static atomic_bool time_ready;
static int config_error;

void ws_post(void *ctx, const ai_event_t *event) {
    (void)ctx;
    goldie_mutex_lock(&mailbox);
    if (count == 32) overflow = true;
    else { events[(head + count) % 32] = *event; ++count; }
    goldie_mutex_unlock(&mailbox);
}
void ws_command(int command) {
    ai_event_t event = {.type = command == 0 ? AI_TOGGLE : AI_STOP};
    ws_post(NULL, &event);
    if (command == 1) {
        goldie_mutex_lock(&mailbox); provision_pending = true; goldie_mutex_unlock(&mailbox);
    }
}
static int start(void *ctx, uint32_t session) {
    (void)ctx;
    if (!atomic_load(&time_ready)) return -8;
    /* Retry configuration after networking is ready, rather than permanently
     * caching a transient boot-time allocation/device failure. */
    ai_cloud_config_t cfg = {.media = ws_media_ops(), .post = ws_post};
    config_error = ws_cloud_config(&cfg);
    if (config_error) return config_error;
    int rc = ai_cloud_init(&cfg);
    if (rc) return rc;
    ws_media_session(session);
    return ai_cloud_start(NULL, session);
}
static int stop(void *ctx) { (void)ctx; return config_error ? 0 : ai_cloud_stop(NULL); }
static int sync_clock(void *ctx) {
    (void)ctx;
    for (;;) {
        if (ws_network_connected() && !atomic_load(&time_ready) && ws_runtime_sync_time() == 0)
            atomic_store(&time_ready, true);
        goldie_msleep(30000);
    }
    return 0;
}
static void require(int rc, const char *step) {
    if (!rc) return;
    printf("[unified] %s failed: %d\n", step, rc);
    char message[128];
    snprintf(message, sizeof(message), "Initialization failed: %s (%d)", step, rc);
    ws_ui_network(message);
    for (;;) goldie_msleep(1000);
}
static int app_task(void *ctx) {
    (void)ctx;
    goldie_mutex_init(&mailbox);
    ws_runtime_prepare();
    void *audio = ws_board_audio();
    require(ws_ui_init(), "UI");
    require(ws_runtime_init(), "runtime");
    require(ws_media_init(audio), "media");
    require(ws_runtime_sdk_init(), "SDK HAL");
    ai_cloud_config_t cfg = {.media = ws_media_ops(), .post = ws_post};
    config_error = ws_cloud_config(&cfg);
    if (!config_error) require(ai_cloud_init(&cfg), "cloud");
    else printf("[unified] Cloud configuration deferred; retry with Start (%d)\n", config_error);
    ai_app_t app;
    ai_app_ops_t ops = {.start = start, .stop = stop, .render = ws_ui_render};
    require(ai_app_init(&app, &ops), "application");
    ws_network_enable(true);
    require(ws_network_init(), "network");
    require(goldie_thread_create(sync_clock, NULL, "ai_time", 4096) ? 0 : -1, "time task");
    bool online = false;
    unsigned last_description = 0;
    for (;;) {
        uint32_t now = sys_now();
        bool connected = ws_network_connected() != 0;
        if (connected != online) {
            online = connected;
            ai_event_t event = {.type = online ? AI_NETWORK_UP : AI_NETWORK_DOWN};
            ai_app_dispatch(&app, &event, now);
        }
        ai_event_t event; bool have = false, configure;
        goldie_mutex_lock(&mailbox);
        if (overflow) {
            overflow = false; count = 0;
            event = (ai_event_t){AI_FAILED, app.session, -9}; have = true;
        } else if (count) { event = events[head]; head = (head + 1) % 32; --count; have = true; }
        configure = provision_pending; provision_pending = false;
        goldie_mutex_unlock(&mailbox);
        if (have) ai_app_dispatch(&app, &event, now);
        if (configure) {
            event = (ai_event_t){.type = AI_STOP}; ai_app_dispatch(&app, &event, now);
            ws_network_provision();
        }
        ai_app_tick(&app, now);
        if (now - last_description >= 1000) {
            last_description = now;
            char description[128];
            ws_network_description(description, sizeof(description));
            if (online && !atomic_load(&time_ready)) {
                size_t used = strlen(description);
                snprintf(description + used, sizeof(description) - used, "\nSyncing time...");
            }
            ws_ui_network(description);
        }
        goldie_msleep(20);
    }
    return 0;
}
static void standalone_entry(void) {
    if (!goldie_thread_create(app_task, NULL, "unified_ai", 8192))
        printf("[unified] Cannot create application task\n");
}
GOLDIE_INIT_CALL_(standalone_entry);
