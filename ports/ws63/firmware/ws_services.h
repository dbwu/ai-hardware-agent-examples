#ifndef WS_SERVICES_H
#define WS_SERVICES_H
#include "ai_app.h"
#include "ai_cloud.h"
#ifdef __cplusplus
extern "C" {
#endif
void ws_post(void *ctx, const ai_event_t *event);
void ws_command(int command); /* 0 toggle, 1 Wi-Fi page/stop, 2 stop */
int ws_media_init(void *audio_service);
void ws_media_session(uint32_t session);
ai_media_ops_t ws_media_ops(void);
int ws_cloud_config(ai_cloud_config_t *config);
int ws_ui_init(void);
void ws_ui_render(void *ctx, const ai_view_t *view);
void ws_ui_network(const char *text);
int ws_network_init(void);
bool ws_network_connected(void);
void ws_network_provision(void);
void ws_network_enable(bool enabled);
void ws_network_description(char *text, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
