#include "../avr_isp_app_i.h"
#include "../helpers/avr_isp_types.h"

void avr_isp_scene_about_widget_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);

    AvrIspApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void avr_isp_scene_about_on_enter(void* context) {
    furi_assert(context);

    AvrIspApp* app = context;
    FuriString* temp_str = furi_string_alloc();
    furi_string_printf(temp_str, "\e#%s\n", "Information");

    furi_string_cat_printf(temp_str, "Version: %s\n", AVR_ISP_VERSION_APP);
    furi_string_cat_printf(temp_str, "Développé par: %s\n", AVR_ISP_DEVELOPED);
    furi_string_cat_printf(temp_str, "Github: %s\n\n", AVR_ISP_GITHUB);

    furi_string_cat_printf(temp_str, "\e#%s\n", "Description");
    furi_string_cat_printf(
        temp_str,
		"Cette application est un \n"
		"programmeur intégré au \n"
		"système AVR basé sur \n"
		"stk500mk1.\n"
		"Elle est compatible avec les \n"
		"microcontrôleurs basés \n"
		"sur AVR, y compris Arduino.\n"
		"Vous pouvez également \n"
		"l'utiliser pour réparer \n"
		"la puce si vous corrompez \n"
		"accidentellement le chargeur\n"
		"de démarrage.\n\n");

    furi_string_cat_printf(temp_str, "\e#%s\n", "Ce qu'il peut faire:");
    furi_string_cat_printf(temp_str, "- Créez un dump de \nvotre puce sur une carte SD\n");
    furi_string_cat_printf(temp_str, "- Flashez le firmware de \nla puce depuis la carte SD\n");
    furi_string_cat_printf(temp_str, "- Agir en tant que ISP USB \nfilaire à l'aide du logiciel \navrdude\n\n");

    furi_string_cat_printf(temp_str, "\e#%s\n", "Série de puces prises en charge:");
    furi_string_cat_printf(
        temp_str,
        "Exemple de commande pour \nflash avrdude : \navrdude.exe -p m328p -c stk500v1 -P COMxx -U flash:r:"
        "X:\\sketch_sample.hex"
        ":i\n");
    furi_string_cat_printf(
        temp_str,
        "Où: \n"
        "-p m328p"
        " marque de votre puce, "
        "-P COMxx"
		" numéro de port com dans le système "
		" lorsque le "
		"programmeur ISP"
		" est activé\n\n");
		


    furi_string_cat_printf(temp_str, "\e#%s\n", "Info");
    furi_string_cat_printf(
        temp_str,
        "ATtinyXXXX\nATmegaXXXX\nAT43Uxxx\nAT76C711\nAT86RF401\nAT90xxxxx\nAT94K\n"
        "ATAxxxxx\nATA664251\nM3000\nLGT8F88P\nLGT8F168P\nLGT8F328P\n");

    furi_string_cat_printf(
        temp_str, "Pour une liste détaillée \ndes puces prises en charge, \nconsultez l'aide d'AVRDude\n");

    widget_add_text_box_element(
        app->widget,
        0,
        0,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!                                                      \e!\n",
        false);
    widget_add_text_box_element(
        app->widget,
        0,
        2,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!       Programmeur ISP      \e!\n",
        false);
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(app->view_dispatcher, AvrIspViewWidget);
}

bool avr_isp_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void avr_isp_scene_about_on_exit(void* context) {
    furi_assert(context);

    AvrIspApp* app = context;
    // Clear views
    widget_reset(app->widget);
}
