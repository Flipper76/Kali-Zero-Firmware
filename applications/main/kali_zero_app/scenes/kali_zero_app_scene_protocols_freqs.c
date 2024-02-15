#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexUseDefaults,
    VarItemListIndexStaticFreqs,
    VarItemListIndexHopperFreqs,
};

void kali_zero_app_scene_protocols_freqs_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_protocols_freqs_use_defaults_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    app->subghz_use_defaults = value;
    app->save_subghz_freqs = true;
}

void kali_zero_app_scene_protocols_freqs_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Par d""\xE9""faut",
        2,
        kali_zero_app_scene_protocols_freqs_use_defaults_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_use_defaults);
    variable_item_set_current_value_text(item, app->subghz_use_defaults ? "ON" : "OFF");

    item = variable_item_list_add(var_item_list, "Frequences statiques", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Frequences de Hopper", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_protocols_freqs_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneProtocolsFreqs));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_protocols_freqs_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, KaliZeroAppSceneProtocolsFreqs, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexStaticFreqs:
            scene_manager_set_scene_state(
                app->scene_manager, KaliZeroAppSceneProtocolsFreqsStatic, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneProtocolsFreqsStatic);
            break;
        case VarItemListIndexHopperFreqs:
            scene_manager_set_scene_state(
                app->scene_manager, KaliZeroAppSceneProtocolsFreqsHopper, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneProtocolsFreqsHopper);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_protocols_freqs_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
