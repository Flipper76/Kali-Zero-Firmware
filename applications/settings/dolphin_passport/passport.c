#include "assets_icons.h"
#include "dolphin/helpers/dolphin_state.h"
#include <core/check.h>
#include <core/record.h>
#include <furi.h>
#include <gui/gui.h>
#include <furi_hal_version.h>
#include "dolphin/dolphin.h"
#include <xtreme/xtreme.h>
#include "math.h"

typedef struct {
    FuriSemaphore* semaphore;
    DolphinStats* stats;
    ViewPort* view_port;
    bool progress_total;
} PassportContext;

static void input_callback(InputEvent* input, void* _ctx) {
    PassportContext* ctx = _ctx;

    if((input->type == InputTypeShort) && (input->key == InputKeyOk)) {
        ctx->progress_total = !ctx->progress_total;
        view_port_update(ctx->view_port);
    }

    if((input->type == InputTypeShort) && (input->key == InputKeyBack)) {
        furi_semaphore_release(ctx->semaphore);
    }
}

static void render_callback(Canvas* canvas, void* _ctx) {
    PassportContext* ctx = _ctx;
    DolphinStats* stats = ctx->stats;

    char level_str[20];
    char xp_str[12];
    const char* mood_str = NULL;
    const Icon* portrait = NULL;

    if(kalizero_assets.is_nsfw) {
        if(stats->butthurt <= 4) {
            portrait = &I_passport_happy_46x49;
            mood_str = "Statut: Mouill""\xE9";
        } else if(stats->butthurt <= 9) {
            portrait = &I_passport_okay_46x49;
            mood_str = "Statut: Excit""\xE9""e";
        } else {
            portrait = &I_passport_bad_46x49;
            mood_str = "Statut: Malheureux";
        }
    } else {
        if(stats->butthurt <= 4) {
            portrait = &I_passport_happy_46x49;
            mood_str = "Humeur: Joyeux";
        } else if(stats->butthurt <= 9) {
            portrait = &I_passport_okay_46x49;
            mood_str = "Humeur: ok";
        } else {
            portrait = &I_passport_bad_46x49;
            mood_str = "Humeur: Aigri ";
        }
    }
    uint32_t xp_progress = 0;
    uint32_t xp_need = dolphin_state_xp_to_levelup(stats->icounter);
    uint32_t xp_above_last_levelup = dolphin_state_xp_above_last_levelup(stats->icounter);
    uint32_t xp_levelup = 0;
    if(ctx->progress_total) {
        xp_levelup = xp_need + stats->icounter;
    } else {
        xp_levelup = xp_need + xp_above_last_levelup;
    }
    uint32_t xp_have = xp_levelup - xp_need;

    if(stats->level == DOLPHIN_LEVEL_COUNT + 1) {
        xp_progress = 0;
    } else {
        xp_progress = xp_need * 64 / xp_levelup;
    }

    // multipass
    canvas_draw_icon(canvas, 0, 0, &I_passport_DB);

    // portrait
    furi_assert((stats->level > 0) && (stats->level <= DOLPHIN_LEVEL_COUNT + 1));
    canvas_draw_icon(canvas, 11, 2, portrait);

    const char* my_name = furi_hal_version_get_name_ptr();
    snprintf(level_str, 12, "Niveau: %hu", stats->level);
    if(stats->level == DOLPHIN_LEVEL_COUNT + 1) {
        snprintf(xp_str, 12, "Niveau Max!");
    } else {
        snprintf(xp_str, 12, "%lu/%lu", xp_have, xp_levelup);
    }
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 58, 10, my_name ? my_name : "Inconnu");
    canvas_draw_str(canvas, 58, 22, mood_str);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 58, 34, level_str);
    canvas_set_font(canvas, FontBatteryPercent);
    canvas_draw_str(canvas, 58, 42, xp_str);
    canvas_set_font(canvas, FontSecondary);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 123 - xp_progress, 45, xp_progress + (xp_progress > 0), 5);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon(canvas, 52, 51, &I_Ok_btn_9x9);
    canvas_draw_str(
        canvas, ctx->progress_total ? 37 : 36, 59, ctx->progress_total ? "Lvl" : "Tot");
}

int32_t passport_app(void* p) {
    UNUSED(p);
    FuriSemaphore* semaphore = furi_semaphore_alloc(1, 0);
    furi_assert(semaphore);

    ViewPort* view_port = view_port_alloc();

    Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
    DolphinStats stats = dolphin_stats(dolphin);
    PassportContext* ctx = malloc(sizeof(PassportContext));
    ctx->stats = &stats;
    ctx->view_port = view_port;
    ctx->semaphore = semaphore;
    ctx->progress_total = false;
    furi_record_close(RECORD_DOLPHIN);
    view_port_draw_callback_set(view_port, render_callback, ctx);
    view_port_input_callback_set(view_port, input_callback, ctx);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    view_port_update(view_port);

    furi_check(furi_semaphore_acquire(semaphore, FuriWaitForever) == FuriStatusOk);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_semaphore_free(semaphore);
    free(ctx);

    return 0;
}
