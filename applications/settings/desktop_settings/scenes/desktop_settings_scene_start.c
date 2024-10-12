#include <applications.h>
#include <lib/toolbox/value_index.h>

#include "../desktop_settings_app.h"
#include "desktop_settings_scene.h"

typedef enum {
    DesktopSettingsPinSetup = 0,
    DesktopSettingsKeybindSetup,
    DesktopSettingsResetKeybinds,
    DesktopSettingsAutoLockDelay,
    DesktopSettingsAutoLockPin,
    DesktopSettingsClockDisplay,
    DesktopSettingsHappyMode,
} DesktopSettingsEntry;

#define AUTO_LOCK_DELAY_COUNT 9
static const char* const auto_lock_delay_text[AUTO_LOCK_DELAY_COUNT] = {
    "OFF",
    "10s",
    "15s",
    "30s",
    "60s",
    "90s",
    "2min",
    "5min",
    "10min",
};
static const uint32_t auto_lock_delay_value[AUTO_LOCK_DELAY_COUNT] =
    {0, 10000, 15000, 30000, 60000, 90000, 120000, 300000, 600000};

#define CLOCK_ENABLE_COUNT 2
const char* const clock_enable_text[CLOCK_ENABLE_COUNT] = {
    "OFF",
    "ON",
};

const uint32_t clock_enable_value[CLOCK_ENABLE_COUNT] = {0, 1};

static void desktop_settings_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void desktop_settings_scene_start_clock_enable_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, clock_enable_text[index]);
    app->settings.display_clock = index;
}

static void desktop_settings_scene_start_auto_lock_delay_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, auto_lock_delay_text[index]);
    app->settings.auto_lock_delay_ms = auto_lock_delay_value[index];
}

static void desktop_settings_scene_start_auto_lock_pin_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t value = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    app->settings.auto_lock_with_pin = value;
}

void desktop_settings_scene_start_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    VariableItemList* variable_item_list = app->variable_item_list;

    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(variable_item_list, "Configuration PIN", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "Modifier raccourcis", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "Rétablir raccourcis", 1, NULL, NULL);

    item = variable_item_list_add(
        variable_item_list,
        "Temps de verrouillage auto",
        AUTO_LOCK_DELAY_COUNT,
        desktop_settings_scene_start_auto_lock_delay_changed,
        app);

    value_index = value_index_uint32(
        app->settings.auto_lock_delay_ms, auto_lock_delay_value, AUTO_LOCK_DELAY_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, auto_lock_delay_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Verrouillage auto Pin",
        2,
        desktop_settings_scene_start_auto_lock_pin_changed,
        app);

    variable_item_set_current_value_index(item, app->settings.auto_lock_with_pin);
    variable_item_set_current_value_text(item, app->settings.auto_lock_with_pin ? "ON" : "OFF");

    item = variable_item_list_add(
        variable_item_list,
        "Afficher horloge",
        CLOCK_ENABLE_COUNT,
        desktop_settings_scene_start_clock_enable_changed,
        app);

    value_index =
        value_index_uint32(app->settings.display_clock, clock_enable_value, CLOCK_ENABLE_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, clock_enable_text[value_index]);

    variable_item_list_add(variable_item_list, "Mode Joyeux", 1, NULL, NULL);

    variable_item_list_set_enter_callback(
        variable_item_list, desktop_settings_scene_start_var_list_enter_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewVarItemList);
}

bool desktop_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DesktopSettingsPinSetup:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppScenePinMenu);
            break;

        case DesktopSettingsKeybindSetup:
            scene_manager_set_scene_state(
                app->scene_manager, DesktopSettingsAppSceneKeybindsType, 0);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
            consumed = true;
            break;

        case DesktopSettingsResetKeybinds:
            scene_manager_set_scene_state(
                app->scene_manager, DesktopSettingsAppSceneKeybindsType, 0);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneKeybindsReset);
            consumed = true;
            break;

        case DesktopSettingsHappyMode:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneHappyMode);
            break;

        default:
            break;
        }
        consumed = true;
    }
    return consumed;
}

void desktop_settings_scene_start_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
