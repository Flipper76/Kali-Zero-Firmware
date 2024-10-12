#pragma once

#include <furi.h>
#include <power/power_service/power.h>
#include <power/power_service/power_settings_api_i.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <assets_icons.h>

#include "views/battery_info.h"
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/variable_item_list.h>

#include "scenes/power_settings_scene.h"

typedef struct {
    PowerSettings settings;

    Power* power;
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    BatteryInfo* battery_info;
    Submenu* submenu;
    VariableItemList* variable_item_list;
    DialogEx* dialog;
    PowerInfo info;

    bool about_battery;
} PowerSettingsApp;

typedef enum {
    PowerSettingsAppViewBatteryInfo,
    PowerSettingsAppViewSubmenu,
    PowerSettingsAppViewDialog,
    PowerSettingsAppViewVariableItemList
} PowerSettingsAppView;

typedef enum {
    RebootTypeDFU,
    RebootTypeNormal
} RebootType;
