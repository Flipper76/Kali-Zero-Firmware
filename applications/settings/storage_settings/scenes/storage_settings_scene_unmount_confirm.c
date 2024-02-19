#include "../storage_settings.h"

static void
    storage_settings_scene_unmount_confirm_dialog_callback(DialogExResult result, void* context) {
    StorageSettings* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void storage_settings_scene_unmount_confirm_on_enter(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    FS_Error sd_status = storage_sd_status(app->fs_api);
    if(sd_status == FSE_NOT_READY) {
        dialog_ex_set_header(dialog_ex, "Monter la carte sd?", 64, 10, AlignCenter, AlignCenter);
        dialog_ex_set_text(
            dialog_ex,
            "Cela peut couper \nl'alimentation\ndes modules externes",
            64,
            32,
            AlignCenter,
            AlignCenter);
        dialog_ex_set_left_button_text(dialog_ex, "Annuler");
        dialog_ex_set_right_button_text(dialog_ex, "Monter");
    } else {
        dialog_ex_set_header(dialog_ex, "Démonter la carte SD?", 64, 10, AlignCenter, AlignCenter);
        dialog_ex_set_text(
            dialog_ex, "La carte SD sera\nindisponible", 64, 32, AlignCenter, AlignCenter);
        dialog_ex_set_left_button_text(dialog_ex, "Annuler");
        dialog_ex_set_right_button_text(dialog_ex, "Démonter");
    }

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(
        dialog_ex, storage_settings_scene_unmount_confirm_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, StorageSettingsViewDialogEx);
}

bool storage_settings_scene_unmount_confirm_on_event(void* context, SceneManagerEvent event) {
    StorageSettings* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultCenter:
        case DialogExResultLeft:
            consumed = scene_manager_previous_scene(app->scene_manager);
            break;
        case DialogExResultRight:
            scene_manager_next_scene(app->scene_manager, StorageSettingsUnmounted);
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void storage_settings_scene_unmount_confirm_on_exit(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);
}
