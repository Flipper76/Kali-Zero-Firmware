#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexForeground,
    VarItemListIndexBackground,
};

void kali_zero_app_scene_misc_vgm_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static const struct {
    char* name;
    RgbColor color;
} vgm_colors[] = {
    // clang-format off
    {"Défaut", {{0, 0, 0}}},
    {"Arc-en-ciel", {{0, 0, 0}}},
    {"ModRgb", {{0, 0, 0}}},
    {"Noir", {{0, 0, 0}}},
    {"Orange", {{255, 130, 0}}},
    {"Rouge", {{255, 0, 0}}},
    {"Marron", {{128, 0, 0}}},
    {"Jaune", {{255, 255, 0}}},
    {"Olive", {{128, 128, 0}}},
    {"Citron vert", {{0, 255, 0}}},
    {"Vert", {{0, 128, 0}}},
    {"Aqua", {{0, 255, 127}}},
    {"Cyan", {{0, 210, 210}}},
    {"Azur", {{0, 127, 255}}},
    {"Sarcelle", {{0, 128, 128}}},
    {"Bleu", {{0, 0, 255}}},
    {"Marine", {{0, 0, 128}}},
    {"Violet", {{128, 0, 128}}},
    {"Fuchsia", {{255, 0, 255}}},
    {"Rose", {{173, 31, 173}}},
    {"Brun", {{165, 42, 42}}},
    {"Blanc", {{255, 192, 203}}},
    // clang-format on
};

static const size_t vgm_colors_count = COUNT_OF(vgm_colors);

static void kali_zero_app_scene_misc_vgm_foreground_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, vgm_colors[index].name);
    kalizero_settings.rpc_color_fg.rgb = vgm_colors[index].color;

    if(strcmp("Défaut", vgm_colors[index].name) == 0) {
        kalizero_settings.rpc_color_fg.mode = ScreenColorModeDefault;
    } else if(strcmp("Rainbow", vgm_colors[index].name) == 0) {
        kalizero_settings.rpc_color_fg.mode = ScreenColorModeRainbow;
    } else if(strcmp("RgbMod", vgm_colors[index].name) == 0) {
        kalizero_settings.rpc_color_fg.mode = ScreenColorModeRgbBacklight;
    } else {
        kalizero_settings.rpc_color_fg.mode = ScreenColorModeCustom;
    }

    app->save_settings = true;
}

static void kali_zero_app_scene_misc_vgm_background_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, vgm_colors[index].name);
    kalizero_settings.rpc_color_bg.rgb = vgm_colors[index].color;

    if(strcmp("Défaut", vgm_colors[index].name) == 0) {
        kalizero_settings.rpc_color_bg.mode = ScreenColorModeDefault;
    } else if(strcmp("Rainbow", vgm_colors[index].name) == 0) {
        kalizero_settings.rpc_color_bg.mode = ScreenColorModeRainbow;
    } else if(strcmp("RgbMod", vgm_colors[index].name) == 0) {
        kalizero_settings.rpc_color_bg.mode = ScreenColorModeRgbBacklight;
    } else {
        kalizero_settings.rpc_color_bg.mode = ScreenColorModeCustom;
    }

    app->save_settings = true;
}

void kali_zero_app_scene_misc_vgm_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        var_item_list,
        "Premier plan",
        vgm_colors_count,
        kali_zero_app_scene_misc_vgm_foreground_changed,
        app);
    ScreenFrameColor color = kalizero_settings.rpc_color_fg;
    bool found = true;
    if(color.mode == ScreenColorModeRainbow) {
        value_index = 1;
    } else if(color.mode == ScreenColorModeRgbBacklight) {
        value_index = 2;
    } else if(color.mode == ScreenColorModeCustom) {
        found = false;
        for(size_t i = 3; i < vgm_colors_count; i++) {
            if(rgbcmp(&color.rgb, &vgm_colors[i].color) != 0) continue;
            value_index = i;
            found = true;
            break;
        }
    } else {
        value_index = 0;
    }

    variable_item_set_current_value_index(item, found ? value_index : 0);
    if(found) {
        variable_item_set_current_value_text(item, vgm_colors[value_index].name);
    } else {
        char str[7];
        snprintf(str, sizeof(str), "%02X%02X%02X", color.rgb.r, color.rgb.g, color.rgb.b);
        variable_item_set_current_value_text(item, str);
    }

    item = variable_item_list_add(
        var_item_list,
        "Arrière-plan",
        vgm_colors_count,
        kali_zero_app_scene_misc_vgm_background_changed,
        app);
    color = kalizero_settings.rpc_color_bg;
    found = true;
    if(color.mode == ScreenColorModeRainbow) {
        value_index = 1;
    } else if(color.mode == ScreenColorModeRgbBacklight) {
        value_index = 2;
    } else if(color.mode == ScreenColorModeCustom) {
        found = false;
        for(size_t i = 3; i < vgm_colors_count; i++) {
            if(rgbcmp(&color.rgb, &vgm_colors[i].color) != 0) continue;
            value_index = i;
            found = true;
            break;
        }
    } else {
        value_index = 0;
    }

    variable_item_set_current_value_index(item, found ? value_index : 0);
    if(found) {
        variable_item_set_current_value_text(item, vgm_colors[value_index].name);
    } else {
        char str[7];
        snprintf(str, sizeof(str), "%02X%02X%02X", color.rgb.r, color.rgb.g, color.rgb.b);
        variable_item_set_current_value_text(item, str);
    }

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_misc_vgm_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneMiscVgm));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_misc_vgm_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMiscVgm, event.event);
        consumed = true;

        switch(event.event) {
        case VarItemListIndexForeground:
        case VarItemListIndexBackground:
            scene_manager_set_scene_state(
                app->scene_manager,
                KaliZeroAppSceneMiscVgmColor,
                event.event - VarItemListIndexForeground);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneMiscVgmColor);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_misc_vgm_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
