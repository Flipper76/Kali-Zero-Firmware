#include "../subghz_i.h"
#include "../helpers/subghz_custom_event.h"
#include <lib/toolbox/value_index.h>
#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>

#define RADIO_DEVICE_COUNT 2
const char* const radio_device_text[RADIO_DEVICE_COUNT] = {
    "Interne",
    "Externe",
};

const uint32_t radio_device_value[RADIO_DEVICE_COUNT] = {
    SubGhzRadioDeviceTypeInternal,
    SubGhzRadioDeviceTypeExternalCC1101,
};

#define TIMESTAMP_NAMES_COUNT 2
const char* const timestamp_names_text[TIMESTAMP_NAMES_COUNT] = {
    "OFF",
    "ON",
};

#define EXT_MOD_POWER_AMP_COUNT 2
const char* const ext_mod_power_amp_text[EXT_MOD_POWER_AMP_COUNT] = {
    "OFF",
    "ON",
};

#define DEBUG_P_COUNT 2
const char* const debug_pin_text[DEBUG_P_COUNT] = {
    "OFF",
    "17(1W)",
};

#define GPS_COUNT 6
const char* const gps_text[GPS_COUNT] = {
    "OFF",
    "9600",
    "19200",
    "38400",
    "57600",
    "115200",
};

#define DEBUG_COUNTER_COUNT 13
const char* const debug_counter_text[DEBUG_COUNTER_COUNT] = {
    "+1",
    "+2",
    "+3",
    "+4",
    "+5",
    "+10",
    "0",
    "-1",
    "-2",
    "-3",
    "-4",
    "-5",
    "-10",
};
const uint32_t debug_counter_val[DEBUG_COUNTER_COUNT] = {
    1,
    2,
    3,
    4,
    5,
    10,
    0,
    -1,
    -2,
    -3,
    -4,
    -5,
    -10,
};

static void subghz_scene_radio_settings_set_device(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    if(!subghz_txrx_radio_device_is_external_connected(
           subghz->txrx, SUBGHZ_DEVICE_CC1101_EXT_NAME) &&
       radio_device_value[index] == SubGhzRadioDeviceTypeExternalCC1101) {
        //ToDo correct if there is more than 1 module
        index = 0;
    }
    variable_item_set_current_value_text(item, radio_device_text[index]);
    subghz_txrx_radio_device_set(subghz->txrx, radio_device_value[index]);
}

static void subghz_scene_receiver_config_set_debug_pin(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, debug_pin_text[index]);

    subghz_txrx_set_debug_pin_state(subghz->txrx, index == 1);
}

static void subghz_scene_receiver_config_set_debug_counter(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, debug_counter_text[index]);
    furi_hal_subghz_set_rolling_counter_mult(debug_counter_val[index]);
}

static void subghz_scene_reciever_config_set_ext_mod_power_amp_text(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, ext_mod_power_amp_text[index]);

    subghz->last_settings->external_module_power_amp = index == 1;

    // Set globally in furi hal
    furi_hal_subghz_set_ext_power_amp(subghz->last_settings->external_module_power_amp);

    subghz_last_settings_save(subghz->last_settings);

    // reinit external device
    const SubGhzRadioDeviceType current = subghz_txrx_radio_device_get(subghz->txrx);
    if(current != SubGhzRadioDeviceTypeInternal) {
        subghz_txrx_radio_device_set(subghz->txrx, SubGhzRadioDeviceTypeInternal);
        subghz_txrx_radio_device_set(subghz->txrx, current);
    }
}

static void subghz_scene_receiver_config_set_gps(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, gps_text[index]);

    switch(index) {
    case 0:
        subghz->last_settings->gps_baudrate = 0;
        break;
    case 1:
        subghz->last_settings->gps_baudrate = 9600;
        break;
    case 2:
        subghz->last_settings->gps_baudrate = 19200;
        break;
    case 3:
        subghz->last_settings->gps_baudrate = 38400;
        break;
    case 4:
        subghz->last_settings->gps_baudrate = 57600;
        break;
    case 5:
        subghz->last_settings->gps_baudrate = 115200;
        break;
    }
    subghz_last_settings_save(subghz->last_settings);

    if(subghz->last_settings->gps_baudrate != 0) {
        subghz_gps_stop(subghz->gps);
        subghz_gps_set_baudrate(subghz->gps, subghz->last_settings->gps_baudrate);
        subghz_gps_start(subghz->gps);
    } else {
        subghz_gps_stop(subghz->gps);
    }
}

static void subghz_scene_receiver_config_set_protocol_file_names(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, timestamp_names_text[index]);

    subghz->last_settings->protocol_file_names = (index == 1);
    subghz_last_settings_save(subghz->last_settings);
}

void subghz_scene_radio_settings_on_enter(void* context) {
    SubGhz* subghz = context;

    VariableItemList* variable_item_list = subghz->variable_item_list;
    uint8_t value_index;
    VariableItem* item;

    uint8_t value_count_device = RADIO_DEVICE_COUNT;
    if(subghz_txrx_radio_device_get(subghz->txrx) == SubGhzRadioDeviceTypeInternal &&
       !subghz_txrx_radio_device_is_external_connected(subghz->txrx, SUBGHZ_DEVICE_CC1101_EXT_NAME))
        value_count_device = 1; // Only 1 item if external disconnected
    item = variable_item_list_add(
        subghz->variable_item_list,
        "Module",
        value_count_device,
        subghz_scene_radio_settings_set_device,
        subghz);
    value_index = value_index_uint32(
        subghz_txrx_radio_device_get(subghz->txrx), radio_device_value, value_count_device);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, radio_device_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Ampli de puissance ext.",
        EXT_MOD_POWER_AMP_COUNT,
        subghz_scene_reciever_config_set_ext_mod_power_amp_text,
        subghz);
    value_index = subghz->last_settings->external_module_power_amp ? 1 : 0;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, ext_mod_power_amp_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Vitesse transm. GPS",
        GPS_COUNT,
        subghz_scene_receiver_config_set_gps,
        subghz);
    value_index = value_index_uint32(
        subghz->last_settings->gps_baudrate,
        (const uint32_t[]){0, 9600, 19200, 38400, 57600, 115200},
        GPS_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, gps_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Noms du protocole",
        TIMESTAMP_NAMES_COUNT,
        subghz_scene_receiver_config_set_protocol_file_names,
        subghz);
    value_index = subghz->last_settings->protocol_file_names;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, timestamp_names_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Incr. compteur.",
        furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug) ? DEBUG_COUNTER_COUNT : 3,
        subghz_scene_receiver_config_set_debug_counter,
        subghz);
    value_index = value_index_uint32(
        furi_hal_subghz_get_rolling_counter_mult(),
        debug_counter_val,
        furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug) ? DEBUG_COUNTER_COUNT : 3);
    furi_hal_subghz_set_rolling_counter_mult(debug_counter_val[value_index]);

    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, debug_counter_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Broche de Debug",
        DEBUG_P_COUNT,
        subghz_scene_receiver_config_set_debug_pin,
        subghz);
    value_index = subghz_txrx_get_debug_pin_state(subghz->txrx);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, debug_pin_text[value_index]);
    variable_item_set_locked(
        item, !furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug), "Activer\nDebug!");

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdVariableItemList);
}

bool subghz_scene_radio_settings_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    UNUSED(subghz);
    UNUSED(event);

    return false;
}

void subghz_scene_radio_settings_on_exit(void* context) {
    SubGhz* subghz = context;
    variable_item_list_set_selected_item(subghz->variable_item_list, 0);
    variable_item_list_reset(subghz->variable_item_list);
}
