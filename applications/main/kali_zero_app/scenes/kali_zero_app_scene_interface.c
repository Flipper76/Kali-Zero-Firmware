#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexGraphics,
    VarItemListIndexMainmenu,
    VarItemListIndexLockscreen,
    VarItemListIndexStatusbar,
    VarItemListIndexFileBrowser,
};

void kali_zero_app_scene_interface_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void kali_zero_app_scene_interface_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(var_item_list, "Graphique", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Menu principal", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Écran de verrouillage", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Barre d'état", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Navigateur de fichiers", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_interface_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneInterface));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_interface_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneInterface, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexGraphics:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneInterfaceGraphics, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneInterfaceGraphics);
            break;
        case VarItemListIndexMainmenu:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneInterfaceMainmenu, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneInterfaceMainmenu);
            break;
        case VarItemListIndexLockscreen:
            scene_manager_set_scene_state(
                app->scene_manager, KaliZeroAppSceneInterfaceLockscreen, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneInterfaceLockscreen);
            break;
        case VarItemListIndexStatusbar:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneInterfaceStatusbar, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneInterfaceStatusbar);
            break;
        case VarItemListIndexFileBrowser:
            scene_manager_set_scene_state(
                app->scene_manager, KaliZeroAppSceneInterfaceFilebrowser, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneInterfaceFilebrowser);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_interface_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
