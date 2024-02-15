#include "../kali_zero_app.h"

enum VarItemListIndex {
    VarItemListIndexStaticFrequency,
    VarItemListIndexRemoveStaticFreq,
    VarItemListIndexAddStaticFreq,
};

void kali_zero_app_scene_protocols_freqs_static_var_item_list_callback(void* context, uint32_t index) {
    KaliZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void kali_zero_app_scene_protocols_freqs_static_frequency_changed(VariableItem* item) {
    KaliZeroApp* app = variable_item_get_context(item);
    app->subghz_static_index = variable_item_get_current_value_index(item);
    uint32_t value = *FrequencyList_get(app->subghz_static_freqs, app->subghz_static_index);
    char text[10] = {0};
    snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
    variable_item_set_current_value_text(item, text);
}

void kali_zero_app_scene_protocols_freqs_static_on_enter(void* context) {
    KaliZeroApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Frequence statique",
        FrequencyList_size(app->subghz_static_freqs),
        kali_zero_app_scene_protocols_freqs_static_frequency_changed,
        app);
    app->subghz_static_index = 0;
    variable_item_set_current_value_index(item, app->subghz_static_index);
    if(FrequencyList_size(app->subghz_static_freqs)) {
        uint32_t value = *FrequencyList_get(app->subghz_static_freqs, app->subghz_static_index);
        char text[10] = {0};
        snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
        variable_item_set_current_value_text(item, text);
    } else {
        variable_item_set_current_value_text(item, "Aucune");
    }

    variable_item_list_add(var_item_list, "Supprimer une fr""\xE9""quence statique", 0, NULL, app);

    variable_item_list_add(var_item_list, "Ajouter une fr""\xE9""quence statique", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, kali_zero_app_scene_protocols_freqs_static_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, KaliZeroAppSceneProtocolsFreqsStatic));

    view_dispatcher_switch_to_view(app->view_dispatcher, KaliZeroAppViewVarItemList);
}

bool kali_zero_app_scene_protocols_freqs_static_on_event(void* context, SceneManagerEvent event) {
    KaliZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, KaliZeroAppSceneProtocolsFreqsStatic, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexRemoveStaticFreq:
            if(!FrequencyList_size(app->subghz_static_freqs)) break;
            uint32_t value =
                *FrequencyList_get(app->subghz_static_freqs, app->subghz_static_index);
            FrequencyList_it_t it;
            FrequencyList_it(it, app->subghz_static_freqs);
            size_t removed = 0;
            while(!FrequencyList_end_p(it)) {
                if(*FrequencyList_ref(it) == value) {
                    FrequencyList_remove(app->subghz_static_freqs, it);
                    removed++;
                } else {
                    FrequencyList_next(it);
                }
            }
            app->save_subghz_freqs = true;
            VariableItem* item =
                variable_item_list_get(app->var_item_list, VarItemListIndexStaticFrequency);
            variable_item_set_values_count(item, FrequencyList_size(app->subghz_static_freqs));
            if(FrequencyList_size(app->subghz_static_freqs)) {
                app->subghz_static_index -= removed;
                uint32_t value =
                    *FrequencyList_get(app->subghz_static_freqs, app->subghz_static_index);
                char text[10] = {0};
                snprintf(
                    text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
                variable_item_set_current_value_text(item, text);
            } else {
                app->subghz_static_index = 0;
                variable_item_set_current_value_text(item, "Aucune");
            }
            variable_item_set_current_value_index(item, app->subghz_static_index);
            break;
        case VarItemListIndexAddStaticFreq:
            scene_manager_set_scene_state(
                app->scene_manager, KaliZeroAppSceneProtocolsFreqsAdd, false);
            scene_manager_next_scene(app->scene_manager, KaliZeroAppSceneProtocolsFreqsAdd);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void kali_zero_app_scene_protocols_freqs_static_on_exit(void* context) {
    KaliZeroApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
