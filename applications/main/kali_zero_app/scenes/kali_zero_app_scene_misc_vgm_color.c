#include "../kali_zero_app.h"

enum ByteInputResult {
    ByteInputResultOk,
};

void kali_zero_app_scene_misc_vgm_color_byte_input_callback(void* context) {
    KaliZeroApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, ByteInputResultOk);
}

void kali_zero_app_scene_misc_vgm_color_on_enter(void* context) {
    KaliZeroApp* app = context;
    ByteInput* byte_input = app->byte_input;

    byte_input_set_header_text(byte_input, "Set VGM Color (#RRGGBB)");

    if(scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneMiscVgmColor)) {
        app->vgm_color = kalizero_settings.rpc_color_bg.rgb;
    } else {
        app->vgm_color = kalizero_settings.rpc_color_fg.rgb;
    }

    byte_input_set_result_callback(
        byte_input,
        kali_zero_app_scene_misc_vgm_color_byte_input_callback,
        NULL,
        app,
        (void*)&app->vgm_color,
        sizeof(app->vgm_color));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewByteInput);
}

bool kali_zero_app_scene_misc_vgm_color_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case ByteInputResultOk:
            if(scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneMiscVgmColor)) {
                kalizero_settings.rpc_color_bg.rgb = app->vgm_color;
            } else {
                kalizero_settings.rpc_color_fg.rgb = app->vgm_color;
            }
            app->save_settings = true;
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_misc_vgm_color_on_exit(void* context) {
    KaliZeroApp* app = context;
    byte_input_set_result_callback(app->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(app->byte_input, "");
}
