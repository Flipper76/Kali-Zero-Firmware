#include "nfc_supported_card_plugin.h"
#include <flipper_application/flipper_application.h>
#include <nfc/protocols/st25tb/st25tb.h>

#define TAG "ticketing"

const uint32_t blankBlock1 = 0x11006040, blankBlock2 = 0x24120925;
static const char* nfc_resources_header = "Flipper EMV resources";
static const uint32_t nfc_resources_file_version = 1;

static bool nfc_ticketing_parser_search_data(
    Storage* storage,
    const char* file_name,
    FuriString* key,
    FuriString* data) {
    bool parsed = false;
    FlipperFormat* file = flipper_format_file_alloc(storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    do {
        // Open file
        if(!flipper_format_file_open_existing(file, file_name)) break;
        // Read file header and version
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, nfc_resources_header) ||
           (version != nfc_resources_file_version))
            break;
        if(!flipper_format_read_string(file, furi_string_get_cstr(key), data)) break;
        parsed = true;
    } while(false);

    furi_string_free(temp_str);
    flipper_format_free(file);
    return parsed;
}

bool ticketing_get_country_name(uint16_t country_code, FuriString* country_name, FuriString* parsed_data) {
    if(!country_code) return false;

    Storage* storage = furi_record_open(RECORD_STORAGE);
	
	FuriString* key;
    key = furi_string_alloc_printf("%04X", country_code);
		FURI_LOG_D(TAG, "getCodePaysValue : %04X", country_code);
	if(nfc_ticketing_parser_search_data(storage, EXT_PATH("nfc/assets/country_code.nfc"), key, country_name)) {
		// Afficher le code de pays et le nom
		FURI_LOG_D(TAG, "Code pays : %s\n", furi_string_get_cstr(key));
		FURI_LOG_D(TAG, "Pays : %s", furi_string_get_cstr(country_name));
		furi_string_cat_printf(parsed_data, "Code pays: %s \n", furi_string_get_cstr(key));
		furi_string_cat_printf(parsed_data, "Pays: %s \n", furi_string_get_cstr(country_name));
	}
		furi_string_free(key);

    furi_record_close(RECORD_STORAGE);
    return true;
}

static void interpret_block_0(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 0");
	    furi_assert(data);
	    furi_assert(parsed_data);	
	FURI_LOG_D(TAG, "Block 0 - Byte 1: %02lX", (data->blocks[0] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 0 - Byte 2: %02lX", (data->blocks[0] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 0 - Byte 3: %02lX", (data->blocks[0] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 0 - Byte 4: %02lX", data->blocks[0] & 0xFF);
	
	FURI_LOG_D(TAG, "BLOC SÉDENTAIRE: %ld", (uint32_t)data->blocks[0]);//a tester

    // Ajouter des interprétations spécifiques pour le bloc 1
    //[BLOC SÉDENTAIRE]IDProduit, informations clés, etc. 	
}

static void interpret_block_1(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 1");
    furi_assert(data);
    furi_assert(parsed_data);

	uint32_t value_code_pays = (((data->blocks[1] >> 24) & 0xFF) << 4) | ((data->blocks[1] >> 20) & 0x0F);
	
	FuriString* str = furi_string_alloc();
	ticketing_get_country_name(value_code_pays, str, parsed_data);

    // Ajouter d'autres interprétations spécifiques pour le bloc 1 si nécessaire
    //[DISTRIBUTION DATA]  Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_2(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 2");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 2 - Byte 1: %02lX", (data->blocks[2] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 2 - Byte 2: %02lX", (data->blocks[2] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 2 - Byte 3: %02lX", (data->blocks[2] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 2 - Byte 4: %02lX", data->blocks[2] & 0xFF);		

    // Ajouter des interprétations spécifiques pour le bloc 1
    //[DISTRIBUTION DATA] Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_3(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 3");
	    furi_assert(data);
	    furi_assert(parsed_data);	
	FURI_LOG_D(TAG, "Block 3 - Byte 1: %02lX", (data->blocks[3] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 3 - Byte 2: %02lX", (data->blocks[3] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 3 - Byte 3: %02lX", (data->blocks[3] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 3 - Byte 4: %02lX", data->blocks[3] & 0xFF);

    // Ajouter des interprétations spécifiques pour le bloc 1
    //[DISTRIBUTION DATA] Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_4(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 4");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 4 - Byte 1: %02lX", (data->blocks[4] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 4 - Byte 2: %02lX", (data->blocks[4] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 4 - Byte 3: %02lX", (data->blocks[4] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 4 - Byte 4: %02lX", data->blocks[4] & 0xFF);		

    // Ajouter des interprétations spécifiques pour le bloc 1
    //[DISTRIBUTION DATA] Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_5(const St25tbData* data, FuriString* parsed_data) {
		furi_assert(data);
	    furi_assert(parsed_data);	

    // Récupérer la valeur du bloc 5 qui correspond au nombre de voyages restant
    uint32_t voyagesRestant = data->blocks[5] & 0xFF;
    uint32_t rechargementRestant = (data->blocks[5] >> 24) & 0xFF;

    // Afficher "Voyages restant"
    // FURI_LOG_D(TAG, "Voyages restant: %02lX", voyagesRestant);
    furi_string_cat_printf(parsed_data, "Voyages restant: %02lX \n", voyagesRestant);

    // Afficher "Rechargement restant"
    // FURI_LOG_D(TAG, "Rechargement restant: %02lX", rechargementRestant);
    furi_string_cat_printf(parsed_data, "Rechargement restant: %02lX \n", rechargementRestant);
	
    // Ajouter des interprétations spécifiques pour le bloc 5
    // COUNTER1(CNT1)//RELOADING1(RLD1)
}

static void interpret_block_6(const St25tbData* data, FuriString* parsed_data) {
    // FURI_LOG_D(TAG, "Interpreting Block 6");
	    furi_assert(data);
	    furi_assert(parsed_data);		
	
	// Récupérer la valeur du bloc 5 qui correspond au nombre de voyages restant
    uint32_t voyagesRestant = data->blocks[6] & 0xFF;
	uint32_t rechargementRestant = (data->blocks[6] >> 24) & 0xFF;

    // Afficher "Voyages restant"
    FURI_LOG_D(TAG, "Voyages restant: %02lX", voyagesRestant);
    // furi_string_cat_printf(parsed_data, "Voyages restant: %02lX \n", voyagesRestant);
	
	// Afficher "Rechargement restant"
    FURI_LOG_D(TAG, "Rechargement restant: %02lX", rechargementRestant);

	// Ajouter des interprétations spécifiques pour le bloc 6
    //COUNTER2(CNT2)  //RELOADING2(RLD2) 
}

static void interpret_block_7(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 7");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 7 - Byte 1: %02lX", (data->blocks[7] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 7 - Byte 2: %02lX", (data->blocks[7] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 7 - Byte 3: %02lX", (data->blocks[7] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 7 - Byte 4: %02lX", data->blocks[7] & 0xFF);		

    // Ajouter des interprétations spécifiques pour le bloc 7
    //[DISTRIBUTION DATA] Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_8(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 8");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 8 - Byte 1: %02lX", (data->blocks[8] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 8 - Byte 2: %02lX", (data->blocks[8] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 8 - Byte 3: %02lX", (data->blocks[8] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 8 - Byte 4: %02lX", data->blocks[8] & 0xFF);		

    // Ajouter des interprétations spécifiques pour le bloc 8
    //[DISTRIBUTION DATA] Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_9(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 9");
	    furi_assert(data);
	    furi_assert(parsed_data);
    FURI_LOG_D(TAG, "Block 9 - Byte 1: %02lX", (data->blocks[9] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 9 - Byte 2: %02lX", (data->blocks[9] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 9 - Byte 3: %02lX", (data->blocks[9] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 9 - Byte 4: %02lX", data->blocks[9] & 0xFF);		

    // Ajouter des interprétations spécifiques pour le bloc 9
    //[DISTRIBUTION DATA] Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_10(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 10");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 10 - Byte 1: %02lX", (data->blocks[10] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 10 - Byte 2: %02lX", (data->blocks[10] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 10 - Byte 3: %02lX", (data->blocks[10] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 10 - Byte 4: %02lX", data->blocks[10] & 0xFF);		

    // Ajouter des interprétations spécifiques pour le bloc 10
    //? CERTIFICATE // 0x0051[DISTRIBUTION DATA] Company/City, Version, End ofticket validity, Tariff, etc.
}

static void interpret_block_11(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 11");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 11 - Byte 1: %02lX", (data->blocks[11] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 11 - Byte 2: %02lX", (data->blocks[11] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 11 - Byte 3: %02lX", (data->blocks[11] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 11 - Byte 4: %02lX", data->blocks[11] & 0xFF);			

    // Ajouter des interprétations spécifiques pour le bloc 11
    // [USAGE DATA] Validation information,timestamp, etc. 
}

static void interpret_block_12(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 12");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 12 - Byte 1: %02lX", (data->blocks[12] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 12 - Byte 2: %02lX", (data->blocks[12] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 12 - Byte 3: %02lX", (data->blocks[12] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 12 - Byte 4: %02lX", data->blocks[12] & 0xFF);			

    // Ajouter des interprétations spécifiques pour le bloc 12
    // [USAGE DATA] Validation information,timestamp, etc. 
}

static void interpret_block_13(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 13");
	    furi_assert(data);
	    furi_assert(parsed_data);	
    FURI_LOG_D(TAG, "Block 13 - Byte 1: %02lX", (data->blocks[13] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 13 - Byte 2: %02lX", (data->blocks[13] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 13 - Byte 3: %02lX", (data->blocks[13] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 13 - Byte 4: %02lX", data->blocks[13] & 0xFF);			

    // Ajouter des interprétations spécifiques pour le bloc 13
    // [USAGE DATA] Validation information,timestamp, etc. 
}

static void interpret_block_14(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 14");
		furi_assert(data);
	    furi_assert(parsed_data);
    FURI_LOG_D(TAG, "Block 14 - Byte 1: %02lX", (data->blocks[14] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 14 - Byte 2: %02lX", (data->blocks[14] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 14 - Byte 3: %02lX", (data->blocks[14] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 14 - Byte 4: %02lX", data->blocks[14] & 0xFF);	

    // Ajouter des interprétations spécifiques pour le bloc 14
    // DATA CERTIFICATE//[USAGE DATA] Validation information,timestamp, etc.?
}

static void interpret_block_15(const St25tbData* data, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "Interpreting Block 15");
	    furi_assert(data);
	    furi_assert(parsed_data);
	FURI_LOG_D(TAG, "Block 15 - Byte 1: %02lX", (data->blocks[15] >> 24) & 0xFF);
    FURI_LOG_D(TAG, "Block 15 - Byte 2: %02lX", (data->blocks[15] >> 16) & 0xFF);
    FURI_LOG_D(TAG, "Block 15 - Byte 3: %02lX", (data->blocks[15] >> 8) & 0xFF);
    FURI_LOG_D(TAG, "Block 15 - Byte 4: %02lX", data->blocks[15] & 0xFF);	

    // Ajouter des interprétations spécifiques pour le bloc 15
    //DISTRIBUTION CERTIFICATE
}

static bool ticketing_parse(const NfcDevice* device, FuriString* parsed_data) {
    FURI_LOG_D(TAG, "ticketing_parse called");
    furi_assert(device);
    furi_assert(parsed_data);

    const St25tbData* data = nfc_device_get_data(device, NfcProtocolSt25tb);

    FURI_LOG_D(TAG, "Detected card type: %d", data->type);

    const uint8_t ticketing_uid_prefix[] = {0xD0, 0x02, 0x33};

    if (memcmp(data->uid, ticketing_uid_prefix, sizeof(ticketing_uid_prefix)) == 0) {
        furi_string_cat_printf(parsed_data, "\e#%s\n", "Carte ticketing");
		furi_string_cat_printf(parsed_data, "UID:%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X\n",
                   data->uid[0], data->uid[1], data->uid[2], data->uid[3], data->uid[4], data->uid[5], data->uid[6], data->uid[7]);			   
    } else {
        FURI_LOG_D(TAG, "Bad card type detected: %d", data->type);
        return false;
    }

    int badBlocksCount = 0;

    if (badBlocksCount > 0) {
        FURI_LOG_D(TAG, "Total bad otp blocks: %d", badBlocksCount);
        return false;
    }

	uint8_t blockCount = st25tb_get_block_count(data->type);

    // Afficher les valeurs de chaque bloc
    for (int i = 0; i < blockCount; i++) {
        FURI_LOG_D(TAG, "Block %d value: %08lX", i, (unsigned long)data->blocks[i]);
		FURI_LOG_D(TAG, "Block %d valueuint32_t: %ld", i, (uint32_t)data->blocks[i]);
		if (i == 255 && blockCount >= 255 && data->blocks[i] != 0xFFFFFFFF) {
            FURI_LOG_D(TAG, "Bad otp block %d", i);
        }

        // Ajouter du code pour interpréter les données de chaque bloc ici
        switch (i) {
            case 0:
                interpret_block_0(data, parsed_data);
                break;
            case 1:
                interpret_block_1(data, parsed_data);
                break;
			case 2:
                interpret_block_2(data, parsed_data);
                break;
			case 3:
                interpret_block_3(data, parsed_data);
                break;
			case 4:
                interpret_block_4(data, parsed_data);
                break;
			case 5:
				interpret_block_5(data, parsed_data);
                break;
			case 6:
                interpret_block_6(data, parsed_data);
                break;
			case 7:
                interpret_block_7(data, parsed_data);
                break;
			case 8:
                interpret_block_8(data, parsed_data);
                break;
			case 9:
                interpret_block_9(data, parsed_data);
                break;
			case 10:
                interpret_block_10(data, parsed_data);
                break;
			case 11:
                interpret_block_11(data, parsed_data);
                break;
			case 12:
                interpret_block_12(data, parsed_data);
                break;
			case 13:
                interpret_block_13(data, parsed_data);
                break;
			case 14:
                interpret_block_14(data, parsed_data);
                break;	
			case 15:
                interpret_block_15(data, parsed_data);
                break;	
            default:
                break;
        }
    }
	
	if (data->system_otp_block != 0xFFFFFFFF) {
        FURI_LOG_D(TAG, "bad sys otp block");
        return false;
    }

    return true;
}

static const NfcSupportedCardsPlugin ticketing_plugin = {
    .protocol = NfcProtocolSt25tb,
    .verify = NULL,
    .read = NULL,
    .parse = ticketing_parse,
};

static const FlipperAppPluginDescriptor ticketing_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &ticketing_plugin,
};

const FlipperAppPluginDescriptor* ticketing_plugin_ep() {
    FURI_LOG_D(TAG, "ticketing_plugin_ep called");
    return &ticketing_plugin_descriptor;
}
