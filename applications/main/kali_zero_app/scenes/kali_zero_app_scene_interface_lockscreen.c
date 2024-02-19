#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexLockOnBoot,
    VarItemListIndexFormatOn10BadPins,
    VarItemListIndexPinUnlockFromApp,
    VarItemListIndexShowTime,
    VarItemListIndexShowSeconds,
    VarItemListIndexShowDate,
    VarItemListIndexShowStatusbar,
    VarItemListIndexUnlockPrompt,
};

void kali_zero_app_scene_interface_lockscreen_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_interface_lockscreen_lock_on_boot_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lock_on_boot = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_lockscreen_bad_pins_format_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.bad_pins_format = value;
    app->save_settings = true;
}

static void
    kali_zero_app_scene_interface_lockscreen_allow_locked_rpc_commands_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.allow_locked_rpc_commands = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_lockscreen_lockscreen_poweroff_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lockscreen_poweroff = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_lockscreen_lockscreen_time_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lockscreen_time = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_lockscreen_lockscreen_seconds_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lockscreen_seconds = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_lockscreen_lockscreen_date_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lockscreen_date = value;
    app->save_settings = true;
}

static void
    kali_zero_app_scene_interface_lockscreen_lockscreen_statusbar_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lockscreen_statusbar = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_lockscreen_lockscreen_prompt_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lockscreen_prompt = value;
    app->save_settings = true;
}

static void
    kali_zero_app_scene_interface_lockscreen_lockscreen_transparent_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.lockscreen_transparent = value;
    app->save_settings = true;
}

void kali_zero_app_scene_interface_lockscreen_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Verrouiller au démarrage",
        2,
        kali_zero_app_scene_interface_lockscreen_lock_on_boot_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lock_on_boot);
    variable_item_set_current_value_text(item, kalizero_settings.lock_on_boot ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Formater si 10 mauvais PIN",
        2,
        kali_zero_app_scene_interface_lockscreen_bad_pins_format_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.bad_pins_format);
    variable_item_set_current_value_text(item, kalizero_settings.bad_pins_format ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Autoriser RPC pendant verrouillage",
        2,
        kali_zero_app_scene_interface_lockscreen_allow_locked_rpc_commands_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.allow_locked_rpc_commands);
    variable_item_set_current_value_text(
        item, kalizero_settings.allow_locked_rpc_commands ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Autoriser la mise hors tension",
        2,
        kali_zero_app_scene_interface_lockscreen_lockscreen_poweroff_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lockscreen_poweroff);
    variable_item_set_current_value_text(item, kalizero_settings.lockscreen_poweroff ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Afficher l'heure",
        2,
        kali_zero_app_scene_interface_lockscreen_lockscreen_time_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lockscreen_time);
    variable_item_set_current_value_text(item, kalizero_settings.lockscreen_time ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Afficher les secondes",
        2,
        kali_zero_app_scene_interface_lockscreen_lockscreen_seconds_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lockscreen_seconds);
    variable_item_set_current_value_text(item, kalizero_settings.lockscreen_seconds ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Afficher la date",
        2,
        kali_zero_app_scene_interface_lockscreen_lockscreen_date_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lockscreen_date);
    variable_item_set_current_value_text(item, kalizero_settings.lockscreen_date ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Afficher la barre d'état",
        2,
        kali_zero_app_scene_interface_lockscreen_lockscreen_statusbar_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lockscreen_statusbar);
    variable_item_set_current_value_text(
        item, kalizero_settings.lockscreen_statusbar ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Invite de déverrouillage",
        2,
        kali_zero_app_scene_interface_lockscreen_lockscreen_prompt_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lockscreen_prompt);
    variable_item_set_current_value_text(item, kalizero_settings.lockscreen_prompt ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Transparent (voir animation)",
        2,
        kali_zero_app_scene_interface_lockscreen_lockscreen_transparent_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.lockscreen_transparent);
    variable_item_set_current_value_text(
        item, kalizero_settings.lockscreen_transparent ? "ON" : "OFF");

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_interface_lockscreen_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneInterfaceLockscreen));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_interface_lockscreen_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, KaliZeroAppSceneInterfaceLockscreen, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_interface_lockscreen_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
