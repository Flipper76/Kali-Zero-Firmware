#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexFlipperName, // TODO: Split into name, mac, serial
    VarItemListIndexShellColor,
};

const char* const shell_color_names[FuriHalVersionColorCount] = {
    "Réél",
    "Noir",
    "Blanc",
    "Transparent",
};
static void kali_zero_app_scene_misc_spoof_shell_color_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, shell_color_names[index]);
    kalizero_settings.spoof_color = index;
    app->save_settings = true;
    app->require_reboot = true;
}

void kali_zero_app_scene_misc_spoof_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void kali_zero_app_scene_misc_spoof_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(var_item_list, "Nom du Dauphin", 0, NULL, app);
    variable_item_set_current_value_text(item, app->device_name);

    item = variable_item_list_add(
        var_item_list,
        "Couleur coque",
        FuriHalVersionColorCount,
        kali_zero_app_scene_misc_spoof_shell_color_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.spoof_color);
    variable_item_set_current_value_text(item, shell_color_names[kalizero_settings.spoof_color]);

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_misc_spoof_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneMiscSpoof));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_misc_spoof_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMiscSpoof, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexFlipperName:
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneMiscSpoofName);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_misc_spoof_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
