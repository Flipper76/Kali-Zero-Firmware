#include "../kali_zero_app.h"

enum TextInputIndex {
    TextInputResultOk,
};

static void kali_zero_app_scene_misc_rename_text_input_callback(void* context) {
    KaliZeroApp* app = context;

    app->save_name = true;
    app->require_reboot = true;
    view_dispatcher_send_custom_event(app->view_dispatcher, TextInputResultOk);
}

static bool
    kali_zero_app_scene_misc_rename_validator(const char* text, FuriString* error, void* context) {
    UNUSED(context);

    for(; *text; ++text) {
        const char c = *text;
        if((c < '0' || c > '9') && (c < 'A' || c > 'Z') && (c < 'a' || c > 'z')) {
			furi_string_printf(error, "Saisir uniquement \ndes lettres \net des chiffres!");
            return false;
        }
    }

    return true;
}

void kali_zero_app_scene_misc_rename_on_enter(void* context) {
    KaliZeroApp* app = context;
    TextInput* text_input = app->text_input;

    text_input_set_header_text(text_input, "Laisser vide par défaut");

    text_input_set_validator(text_input, kali_zero_app_scene_misc_rename_validator, NULL);

    text_input_set_minimum_length(text_input, 0);

    text_input_set_result_callback(
        text_input,
        kali_zero_app_scene_misc_rename_text_input_callback,
        app,
        app->device_name,
        FURI_HAL_VERSION_ARRAY_NAME_LENGTH,
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewTextInput);
}

bool kali_zero_app_scene_misc_rename_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case TextInputResultOk:
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_misc_rename_on_exit(void* context) {
    KaliZeroApp* app = context;
    text_input_reset(app->text_input);
}
