#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <furi_hal_serial_types.h>
#include <gui/canvas.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KALIZERO_SETTINGS_PATH CFG_PATH("kalizero_settings.txt")
#define KALIZERO_ASSETS_OLD_PATH EXT_PATH("dolphin_custom")
#define KALIZERO_ASSETS_PATH EXT_PATH("asset_packs")
#define KALIZERO_MENU_OLD_PATH CFG_PATH("kali_zero_apps.txt")
#define KALIZERO_MENU_PATH CFG_PATH("kalizero_menu.txt")
#define KALIZERO_ASSETS_PACK_NAME_LEN 32

typedef enum {
    BatteryIconOff,
    BatteryIconBar,
    BatteryIconPercent,
    BatteryIconInvertedPercent,
    BatteryIconRetro3,
    BatteryIconRetro5,
    BatteryIconBarPercent,
    BatteryIconCount,
} BatteryIcon;

typedef enum {
    MenuStyleList,
    MenuStyleWii,
    MenuStyleDsi,
    MenuStylePs4,
    MenuStyleVertical,
    MenuStyleC64,
    MenuStyleCompact,
    MenuStyleTerminal,
    MenuStyleCount,
} MenuStyle;

typedef enum {
    SpiDefault, // cs on pa4
    SpiExtra, // cs on pc3
    SpiCount,
} SpiHandle;

_Static_assert(sizeof(MenuStyle) == sizeof(uint8_t), "enum too big, fix load/save");
_Static_assert(sizeof(BatteryIcon) == sizeof(uint8_t), "enum too big, fix load/save");
_Static_assert(sizeof(SpiHandle) == sizeof(uint8_t), "enum too big, fix load/save");
_Static_assert(sizeof(FuriHalSerialId) == sizeof(uint8_t), "enum too big, fix load/save");

typedef struct {
    char asset_pack[KALIZERO_ASSETS_PACK_NAME_LEN];
    uint32_t anim_speed;
    int32_t cycle_anims;
    bool unlock_anims;
    bool credits_anim;
    MenuStyle menu_style;
    bool lock_on_boot;
    bool bad_pins_format;
    bool allow_locked_rpc_commands;
    bool lockscreen_poweroff;
    bool lockscreen_time;
    bool lockscreen_seconds;
    bool lockscreen_date;
    bool lockscreen_statusbar;
    bool lockscreen_prompt;
    bool lockscreen_transparent;
    BatteryIcon battery_icon;
    bool statusbar_clock;
    bool status_icons;
    bool bar_borders;
    bool bar_background;
    bool sort_dirs_first;
    bool show_hidden_files;
    bool show_internal_tab;
    uint32_t favorite_timeout;
    bool bad_bt;
    bool bad_bt_remember;
    bool dark_mode;
    bool rgb_backlight;
    uint32_t butthurt_timer;
    uint32_t charge_cap;
    SpiHandle spi_cc1101_handle;
    SpiHandle spi_nrf24_handle;
    FuriHalSerialId uart_esp_channel;
    FuriHalSerialId uart_nmea_channel;
    FuriHalSerialId uart_general_channel;
    bool file_naming_prefix_after;
} KaliZeroSettings;

typedef struct {
    bool is_nsfw; // TODO: replace with packs text support
    uint8_t* fonts[FontTotalNumber];
    CanvasFontParameters* font_params[FontTotalNumber];
} KaliZeroAssets;

void kalizero_settings_load();
void kalizero_settings_save();
extern KaliZeroSettings kalizero_settings;

void kalizero_assets_init();
void kalizero_assets_free();
extern KaliZeroAssets kalizero_assets;

#ifdef __cplusplus
}
#endif
