#include "../infrared_app_i.h"

#include "common/infrared_scene_universal_common.h"

void infrared_scene_universal_digital_sign_on_enter(void* context) {
    infrared_scene_universal_common_on_enter(context);

    InfraredApp* infrared = context;
    ButtonPanel* button_panel = infrared->button_panel;
    InfraredBruteForce* brute_force = infrared->brute_force;

    infrared_brute_force_set_db_filename(brute_force, EXT_PATH("infrared/assets/digital_sign.ir"));

    button_panel_reserve(button_panel, 2, 2);
    uint32_t i = 0;
    button_panel_add_item(
        button_panel,
        i,
        0,
        0,
        6,
        24,
        &I_power_19x20,
        &I_power_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 4, 46, &I_power_text_24x5);
    infrared_brute_force_add_record(brute_force, i++, "POWER");

    button_panel_add_item(
        button_panel,
        i,
        1,
        0,
        38,
        24,
        &I_input_19x20,
        &I_input_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 36, 46, &I_input_text_24x5);
    infrared_brute_force_add_record(brute_force, i++, "SOURCE");

    button_panel_add_item(
        button_panel,
        i,
        0,
        1,
        6,
        58,
        &I_play_19x20,
        &I_play_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 6, 80, &I_play_text_19x5);
    infrared_brute_force_add_record(brute_force, i++, "PLAY");

    button_panel_add_item(
        button_panel,
        i,
        1,
        1,
        38,
        58,
        &I_stop_19x20,
        &I_stop_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 38, 80, &I_stop_text_19x5);
    infrared_brute_force_add_record(brute_force, i++, "STOP");

    button_panel_add_label(button_panel, 1, 11, FontPrimary, "Sign Digital");

    view_set_orientation(view_stack_get_view(infrared->view_stack), ViewOrientationVertical);
    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewStack);

    infrared_show_loading_popup(infrared, true);
    bool success = infrared_brute_force_calculate_messages(brute_force);
    infrared_show_loading_popup(infrared, false);

    if(!success) {
        scene_manager_next_scene(infrared->scene_manager, InfraredSceneErrorDatabases);
    }
}

bool infrared_scene_universal_digital_sign_on_event(void* context, SceneManagerEvent event) {
    return infrared_scene_universal_common_on_event(context, event);
}

void infrared_scene_universal_digital_sign_on_exit(void* context) {
    infrared_scene_universal_common_on_exit(context);
}
