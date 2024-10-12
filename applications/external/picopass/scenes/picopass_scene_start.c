#include "../picopass_i.h"
enum SubmenuIndex {
    SubmenuIndexRead,
    SubmenuIndexSaved,
    SubmenuIndexLoclass,
    SubmenuIndexAcknowledgements,
    SubmenuIndexKeygenAttack,
};

void picopass_scene_start_submenu_callback(void* context, uint32_t index) {
    Picopass* picopass = context;
    view_dispatcher_send_custom_event(picopass->view_dispatcher, index);
}
void picopass_scene_start_on_enter(void* context) {
    Picopass* picopass = context;

    Submenu* submenu = picopass->submenu;
    submenu_add_item(
        submenu, "Read Card", SubmenuIndexRead, picopass_scene_start_submenu_callback, picopass);
    submenu_add_item(
        submenu, "Saved", SubmenuIndexSaved, picopass_scene_start_submenu_callback, picopass);
    submenu_add_item(
        submenu, "Loclass", SubmenuIndexLoclass, picopass_scene_start_submenu_callback, picopass);
    submenu_add_item(
        submenu,
        "Acknowledgements",
        SubmenuIndexAcknowledgements,
        picopass_scene_start_submenu_callback,
        picopass);
    submenu_add_item(
        submenu,
        "Elite Keygen Attack",
        SubmenuIndexKeygenAttack,
        picopass_scene_start_submenu_callback,
        picopass);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(picopass->scene_manager, PicopassSceneStart));
    picopass_device_clear(picopass->dev);
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewMenu);
}

bool picopass_scene_start_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneStart, SubmenuIndexRead);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneEliteDictAttack);
            consumed = true;
        } else if(event.event == SubmenuIndexSaved) {
            // Explicitly save state so that the correct item is
            // reselected if the user cancels loading a file.
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneStart, SubmenuIndexSaved);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneFileSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexLoclass) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneStart, PicopassSceneLoclass);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneLoclass);
            consumed = true;
        } else if(event.event == SubmenuIndexAcknowledgements) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneStart, PicopassSceneAcknowledgements);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneAcknowledgements);
            consumed = true;
        } else if(event.event == SubmenuIndexKeygenAttack) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneStart, SubmenuIndexKeygenAttack);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneEliteKeygenAttack);
            consumed = true;
        }
    }

    return consumed;
}

void picopass_scene_start_on_exit(void* context) {
    Picopass* picopass = context;
    submenu_reset(picopass->submenu);
}
