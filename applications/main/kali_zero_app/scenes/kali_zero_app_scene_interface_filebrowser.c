#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexSortDirsFirst,
    VarItemListIndexShowHiddenFiles,
    VarItemListIndexShowInternalTab,
    VarItemListIndexFavoriteTimeout,
};

void kali_zero_app_scene_interface_filebrowser_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_interface_filebrowser_sort_dirs_first_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.sort_dirs_first = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_filebrowser_show_hidden_files_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.show_hidden_files = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_filebrowser_show_internal_tab_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.show_internal_tab = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_filebrowser_favorite_timeout_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint32_t value = variable_item_get_current_value_index(item);
    char text[6];
    snprintf(text, sizeof(text), "%lu S", value);
    variable_item_set_current_value_text(item, value ? text : "OFF");
    kalizero_settings.favorite_timeout = value;
    app->save_settings = true;
}

void kali_zero_app_scene_interface_filebrowser_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Dossiers au-dessus des fichiers",
        2,
        kali_zero_app_scene_interface_filebrowser_sort_dirs_first_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.sort_dirs_first);
    variable_item_set_current_value_text(item, kalizero_settings.sort_dirs_first ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Afficher les fichiers cachés",
        2,
        kali_zero_app_scene_interface_filebrowser_show_hidden_files_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.show_hidden_files);
    variable_item_set_current_value_text(item, kalizero_settings.show_hidden_files ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
         "Afficher l'onglet interne",
        2,
        kali_zero_app_scene_interface_filebrowser_show_internal_tab_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.show_internal_tab);
    variable_item_set_current_value_text(item, kalizero_settings.show_internal_tab ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Délai d'attente favori",
        61,
        kali_zero_app_scene_interface_filebrowser_favorite_timeout_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.favorite_timeout);
    char text[4];
    snprintf(text, sizeof(text), "%lu S", kalizero_settings.favorite_timeout);
    variable_item_set_current_value_text(item, kalizero_settings.favorite_timeout ? text : "OFF");

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_interface_filebrowser_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneInterfaceFilebrowser));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_interface_filebrowser_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, KaliZeroAppSceneInterfaceFilebrowser, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_interface_filebrowser_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
