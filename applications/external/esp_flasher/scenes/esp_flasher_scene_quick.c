#include "../esp_flasher_app_i.h"

// Marauder firmware source - https://github.com/justcallmekoko/ESP32Marauder
// Wardriver firmware source - https://github.com/Sil333033/flipperzero-wardriver
// AirTag Scanner firmware source - https://github.com/MatthewKuKanich/ESP32-AirTag-Scanner
// Black Magic firmware source - https://github.com/flipperdevices/blackmagic-esp32-s2

// DO NOT use as an example, you should split into different scene files for each screen
// To keep in a single file, this is setup in an unusual and confusing way
// You can find more info in comments throughout

// There's one entry for each item on quick flash menu
// Following each one, there are its submenu items
// Not all boards support automatic bootmode, so here we separate into those that do and don't
enum QuickState {
    QuickStart,
    QuickS2Boot,
    QuickS2Boot_Marauder,
    QuickS2Boot_Wardriver,
    QuickS2Boot_Blackmagic,
    QuickWROOMBoot,
    QuickWROOMBoot_Marauder,
    QuickWROOMBoot_Wardriver,
    QuickWROOMBoot_Airtag,
    QuickS3Boot,
    QuickS3Boot_Marauder,
    QuickS3Boot_Wardriver,
    QuickS3Boot_Airtag,
    QuickWROOM,
    QuickWROOM_Marauder,
    QuickWROOM_Wardriver,
    QuickWROOM_Airtag,
    QuickS2,
    QuickS2_Marauder,
    QuickS2_Wardriver,
    QuickS2_Blackmagic,
    QuickS3,
    QuickS3_Marauder,
    QuickS3_Wardriver,
    QuickS3_Airtag,
};

void esp_flasher_scene_quick_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    EspFlasherApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void esp_flasher_scene_quick_on_enter(void* context) {
    furi_assert(context);
    EspFlasherApp* app = context;
    Submenu* submenu = app->submenu;
    uint32_t state = scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneQuick);

    // State indicates the currently selected (or activated) item
    // So map quick flash menu values to show the quick flash menu
    // And map submenu values to show corresponding submenu
    switch(state) {
    case QuickStart:
    case QuickS2Boot:
    case QuickWROOMBoot:
    case QuickS3Boot:
    case QuickWROOM:
    case QuickS2:
    case QuickS3:
        submenu_set_header(submenu, "Choose Board:");
        submenu_add_item(
            submenu,
            "WiFi Dev / RL S2 / Xeon",
            QuickS2Boot,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "SCE Multi-Fucc",
            QuickWROOMBoot,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu, "RL ESP32-S3", QuickS3Boot, esp_flasher_scene_quick_submenu_callback, app);
        submenu_add_item(
            submenu,
            "Other ESP32-WROOM",
            QuickWROOM,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu, "Other ESP32-S2", QuickS2, esp_flasher_scene_quick_submenu_callback, app);
        submenu_add_item(
            submenu, "Other ESP32-S3", QuickS3, esp_flasher_scene_quick_submenu_callback, app);
        break;
    case QuickS2Boot_Marauder:
    case QuickS2Boot_Wardriver:
    case QuickS2Boot_Blackmagic:
    case QuickS2_Marauder:
    case QuickS2_Wardriver:
    case QuickS2_Blackmagic:
        submenu_set_header(submenu, "Choose Firmware:");
        submenu_add_item(
            submenu,
            "Marauder (has Evil Portal)",
            state > QuickS2 ? QuickS2_Marauder : QuickS2Boot_Marauder,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Wardriver",
            state > QuickS2 ? QuickS2_Wardriver : QuickS2Boot_Wardriver,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Black Magic",
            state > QuickS2 ? QuickS2_Blackmagic : QuickS2Boot_Blackmagic,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    case QuickWROOMBoot_Marauder:
    case QuickWROOMBoot_Wardriver:
    case QuickWROOMBoot_Airtag:
    case QuickWROOM_Marauder:
    case QuickWROOM_Wardriver:
    case QuickWROOM_Airtag:
        submenu_set_header(submenu, "Choose Firmware:");
        submenu_add_item(
            submenu,
            "Marauder (has Evil Portal)",
            state > QuickWROOM ? QuickWROOM_Marauder : QuickWROOMBoot_Marauder,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Wardriver",
            state > QuickWROOM ? QuickWROOM_Wardriver : QuickWROOMBoot_Wardriver,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "AirTag Scanner",
            state > QuickWROOM ? QuickWROOM_Airtag : QuickWROOMBoot_Airtag,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    case QuickS3Boot_Marauder:
    case QuickS3Boot_Wardriver:
    case QuickS3Boot_Airtag:
    case QuickS3_Marauder:
    case QuickS3_Wardriver:
    case QuickS3_Airtag:
        submenu_set_header(submenu, "Choose Firmware:");
        submenu_add_item(
            submenu,
            "Marauder (has Evil Portal)",
            state > QuickS3 ? QuickS3_Marauder : QuickS3Boot_Marauder,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Wardriver",
            state > QuickS3 ? QuickS3_Wardriver : QuickS3Boot_Wardriver,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "AirTag Scanner",
            state > QuickS3 ? QuickS3_Airtag : QuickS3Boot_Airtag,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    default:
        break;
    }

    submenu_set_selected_item(submenu, state);

    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewSubmenu);
}

bool esp_flasher_scene_quick_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    EspFlasherApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;

        bool enter_bootloader = false;
        bool s3 = false;
        const char* boot = NULL; // 0x1000 (or 0x0 on S3)
        const char* part = NULL; // 0x8000
        const char* app0 = NULL; // 0xE000
        const char* firm = NULL; // 0x10000

        switch(event.event) {
        case QuickS2Boot:
        case QuickWROOMBoot:
        case QuickS3Boot:
        case QuickWROOM:
        case QuickS2:
        case QuickS3:
            // Select first item of submenu
            scene_manager_set_scene_state(
                app->scene_manager, EspFlasherSceneQuick, event.event + 1);
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneQuick);
            return consumed;

        case QuickS2Boot_Marauder:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS2_Marauder:
            boot = APP_DATA_PATH("assets/marauder/s2/esp32_marauder.ino.bootloader.bin");
            part = APP_DATA_PATH("assets/marauder/esp32_marauder.ino.partitions.bin");
            app0 = APP_DATA_PATH("assets/marauder/boot_app0.bin");
            firm = APP_DATA_PATH("assets/marauder/s2/esp32_marauder.flipper.bin");
            break;

        case QuickS2Boot_Wardriver:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS2_Wardriver:
            boot = APP_DATA_PATH("assets/wardriver/f0-wardrive-s2.bin");
            break;

        case QuickS2Boot_Blackmagic:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS2_Blackmagic:
            boot = APP_DATA_PATH("assets/blackmagic/s2/bootloader.bin");
            part = APP_DATA_PATH("assets/blackmagic/s2/partition-table.bin");
            firm = APP_DATA_PATH("assets/blackmagic/s2/blackmagic.bin");
            break;

        case QuickWROOMBoot_Marauder:
            enter_bootloader = true;
            /* fallthrough */
        case QuickWROOM_Marauder:
            boot = APP_DATA_PATH("assets/marauder/wroom/esp32_marauder.ino.bootloader.bin");
            part = APP_DATA_PATH("assets/marauder/esp32_marauder.ino.partitions.bin");
            app0 = APP_DATA_PATH("assets/marauder/boot_app0.bin");
            firm = APP_DATA_PATH("assets/marauder/wroom/esp32_marauder.dev_board_pro.bin");
            break;

        case QuickWROOMBoot_Wardriver:
            enter_bootloader = true;
            /* fallthrough */
        case QuickWROOM_Wardriver:
            boot = APP_DATA_PATH("assets/wardriver/f0-wardrive-wroom.bin");
            break;

        case QuickWROOMBoot_Airtag:
            enter_bootloader = true;
            /* fallthrough */
        case QuickWROOM_Airtag:
            boot = APP_DATA_PATH("assets/airtag/wroom/airtag_scanner.ino.bootloader.bin");
            part = APP_DATA_PATH("assets/airtag/airtag_scanner.ino.partitions.bin");
            firm = APP_DATA_PATH("assets/airtag/wroom/airtag_scanner.ino.bin");
            break;

        case QuickS3Boot_Marauder:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS3_Marauder:
            s3 = true;
            boot = APP_DATA_PATH("assets/marauder/s3/esp32_marauder.ino.bootloader.bin");
            part = APP_DATA_PATH("assets/marauder/esp32_marauder.ino.partitions.bin");
            app0 = APP_DATA_PATH("assets/marauder/boot_app0.bin");
            firm = APP_DATA_PATH("assets/marauder/s3/esp32_marauder.multiboardS3.bin");
            break;

        case QuickS3Boot_Wardriver:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS3_Wardriver:
            s3 = true;
            boot = APP_DATA_PATH("assets/wardriver/f0-wardrive-s3.bin");
            break;

        case QuickS3Boot_Airtag:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS3_Airtag:
            s3 = true;
            boot = APP_DATA_PATH("assets/airtag/s3/airtag_scanner.ino.bootloader.bin");
            part = APP_DATA_PATH("assets/airtag/airtag_scanner.ino.partitions.bin");
            firm = APP_DATA_PATH("assets/airtag/s3/airtag_scanner.ino.bin");
            break;

        default:
            consumed = false;
            return consumed;
        }

        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, event.event);
        memset(app->selected_flash_options, 0, sizeof(app->selected_flash_options));
        app->bin_file_path_boot[0] = '\0';
        app->bin_file_path_part[0] = '\0';
        app->bin_file_path_nvs[0] = '\0';
        app->bin_file_path_boot_app0[0] = '\0';
        app->bin_file_path_app_a[0] = '\0';
        app->bin_file_path_app_b[0] = '\0';
        app->bin_file_path_custom[0] = '\0';

        app->selected_flash_options[SelectedFlashS3Mode] = s3;
        if(boot) {
            app->selected_flash_options[SelectedFlashBoot] = true;
            strncpy(app->bin_file_path_boot, boot, sizeof(app->bin_file_path_boot));
        }
        if(part) {
            app->selected_flash_options[SelectedFlashPart] = true;
            strncpy(app->bin_file_path_part, part, sizeof(app->bin_file_path_part));
        }
        if(app0) {
            app->selected_flash_options[SelectedFlashBootApp0] = true;
            strncpy(app->bin_file_path_boot_app0, app0, sizeof(app->bin_file_path_boot_app0));
        }
        if(firm) {
            app->selected_flash_options[SelectedFlashAppA] = true;
            strncpy(app->bin_file_path_app_a, firm, sizeof(app->bin_file_path_app_a));
        }

        app->reset = false;
        app->quickflash = true;
        app->turbospeed = true;
        app->boot = enter_bootloader;
        scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
    } else if(event.type == SceneManagerEventTypeBack) {
        uint32_t state = scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneQuick);
        // Pressing back from submenu, check if in submenu, select corresponding item in quick flash menu
        if(state > QuickS3)
            state = QuickS3;
        else if(state > QuickS2)
            state = QuickS2;
        else if(state > QuickWROOM)
            state = QuickWROOM;
        else if(state > QuickS3Boot)
            state = QuickS3Boot;
        else if(state > QuickWROOMBoot)
            state = QuickWROOMBoot;
        else if(state > QuickS2Boot)
            state = QuickS2Boot;
        // If pressing back from quick flash menu (not submenu), state will not matter
        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, state);
    }

    return consumed;
}

void esp_flasher_scene_quick_on_exit(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    submenu_reset(app->submenu);
}
