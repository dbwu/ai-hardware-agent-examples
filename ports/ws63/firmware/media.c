#include "ws_services.h"
#include "ws63_config.h"
#include "ai_audio.h"
#include "ai_playback.h"
#include "goldie_osal.h"
#include "audio_service.h"
#include "ws_board.h"

static AudioService *hw;
static ws_board_capture_t capture_format;
static goldie_mutex tx, rx;
static bool enabled, playing;
static uint32_t session, capture_epoch;
static ai_uplink_fn send_audio;
static void *send_ctx;
static ai_playback_t queue;
static uint8_t storage[WS_AI_PLAYBACK_BYTES];

static void enable(void *ctx, bool on, ai_uplink_fn send, void *arg) {
    (void)ctx;
    goldie_mutex_lock(&tx);
    ++capture_epoch; enabled = on; send_audio = send; send_ctx = arg;
    goldie_mutex_unlock(&tx);
}
static void reset(void *ctx) {
    (void)ctx;
    goldie_mutex_lock(&rx);
    ai_playback_reset(&queue);
    if (playing) hw->play_stop();
    playing = false;
    hw->reset_playbuffer();
    goldie_mutex_unlock(&rx);
}
static int push(void *ctx, const uint8_t *data, size_t size) {
    (void)ctx;
    goldie_mutex_lock(&rx);
    int rc = ai_playback_push(&queue, data, size);
    goldie_mutex_unlock(&rx);
    return rc;
}
static void finish(void *ctx) {
    (void)ctx;
    goldie_mutex_lock(&rx); ai_playback_finish(&queue); goldie_mutex_unlock(&rx);
}
static int capture(void *ctx) {
    (void)ctx;
    int16_t pcm[320]; uint8_t encoded[320];
    unsigned failures = 0;
    hw->record_start();
    for (;;) {
        goldie_mutex_lock(&tx);
        uint32_t epoch = capture_epoch;
        goldie_mutex_unlock(&tx);
        int bytes = hw->audio_read(pcm, sizeof(pcm));
        goldie_mutex_lock(&tx);
        if (enabled && epoch == capture_epoch) {
            int rc = -1;
            if (bytes > 0 && bytes <= (int)sizeof(pcm) && bytes % (int)(capture_format.channels * sizeof(int16_t)) == 0) {
                int size = ai_audio_encode_slots(pcm, (size_t)bytes / (capture_format.channels * sizeof(int16_t)),
                                                 capture_format.channels, capture_format.microphone, capture_format.reference,
                                                 encoded, sizeof(encoded));
                if (size > 0) rc = send_audio(send_ctx, encoded, (size_t)size);
            }
            if (rc >= 0) failures = 0;
            else if (++failures >= 30) {
                enabled = false;
                ai_event_t event = {AI_FAILED, session, -5};
                ws_post(NULL, &event);
            }
        } else failures = 0;
        goldie_mutex_unlock(&tx);
        if (bytes <= 0) goldie_msleep(10);
    }
    return 0;
}
static int playback(void *ctx) {
    (void)ctx;
    uint8_t encoded[160]; int16_t pcm[160];
    for (;;) {
        goldie_mutex_lock(&rx);
        unsigned level = playing ? hw->get_valid_length(NULL) : 0;
        if (level < 4096) {
            size_t n = ai_playback_pull(&queue, encoded, sizeof(encoded));
            if (n) {
                if (!playing) {
                    /* GoldieOS fixed-rate backends (WS63 and Windows) may
                     * omit this callback, as in the vendor audio bridge. */
                    if (hw->audio_output_config) hw->audio_output_config(8000);
                    hw->play_start(); playing = true;
                }
                for (size_t i = 0; i < n; ++i) pcm[i] = ai_alaw_decode(encoded[i]);
                /* WS63 output is mono PCM16, unlike ESP32's stereo TX. */
                int rc = hw->audio_write(pcm, (unsigned)(n * sizeof(pcm[0])));
                if (rc < 0) {
                    ai_playback_reset(&queue);
                    hw->play_stop(); playing = false; hw->reset_playbuffer();
                    ai_event_t event = {AI_FAILED, session, -6}; ws_post(NULL, &event);
                }
            }
        }
        if (queue.finished && !queue.size && (!playing || !hw->get_valid_length(NULL))) {
            queue.finished = false;
            if (playing) hw->play_stop();
            playing = false;
            ai_event_t event = {AI_TURN_FINISHED, session, 0}; ws_post(NULL, &event);
        }
        goldie_mutex_unlock(&rx);
        goldie_msleep(5);
    }
    return 0;
}
int ws_media_init(void *audio_service) {
    capture_format = ws_board_capture_format();
    if (!capture_format.channels || capture_format.channels > 8 ||
        capture_format.microphone >= capture_format.channels ||
        (capture_format.reference != AI_AUDIO_NO_REFERENCE && capture_format.reference >= capture_format.channels)) return -21;
    hw = audio_service;
    if (!hw) return -10;
    if (!hw->audio_read) return -11;
    if (!hw->audio_write) return -12;
    if (!hw->record_start) return -13;
    if (!hw->play_start) return -14;
    if (!hw->play_stop) return -15;
    /* audio_output_config is optional; fixed-rate BSPs already use 8 kHz. */
    if (!hw->get_valid_length) return -17;
    if (!hw->reset_playbuffer) return -18;
    goldie_mutex_init(&tx); goldie_mutex_init(&rx);
    if (ai_playback_init(&queue, storage, sizeof(storage), 1600)) return -1;
    if (!goldie_thread_create(capture, NULL, "ai_capture", 8192)) return -19;
    if (!goldie_thread_create(playback, NULL, "ai_playback", 8192)) return -20;
    return 0;
}
void ws_media_session(uint32_t id) {
    goldie_mutex_lock(&tx); goldie_mutex_lock(&rx);
    session = id;
    goldie_mutex_unlock(&rx); goldie_mutex_unlock(&tx);
}
ai_media_ops_t ws_media_ops(void) {
    ai_media_ops_t ops = {.enable = enable, .reset = reset, .push = push, .finish = finish};
    return ops;
}
