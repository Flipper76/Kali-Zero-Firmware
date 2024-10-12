#include "../storage_settings.h"
#include <power/power_service/power.h>
#include <furi_hal.h>

#define STORAGE_SETTINGS_SCENE_FACTORY_RESET_CONFIRM_COUNT 5

static void
    storage_settings_scene_factory_reset_dialog_callback(DialogExResult result, void* context) {
    StorageSettings* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void storage_settings_scene_factory_reset_on_enter(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(dialog_ex, storage_settings_scene_factory_reset_dialog_callback);

    dialog_ex_set_left_button_text(dialog_ex, "Annuler");
    dialog_ex_set_right_button_text(dialog_ex, "Effacer");

    dialog_ex_set_header(dialog_ex, "Confirmer Factory Reset", 64, 10, AlignCenter, AlignCenter);
    dialog_ex_set_text(
        dialog_ex,
        "Le stockage interne \r\nsera effacé \nLes données et les \nparamètres seront perdus!",
        64,
        32,
        AlignCenter,
        AlignTop);

    view_dispatcher_switch_to_view(app->view_dispatcher, StorageSettingsViewDialogEx);
}

bool storage_settings_scene_factory_reset_on_event(void* context, SceneManagerEvent event) {
    StorageSettings* app = context;
    bool consumed = false;

    uint32_t counter =
        scene_manager_get_scene_state(app->scene_manager, StorageSettingsFactoryReset);

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultLeft:
            scene_manager_set_scene_state(app->scene_manager, StorageSettingsFactoryReset, 0);
            consumed = scene_manager_previous_scene(app->scene_manager);
            break;
        case DialogExResultRight:
            counter++;
            if(counter < STORAGE_SETTINGS_SCENE_FACTORY_RESET_CONFIRM_COUNT) {
                furi_string_printf(
                    app->text_string,
                    "%ld appuie gauche",
                    STORAGE_SETTINGS_SCENE_FACTORY_RESET_CONFIRM_COUNT - counter);
                dialog_ex_set_text(
                    app->dialog_ex,
                    furi_string_get_cstr(app->text_string),
                    64,
                    32,
                    AlignCenter,
                    AlignCenter);
                scene_manager_set_scene_state(
                    app->scene_manager, StorageSettingsFactoryReset, counter);
            } else {
                furi_hal_rtc_reset_registers();
                furi_hal_rtc_set_flag(FuriHalRtcFlagStorageFormatInternal);

                Power* power = furi_record_open(RECORD_POWER);
                power_reboot(power, PowerBootModeNormal);
            }

            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void storage_settings_scene_factory_reset_on_exit(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);

    furi_string_reset(app->text_string);
}
