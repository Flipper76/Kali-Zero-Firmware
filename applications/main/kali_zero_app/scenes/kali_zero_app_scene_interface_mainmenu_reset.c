#include "../kali_zero_app.h"

static void kali_zero_app_scene_interface_mainmenu_reset_dialog_callback(
    DialogExResult result,
    void* context) {
    KaliZeroApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void kali_zero_app_scene_interface_mainmenu_reset_on_enter(void* context) {
    KaliZeroApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;
	
    dialog_ex_set_header(dialog_ex, "R""\xE9""initialiser le menu Apps?", 64, 10, AlignCenter, AlignCenter);
    dialog_ex_set_text(dialog_ex, "Vos modifs seront perdues!", 64, 32, AlignCenter, AlignCenter);
    dialog_ex_set_left_button_text(dialog_ex, "Annuler");
    dialog_ex_set_right_button_text(dialog_ex, "R""\xE9""initialiser");

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(
        dialog_ex, kali_zero_app_scene_interface_mainmenu_reset_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewDialogEx);
}

bool kali_zero_app_scene_interface_mainmenu_reset_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultRight:
            storage_common_remove(furi_record_open(RECORD_STORAGE), KALIZERO_MENU_PATH);
            furi_record_close(RECORD_STORAGE);
            app->save_mainmenu_apps = false;
            app->require_reboot = true;
            kali_zero_app_apply(app);
            break;
        case DialogExResultLeft:
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void kali_zero_app_scene_interface_mainmenu_reset_on_exit(void* context) {
    KaliZeroApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);
}
