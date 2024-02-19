#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexBadkbMode,
    VarItemListIndexBadbtRemember,
    VarItemListIndexSubghzFreqs,
    VarItemListIndexSubghzExtend,
    VarItemListIndexGpioPins,
};

void kali_zero_app_scene_protocols_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_protocols_bad_bt_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "BT" : "USB");
    kalizero_settings.bad_bt = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_protocols_bad_bt_remember_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.bad_bt_remember = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_protocols_subghz_extend_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    app->subghz_extend = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");
    app->save_subghz = true;
}

static void kali_zero_app_scene_protocols_file_naming_prefix_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "Aprés" : "Avant");
    kalizero_settings.file_naming_prefix_after = value;
    app->save_settings = true;
}

void kali_zero_app_scene_protocols_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Mode BadKB", 2, kali_zero_app_scene_protocols_bad_bt_changed, app);
    variable_item_set_current_value_index(item, kalizero_settings.bad_bt);
    variable_item_set_current_value_text(item, kalizero_settings.bad_bt ? "BT" : "USB");

    item = variable_item_list_add(
        var_item_list,
        "Se souvenir BadBT",
        2,
        kali_zero_app_scene_protocols_bad_bt_remember_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.bad_bt_remember);
    variable_item_set_current_value_text(item, kalizero_settings.bad_bt_remember ? "ON" : "OFF");

    item = variable_item_list_add(var_item_list, "Fréquences subGHz", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(
        var_item_list, "Éxtension SubGHz", 2, kali_zero_app_scene_protocols_subghz_extend_changed, app);
    variable_item_set_current_value_index(item, app->subghz_extend);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");

    item = variable_item_list_add(var_item_list, "GPIO Pins", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(
        var_item_list,
        "Préfixe nom de fichier",
        2,
        kali_zero_app_scene_protocols_file_naming_prefix_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.file_naming_prefix_after);
    variable_item_set_current_value_text(
        item, kalizero_settings.file_naming_prefix_after ? "Aprés" : "Avant");;

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_protocols_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneProtocols));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_protocols_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneProtocols, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexSubghzFreqs:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneProtocolsFreqs, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneProtocolsFreqs);
            break;
        case VarItemListIndexGpioPins:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneProtocolsGpio, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneProtocolsGpio);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_protocols_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
