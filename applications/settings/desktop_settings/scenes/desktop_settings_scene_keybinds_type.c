#include <applications.h>

#include "../desktop_settings_app.h"
#include "desktop_settings_scene.h"

static void desktop_settings_scene_keybinds_type_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void desktop_settings_scene_keybinds_type_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    submenu_add_item(
        submenu,
        "Appuyer",
        DesktopKeybindTypePress,
        desktop_settings_scene_keybinds_type_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "Tenir",
        DesktopKeybindTypeHold,
        desktop_settings_scene_keybinds_type_submenu_callback,
        app);

    submenu_set_header(submenu, "Type raccourci clavier:");

    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsType));

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_keybinds_type_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        scene_manager_set_scene_state(
            app->scene_manager, DesktopSettingsAppSceneKeybindsType, event.event);
        scene_manager_set_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey, 0);
        scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneKeybindsKey);
    }
    return consumed;
}

void desktop_settings_scene_keybinds_type_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    submenu_reset(app->submenu);
}
