#include "../subghz_i.h" // IWYU pragma: keep

enum SubmenuIndex {
    SubmenuIndexEmulate,
    SubmenuIndexEdit,
    SubmenuIndexDelete,
    SubmenuIndexGeo,
};

void subghz_scene_saved_menu_submenu_callback(void* context, uint32_t index) {
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, index);
}

void subghz_scene_saved_menu_on_enter(void* context) {
    SubGhz* subghz = context;
    submenu_add_item(
        subghz->submenu,
        "Emulater",
        SubmenuIndexEmulate,
        subghz_scene_saved_menu_submenu_callback,
        subghz);

    submenu_add_item(
        subghz->submenu,
        "Renommer",
        SubmenuIndexEdit,
        subghz_scene_saved_menu_submenu_callback,
        subghz);

    submenu_add_item(
        subghz->submenu,
        "Supprimer",
        SubmenuIndexDelete,
        subghz_scene_saved_menu_submenu_callback,
        subghz);

    if(!isnanf(subghz_txrx_get_latitude(subghz->txrx)) ||
       !isnanf(subghz_txrx_get_longitude(subghz->txrx)) || subghz->gps) {
        submenu_add_item(
            subghz->submenu,
            "Geographic info",
            SubmenuIndexGeo,
            subghz_scene_saved_menu_submenu_callback,
            subghz);
    }

    submenu_set_selected_item(
        subghz->submenu,
        scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneSavedMenu));

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdMenu);
}

bool subghz_scene_saved_menu_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexEmulate) {
            scene_manager_set_scene_state(
                subghz->scene_manager, SubGhzSceneSavedMenu, SubmenuIndexEmulate);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneTransmitter);
            return true;
        } else if(event.event == SubmenuIndexDelete) {
            scene_manager_set_scene_state(
                subghz->scene_manager, SubGhzSceneSavedMenu, SubmenuIndexDelete);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneDelete);
            return true;
        } else if(event.event == SubmenuIndexEdit) {
            scene_manager_set_scene_state(
                subghz->scene_manager, SubGhzSceneSavedMenu, SubmenuIndexEdit);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneSaveName);
            return true;
        } else if(event.event == SubmenuIndexGeo) {
            scene_manager_set_scene_state(
                subghz->scene_manager, SubGhzSceneSavedMenu, SubmenuIndexGeo);
            scene_manager_set_scene_state(subghz->scene_manager, SubGhzSceneShowGps, true);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneShowGps);
            return true;
        }
    }
    return false;
}

void subghz_scene_saved_menu_on_exit(void* context) {
    SubGhz* subghz = context;
    submenu_reset(subghz->submenu);
}
