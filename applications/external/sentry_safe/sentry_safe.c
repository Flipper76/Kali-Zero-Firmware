#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include <furi_hal.h>
#include <expansion/expansion.h>

typedef struct {
    uint8_t status;
    FuriMutex* mutex;
} SentryState;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

const char* status_texts[3] = {"[Press OK to open safe]", "Sending...", "Done !"};

static void sentry_safe_render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    const SentryState* sentry_state = ctx;
    furi_mutex_acquire(sentry_state->mutex, FuriWaitForever);

    // Before the function is called, the state is set with the canvas_reset(canvas)

    // Frame
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    // Message
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_frame(canvas, 22, 4, 84, 24);
    canvas_draw_str_aligned(canvas, 64, 15, AlignCenter, AlignBottom, "BLACK <-> GND");
    canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignBottom, "GREEN <-> C1 ");
    canvas_draw_str_aligned(
        canvas, 64, 50, AlignCenter, AlignBottom, status_texts[sentry_state->status]);

    furi_mutex_release(sentry_state->mutex);
}

static void sentry_safe_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

void send_request(int command, int a, int b, int c, int d, int e) {
    int checksum = (command + a + b + c + d + e);

    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);
    furi_hal_gpio_write(&gpio_ext_pc1, false);
    furi_delay_ms(3.4);
    furi_hal_gpio_write(&gpio_ext_pc1, true);

    FuriHalSerialHandle* serial_handle = furi_hal_serial_control_acquire(FuriHalSerialIdLpuart);
    furi_check(serial_handle);
    furi_hal_serial_init(serial_handle, 4800);

    uint8_t data[8] = {0x0, command, a, b, c, d, e, checksum};
    furi_hal_serial_tx(serial_handle, data, 8);

    furi_delay_ms(100);

    furi_hal_serial_deinit(serial_handle);
    furi_hal_serial_control_release(serial_handle);
}

void reset_code(int a, int b, int c, int d, int e) {
    send_request(0x75, a, b, c, d, e);
}

void try_code(int a, int b, int c, int d, int e) {
    send_request(0x71, a, b, c, d, e);
}

int32_t sentry_safe_app(void* p) {
    UNUSED(p);

    // Disable expansion protocol to avoid interference with UART Handle
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(Event));

    SentryState* sentry_state = malloc(sizeof(SentryState));

    sentry_state->status = 0;

    sentry_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!sentry_state->mutex) {
        FURI_LOG_E("SentrySafe", "cannot create mutex\r\n");
        furi_message_queue_free(event_queue);
        free(sentry_state);
        // Return previous state of expansion
        expansion_enable(expansion);
        furi_record_close(RECORD_EXPANSION);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, sentry_safe_render_callback, sentry_state);
    view_port_input_callback_set(view_port, sentry_safe_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    Event event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        furi_mutex_acquire(sentry_state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyOk:

                        if(sentry_state->status == 2) {
                            sentry_state->status = 0;

                        } else if(sentry_state->status == 0) {
                            sentry_state->status = 1;

                            reset_code(1, 2, 3, 4, 5);
                            furi_delay_ms(500);
                            try_code(1, 2, 3, 4, 5);

                            sentry_state->status = 2;
                        }

                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        furi_mutex_release(sentry_state->mutex);
        view_port_update(view_port);
    }

    // Reset GPIO pins to default state
    furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(sentry_state->mutex);
    free(sentry_state);

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
