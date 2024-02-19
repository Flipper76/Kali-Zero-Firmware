#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexSpiCc1101Handle,
    VarItemListIndexSpiNrf24Handle,
    VarItemListIndexUartEspChannel,
    VarItemListIndexUartNmeaChannel,
    VarItemListIndexUartGeneralChannel,
};

void kali_zero_app_scene_protocols_gpio_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_protocols_gpio_cc1101_handle_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    kalizero_settings.spi_cc1101_handle =
        variable_item_get_current_value_index(item) == 0 ? SpiDefault : SpiExtra;
    variable_item_set_current_value_text(
        item, kalizero_settings.spi_cc1101_handle == SpiDefault ? "Défaut" : "Extra");
    app->save_settings = true;
}

static void kali_zero_app_scene_protocols_gpio_nrf24_handle_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    kalizero_settings.spi_nrf24_handle =
        variable_item_get_current_value_index(item) == 0 ? SpiDefault : SpiExtra;
    variable_item_set_current_value_text(
        item, kalizero_settings.spi_nrf24_handle == SpiDefault ? "Défaut" : "Extra");
    app->save_settings = true;
}

static void kali_zero_app_scene_protocols_gpio_esp32_channel_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    kalizero_settings.uart_esp_channel = variable_item_get_current_value_index(item) == 0 ?
                                           FuriHalSerialIdUsart :
                                           FuriHalSerialIdLpuart;
    variable_item_set_current_value_text(
        item, kalizero_settings.uart_esp_channel == FuriHalSerialIdUsart ? "13,14" : "15,16");
    app->save_settings = true;
}

static void kali_zero_app_scene_protocols_gpio_nmea_channel_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    kalizero_settings.uart_nmea_channel = variable_item_get_current_value_index(item) == 0 ?
                                            FuriHalSerialIdUsart :
                                            FuriHalSerialIdLpuart;
    variable_item_set_current_value_text(
        item, kalizero_settings.uart_nmea_channel == FuriHalSerialIdUsart ? "13,14" : "15,16");
    app->save_settings = true;
}

static void kali_zero_app_scene_protocols_gpio_general_channel_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    kalizero_settings.uart_general_channel = variable_item_get_current_value_index(item) == 0 ?
                                               FuriHalSerialIdUsart :
                                               FuriHalSerialIdLpuart;
    variable_item_set_current_value_text(
        item, kalizero_settings.uart_general_channel == FuriHalSerialIdUsart ? "13,14" : "15,16");
    app->save_settings = true;
}

void kali_zero_app_scene_protocols_gpio_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "SPI CC1101 Handle",
        2,
        kali_zero_app_scene_protocols_gpio_cc1101_handle_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.spi_cc1101_handle);
    variable_item_set_current_value_text(
        item, kalizero_settings.spi_cc1101_handle == SpiDefault ? "Défaut" : "Extra");

    item = variable_item_list_add(
        var_item_list,
        "SPI NRF24 Handle",
        2,
        kali_zero_app_scene_protocols_gpio_nrf24_handle_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.spi_nrf24_handle);
    variable_item_set_current_value_text(
        item, kalizero_settings.spi_nrf24_handle == SpiDefault ? "Défaut" : "Extra");

    item = variable_item_list_add(
        var_item_list,
        "Canal UART ESP32/ESP8266",
        2,
        kali_zero_app_scene_protocols_gpio_esp32_channel_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.uart_esp_channel);
    variable_item_set_current_value_text(
        item, kalizero_settings.uart_esp_channel == FuriHalSerialIdUsart ? "13,14" : "15,16");

    item = variable_item_list_add(
        var_item_list,
        "Canal UART NMEA",
        2,
        kali_zero_app_scene_protocols_gpio_nmea_channel_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.uart_nmea_channel);
    variable_item_set_current_value_text(
        item, kalizero_settings.uart_nmea_channel == FuriHalSerialIdUsart ? "13,14" : "15,16");

    item = variable_item_list_add(
        var_item_list,
        "Canal UART Général",
        2,
        kali_zero_app_scene_protocols_gpio_general_channel_changed,
        app);
    variable_item_set_current_value_index(item, kalizero_settings.uart_general_channel);
    variable_item_set_current_value_text(
        item, kalizero_settings.uart_general_channel == FuriHalSerialIdUsart ? "13,14" : "15,16");

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_protocols_gpio_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneProtocolsGpio));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_protocols_gpio_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, KaliZeroAppSceneProtocolsGpio, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_protocols_gpio_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
