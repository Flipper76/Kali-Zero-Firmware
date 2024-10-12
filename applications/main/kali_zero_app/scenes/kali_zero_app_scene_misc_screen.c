#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexDarkMode,
    VarItemListIndexLeftHanded,
    VarItemListIndexRgbBacklight,
    VarItemListIndexLcdColor0,
    VarItemListIndexLcdColor1,
    VarItemListIndexLcdColor2,
    VarItemListIndexRainbowLcd,
    VarItemListIndexRainbowSpeed,
    VarItemListIndexRainbowInterval,
    VarItemListIndexRainbowSaturation,
};

void kali_zero_app_scene_misc_screen_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_misc_screen_dark_mode_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    kalizero_settings.dark_mode = value;
    app->save_settings = true;
}

static void kali_zero_app_scene_misc_screen_hand_orient_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    if(value) {
        furi_hal_rtc_set_flag(FuriHalRtcFlagHandOrient);
    } else {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagHandOrient);
    }
}

static void kali_zero_app_scene_misc_screen_rgb_backlight_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexRgbBacklight);
}

static const struct {
    char* name;
    RgbColor color;
} lcd_colors[] = {
    // clang-format off
    {"Off", {{0, 0, 0}}},
    {"Orange", {{255, 69, 0}}},
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
static const size_t lcd_sz = COUNT_OF(lcd_colors);
static void kali_zero_app_scene_misc_screen_lcd_color_changed(VariableItem* item, uint8_t led) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, lcd_colors[index].name);
    rgb_backlight_set_color(led, &lcd_colors[index].color);
    app->save_backlight = true;
}
static void kali_zero_app_scene_misc_screen_lcd_color_0_changed(VariableItem* item) {
    kali_zero_app_scene_misc_screen_lcd_color_changed(item, 0);
}
static void kali_zero_app_scene_misc_screen_lcd_color_1_changed(VariableItem* item) {
    kali_zero_app_scene_misc_screen_lcd_color_changed(item, 1);
}
static void kali_zero_app_scene_misc_screen_lcd_color_2_changed(VariableItem* item) {
    kali_zero_app_scene_misc_screen_lcd_color_changed(item, 2);
}
static const struct {
    uint8_t led;
    VariableItemChangeCallback cb;
} lcd_cols[] = {
    {0, kali_zero_app_scene_misc_screen_lcd_color_0_changed},
    {1, kali_zero_app_scene_misc_screen_lcd_color_1_changed},
    {2, kali_zero_app_scene_misc_screen_lcd_color_2_changed},
};

const char* const rainbow_lcd_names[RGBBacklightRainbowModeCount] = {
    "OFF",
    "Wave",
    "Static",
};
static void kali_zero_app_scene_misc_screen_rainbow_lcd_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rainbow_lcd_names[index]);
    rgb_backlight_set_rainbow_mode(index);
    app->save_backlight = true;
}

static void kali_zero_app_scene_misc_screen_rainbow_speed_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item) + 1;
    char str[4];
    snprintf(str, sizeof(str), "%d", index);
    variable_item_set_current_value_text(item, str);
    rgb_backlight_set_rainbow_speed(index);
    app->save_backlight = true;
}

const char* const rainbow_interval_names[] = {
    "0.1 S",
    "0.2 S",
    "0.25 S",
    "0.5 S",
    "0.75 S",
    "1 S",
    "1.25 S",
    "1.5 S",
    "1.75 S",
    "2 S",
    "2.5 S",
    "3 S",
    "4 S",
    "5 S",
};
const uint32_t rainbow_interval_values[COUNT_OF(rainbow_interval_names)] = {
    100,
    200,
    250,
    500,
    750,
    1000,
    1250,
    1500,
    1750,
    2000,
    2500,
    3000,
    4000,
    5000,
};
static void kali_zero_app_scene_misc_screen_rainbow_interval_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rainbow_interval_names[index]);
    rgb_backlight_set_rainbow_interval(rainbow_interval_values[index]);
    app->save_backlight = true;
}

static void kali_zero_app_scene_misc_screen_rainbow_saturation_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item) + 1;
    char str[4];
    snprintf(str, sizeof(str), "%d", index);
    variable_item_set_current_value_text(item, str);
    rgb_backlight_set_rainbow_saturation(index);
    app->save_backlight = true;
}

void kali_zero_app_scene_misc_screen_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        var_item_list, "Mode sombre", 2, kali_zero_app_scene_misc_screen_dark_mode_changed, app);
    variable_item_set_current_value_index(item, kalizero_settings.dark_mode);
    variable_item_set_current_value_text(item, kalizero_settings.dark_mode ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "Mode Gaucher", 2, kali_zero_app_scene_misc_screen_hand_orient_changed, app);
    value_index = furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, value_index ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Rétroéclairage RVB",
        2,
        kali_zero_app_scene_misc_screen_rgb_backlight_changed,
        app);
    value_index = kalizero_settings.rgb_backlight;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, value_index ? "ON" : "OFF");

    RgbColor color;
    for(size_t i = 0; i < COUNT_OF(lcd_cols); i++) {
        char name[12];
        snprintf(name, sizeof(name), "LCD LED %u", lcd_cols[i].led + 1);
        item = variable_item_list_add(var_item_list, name, lcd_sz, lcd_cols[i].cb, app);
        rgb_backlight_get_color(lcd_cols[i].led, &color);
        bool found = false;
        for(size_t i = 0; i < lcd_sz; i++) {
            if(rgbcmp(&color, &lcd_colors[i].color) != 0) continue;
            value_index = i;
            found = true;
            break;
        }
        variable_item_set_current_value_index(item, found ? value_index : lcd_sz);
        if(found) {
            variable_item_set_current_value_text(item, lcd_colors[value_index].name);
        } else {
            char str[7];
            snprintf(str, sizeof(str), "%02X%02X%02X", color.r, color.g, color.b);
            variable_item_set_current_value_text(item, str);
        }
        variable_item_set_locked(item, !kalizero_settings.rgb_backlight, "Nécessite un \nrétroéclairage \nRVB!");
    }

    item = variable_item_list_add(
        var_item_list,
        "Écran LCD arc-en-ciel",
        RGBBacklightRainbowModeCount,
        kali_zero_app_scene_misc_screen_rainbow_lcd_changed,
        app);
    value_index = rgb_backlight_get_rainbow_mode();
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rainbow_lcd_names[value_index]);
    variable_item_set_locked(item, !kalizero_settings.rgb_backlight, "Nécessite un \nrétroéclairage \nRVB!");

    item = variable_item_list_add(
        var_item_list,
        "Vitesse arc-en-ciel",
        25,
        kali_zero_app_scene_misc_screen_rainbow_speed_changed,
        app);
    value_index = rgb_backlight_get_rainbow_speed();
    variable_item_set_current_value_index(item, value_index - 1);
    char speed_str[4];
    snprintf(speed_str, sizeof(speed_str), "%d", value_index);
    variable_item_set_current_value_text(item, speed_str);
    variable_item_set_locked(item, !kalizero_settings.rgb_backlight, "Nécessite un \nrétroéclairage \nRVB!");

    item = variable_item_list_add(
        var_item_list,
        "Intervalle arc-en-ciel",
        COUNT_OF(rainbow_interval_values),
        kali_zero_app_scene_misc_screen_rainbow_interval_changed,
        app);
    value_index = value_index_uint32(
        rgb_backlight_get_rainbow_interval(),
        rainbow_interval_values,
        COUNT_OF(rainbow_interval_values));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rainbow_interval_names[value_index]);
    variable_item_set_locked(item, !kalizero_settings.rgb_backlight, "Nécessite un \nrétroéclairage \nRVB!");

    item = variable_item_list_add(
        var_item_list,
        "Saturation arc-en-ciel",
        255,
        kali_zero_app_scene_misc_screen_rainbow_saturation_changed,
        app);
    value_index = rgb_backlight_get_rainbow_saturation();
    variable_item_set_current_value_index(item, value_index - 1);
    char saturation_str[4];
    snprintf(saturation_str, sizeof(saturation_str), "%d", value_index);
    variable_item_set_current_value_text(item, saturation_str);
    variable_item_set_locked(item, !kalizero_settings.rgb_backlight, "Nécessite un \nrétroéclairage \nRVB!");

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_misc_screen_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneMiscScreen));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_misc_screen_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, KaliZeroAppSceneMiscScreen, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexRgbBacklight: {
            VariableItem* item =
                variable_item_list_get(app->var_item_list, VarItemListIndexRgbBacklight);
            bool value = variable_item_get_current_value_index(item);
            if(value == kalizero_settings.rgb_backlight) value = !value; // Invoked via click
            bool change = !value; // Change without confirm if going from ON to OFF
            if(value) {
                DialogMessage* msg = dialog_message_alloc();
                dialog_message_set_header(msg, "Rétroéclairage RVB", 64, 0, AlignCenter, AlignTop);
                dialog_message_set_buttons(msg, "Non", NULL, "Oui");
                dialog_message_set_text(
                    msg,
                    "Cette option nécessite \nl'installation\n d'une modification \nmatérielle!\nEst-elle installé?",
                    64,
                    32,
                    AlignCenter,
                    AlignCenter);
                if(dialog_message_show(app->dialogs, msg) == DialogMessageButtonRight) {
                    change = true;
                }
                dialog_message_free(msg);
            }
            if(change) {
                kalizero_settings.rgb_backlight = value;
                app->save_settings = true;
                app->save_backlight = true;
                notification_message(app->notification, &sequence_display_backlight_on);
                rgb_backlight_reconfigure(value);

                for(size_t i = 0; i < COUNT_OF(lcd_cols); i++) {
                    variable_item_set_locked(
                        variable_item_list_get(app->var_item_list, VarItemListIndexLcdColor0 + i),
                        !value,
                        "Nécessite un \nrétroéclairage \nRVB!");
                }
                variable_item_set_locked(
                    variable_item_list_get(app->var_item_list, VarItemListIndexRainbowLcd),
                    !value,
                    "Nécessite un \nrétroéclairage \nRVB!");
                variable_item_set_locked(
                    variable_item_list_get(app->var_item_list, VarItemListIndexRainbowSpeed),
                    !value,
                    "Nécessite un \nrétroéclairage \nRVB!");
                variable_item_set_locked(
                    variable_item_list_get(app->var_item_list, VarItemListIndexRainbowInterval),
                    !value,
                    "Nécessite un \nrétroéclairage \nRVB!");
                variable_item_set_locked(
                    variable_item_list_get(app->var_item_list, VarItemListIndexRainbowSaturation),
                    !value,
                    "Nécessite un \nrétroéclairage \nRVB!");
            } else {
                value = !value;
            }
            variable_item_set_current_value_index(item, value);
            variable_item_set_current_value_text(item, value ? "ON" : "OFF");
            break;
        }
        case VarItemListIndexLcdColor0:
        case VarItemListIndexLcdColor1:
        case VarItemListIndexLcdColor2:
            scene_manager_set_scene_state(
                app->scene_manager,
                KaliZeroAppSceneMiscScreenColor,
                event.event - VarItemListIndexLcdColor0);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneMiscScreenColor);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_misc_screen_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
