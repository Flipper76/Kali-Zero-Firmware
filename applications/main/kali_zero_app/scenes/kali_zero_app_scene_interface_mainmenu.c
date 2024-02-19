#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexMenuStyle,
    VarItemListIndexResetMenu,
    VarItemListIndexApp,
    VarItemListIndexAddApp,
    VarItemListIndexMoveApp,
    VarItemListIndexRemoveApp,
};

void kali_zero_app_scene_interface_mainmenu_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

const char* const menu_style_names[MenuStyleCount] = {
    "List",
    "Wii",
    "DSi",
    "PS4",
    "Vertical",
    "C64",
    "Compact",
    "Terminal",
};
static void kali_zero_app_scene_interface_mainmenu_menu_style_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, menu_style_names[index]);
    kalizero_settings.menu_style = index;
    app->save_settings = true;
}

static void kali_zero_app_scene_interface_mainmenu_app_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    app->mainmenu_app_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(
        item, *CharList_get(app->mainmenu_app_labels, app->mainmenu_app_index));
    size_t count = CharList_size(app->mainmenu_app_labels);
    char label[20];
    snprintf(label, 20, "App  %u/%u", 1 + app->mainmenu_app_index, count);
    variable_item_set_item_label(item, label);
}

static void kali_zero_app_scene_interface_mainmenu_move_app_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t idx = app->mainmenu_app_index;
    size_t size = CharList_size(app->mainmenu_app_labels);
    uint8_t dir = variable_item_get_current_value_index(item);
    if(size >= 2) {
        if(dir == 2 && idx != size - 1) {
            // Right
            CharList_swap_at(app->mainmenu_app_labels, idx, idx + 1);
            CharList_swap_at(app->mainmenu_app_exes, idx, idx + 1);
            app->mainmenu_app_index++;
        } else if(dir == 0 && idx != 0) {
            // Left
            CharList_swap_at(app->mainmenu_app_labels, idx, idx - 1);
            CharList_swap_at(app->mainmenu_app_exes, idx, idx - 1);
            app->mainmenu_app_index--;
        }
        view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexMoveApp);
    }
    variable_item_set_current_value_index(item, 1);
}

void kali_zero_app_scene_interface_mainmenu_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Style de menu",
        MenuStyleCount,
        kali_zero_app_scene_interface_mainmenu_menu_style_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.menu_style);
    variable_item_set_current_value_text(item, menu_style_names[kalizero_settings.menu_style]);

    variable_item_list_add(var_item_list, "Réinitialiser le menu", 0, NULL, app);

    size_t count = CharList_size(app->mainmenu_app_labels);
    item = variable_item_list_add(
        var_item_list, "App", count, kali_zero_app_scene_interface_mainmenu_app_changed, app);
    if(count) {
        app->mainmenu_app_index = CLAMP(app->mainmenu_app_index, count - 1, 0U);
        char label[20];
        snprintf(label, 20, "App  %u/%u", 1 + app->mainmenu_app_index, count);
        variable_item_set_item_label(item, label);
        variable_item_set_current_value_text(
            item, *CharList_get(app->mainmenu_app_labels, app->mainmenu_app_index));
    } else {
        app->mainmenu_app_index = 0;
        variable_item_set_current_value_text(item, "Aucune");
    }
    variable_item_set_current_value_index(item, app->mainmenu_app_index);

    variable_item_list_add(var_item_list, "Ajouter App", 0, NULL, app);

    item = variable_item_list_add(
        var_item_list, "déplacer App", 3, kali_zero_app_scene_interface_mainmenu_move_app_changed, app);
    variable_item_set_current_value_text(item, "");
    variable_item_set_current_value_index(item, 1);
    variable_item_set_locked(item, count < 2, "Impossible de déplacer\navec moins\nde 2 apps!");

    variable_item_list_add(var_item_list, "Supprimer App", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_interface_mainmenu_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneInterfaceMainmenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_interface_mainmenu_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, KaliZeroAppSceneInterfaceMainmenu, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexResetMenu:
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneInterfaceMainmenuReset);
            break;
        case VarItemListIndexRemoveApp:
            if(!CharList_size(app->mainmenu_app_labels)) break;
            if(!CharList_size(app->mainmenu_app_exes)) break;
            free(*CharList_get(app->mainmenu_app_labels, app->mainmenu_app_index));
            free(*CharList_get(app->mainmenu_app_exes, app->mainmenu_app_index));
            CharList_remove_v(
                app->mainmenu_app_labels, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            CharList_remove_v(
                app->mainmenu_app_exes, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            /* fall through */
        case VarItemListIndexMoveApp: {
            app->save_mainmenu_apps = true;
            app->require_reboot = true;
            size_t count = CharList_size(app->mainmenu_app_labels);
            VariableItem* item = variable_item_list_get(app->var_item_list, VarItemListIndexApp);
            if(count) {
                app->mainmenu_app_index = CLAMP(app->mainmenu_app_index, count - 1, 0U);
                char label[20];
                snprintf(label, 20, "App  %u/%u", 1 + app->mainmenu_app_index, count);
                variable_item_set_item_label(item, label);
                variable_item_set_current_value_text(
                    item, *CharList_get(app->mainmenu_app_labels, app->mainmenu_app_index));
            } else {
                app->mainmenu_app_index = 0;
                variable_item_set_item_label(item, "App");
                variable_item_set_current_value_text(item, "Aucune");
            }
            variable_item_set_current_value_index(item, app->mainmenu_app_index);
            variable_item_set_values_count(item, count);
            break;
        }
        case VarItemListIndexAddApp:
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneInterfaceMainmenuAdd);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_interface_mainmenu_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
