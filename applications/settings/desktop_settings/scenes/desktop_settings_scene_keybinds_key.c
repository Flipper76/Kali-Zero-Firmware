#include <applications.h>

#include "../desktop_settings_app.h"
#include "desktop_settings_scene.h"

static void desktop_settings_scene_keybinds_key_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void desktop_settings_scene_keybinds_key_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    submenu_add_item(
        submenu, "Haut", KeybindKeyUp, desktop_settings_scene_keybinds_key_submenu_callback, app);

    submenu_add_item(
        submenu, "Bas", KeybindKeyDown, desktop_settings_scene_keybinds_key_submenu_callback, app);

    submenu_add_item(
        submenu,
        "Droite",
        KeybindKeyRight,
        desktop_settings_scene_keybinds_key_submenu_callback,
        app);

    submenu_add_item(
        submenu, "Gauche", KeybindKeyLeft, desktop_settings_scene_keybinds_key_submenu_callback, app);

    submenu_set_header(submenu, "raccourci clavier:");

    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey));

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_keybinds_key_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        scene_manager_set_scene_state(
            app->scene_manager, DesktopSettingsAppSceneKeybindsKey, event.event);
        scene_manager_set_scene_state(
            app->scene_manager,
            DesktopSettingsAppSceneKeybindsActionType,
            DesktopSettingsAppKeybindActionTypeRemoveKeybind);
        scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneKeybindsActionType);
    }
    return consumed;
}

void desktop_settings_scene_keybinds_key_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    submenu_reset(app->submenu);
}
