#include "../power_settings_app.h"

enum PowerSettingsSubmenuIndex {
    PowerSettingsSubmenuIndexBatteryInfo,
    PowerSettingsSubmenuIndexReboot,
    PowerSettingsSubmenuIndexOff,
    PowerSettingsSubmenuShutdownIdle
};

static void power_settings_scene_start_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void power_settings_scene_start_on_enter(void* context) {
    PowerSettingsApp* app = context;
    Submenu* submenu = app->submenu;

  submenu_add_item(
        submenu,
        "Info Batterie",
        PowerSettingsSubmenuIndexBatteryInfo,
        power_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Redémarrage",
        PowerSettingsSubmenuIndexReboot,
        power_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Éteindre",
        PowerSettingsSubmenuIndexOff,
        power_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Passage en veille",
        PowerSettingsSubmenuShutdownIdle,
        power_settings_scene_start_submenu_callback,
        app);
    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, PowerSettingsAppSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, PowerSettingsAppViewSubmenu);
}

bool power_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    PowerSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PowerSettingsSubmenuIndexBatteryInfo) {
            scene_manager_next_scene(app->scene_manager, PowerSettingsAppSceneBatteryInfo);
        } else if(event.event == PowerSettingsSubmenuIndexReboot) {
            scene_manager_next_scene(app->scene_manager, PowerSettingsAppSceneReboot);
        } else if(event.event == PowerSettingsSubmenuIndexOff) {
            scene_manager_next_scene(app->scene_manager, PowerSettingsAppScenePowerOff);
        } else if(event.event == PowerSettingsSubmenuShutdownIdle) {
            scene_manager_next_scene(app->scene_manager, PowerSettingsAppSceneShutdownIdle);
        }
        scene_manager_set_scene_state(app->scene_manager, PowerSettingsAppSceneStart, event.event);
        consumed = true;
    }
    return consumed;
}

void power_settings_scene_start_on_exit(void* context) {
    PowerSettingsApp* app = context;
    submenu_reset(app->submenu);
}
