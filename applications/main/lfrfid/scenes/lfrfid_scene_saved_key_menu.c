#include "../lfrfid_i.h"
#include <dolphin/dolphin.h>

typedef enum {
    SubmenuIndexEmulate,
    SubmenuIndexWrite,
    SubmenuIndexWriteAndSetPass,
    SubmenuIndexEdit,
    SubmenuIndexDelete,
    SubmenuIndexInfo,
} SubmenuIndex;

static void lfrfid_scene_saved_key_menu_submenu_callback(void* context, uint32_t index) {
    LfRfid* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void lfrfid_scene_saved_key_menu_on_enter(void* context) {
    LfRfid* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu, "Emuler", SubmenuIndexEmulate, lfrfid_scene_saved_key_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "Encoder", SubmenuIndexWrite, lfrfid_scene_saved_key_menu_submenu_callback, app);
    submenu_add_item(
        submenu,
        "Encoder et ajouter mdp",
        SubmenuIndexWriteAndSetPass,
        lfrfid_scene_saved_key_menu_submenu_callback,
        app);
    submenu_add_item(
        submenu, "Editer", SubmenuIndexEdit, lfrfid_scene_saved_key_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "Supprimer", SubmenuIndexDelete, lfrfid_scene_saved_key_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, lfrfid_scene_saved_key_menu_submenu_callback, app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, LfRfidSceneSavedKeyMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewSubmenu);
}

bool lfrfid_scene_saved_key_menu_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexEmulate) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneEmulate);
            dolphin_deed(DolphinDeedRfidEmulate);
            consumed = true;
        } else if(event.event == SubmenuIndexWrite) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneWrite);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteAndSetPass) {
            scene_manager_set_scene_state(
                app->scene_manager, LfRfidSceneEnterPassword, LfRfidSceneWriteAndSetPass);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneEnterPassword);
            consumed = true;
        } else if(event.event == SubmenuIndexEdit) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSaveData);
            consumed = true;
        } else if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneDeleteConfirm);
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSavedInfo);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, LfRfidSceneSavedKeyMenu, event.event);
    }

    return consumed;
}

void lfrfid_scene_saved_key_menu_on_exit(void* context) {
    LfRfid* app = context;

    submenu_reset(app->submenu);
}
