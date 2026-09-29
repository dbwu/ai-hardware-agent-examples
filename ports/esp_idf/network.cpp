#include "esp_services.h"
#include "wifi_manager.h"
#include "ssid_manager.h"
#include "esp_sntp.h"
#include "esp_timer.h"
#include <atomic>
#include <cstdio>
#include <ctime>

static std::atomic<bool> online{false}, retry_station{false};
static bool sntp_started;
static int64_t connecting_since;

int esp_network_init(void) {
    auto &wifi = WifiManager::GetInstance();
    wifi.SetEventCallback([](WifiEvent event, const std::string &) {
        if (event == WifiEvent::Connected) online = true;
        if (event == WifiEvent::Disconnected || event == WifiEvent::ConfigModeEnter)
            online = false;
        if (event == WifiEvent::ConfigModeExit) retry_station = true;
    });
    WifiManagerConfig config;
    config.ssid_prefix = "AI-Setup";
    config.language = "zh-CN";
    if (!wifi.Initialize(config)) return -1;
    connecting_since = esp_timer_get_time();
    if (SsidManager::GetInstance().GetSsidList().empty()) wifi.StartConfigAp();
    else wifi.StartStation();
    return 0;
}
void esp_network_poll(void) {
    auto &wifi = WifiManager::GetInstance();
    if (retry_station.exchange(false)) {
        wifi.StartStation();
        connecting_since = esp_timer_get_time();
    }
    if (online && !sntp_started) {
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, const_cast<char *>("pool.ntp.org"));
#if CONFIG_LWIP_SNTP_MAX_SERVERS > 1
        esp_sntp_setservername(1, const_cast<char *>("ntp.aliyun.com"));
#endif
        esp_sntp_init();
        sntp_started = true;
    }
    if (online) connecting_since = esp_timer_get_time();
    else if (!wifi.IsConfigMode() && esp_timer_get_time() - connecting_since > 30000000)
        wifi.StartConfigAp();
}
bool esp_network_connected(void) { return online.load(); }
void esp_network_provision(void) {
    online = false;
    WifiManager::GetInstance().StartConfigAp();
}
bool esp_network_time_ready(void) { return std::time(nullptr) >= 1704067200; }
void esp_network_description(char *text, size_t capacity) {
    auto &wifi = WifiManager::GetInstance();
    if (wifi.IsConfigMode())
        std::snprintf(text, capacity, "Setup: %s\n%s", wifi.GetApSsid().c_str(), wifi.GetApWebUrl().c_str());
    else if (online)
        std::snprintf(text, capacity, "Wi-Fi connected\n%s%s", wifi.GetIpAddress().c_str(),
                      esp_network_time_ready() ? "" : "\nSyncing time...");
    else std::snprintf(text, capacity, "Connecting Wi-Fi...\nTap Wi-Fi to configure");
}
