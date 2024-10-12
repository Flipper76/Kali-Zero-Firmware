#include "../bt_settings_app.h"
#include <furi_hal_bt.h>
#include <applications/main/bad_kb/bad_kb_paths.h>

void bt_settings_scene_forget_dev_confirm_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    BtSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void bt_settings_scene_forget_dev_confirm_on_enter(void* context) {
    BtSettingsApp* app = context;
    DialogEx* dialog = app->dialog;
    dialog_ex_set_header(dialog, "Dissocier?", 64, 3, AlignCenter, AlignTop);
    dialog_ex_set_text(
        dialog, "Toutes associations \nseront perdues!", 64, 22, AlignCenter, AlignTop);
    dialog_ex_set_left_button_text(dialog, "Retour");
    dialog_ex_set_right_button_text(dialog, "Dissocier");
    dialog_ex_set_context(dialog, app);
    dialog_ex_set_result_callback(dialog, bt_settings_scene_forget_dev_confirm_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, BtSettingsAppViewDialog);
}

bool bt_settings_scene_forget_dev_confirm_on_event(void* context, SceneManagerEvent event) {
    BtSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultLeft) {
            consumed = scene_manager_previous_scene(app->scene_manager);
        } else if(event.event == DialogExResultRight) {
            bt_keys_storage_set_default_path(app->bt);
            bt_forget_bonded_devices(app->bt);

            // also remove keys for apps
            const char* keys_paths[] = {
                BAD_KB_KEYS_PATH,
                EXT_PATH("apps_data/hid_ble/.bt_hid.keys"),
                EXT_PATH("apps_data/air_mouse/.bt_hid.keys"),
                EXT_PATH("apps_data/vgm_air_mouse/.bt_hid.keys"),
                EXT_PATH("apps_data/pc_monitor/.bt_serial.keys"),
                EXT_PATH("apps_data/totp/.bt_hid_00.keys"),
            };
            Storage* storage = furi_record_open(RECORD_STORAGE);
            for(size_t i = 0; i < COUNT_OF(keys_paths); i++) {
                storage_simply_remove(storage, keys_paths[i]);
            }
            furi_record_close(RECORD_STORAGE);

            scene_manager_next_scene(app->scene_manager, BtSettingsAppSceneForgetDevSuccess);
            consumed = true;
        }
    }

    return consumed;
}

void bt_settings_scene_forget_dev_confirm_on_exit(void* context) {
    BtSettingsApp* app = context;
    dialog_ex_reset(app->dialog);
}
