#include "uart_terminal_app_i.h"

#include <furi.h>
#include <furi_hal.h>
#include <expansion/expansion.h>

static bool uart_terminal_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool uart_terminal_app_back_event_callback(void* context) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void uart_terminal_app_tick_event_callback(void* context) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

UART_TerminalApp* uart_terminal_app_alloc() {
    UART_TerminalApp* app = malloc(sizeof(UART_TerminalApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&uart_terminal_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, uart_terminal_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, uart_terminal_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, uart_terminal_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        UART_TerminalAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    for(int i = 0; i < START_MENU_ITEMS; ++i) {
        app->selected_option_index[i] = 0;
    }

    for(int i = 0; i < SETUP_MENU_ITEMS; ++i) {
        app->setup_selected_option_index[i] = 0;
    }

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UART_TerminalAppViewHelp, widget_get_view(app->widget));

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UART_TerminalAppViewConsoleOutput, text_box_get_view(app->text_box));
    app->text_box_store = furi_string_alloc();
    furi_string_reserve(app->text_box_store, UART_TERMINAL_TEXT_BOX_STORE_SIZE);

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UART_TerminalAppViewTextInput, text_input_get_view(app->text_input));

    app->hex_input = uart_hex_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        UART_TerminalAppViewHexInput,
        uart_hex_input_get_view(app->hex_input));

    app->setup_selected_option_index[BAUDRATE_ITEM_IDX] = DEFAULT_BAUDRATE_OPT_IDX;

    app->old_term_mode = 0;
    app->TERMINAL_MODE = 0;
    app->atmode_was_set = false;

    scene_manager_next_scene(app->scene_manager, UART_TerminalSceneStart);

    return app;
}

void uart_terminal_app_free(UART_TerminalApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, UART_TerminalAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, UART_TerminalAppViewHelp);
    view_dispatcher_remove_view(app->view_dispatcher, UART_TerminalAppViewConsoleOutput);
    view_dispatcher_remove_view(app->view_dispatcher, UART_TerminalAppViewTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, UART_TerminalAppViewHexInput);

    variable_item_list_free(app->var_item_list);
    widget_free(app->widget);
    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);
    text_input_free(app->text_input);
    uart_hex_input_free(app->hex_input);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    uart_terminal_uart_free(app->uart);

    // Close records
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t uart_terminal_app(void* p) {
    UNUSED(p);
    // Disable expansion protocol to avoid interference with UART Handle
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    UART_TerminalApp* uart_terminal_app = uart_terminal_app_alloc();

    uart_terminal_app->uart = uart_terminal_uart_init(uart_terminal_app);

    view_dispatcher_run(uart_terminal_app->view_dispatcher);

    uart_terminal_app_free(uart_terminal_app);

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
