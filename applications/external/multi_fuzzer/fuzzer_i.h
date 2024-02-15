#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>

#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>

#include "scenes/fuzzer_scene.h"
#include "views/main_menu.h"
#include "views/attack.h"
#include "views/field_editor.h"

#include "helpers/fuzzer_types.h"
#include "lib/worker/fake_worker.h"

#include <flipper_format/flipper_format_i.h>
#include "fuzzer_icons.h"

#include <assets_icons.h>

#define FUZZ_TIME_DELAY_MAX (80)
#define KEY_NAME_SIZE 24

typedef struct {
    const char* custom_dict_extension;
    const char* custom_dict_folder;
    const char* key_extension;
    const char* path_key_folder;
    const Icon* key_icon;
    const char* file_prefix;
} FuzzerConsts;

typedef struct {
    Gui* gui;
    NotificationApp* notifications;

    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Popup* popup;
    DialogsApp* dialogs;
    TextInput* text_input;
    FuzzerViewMain* main_view;
    FuzzerViewAttack* attack_view;
    FuzzerViewFieldEditor* field_editor_view;

    FuriString* file_path;
    char key_name[KEY_NAME_SIZE + 1];

    FuzzerState fuzzer_state;
    FuzzerConsts* fuzzer_const;

    FuzzerWorker* worker;
    FuzzerPayload* payload;
} PacsFuzzerApp;