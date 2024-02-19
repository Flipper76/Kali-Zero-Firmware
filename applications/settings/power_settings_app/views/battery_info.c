#include "battery_info.h"
#include <furi.h>
#include <gui/elements.h>
#include <assets_icons.h>
#include <locale/locale.h>

#define LOW_CHARGE_THRESHOLD (10)
#define HIGH_DRAIN_CURRENT_THRESHOLD (-100)

static void draw_stat(Canvas* canvas, int x, int y, const Icon* icon, char* val) {
    canvas_draw_frame(canvas, x - 7, y + 7, 30, 13);
    canvas_draw_icon(canvas, x, y, icon);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x - 4, y + 16, 24, 6);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, x + 8, y + 22, AlignCenter, AlignBottom, val);
}

static void draw_battery(Canvas* canvas, BatteryInfoModel* data, int x, int y) {
    char emote[20] = {};
    char header[20] = {};
    char value[20] = {};

    int32_t current = 1000.0f * data->gauge_current;

    // Draw battery
    canvas_draw_icon(canvas, x, y, &I_BatteryBody_52x28);
    if(current > 0) {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceCharging_29x14);
    } else if(current < HIGH_DRAIN_CURRENT_THRESHOLD) {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceConfused_29x14);
    } else if(data->charge < LOW_CHARGE_THRESHOLD) {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceNopower_29x14);
    } else {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceNormal_29x14);
    }

    // Draw bubble
    elements_bubble(canvas, 53, 0, 71, data->alt ? 28 : 39);

    // Set text
    if(current > 0) {
        snprintf(emote, sizeof(emote), "%s", "Délicieux!");
        snprintf(header, sizeof(header), "%s", "Recharge à");
        snprintf(
            value,
            sizeof(value),
            "%lu.%luV   %lumA",
            (uint32_t)(data->vbus_voltage),
            (uint32_t)(data->vbus_voltage * 10) % 10,
            current);
    } else if(current < -5) {
        // 0-5ma deadband
        snprintf(
            emote,
            sizeof(emote),
            "%s",
            current < HIGH_DRAIN_CURRENT_THRESHOLD ? "Oh non!" : "Om-nom-nom!");
        snprintf(header, sizeof(header), "%s", "La consommation est");
        snprintf(
            value,
            sizeof(value),
            "%ld %s",
            ABS(current),
            current < HIGH_DRAIN_CURRENT_THRESHOLD ? "mA!" : "mA");
    } else if(data->vbus_voltage > 0) {
        if(data->charge_voltage_limit < 4.2) {
            // Non-default battery charging limit, mention it
            snprintf(emote, sizeof(emote), "Chargée!");
            snprintf(header, sizeof(header), "Limité à");
            snprintf(
                value,
                sizeof(value),
                "%lu.%luV",
                (uint32_t)(data->charge_voltage_limit),
                (uint32_t)(data->charge_voltage_limit * 10) % 10);
        } else {
            snprintf(header, sizeof(header), "Chargée!");
        }
    } else {
        snprintf(header, sizeof(header), "Dormance...");
        snprintf(value, sizeof(value), "(~%ld mA)", ABS(current));
    }

    if(data->alt) {
        if(!strcmp(value, "")) {
            canvas_draw_str_aligned(canvas, x + 92, y + 14, AlignCenter, AlignCenter, header);
        } else if(!strcmp(header, "")) {
            canvas_draw_str_aligned(canvas, x + 92, y + 14, AlignCenter, AlignCenter, value);
        } else {
            canvas_draw_str_aligned(canvas, x + 92, y + 9, AlignCenter, AlignCenter, header);
            canvas_draw_str_aligned(canvas, x + 92, y + 19, AlignCenter, AlignCenter, value);
        }
    } else {
        if(!strcmp(emote, "")) {
            canvas_draw_str_aligned(canvas, x + 92, y + 9, AlignCenter, AlignCenter, header);
            canvas_draw_str_aligned(canvas, x + 92, y + 21, AlignCenter, AlignCenter, value);
        } else {
            canvas_draw_str_aligned(canvas, 92, y + 3, AlignCenter, AlignCenter, emote);
            canvas_draw_str_aligned(canvas, 92, y + 15, AlignCenter, AlignCenter, header);
            canvas_draw_str_aligned(canvas, 92, y + 27, AlignCenter, AlignCenter, value);
        }
    }
}

static void battery_info_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    BatteryInfoModel* model = context;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    draw_battery(canvas, model, 0, model->alt ? 0 : 5);

    char batt_level[10];
    char temperature[10];
    char voltage[10];
    char health[10];

    snprintf(batt_level, sizeof(batt_level), "%lu%%", (uint32_t)model->charge);
    if(locale_get_measurement_unit() == LocaleMeasurementUnitsMetric) {
        snprintf(temperature, sizeof(temperature), "%lu C", (uint32_t)model->gauge_temperature);
    } else {
        snprintf(
            temperature,
            sizeof(temperature),
            "%lu F",
            (uint32_t)locale_celsius_to_fahrenheit(model->gauge_temperature));
    }
    snprintf(
        voltage,
        sizeof(voltage),
        "%lu.%01lu V",
        (uint32_t)model->gauge_voltage,
        (uint32_t)(model->gauge_voltage * 10) % 10UL);
    snprintf(health, sizeof(health), "%d%%", model->health);

    int h = model->alt ? 28 : 42;
    draw_stat(canvas, 8, h, &I_Battery_16x16, batt_level);
    draw_stat(canvas, 40, h, &I_Temperature_16x16, temperature);
    draw_stat(canvas, 72, h, &I_Voltage_16x16, voltage);
    draw_stat(canvas, 104, h, &I_Health_16x16, health);

    if(model->alt) {
        elements_button_left(canvas, "Retour");
        elements_button_right(canvas, "Suivant");
        char uptime[15];
        uint32_t sec = furi_get_tick() / furi_kernel_get_tick_frequency();
        snprintf(
            uptime, sizeof(uptime), "%02luh%02lum%02lus", sec / 3600, sec / 60 % 60, sec % 60);
        canvas_draw_str_aligned(canvas, 64, 61, AlignCenter, AlignBottom, uptime);
    }
}

static bool battery_info_input_callback(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    BatteryInfo* battery_info = context;

    bool about_battery;
    with_view_model(
        battery_info->view, BatteryInfoModel * model, { about_battery = model->alt; }, false);
    if(about_battery && event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            event->key = InputKeyBack;
        } else if(event->key == InputKeyRight) {
            event->key = InputKeyBack;
            battery_info->exit_to_about = true;
        }
    }

    return false;
}

BatteryInfo* battery_info_alloc() {
    BatteryInfo* battery_info = malloc(sizeof(BatteryInfo));
    battery_info->view = view_alloc();
    view_set_context(battery_info->view, battery_info);
    view_allocate_model(battery_info->view, ViewModelTypeLocking, sizeof(BatteryInfoModel));
    view_set_draw_callback(battery_info->view, battery_info_draw_callback);
    view_set_input_callback(battery_info->view, battery_info_input_callback);

    return battery_info;
}

void battery_info_free(BatteryInfo* battery_info) {
    furi_assert(battery_info);
    view_free(battery_info->view);
    free(battery_info);
}

View* battery_info_get_view(BatteryInfo* battery_info) {
    furi_assert(battery_info);
    return battery_info->view;
}

void battery_info_set_data(BatteryInfo* battery_info, BatteryInfoModel* data) {
    furi_assert(battery_info);
    furi_assert(data);
    with_view_model(
        battery_info->view,
        BatteryInfoModel * model,
        { memcpy(model, data, sizeof(BatteryInfoModel)); },
        true);
}
