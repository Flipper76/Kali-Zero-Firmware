#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stm32wbxx_ll_dma.h>
#include "wav_player_hal.h"
#include "wav_parser.h"
#include "wav_player_view.h"
#include <math.h>

#include <wav_player_icons.h>

#include <assets_icons.h>

#define TAG "WavPlayer"

#define WAVPLAYER_FOLDER "/ext/wav_player"

static bool open_wav_stream(WavPlayerApp* app) {
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    bool result = false;

    if(furi_string_empty(app->path)) {
        furi_string_set(app->path, WAVPLAYER_FOLDER);

        DialogsFileBrowserOptions browser_options;
        dialog_file_browser_set_basic_options(&browser_options, ".wav", &I_music_10px);
        browser_options.base_path = WAVPLAYER_FOLDER;
        browser_options.hide_ext = false;

        bool ret = dialog_file_browser_show(dialogs, app->path, app->path, &browser_options);

        furi_record_close(RECORD_DIALOGS);

        if(!ret) return false;
    }

    if(!file_stream_open(
           app->stream, furi_string_get_cstr(app->path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Cannot open file \"%s\"", furi_string_get_cstr(app->path));
    } else {
        result = true;
    }

    return result;
}

typedef enum {
    WavPlayerEventHalfTransfer,
    WavPlayerEventFullTransfer,
    WavPlayerEventCtrlVolUp,
    WavPlayerEventCtrlVolDn,
    WavPlayerEventCtrlMoveL,
    WavPlayerEventCtrlMoveR,
    WavPlayerEventCtrlOk,
    WavPlayerEventCtrlBack,
} WavPlayerEventType;

typedef struct {
    WavPlayerEventType type;
} WavPlayerEvent;

static void wav_player_dma_isr(void* ctx) {
    FuriMessageQueue* event_queue = ctx;

    // half of transfer
    if(LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);
        // fill first half of buffer
        WavPlayerEvent event = {.type = WavPlayerEventHalfTransfer};
        furi_message_queue_put(event_queue, &event, 0);
    }

    // transfer complete
    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        // fill second half of buffer
        WavPlayerEvent event = {.type = WavPlayerEventFullTransfer};
        furi_message_queue_put(event_queue, &event, 0);
    }
}

static void exit_callback(void* ctx) {
    FuriMessageQueue* event_queue = ctx;

    WavPlayerEvent event;
    event.type = WavPlayerEventCtrlBack;
    furi_message_queue_put(event_queue, &event, 0);
}

static bool thread_exit_signal_callback(uint32_t signal, void* arg, void* ctx) {
    UNUSED(arg);
    if(signal == FuriSignalExit) {
        exit_callback(ctx);
        return true;
    }
    return false;
}

static WavPlayerApp* app_alloc() {
    WavPlayerApp* app = malloc(sizeof(WavPlayerApp));
    app->samples_count_half = 1024 * 4;
    app->samples_count = app->samples_count_half * 2;
    app->storage = furi_record_open(RECORD_STORAGE);
    app->stream = file_stream_alloc(app->storage);
    app->parser = wav_parser_alloc();
    app->sample_buffer = malloc(sizeof(*app->sample_buffer) * app->samples_count);
    app->tmp_buffer = malloc(sizeof(*app->tmp_buffer) * app->samples_count);
    app->queue = furi_message_queue_alloc(10, sizeof(WavPlayerEvent));

    app->volume = 10.0f;
    app->play = true;

    app->gui = furi_record_open(RECORD_GUI);
    app->view_holder = view_holder_alloc();
    app->view = wav_player_view_alloc();

    app->path = furi_string_alloc();

    view_holder_set_back_callback(app->view_holder, exit_callback, app->queue);
    view_holder_attach_to_gui(app->view_holder, app->gui);
    view_holder_set_view(app->view_holder, wav_player_view_get_view(app->view));
    view_holder_send_to_front(app->view_holder);

    app->notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(app->notification, &sequence_display_backlight_enforce_on);

    furi_thread_set_signal_callback(
        furi_thread_get_current(), thread_exit_signal_callback, app->queue);

    return app;
}

static void app_free(WavPlayerApp* app) {
    view_holder_set_view(app->view_holder, NULL);
    view_holder_free(app->view_holder);
    wav_player_view_free(app->view);
    furi_record_close(RECORD_GUI);

    furi_message_queue_free(app->queue);
    free(app->tmp_buffer);
    free(app->sample_buffer);
    wav_parser_free(app->parser);
    stream_free(app->stream);
    furi_record_close(RECORD_STORAGE);

    furi_string_free(app->path);

    notification_message(app->notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);
    free(app);
}

// TODO: that works only with 8-bit 2ch audio
static bool fill_data(WavPlayerApp* app, size_t index) {
    if(app->num_channels == 1 && app->bits_per_sample == 8) {
        uint8_t* sample_buffer_start = &app->sample_buffer[index];
        size_t count = stream_read(app->stream, app->tmp_buffer, app->samples_count_half);

        for(size_t i = count; i < app->samples_count_half; i++) {
            app->tmp_buffer[i] = 0;
        }

        //for(size_t i = 0; i < app->samples_count; i += 2)
        for(size_t i = 0; i < app->samples_count_half; i++) //now works with mono!
        {
            float data = app->tmp_buffer[i];
            data -= UINT8_MAX / 2; // to signed
            data /= UINT8_MAX / 2; // scale -1..1

            data *= app->volume; // volume
            data = tanhf(data); // hyperbolic tangent limiter

            data *= UINT8_MAX / 2; // scale -128..127
            data += UINT8_MAX / 2; // to unsigned

            if(data < 0) {
                data = 0;
            }

            if(data > 255) {
                data = 255;
            }

            //uint8_t data = app->tmp_buffer[i];

            sample_buffer_start[i] = data;
        }

        wav_player_view_set_data(app->view, sample_buffer_start, app->samples_count_half);

        return count != app->samples_count_half;
    }

    if(app->num_channels == 1 && app->bits_per_sample == 16) {
        uint8_t* sample_buffer_start = &app->sample_buffer[index];
        size_t count = stream_read(app->stream, app->tmp_buffer, app->samples_count);

        for(size_t i = count; i < app->samples_count; i++) {
            //app->tmp_buffer[i] = 0;
        }

        for(size_t i = 0; i < app->samples_count; i += 2) {
            int16_t int_16 =
                (((int16_t)app->tmp_buffer[i + 1] << 8) + (int16_t)app->tmp_buffer[i]);

            float data = ((float)int_16 / 256.0 + 127.0);
            data -= UINT8_MAX / 2; // to signed
            data /= UINT8_MAX / 2; // scale -1..1

            data *= app->volume; // volume
            data = tanhf(data); // hyperbolic tangent limiter

            data *= UINT8_MAX / 2; // scale -128..127
            data += UINT8_MAX / 2; // to unsigned

            if(data < 0) {
                data = 0;
            }

            if(data > 255) {
                data = 255;
            }

            sample_buffer_start[i / 2] = data;
        }

        wav_player_view_set_data(app->view, sample_buffer_start, app->samples_count_half);

        return count != app->samples_count;
    }

    if(app->num_channels == 2 && app->bits_per_sample == 16) {
        uint8_t* sample_buffer_start = &app->sample_buffer[index];
        size_t count = stream_read(app->stream, app->tmp_buffer, app->samples_count);

        for(size_t i = 0; i < app->samples_count; i += 4) {
            int16_t L = (((int16_t)app->tmp_buffer[i + 1] << 8) + (int16_t)app->tmp_buffer[i]);
            int16_t R = (((int16_t)app->tmp_buffer[i + 3] << 8) + (int16_t)app->tmp_buffer[i + 2]);
            int32_t int_16 = L / 2 + R / 2; // (L + R) / 2

            float data = ((float)int_16 / 256.0 + 127.0);
            data -= UINT8_MAX / 2; // to signed
            data /= UINT8_MAX / 2; // scale -1..1

            data *= app->volume; // volume
            data = tanhf(data); // hyperbolic tangent limiter

            data *= UINT8_MAX / 2; // scale -128..127
            data += UINT8_MAX / 2; // to unsigned

            if(data < 0) {
                data = 0;
            }

            if(data > 255) {
                data = 255;
            }

            sample_buffer_start[i / 4] = data;
        }

        count = stream_read(app->stream, app->tmp_buffer, app->samples_count);

        for(size_t i = 0; i < app->samples_count; i += 4) {
            int16_t L = (((int16_t)app->tmp_buffer[i + 1] << 8) + (int16_t)app->tmp_buffer[i]);
            int16_t R = (((int16_t)app->tmp_buffer[i + 3] << 8) + (int16_t)app->tmp_buffer[i + 2]);
            int32_t int_16 = L / 2 + R / 2; // (L + R) / 2

            float data = ((float)int_16 / 256.0 + 127.0);
            data -= UINT8_MAX / 2; // to signed
            data /= UINT8_MAX / 2; // scale -1..1

            data *= app->volume; // volume
            data = tanhf(data); // hyperbolic tangent limiter

            data *= UINT8_MAX / 2; // scale -128..127
            data += UINT8_MAX / 2; // to unsigned

            if(data < 0) {
                data = 0;
            }

            if(data > 255) {
                data = 255;
            }

            sample_buffer_start[i / 4 + app->samples_count / 4] = data;
        }

        wav_player_view_set_data(app->view, sample_buffer_start, app->samples_count_half);

        return count != app->samples_count;
    }

    if(app->num_channels == 2 && app->bits_per_sample == 8) {
        uint8_t* sample_buffer_start = &app->sample_buffer[index];
        size_t count = stream_read(app->stream, app->tmp_buffer, app->samples_count);

        for(size_t i = count; i < app->samples_count; i++) {
            app->tmp_buffer[i] = 0;
        }

        for(size_t i = 0; i < app->samples_count; i += 2) {
            float data = (app->tmp_buffer[i] + app->tmp_buffer[i + 1]) / 2; // (L + R) / 2
            data -= UINT8_MAX / 2; // to signed
            data /= UINT8_MAX / 2; // scale -1..1

            data *= app->volume; // volume
            data = tanhf(data); // hyperbolic tangent limiter

            data *= UINT8_MAX / 2; // scale -128..127
            data += UINT8_MAX / 2; // to unsigned

            if(data < 0) {
                data = 0;
            }

            if(data > 255) {
                data = 255;
            }

            //uint8_t data = app->tmp_buffer[i];

            sample_buffer_start[i / 2] = data;
        }

        wav_player_view_set_data(app->view, sample_buffer_start, app->samples_count_half);

        return count != app->samples_count;
    }

    return true;
}

static void ctrl_callback(WavPlayerCtrl ctrl, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    WavPlayerEvent event;

    switch(ctrl) {
    case WavPlayerCtrlVolUp:
        event.type = WavPlayerEventCtrlVolUp;
        furi_message_queue_put(event_queue, &event, 0);
        break;
    case WavPlayerCtrlVolDn:
        event.type = WavPlayerEventCtrlVolDn;
        furi_message_queue_put(event_queue, &event, 0);
        break;
    case WavPlayerCtrlMoveL:
        event.type = WavPlayerEventCtrlMoveL;
        furi_message_queue_put(event_queue, &event, 0);
        break;
    case WavPlayerCtrlMoveR:
        event.type = WavPlayerEventCtrlMoveR;
        furi_message_queue_put(event_queue, &event, 0);
        break;
    case WavPlayerCtrlOk:
        event.type = WavPlayerEventCtrlOk;
        furi_message_queue_put(event_queue, &event, 0);
        break;
    default:
        break;
    }
}

static void app_run(WavPlayerApp* app) {
    if(!open_wav_stream(app)) return;

    if(!wav_parser_parse(app->parser, app->stream, app)) return;

    wav_player_view_set_volume(app->view, app->volume);
    wav_player_view_set_start(app->view, wav_parser_get_data_start(app->parser));
    wav_player_view_set_current(app->view, stream_tell(app->stream));
    wav_player_view_set_end(app->view, wav_parser_get_data_end(app->parser));
    wav_player_view_set_play(app->view, app->play);

    wav_player_view_set_context(app->view, app->queue);
    wav_player_view_set_ctrl_callback(app->view, ctrl_callback);

    bool eof = fill_data(app, 0);
    eof = fill_data(app, app->samples_count_half);

    if(furi_hal_speaker_acquire(1000)) {
        wav_player_speaker_init(app->sample_rate);
        wav_player_dma_init((uint32_t)app->sample_buffer, app->samples_count);

        furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, wav_player_dma_isr, app->queue);

        wav_player_dma_start();
        wav_player_speaker_start();

        WavPlayerEvent event;

        while(1) {
            if(furi_message_queue_get(app->queue, &event, FuriWaitForever) == FuriStatusOk) {
                if(event.type == WavPlayerEventHalfTransfer) {
                    wav_player_view_set_chans(app->view, app->num_channels);
                    wav_player_view_set_bits(app->view, app->bits_per_sample);

                    eof = fill_data(app, 0);
                    wav_player_view_set_current(app->view, stream_tell(app->stream));
                    if(eof) {
                        stream_seek(
                            app->stream,
                            wav_parser_get_data_start(app->parser),
                            StreamOffsetFromStart);
                    }

                } else if(event.type == WavPlayerEventFullTransfer) {
                    wav_player_view_set_chans(app->view, app->num_channels);
                    wav_player_view_set_bits(app->view, app->bits_per_sample);

                    eof = fill_data(app, app->samples_count_half);
                    wav_player_view_set_current(app->view, stream_tell(app->stream));
                    if(eof) {
                        stream_seek(
                            app->stream,
                            wav_parser_get_data_start(app->parser),
                            StreamOffsetFromStart);
                    }
                } else if(event.type == WavPlayerEventCtrlVolUp) {
                    if(app->volume < 9.9) app->volume += 0.4;
                    wav_player_view_set_volume(app->view, app->volume);
                } else if(event.type == WavPlayerEventCtrlVolDn) {
                    if(app->volume > 0.01) app->volume -= 0.4;
                    wav_player_view_set_volume(app->view, app->volume);
                } else if(event.type == WavPlayerEventCtrlMoveL) {
                    int32_t seek =
                        stream_tell(app->stream) - wav_parser_get_data_start(app->parser);
                    seek = MIN(
                        seek,
                        (int32_t)(wav_parser_get_data_len(app->parser) / (size_t)100) % 2 ?
                            ((int32_t)(wav_parser_get_data_len(app->parser) / (size_t)100) - 1) :
                            (int32_t)(wav_parser_get_data_len(app->parser) / (size_t)100));
                    stream_seek(app->stream, -seek, StreamOffsetFromCurrent);
                    wav_player_view_set_current(app->view, stream_tell(app->stream));
                } else if(event.type == WavPlayerEventCtrlMoveR) {
                    int32_t seek = wav_parser_get_data_end(app->parser) - stream_tell(app->stream);
                    seek = MIN(
                        seek,
                        (int32_t)(wav_parser_get_data_len(app->parser) / (size_t)100) % 2 ?
                            ((int32_t)(wav_parser_get_data_len(app->parser) / (size_t)100) - 1) :
                            (int32_t)(wav_parser_get_data_len(app->parser) / (size_t)100));
                    stream_seek(app->stream, seek, StreamOffsetFromCurrent);
                    wav_player_view_set_current(app->view, stream_tell(app->stream));
                } else if(event.type == WavPlayerEventCtrlOk) {
                    app->play = !app->play;
                    wav_player_view_set_play(app->view, app->play);

                    if(!app->play) {
                        wav_player_speaker_stop();
                    } else {
                        wav_player_speaker_start();
                    }
                } else if(event.type == WavPlayerEventCtrlBack) {
                    break;
                }
            }
        }

        wav_player_speaker_stop();
        wav_player_dma_stop();
        furi_hal_speaker_release();
    }

    // Reset GPIO pin and bus states
    wav_player_hal_deinit();

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, NULL, NULL);
}

int32_t wav_player_app(void* p) {
    const char* args = p;

    WavPlayerApp* app = app_alloc();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage_simply_mkdir(storage, WAVPLAYER_FOLDER)) {
        FURI_LOG_E(TAG, "Could not create folder %s", WAVPLAYER_FOLDER);
    }
    furi_record_close(RECORD_STORAGE);

    if(args && strlen(args)) {
        furi_string_set(app->path, args);
    }

    app_run(app);
    app_free(app);
    return 0;
}
