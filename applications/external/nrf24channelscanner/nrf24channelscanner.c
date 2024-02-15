#include <stdio.h>
#include <furi.h>
#include <furi_hal_power.h>
#include <gui/gui.h>
#include <input/input.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include <nrf24.h>
#include "nrf24channelscanner_icons.h"
#include <assets_icons.h>

const uint8_t num_channels = 128;
static uint8_t nrf24values[128] = {0}; //to store channel data

bool ifNotFoundNrf = false; //to show error message
bool szuz = true; //to show welcome screen
static bool isScanning = false; //to track the progress
static bool stopNrfScan = false; //to exit thread

static bool isInfiniteScan = false; //to prevent stop scan when OK long pressed

static bool threadStoppedsoFree = false; //indicate if I can free the thread from ram.
static uint8_t currCh = 0; //for the progress bar or the channel selector

static int delayPerChan = 150; //can set via up / down.

bool showFreq = true;

FuriThread* thread;

typedef enum {
    EventTypeKey,
    EventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, 1);
    canvas_draw_icon(canvas, 100, 0, &I_Pin_back_arrow_10x8);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 112, 8, "Exit");
    canvas_draw_icon(canvas, 1, 0, &I_Ok_btn_9x9);
    canvas_set_font(canvas, FontSecondary);
    if(isScanning) {
        canvas_draw_str(canvas, 12, 8, "Stop");
    } else {
        canvas_draw_str(canvas, 12, 8, "Scan");
    }
    canvas_draw_line(canvas, 0, 11, 127, 11);

    if(ifNotFoundNrf) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 23, 35, "NRF24 not found!");
        return;
    }

    canvas_draw_line(canvas, currCh, 12, currCh, 13); //draw the current channel

    //draw hello mesage
    if(szuz) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 1, 22, "OK: scan / stop. Long: infinite.");
        canvas_draw_str(canvas, 1, 33, "Up / Down to change channel time.");
        canvas_draw_str(canvas, 1, 44, "Left / Right to select channel");
        canvas_draw_str(canvas, 1, 56, "  to get it's frequency");
    }

    //draw freq ir the progress
    canvas_set_font(canvas, FontSecondary);
    if(isScanning) {
        if(isInfiniteScan)
            canvas_draw_str(canvas, 37, 8, "scanning...");
        else
            canvas_draw_str(canvas, 37, 8, "scanning");

    } else {
        if(showFreq) {
            int freq = 2400 + currCh;
            char strfreq[10] = {0};
            snprintf(strfreq, sizeof(strfreq), "%d MHZ", freq);
            canvas_draw_str(canvas, 40, 8, strfreq);
        } else {
            //show delay
            int dly = delayPerChan;
            char strdel[10] = {0};
            snprintf(strdel, sizeof(strdel), "%d us", dly);
            canvas_draw_str(canvas, 40, 8, strdel);
        }
    }

    //draw the chart
    for(int i = 0; i < num_channels; ++i) {
        int h = 64 - nrf24values[i];
        if(h < 11) h = 12;
        canvas_draw_line(canvas, i, h, i, 64);
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static int32_t scanner(void* context) {
    UNUSED(context);
    isScanning = true;
    stopNrfScan = false;
    threadStoppedsoFree = false;
    uint8_t tmp = 0;
    currCh = 0;
    nrf24_set_rx_mode(nrf24_HANDLE, false);
    nrf24_write_reg(nrf24_HANDLE, REG_EN_AA, 0x0);
    nrf24_write_reg(nrf24_HANDLE, REG_RF_SETUP, 0x0f);
    while(true) { //scan until stopped somehow
        if(stopNrfScan) break;
        for(uint8_t i = 0; i < num_channels; i++) {
            if(stopNrfScan) break;
            currCh = i;
            nrf24_write_reg(nrf24_HANDLE, REG_RF_CH, i);
            nrf24_set_rx_mode(nrf24_HANDLE, true);
            for(uint8_t ii = 0; ii < 3; ++ii) {
                nrf24_flush_rx(nrf24_HANDLE);
                furi_delay_us(delayPerChan);
                tmp = nrf24_get_rdp(nrf24_HANDLE);
                if(tmp > 0 && nrf24values[i] < 65) {
                    nrf24values[i]++; //don't overrun it
                }
                if(nrf24values[i] > 50 && !isInfiniteScan) {
                    stopNrfScan = true; //stop, bc maxed, but only when not infinite
                }
            }
        }
        furi_delay_ms(1);
        //for screen refresh.
    }
    //cleanup
    nrf24_set_idle(nrf24_HANDLE);
    isScanning = false;
    threadStoppedsoFree = true;
    currCh = 0;
    return 0;
}

void ChangeFreq(int delta) {
    currCh += delta;
    if(currCh > num_channels) currCh = 0;
    showFreq = true;
}

void ChangeDelay(int delta) {
    delayPerChan += delta;
    if(delayPerChan > 4000) delayPerChan = 4000;
    if(delayPerChan < 120) delayPerChan = 120;
    if(delayPerChan == 170) delayPerChan = 150; //rounding for the next
    showFreq = false;
}

// Main entry of the application
int32_t nrf24channelscanner_main(void* p) {
    UNUSED(p);

    Event event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(Event));

    //turn on 5v for some modules
    uint8_t attempts = 0;
    bool otg_was_enabled = furi_hal_power_is_otg_enabled();
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }

    nrf24_init();

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    while(true) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.type == EventTypeKey) {
            szuz = false; //hit any button, so hide welcome screen
            if((event.input.type == InputTypeShort || event.input.type == InputTypeLong) &&
               event.input.key == InputKeyBack) {
                if(isScanning) {
                    stopNrfScan = true; //if running, stop it.
                    notification_message(notification, &sequence_blink_yellow_100);
                    furi_thread_join(thread);
                    furi_thread_free(thread);
                }
                break;
            }
            //isInfiniteScan
            if((event.input.type == InputTypeShort || event.input.type == InputTypeLong) &&
               event.input.key == InputKeyOk) {
                if(isScanning) {
                    notification_message(notification, &sequence_blink_yellow_100);
                    stopNrfScan = true;
                    furi_thread_join(thread);
                    furi_thread_free(thread);
                    threadStoppedsoFree = false; //to prevent double free
                    continue;
                }
                memset(nrf24values, 0, sizeof(nrf24values));
                if(nrf24_check_connected(nrf24_HANDLE)) {
                    threadStoppedsoFree = false;
                    ifNotFoundNrf = false;
                    notification_message(notification, &sequence_blink_green_100);
                    isInfiniteScan = (event.input.type == InputTypeLong);
                    thread = furi_thread_alloc();
                    furi_thread_set_name(thread, "nrfscannerth");
                    furi_thread_set_stack_size(thread, 1024);
                    furi_thread_set_callback(thread, scanner);
                    furi_thread_start(thread);
                } else {
                    ifNotFoundNrf = true;
                    notification_message(notification, &sequence_error);
                }
            }
            //change the delay
            if(event.input.type == InputTypeShort && event.input.key == InputKeyUp) {
                ChangeDelay(50);
            }
            if(event.input.type == InputTypeShort && event.input.key == InputKeyDown) {
                ChangeDelay(-50);
            }

            if(!isScanning) {
                if(event.input.type == InputTypeLong && event.input.key == InputKeyLeft)
                    ChangeFreq(-10);
                if(event.input.type == InputTypeShort && event.input.key == InputKeyLeft)
                    ChangeFreq(-1);
                if(event.input.type == InputTypeLong && event.input.key == InputKeyRight)
                    ChangeFreq(10);
                if(event.input.type == InputTypeShort && event.input.key == InputKeyRight)
                    ChangeFreq(1);
            }
        }
        if(threadStoppedsoFree) {
            threadStoppedsoFree = false;
            furi_thread_join(thread);
            furi_thread_free(thread);
        }
    }
    nrf24_deinit();

    if(furi_hal_power_is_otg_enabled() && !otg_was_enabled) {
        furi_hal_power_disable_otg();
    }

    furi_message_queue_free(event_queue);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
