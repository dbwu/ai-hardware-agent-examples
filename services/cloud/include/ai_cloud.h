#ifndef AI_CLOUD_H
#define AI_CLOUD_H
#include "ai_app.h"
#include <stddef.h>
#include <stdint.h>

/* send result: 0 accepted; +1 temporarily paused; negative = failure. */
typedef int (*ai_uplink_fn)(void *ctx, const uint8_t *data, size_t size);
typedef struct {
    void *ctx;
    /* enable(false) is an uplink barrier: no send callback may run afterwards.
     * reset discards queued playback; push copies borrowed G711A data.
     * Operations must be safe from the SDK callback thread. */
    void (*enable)(void *ctx, bool enabled, ai_uplink_fn send, void *send_ctx);
    void (*reset)(void *ctx);
    int (*push)(void *ctx, const uint8_t *data, size_t size);
    void (*finish)(void *ctx);
} ai_media_ops_t;

typedef struct {
    const char *config_json;
    const char *agent_id;
    const char *startup_json;
    ai_media_ops_t media;
    void *event_ctx;
    /* Must copy/enqueue the event; never call ai_app_dispatch inline. */
    void (*post)(void *ctx, const ai_event_t *event);
} ai_cloud_config_t;

/* One cloud session per device. Init/config pointers live for the application
 * lifetime. Lifecycle calls are serialized by the app owner; no GUI/RTOS API.
 * The selected SDK port must join callback threads before destroy returns. */
int ai_cloud_init(const ai_cloud_config_t *config);
int ai_cloud_start(void *unused, uint32_t session);
int ai_cloud_stop(void *unused);
#endif
