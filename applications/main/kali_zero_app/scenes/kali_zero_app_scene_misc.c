#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexScreen,
    VarItemListIndexDolphin,
    VarItemListIndexSpoof,
    VarItemListIndexVgm,
    VarItemListIndexChargeCap,
    VarItemListIndexShowKaliZeroIntro,
};

void kali_zero_app_scene_misc_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

#define CHARGE_CAP_INTV 5
static void kali_zero_app_scene_misc_charge_cap_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    char cap_str[6];
    uint32_t value = (variable_item_get_current_value_index(item) + 1) * CHARGE_CAP_INTV;
    snprintf(cap_str, sizeof(cap_str), "%lu%%", value);
    variable_item_set_current_value_text(item, cap_str);
    kalizero_settings.charge_cap = value;
    app->save_settings = true;
}

void kali_zero_app_scene_misc_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(var_item_list, "Écran", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Dauphin", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Changer le nom du Flipper", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "VGM Options", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    char cap_str[6];
    value_index = kalizero_settings.charge_cap / CHARGE_CAP_INTV;
    snprintf(cap_str, 6, "%lu%%", (uint32_t)value_index * CHARGE_CAP_INTV);
    item = variable_item_list_add(
        var_item_list,
        "Cap. de Charge",
        100 / CHARGE_CAP_INTV,
        kali_zero_app_scene_misc_charge_cap_changed,
        app);
    variable_item_set_current_value_index(item, value_index - 1);
    variable_item_set_current_value_text(item, cap_str);

    variable_item_list_add(var_item_list, "Afficher Intro Kali Zéro", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_misc_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneMisc));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_misc_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMisc, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexScreen:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMiscScreen, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneMiscScreen);
            break;
        case VarItemListIndexDolphin:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMiscDolphin, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneMiscDolphin);
            break;
        case VarItemListIndexSpoof:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMiscSpoof, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneMiscSpoof);
            break;
        case VarItemListIndexVgm:
            scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMiscVgm, 0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneMiscVgm);
            break;
        case VarItemListIndexShowKaliZeroIntro: {
            for(int i = 0; i < 10; i++) {
                if(storage_common_copy(
                       app->storage, EXT_PATH("dolphin/firstboot.bin"), SLIDESHOW_FS_PATH)) {
                    app->show_slideshow = true;
                    kali_zero_app_apply(app);
                    break;
                }
            }
            break;
        }
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_misc_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
