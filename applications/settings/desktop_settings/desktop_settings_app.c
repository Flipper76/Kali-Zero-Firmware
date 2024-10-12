#include <furi.h>
#include <gui/modules/popup.h>
#include <gui/modules/dialog_ex.h>
#include <gui/scene_manager.h>

#include <desktop/desktop.h>
#include <desktop/views/desktop_view_pin_input.h>

#include "desktop_settings_app.h"
#include "scenes/desktop_settings_scene.h"

const char* EXTRA_KEYBINDS[] = {
    "Apps Menu",
    "Archive",
    "Clock",
    "Device Info",
    "Lock Menu",
    "Lock Keypad",
    "Lock with PIN",
    "Passeport",
    "Wipe Device",
};
const size_t EXTRA_KEYBINDS_COUNT = COUNT_OF(EXTRA_KEYBINDS);

static bool desktop_settings_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    DesktopSettingsApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool desktop_settings_back_event_callback(void* context) {
    furi_assert(context);
    DesktopSettingsApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

FuriString* desktop_settings_app_get_keybind(DesktopSettingsApp* app) {
    DesktopKeybindType type =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
    DesktopKeybindKey key =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey);
    return app->keybinds[type][key];
}

void desktop_settings_app_set_keybind(DesktopSettingsApp* app, const char* value) {
    DesktopKeybindType type =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
    DesktopKeybindKey key =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey);
    furi_string_set(app->keybinds[type][key], value);
    app->save_keybinds = true;
}

DesktopSettingsApp* desktop_settings_app_alloc(void) {
    DesktopSettingsApp* app = malloc(sizeof(DesktopSettingsApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&desktop_settings_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, desktop_settings_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, desktop_settings_back_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->popup = popup_alloc();
    app->submenu = submenu_alloc();
    app->variable_item_list = variable_item_list_alloc();
    app->pin_input_view = desktop_view_pin_input_alloc();
    app->pin_setup_howto_view = desktop_settings_view_pin_setup_howto_alloc();
    app->pin_setup_howto2_view = desktop_settings_view_pin_setup_howto2_alloc();
    app->dialog_ex = dialog_ex_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, DesktopSettingsAppViewMenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher,
        DesktopSettingsAppViewVarItemList,
        variable_item_list_get_view(app->variable_item_list));
    view_dispatcher_add_view(
        app->view_dispatcher, DesktopSettingsAppViewIdPopup, popup_get_view(app->popup));
    view_dispatcher_add_view(
        app->view_dispatcher,
        DesktopSettingsAppViewIdPinInput,
        desktop_view_pin_input_get_view(app->pin_input_view));
    view_dispatcher_add_view(
        app->view_dispatcher,
        DesktopSettingsAppViewIdPinSetupHowto,
        desktop_settings_view_pin_setup_howto_get_view(app->pin_setup_howto_view));
    view_dispatcher_add_view(
        app->view_dispatcher,
        DesktopSettingsAppViewIdPinSetupHowto2,
        desktop_settings_view_pin_setup_howto2_get_view(app->pin_setup_howto2_view));
    view_dispatcher_add_view(
        app->view_dispatcher, DesktopSettingsAppViewDialogEx, dialog_ex_get_view(app->dialog_ex));
    return app;
}

void desktop_settings_app_free(DesktopSettingsApp* app) {
    furi_assert(app);
    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewIdPopup);
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewIdPinInput);
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewIdPinSetupHowto);
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewIdPinSetupHowto2);
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewDialogEx);
    variable_item_list_free(app->variable_item_list);
    submenu_free(app->submenu);
    popup_free(app->popup);
    desktop_view_pin_input_free(app->pin_input_view);
    desktop_settings_view_pin_setup_howto_free(app->pin_setup_howto_view);
    desktop_settings_view_pin_setup_howto2_free(app->pin_setup_howto2_view);
    dialog_ex_free(app->dialog_ex);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    // Records
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_GUI);
    free(app);
}

extern int32_t desktop_settings_app(void* p) {
    DesktopSettingsApp* app = desktop_settings_app_alloc();
    Desktop* desktop = furi_record_open(RECORD_DESKTOP);

    desktop_api_get_settings(desktop, &app->settings);
    desktop_keybinds_load(desktop, &app->keybinds);

    if(p && (strcmp(p, DESKTOP_SETTINGS_RUN_PIN_SETUP_ARG) == 0)) {
        scene_manager_next_scene(app->scene_manager, DesktopSettingsAppScenePinSetupHowto);
    } else {
        scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneStart);
    }

    view_dispatcher_run(app->view_dispatcher);

    if(app->save_keybinds) {
        desktop_keybinds_save(desktop, &app->keybinds);
    }
    desktop_keybinds_free(&app->keybinds);
    desktop_api_set_settings(desktop, &app->settings);
    furi_record_close(RECORD_DESKTOP);

    desktop_settings_app_free(app);

    return 0;
}
