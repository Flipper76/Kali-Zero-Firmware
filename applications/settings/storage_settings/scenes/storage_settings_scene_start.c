#include "../storage_settings.h"

enum StorageSettingsStartSubmenuIndex {
    StorageSettingsStartSubmenuIndexInternalInfo,
    StorageSettingsStartSubmenuIndexSDInfo,
    StorageSettingsStartSubmenuIndexUnmount,
    StorageSettingsStartSubmenuIndexFormat,
    StorageSettingsStartSubmenuIndexBenchy,
    StorageSettingsStartSubmenuIndexFactoryReset,
    StorageSettingsStartSubmenuIndexWipeDevice,
};

static void storage_settings_scene_start_submenu_callback(void* context, uint32_t index) {
    StorageSettings* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void storage_settings_scene_start_on_enter(void* context) {
    StorageSettings* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "Au sujet du stockage interne",
        StorageSettingsStartSubmenuIndexInternalInfo,
        storage_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Au sujet de la carte SD",
        StorageSettingsStartSubmenuIndexSDInfo,
        storage_settings_scene_start_submenu_callback,
        app);

    FS_Error sd_status = storage_sd_status(app->fs_api);
    if(sd_status != FSE_OK) {
        submenu_add_item(
            submenu,
            "Monter la carte sd",
            StorageSettingsStartSubmenuIndexUnmount,
            storage_settings_scene_start_submenu_callback,
            app);
    } else {
        submenu_add_item(
            submenu,
            "D""\xE9""monter la carte SD",
            StorageSettingsStartSubmenuIndexUnmount,
            storage_settings_scene_start_submenu_callback,
            app);
    }

    submenu_add_item(
        submenu,
        "Formater la carte SD",
        StorageSettingsStartSubmenuIndexFormat,
        storage_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Carte SD",
        StorageSettingsStartSubmenuIndexBenchy,
        storage_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Factory Reset",
        StorageSettingsStartSubmenuIndexFactoryReset,
        storage_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Effacer l'appareil",
        StorageSettingsStartSubmenuIndexWipeDevice,
        storage_settings_scene_start_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, StorageSettingsStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, StorageSettingsViewSubmenu);
}

bool storage_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    StorageSettings* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case StorageSettingsStartSubmenuIndexSDInfo:
            scene_manager_set_scene_state(
                app->scene_manager, StorageSettingsStart, StorageSettingsStartSubmenuIndexSDInfo);
            scene_manager_next_scene(app->scene_manager, StorageSettingsSDInfo);
            consumed = true;
            break;
        case StorageSettingsStartSubmenuIndexInternalInfo:
            scene_manager_set_scene_state(
                app->scene_manager,
                StorageSettingsStart,
                StorageSettingsStartSubmenuIndexInternalInfo);
            scene_manager_next_scene(app->scene_manager, StorageSettingsInternalInfo);
            consumed = true;
            break;
        case StorageSettingsStartSubmenuIndexUnmount:
            scene_manager_set_scene_state(
                app->scene_manager, StorageSettingsStart, StorageSettingsStartSubmenuIndexUnmount);
            scene_manager_next_scene(app->scene_manager, StorageSettingsUnmountConfirm);
            consumed = true;
            break;
        case StorageSettingsStartSubmenuIndexFormat:
            scene_manager_set_scene_state(
                app->scene_manager, StorageSettingsStart, StorageSettingsStartSubmenuIndexFormat);
            scene_manager_next_scene(app->scene_manager, StorageSettingsFormatConfirm);
            consumed = true;
            break;
        case StorageSettingsStartSubmenuIndexBenchy:
            scene_manager_set_scene_state(
                app->scene_manager, StorageSettingsStart, StorageSettingsStartSubmenuIndexBenchy);
            scene_manager_next_scene(app->scene_manager, StorageSettingsBenchmark);
            consumed = true;
            break;
        case StorageSettingsStartSubmenuIndexFactoryReset:
            scene_manager_set_scene_state(
                app->scene_manager,
                StorageSettingsStart,
                StorageSettingsStartSubmenuIndexFactoryReset);
            scene_manager_next_scene(app->scene_manager, StorageSettingsFactoryReset);
            consumed = true;
            break;
        case StorageSettingsStartSubmenuIndexWipeDevice:
            scene_manager_set_scene_state(
                app->scene_manager,
                StorageSettingsStart,
                StorageSettingsStartSubmenuIndexWipeDevice);
            scene_manager_next_scene(app->scene_manager, StorageSettingsWipeDevice);
            consumed = true;
            break;
        }
    }
    return consumed;
}

void storage_settings_scene_start_on_exit(void* context) {
    StorageSettings* app = context;
    submenu_reset(app->submenu);
}
