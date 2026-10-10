#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wininet.h>
#include <time.h>
#include <stdio.h>
#include <stdatomic.h>
#include "runtime.h"
#include "ws_services.h"
#include "ws_board.h"
extern int convai_platform_win_init(void);
extern int win_device_id(char *, size_t);
static atomic_bool enabled = true;
unsigned int sys_now(void) { return GetTickCount(); }
void ws_runtime_prepare(void) { ws_board_prepare(); }
int ws_runtime_init(void) { return ws_board_init(); }
int ws_runtime_sync_time(void) { return time(NULL) >= 1704067200 ? 0 : -1; }
int ws_runtime_sdk_init(void) { return convai_platform_win_init(); }
int ws_runtime_device_id(char *text, size_t size) { return win_device_id(text,size); }
int ws_network_init(void) { return 0; }
bool ws_network_connected(void) {
    DWORD flags;
    return atomic_load(&enabled) && InternetGetConnectedState(&flags,0);
}
void ws_network_enable(bool value) { atomic_store(&enabled,value); }
void ws_network_provision(void) { /* No provisioning on the desktop target. */ }
void ws_network_description(char *text, size_t capacity) {
    snprintf(text,capacity,"Windows network: %s",ws_network_connected() ? "available" : "offline");
}
