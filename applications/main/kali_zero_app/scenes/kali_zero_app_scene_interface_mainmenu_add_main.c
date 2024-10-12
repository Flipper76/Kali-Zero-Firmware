#include "../kali_zero_app.h"

static void
    kali_zero_app_scene_interface_mainmenu_add_main_submenu_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    const char* name = (const char*)index;

    CharList_push_back(app->mainmenu_app_exes, strdup(name));
    CharList_push_back(app->mainmenu_app_labels, strdup(name));
    app->mainmenu_app_index = CharList_size(app->mainmenu_app_labels) - 1;
    app->save_mainmenu_apps = true;
    scene_manager_search_and_switch_to_previous_scene(
        app->scene_manager, KaliZeroAppSceneInterfaceMainmenu);
}

void kali_zero_app_scene_interface_mainmenu_add_main_on_enter(void* context) {
    KaliZeroApp* app = context;
    Submenu* submenu = app->submenu;

    for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_APPS[i].name,
            (uint32_t)FLIPPER_APPS[i].name,
            kali_zero_app_scene_interface_mainmenu_add_main_submenu_callback,
            app);
    }
    for(size_t i = 0; i < FLIPPER_EXTERNAL_APPS_COUNT - 1; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_EXTERNAL_APPS[i].name,
            (uint32_t)FLIPPER_EXTERNAL_APPS[i].name,
            kali_zero_app_scene_interface_mainmenu_add_main_submenu_callback,
            app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewSubmenu);
}

bool kali_zero_app_scene_interface_mainmenu_add_main_on_event(
    void* context,
    SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void kali_zero_app_scene_interface_mainmenu_add_main_on_exit(void* context) {
    KaliZeroApp* app = context;
    submenu_reset(app->submenu);
}
