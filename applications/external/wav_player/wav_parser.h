#pragma once
#include <toolbox/stream/stream.h>

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/view_holder.h>
#include <toolbox/stream/file_stream.h>

#include "wav_player_view.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FormatTagPCM = 0x0001,
    FormatTagIEEE_FLOAT = 0x0003,
} FormatTag;

typedef struct {
    uint8_t riff[4];
    uint32_t size;
    uint8_t wave[4];
} WavHeaderChunk;

typedef struct {
    uint8_t fmt[4];
    uint32_t size;
    uint16_t tag;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_per_sec;
    uint16_t block_align;
    uint16_t bits_per_sample;
} WavFormatChunk;

typedef struct {
    uint8_t data[4];
    uint32_t size;
} WavDataChunk;

typedef struct WavParser WavParser;

typedef struct {
    Storage* storage;
    Stream* stream;
    WavParser* parser;
    uint8_t* sample_buffer;
    uint8_t* tmp_buffer;

    uint32_t sample_rate;

    uint16_t num_channels;
    uint16_t bits_per_sample;

    size_t samples_count_half;
    size_t samples_count;

    FuriMessageQueue* queue;

    float volume;
    bool play;

    WavPlayerView* view;
    ViewHolder* view_holder;
    Gui* gui;
    NotificationApp* notification;

    FuriString* path;
} WavPlayerApp;

WavParser* wav_parser_alloc();

void wav_parser_free(WavParser* parser);

bool wav_parser_parse(WavParser* parser, Stream* stream, WavPlayerApp* app);

size_t wav_parser_get_data_start(WavParser* parser);

size_t wav_parser_get_data_end(WavParser* parser);

size_t wav_parser_get_data_len(WavParser* parser);

#ifdef __cplusplus
}
#endif
