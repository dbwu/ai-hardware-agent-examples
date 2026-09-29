#ifndef ESP_SERVICES_H
#define ESP_SERVICES_H
#include "ai_cloud.h"
#ifdef __cplusplus
extern "C" {
#endif

int esp_media_init(void (*post)(void *, const ai_event_t *), void *ctx);
ai_media_ops_t esp_media_ops(void);
void esp_media_set_session(uint32_t session);
int esp_network_init(void);
void esp_network_poll(void);
bool esp_network_connected(void);
void esp_network_provision(void);
void esp_network_description(char *text, size_t capacity);
bool esp_network_time_ready(void);
int esp_cloud_config(ai_cloud_config_t *config);

#ifdef __cplusplus
}
#endif
#endif
