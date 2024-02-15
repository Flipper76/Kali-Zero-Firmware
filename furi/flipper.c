#include "flipper.h"
#include <applications.h>
#include <furi.h>
#include <furi_hal_version.h>
#include <furi_hal_memory.h>
#include <furi_hal_light.h>
#include <furi_hal_rtc.h>
#include <storage/storage.h>
#include <gui/canvas_i.h>

#include <FreeRTOS.h>

#define TAG "Flipper"

static void flipper_print_version(const char* target, const Version* version) {
    if(version) {
        FURI_LOG_I(
            TAG,
            "\r\n\t%s version:\t%s\r\n"
            "\tBuild date:\t\t%s\r\n"
            "\tGit Commit:\t\t%s (%s)%s\r\n"
            "\tGit Branch:\t\t%s",
            target,
            version_get_version(version),
            version_get_builddate(version),
            version_get_githash(version),
            version_get_gitbranchnum(version),
            version_get_dirty_flag(version) ? " (dirty)" : "",
            version_get_gitbranch(version));
    } else {
        FURI_LOG_I(TAG, "Aucune information \nde build pour %s", target);
    }
}

#ifndef FURI_RAM_EXEC
#include <bt/bt_settings.h>
#include <bt/bt_service/bt_i.h>
#include <power/power_settings.h>
#include <desktop/desktop_settings.h>
#include <notification/notification_app.h>
#include <dolphin/helpers/dolphin_state.h>
#include <applications/main/u2f/u2f_data.h>
#include <expansion/expansion_settings_filename.h>
#include <applications/main/archive/helpers/archive_favorites.h>
#include <xtreme/namespoof.h>
#include <xtreme/xtreme.h>

void flipper_migrate_files() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // Revert cringe
    FURI_LOG_I(TAG, "Migration�:�supprimer les \nfichiers inutilis�s");
    storage_common_remove(storage, INT_PATH(".passport.settings"));
    storage_common_remove(storage, INT_PATH(".region_data"));

    // Migrate files (use copy+remove to not overwrite dst but still delete src)
    FURI_LOG_I(TAG, "Migration: Renomme en externe");
    storage_common_copy(storage, ARCHIVE_FAV_OLD_PATH, ARCHIVE_FAV_PATH);
    storage_common_remove(storage, ARCHIVE_FAV_OLD_PATH);
    storage_common_copy(storage, DESKTOP_KEYBINDS_OLD_PATH, DESKTOP_KEYBINDS_PATH);
    storage_common_remove(storage, DESKTOP_KEYBINDS_OLD_PATH);
    // Int -> Ext
    FURI_LOG_I(TAG, "Migration: Interne vers ext");
    storage_common_copy(storage, BT_SETTINGS_OLD_PATH, BT_SETTINGS_PATH);
    storage_common_remove(storage, BT_SETTINGS_OLD_PATH);
    storage_common_copy(storage, DOLPHIN_STATE_OLD_PATH, DOLPHIN_STATE_PATH);
    storage_common_remove(storage, DOLPHIN_STATE_OLD_PATH);
    storage_common_copy(storage, POWER_SETTINGS_OLD_PATH, POWER_SETTINGS_PATH);
    storage_common_remove(storage, POWER_SETTINGS_OLD_PATH);
    storage_common_copy(storage, BT_KEYS_STORAGE_OLD_PATH, BT_KEYS_STORAGE_PATH);
    storage_common_remove(storage, BT_KEYS_STORAGE_OLD_PATH);
    storage_common_copy(storage, EXPANSION_SETTINGS_OLD_PATH, EXPANSION_SETTINGS_PATH);
    storage_common_remove(storage, EXPANSION_SETTINGS_OLD_PATH);
    // storage_common_copy(storage, NOTIFICATION_SETTINGS_OLD_PATH, NOTIFICATION_SETTINGS_PATH); // Not compatible anyway
    storage_common_remove(storage, NOTIFICATION_SETTINGS_OLD_PATH);
    // Ext -> Int
    FURI_LOG_I(TAG, "Migration: Ext vers interne");
    storage_common_copy(storage, DESKTOP_SETTINGS_OLD_PATH, DESKTOP_SETTINGS_PATH);
    storage_common_remove(storage, DESKTOP_SETTINGS_OLD_PATH);

    // Special care for U2F
    FURI_LOG_I(TAG, "Migration: U2F");
    FileInfo file_info;
    if(storage_common_stat(storage, U2F_CNT_OLD_FILE, &file_info) == FSE_OK &&
       file_info.size > 200) { // Is on Int and has content
        storage_common_rename(storage, U2F_CNT_OLD_FILE, U2F_CNT_FILE); // Int -> Ext
    }
    storage_common_copy(storage, U2F_KEY_OLD_FILE, U2F_KEY_FILE); // Ext -> Int

    FURI_LOG_I(TAG, "Migration: Packs ressources");
    storage_common_migrate(storage, KALIZERO_ASSETS_OLD_PATH, KALIZERO_ASSETS_PATH);

    furi_record_close(RECORD_STORAGE);
}

static void flipper_boot_status(Canvas* canvas, const char* text) {
    FURI_LOG_I(TAG, text);
    canvas_reset(canvas);
    canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, text);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 24, AlignCenter, AlignCenter, "Kali Zero d""\xE9""marre");
    canvas_commit(canvas);
}
#endif

void flipper_start_service(const FlipperInternalApplication* service) {
    FURI_LOG_D(TAG, "D�marrage du service %s", service->name);

    FuriThread* thread =
        furi_thread_alloc_ex(service->name, service->stack_size, service->app, NULL);
    furi_thread_mark_as_service(thread);
    furi_thread_set_appid(thread, service->appid);

    furi_thread_start(thread);
}

void flipper_init() {
    furi_hal_light_sequence("rgb WB");
    flipper_print_version("Firmware", furi_hal_version_get_firmware_version());
    FURI_LOG_I(TAG, "Boot mode %d", furi_hal_rtc_get_boot_mode());
#ifndef FURI_RAM_EXEC
    Canvas* canvas = canvas_init();

    // Start storage service first, thanks OFW :/
    flipper_boot_status(canvas, "Initialisation du stockage");
#endif
    flipper_start_service(&FLIPPER_SERVICES[0]);

#ifndef FURI_RAM_EXEC
    if(furi_hal_is_normal_boot()) {
        // Wait for storage record
        furi_record_open(RECORD_STORAGE);
        furi_record_close(RECORD_STORAGE);
        
        flipper_boot_status(canvas, "Migration de fichiers");
        flipper_migrate_files();
        
        flipper_boot_status(canvas, "D""\xE9""marrage usurpation de nom");
        namespoof_init();
        
        flipper_boot_status(canvas, "Chargement param. Kali Zero");
        kalizero_settings_load();
        furi_hal_light_sequence("rgb RB");
        
        flipper_boot_status(canvas, "Chargement actifs Kali Zero");
        kalizero_assets_init();
    } else {
        FURI_LOG_I(TAG, "D�marrage sp�cial, exclus option");
    }
	flipper_boot_status(canvas, "Initialisation des services");
#endif

    // Everything else
    for(size_t i = 1; i < FLIPPER_SERVICES_COUNT; i++) {
        flipper_start_service(&FLIPPER_SERVICES[i]);
    }
#ifndef FURI_RAM_EXEC
    canvas_free(canvas);
#endif

    FURI_LOG_I(TAG, "D�marrage termin�");
}

void vApplicationGetIdleTaskMemory(
    StaticTask_t** tcb_ptr,
    StackType_t** stack_ptr,
    uint32_t* stack_size) {
    *tcb_ptr = memmgr_alloc_from_pool(sizeof(StaticTask_t));
    *stack_ptr = memmgr_alloc_from_pool(sizeof(StackType_t) * configIDLE_TASK_STACK_DEPTH);
    *stack_size = configIDLE_TASK_STACK_DEPTH;
}

void vApplicationGetTimerTaskMemory(
    StaticTask_t** tcb_ptr,
    StackType_t** stack_ptr,
    uint32_t* stack_size) {
    *tcb_ptr = memmgr_alloc_from_pool(sizeof(StaticTask_t));
    *stack_ptr = memmgr_alloc_from_pool(sizeof(StackType_t) * configTIMER_TASK_STACK_DEPTH);
    *stack_size = configTIMER_TASK_STACK_DEPTH;
}
