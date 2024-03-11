#pragma once

#include <extra_beacon.h>

#define FINDMY_STATE_HEADER "FindMy Flipper State"
#define FINDMY_STATE_VER 1
#define FINDMY_STATE_DIR EXT_PATH("apps_data/findmy")
#define FINDMY_STATE_PATH FINDMY_STATE_DIR "/findmy_state.txt"

typedef struct {
    bool beacon_active;
    uint8_t broadcast_interval;
    uint8_t transmit_power;

    uint8_t mac[EXTRA_BEACON_MAC_ADDR_SIZE];
    uint8_t data[EXTRA_BEACON_MAX_DATA_SIZE];

    // Generated from the other state values
    GapExtraBeaconConfig config;
} FindMyState;

bool findmy_state_load(FindMyState* out_state);

void findmy_state_apply(FindMyState* state);

void findmy_state_sync_config(FindMyState* state);

void findmy_state_save(FindMyState* state);
