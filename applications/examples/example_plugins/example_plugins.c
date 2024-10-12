/**
 * @file example_plugins.c
 * @brief Plugin host application example.
 *
 * Loads a single plugin and calls its methods.
 */

#include "plugin_interface.h"

#include <furi.h>

#include <flipper_application/flipper_application.h>
#include <loader/firmware_api/firmware_api.h>
#include <storage/storage.h>

#define TAG "ExamplePlugins"

int32_t example_plugins_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "Démarrage");

    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperApplication* app = flipper_application_alloc(storage, firmware_api_interface);

    do {
        FlipperApplicationPreloadStatus preload_res =
            flipper_application_preload(app, APP_DATA_PATH("plugins/example_plugin1.fal"));

        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            FURI_LOG_E(TAG, "Échec du préchargement du plugin");
            break;
        }

        if(!flipper_application_is_plugin(app)) {
            FURI_LOG_E(TAG, "Le fichier de plugin n'est pas une bibliothèque");
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            FURI_LOG_E(TAG, "Impossible de charger le fichier du plug-in");
            break;
        }

        const FlipperAppPluginDescriptor* app_descriptor =
            flipper_application_plugin_get_descriptor(app);

        FURI_LOG_I(
            TAG,
            "Plugin chargé pour appid '%s', API %lu",
            app_descriptor->appid,
            app_descriptor->ep_api_version);

        furi_check(app_descriptor->ep_api_version == PLUGIN_API_VERSION);
        furi_check(strcmp(app_descriptor->appid, PLUGIN_APP_ID) == 0);

        const ExamplePlugin* plugin = app_descriptor->entry_point;

        FURI_LOG_I(TAG, "Nom du plugin: %s", plugin->name);
        FURI_LOG_I(TAG, "Méthode1 du plugin: %d", plugin->method1());
        FURI_LOG_I(TAG, "Méthode2 du plugin(7,8): %d", plugin->method2(7, 8));
        FURI_LOG_I(TAG, "Méthode2 du plugin(1337,228): %d", plugin->method2(1337, 228));
    } while(false);
    flipper_application_free(app);

    furi_record_close(RECORD_STORAGE);
    FURI_LOG_I(TAG, "Au revoir!");

    return 0;
}
