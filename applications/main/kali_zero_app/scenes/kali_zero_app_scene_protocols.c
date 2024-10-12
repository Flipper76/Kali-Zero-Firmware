#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexSubghzFreqs,
    VarItemListIndexSubghzBypass,
    VarItemListIndexSubghzExtend,
    VarItemListIndexGpioPins,
    VarItemListIndexFileNamingPrefix,
};

void kali_zero_app_scene_protocols_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_protocols_subghz_bypass_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexSubghzBypass);
}

static void kali_zero_app_scene_protocols_subghz_extend_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexSubghzExtend);
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

    item = variable_item_list_add(var_item_list, "SubGHz Freqs", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(
        var_item_list,
        "Se souvenir BadBT",
        2,
        kali_zero_app_scene_protocols_subghz_bypass_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_bypass);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Extension SubGHz",
        2,
        kali_zero_app_scene_protocols_subghz_extend_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_extend);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");
    variable_item_set_locked(item, !app->subghz_bypass, "Must bypass\nregion lock\nfirst!");

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
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneProtocols));

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
        case VarItemListIndexSubghzBypass:
        case VarItemListIndexSubghzExtend: {
            bool* setting = (event.event == VarItemListIndexSubghzBypass) ? &app->subghz_bypass :
                                                                            &app->subghz_extend;
            VariableItem* item = variable_item_list_get(app->var_item_list, event.event);
            bool value = variable_item_get_current_value_index(item);
            if(value == *setting) value = !value; // Invoked via click
            bool change = !value; // Change without confirm if going from ON to OFF
            if(value) {
                DialogMessage* msg = dialog_message_alloc();
                dialog_message_set_header(msg, "Are you sure?", 64, 4, AlignCenter, AlignTop);
                dialog_message_set_buttons(msg, "No", NULL, "Yes");
                dialog_message_set_text(
                    msg,
                    (event.event == VarItemListIndexSubghzBypass) ? "Unlocks TX to 300-350,\n"
                                                                    "387-467, 779-928 MHz\n"
                                                                    "Use responsibly, check\n"
                                                                    "local laws" :
                                                                    "Extends TX to 281-361,\n"
                                                                    "378-481, 749-962 MHz\n"
                                                                    "Use at own risk, may\n"
                                                                    "damage Flipper",
                    64,
                    36,
                    AlignCenter,
                    AlignCenter);
                if(dialog_message_show(app->dialogs, msg) == DialogMessageButtonRight) {
                    change = true;
                }
                dialog_message_free(msg);
            }
            if(change) {
                *setting = value;
                app->save_subghz = true;
                app->require_reboot = true;
                if(event.event == VarItemListIndexSubghzBypass) {
                    variable_item_set_locked(
                        variable_item_list_get(app->var_item_list, VarItemListIndexSubghzExtend),
                        !value,
                        NULL);
                }
            } else {
                value = !value;
            }
            variable_item_set_current_value_index(item, value);
            variable_item_set_current_value_text(item, value ? "ON" : "OFF");
            break;
        }
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
