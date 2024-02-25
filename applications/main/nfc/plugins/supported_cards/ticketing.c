#include "nfc_supported_card_plugin.h"
#include <flipper_application/flipper_application.h>
#include <nfc/protocols/st25tb/st25tb.h>

#define TAG "ticketing"

static const char* nfc_resources_header = "Flipper EMV resources";
static const uint32_t nfc_resources_file_version = 1;

FuriString* blocSedent;
FuriString* blocSedentHex;

FuriString* codePays;
FuriString* pays;

FuriString* voyagesRestant;
FuriString* rechargementRestant;

FuriString* voyagesRestant2;
FuriString* rechargementRestant2;

FuriString* certif;
FuriString* certifHex;

FuriString* certifData;
FuriString* certifDataHex;

FuriString* certifDistr;
FuriString* certifDistrHex;

FuriString* distributionDataBloc1;
FuriString* distributionDataBloc1Hex;

FuriString* distributionDataBloc2;
FuriString* distributionDataBloc2Hex;

FuriString* distributionDataBloc3;
FuriString* distributionDataBloc3Hex;

FuriString* distributionDataBloc4;
FuriString* distributionDataBloc4Hex;

FuriString* distributionDataBloc7;
FuriString* distributionDataBloc7Hex;

FuriString* distributionDataBloc8;
FuriString* distributionDataBloc8Hex;

FuriString* distributionDataBloc9;
FuriString* distributionDataBloc9Hex;

FuriString* distributionDataBloc10;
FuriString* distributionDataBloc10Hex;

FuriString* usageDataBloc11;
FuriString* usageDataBloc11Hex;

FuriString* usageDataBloc12;
FuriString* usageDataBloc12Hex;

FuriString* usageDataBloc13;
FuriString* usageDataBloc13Hex;

FuriString* usageDataBloc14;
FuriString* usageDataBloc14Hex;


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
        // Ouvrir le fichier
        if(!flipper_format_file_open_existing(file, file_name)) break;
        // Lire l'en-tête et la version du fichier
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

bool ticketing_get_country_name(uint16_t country_code, FuriString* country_name) {
    if(!country_code) return false;

    Storage* storage = furi_record_open(RECORD_STORAGE);
	
	FuriString* key;
    key = furi_string_alloc_printf("%04X", country_code);
		FURI_LOG_D(TAG, "getCodePaysValue : %04X", country_code);
	if(nfc_ticketing_parser_search_data(storage, EXT_PATH("nfc/assets/country_code.nfc"), key, country_name)) {
		// Afficher le code de pays et le nom
		FURI_LOG_D(TAG, "Code pays : %s\n", furi_string_get_cstr(key));
		FURI_LOG_D(TAG, "Pays : %s", furi_string_get_cstr(country_name));
		
		codePays = furi_string_alloc_printf("%s", furi_string_get_cstr(key));
		pays = furi_string_alloc_printf("%s", furi_string_get_cstr(country_name));

	}
	furi_string_free(key);

    furi_record_close(RECORD_STORAGE);
    return true;
}

static void interpret_block_0(const St25tbData* data) {
	
	blocSedent = furi_string_alloc_printf("%08lX", data->blocks[0]);
	blocSedentHex = furi_string_alloc_printf("%lu", data->blocks[0]);
	
	FURI_LOG_D(TAG, "BLOC SÉDENTAIRE: %ld", (uint32_t)data->blocks[0]);
}

static void interpret_block_1(const St25tbData* data) {
    FURI_LOG_D(TAG, "Interpreting Block 1");
    furi_assert(data);

	uint32_t value_code_pays = (((data->blocks[1] >> 24) & 0xFF) << 4) | ((data->blocks[1] >> 20) & 0x0F);
	
	FuriString* str = furi_string_alloc();
	ticketing_get_country_name(value_code_pays, str);
	
	uint8_t byte2 = (data->blocks[1] >> 16) & 0xFF;
	uint8_t byte3 = (data->blocks[1] >> 8) & 0xFF;
	uint8_t byte4 = data->blocks[1] & 0xFF;

	uint32_t value_distributionData = (byte2 << 16) | (byte3 << 8) | byte4;

	distributionDataBloc1 = furi_string_alloc_printf("%ld", value_distributionData);
	distributionDataBloc1Hex = furi_string_alloc_printf("%05lX", value_distributionData);
}

static void interpret_block_2(const St25tbData* data) {
	furi_assert(data);

	distributionDataBloc2 = furi_string_alloc_printf("%ld", data->blocks[2]);
	distributionDataBloc2Hex = furi_string_alloc_printf("%08lX", data->blocks[2]);
}

static void interpret_block_3(const St25tbData* data) {
	furi_assert(data);

	distributionDataBloc3 = furi_string_alloc_printf("%ld", data->blocks[3]);
	distributionDataBloc3Hex = furi_string_alloc_printf("%08lX", data->blocks[3]);
}

static void interpret_block_4(const St25tbData* data) {
	furi_assert(data);
		
	distributionDataBloc4 = furi_string_alloc_printf("%ld", data->blocks[4]);
	distributionDataBloc4Hex = furi_string_alloc_printf("%08lX", data->blocks[4]);	
}

static void interpret_block_5(const St25tbData* data) {
	furi_assert(data);

	voyagesRestant = furi_string_alloc_printf("%ld", data->blocks[5] & 0xFF);
	rechargementRestant = furi_string_alloc_printf("%ld", (data->blocks[5] >> 24) & 0xFF);
}

static void interpret_block_6(const St25tbData* data) {
	furi_assert(data);

	voyagesRestant2 = furi_string_alloc_printf("%02lX", data->blocks[6] & 0xFF);
	rechargementRestant2 = furi_string_alloc_printf("%02lX", (data->blocks[6] >> 24) & 0xFF);
}

static void interpret_block_7(const St25tbData* data) {
    furi_assert(data);

	distributionDataBloc7 = furi_string_alloc_printf("%ld", data->blocks[7]);
	distributionDataBloc7Hex = furi_string_alloc_printf("%08lX", data->blocks[7]);	
}

static void interpret_block_8(const St25tbData* data) {
    furi_assert(data);
	
	distributionDataBloc8 = furi_string_alloc_printf("%ld", data->blocks[8]);
	distributionDataBloc8Hex = furi_string_alloc_printf("%08lX", data->blocks[8]);	
}

static void interpret_block_9(const St25tbData* data) {
	furi_assert(data);
	
	distributionDataBloc9 = furi_string_alloc_printf("%ld", data->blocks[9]);
	distributionDataBloc9Hex = furi_string_alloc_printf("%08lX", data->blocks[9]);	
}

static void interpret_block_10(const St25tbData* data) {
	furi_assert(data);
		
	uint8_t byte3 = (data->blocks[10] >> 8) & 0xFF;
	uint8_t byte4 = data->blocks[10] & 0xFF;

	uint32_t value_certif = (byte3 << 8) | byte4;
	
	certif = furi_string_alloc_printf("%ld", value_certif);
	certifHex = furi_string_alloc_printf("%04lX", value_certif);
	
	uint8_t byte1 = (data->blocks[10] >> 24) & 0xFF; 
	uint8_t byte2 = (data->blocks[10] >> 16) & 0xFF; 

	uint32_t value_distributionData = (byte1 << 8) | byte2;
	
	distributionDataBloc10 = furi_string_alloc_printf("%ld", value_distributionData);
	distributionDataBloc10Hex = furi_string_alloc_printf("%04lX", value_distributionData);	
}

static void interpret_block_11(const St25tbData* data) {
	furi_assert(data);

	usageDataBloc11 = furi_string_alloc_printf("%ld", data->blocks[11]);
	usageDataBloc11Hex = furi_string_alloc_printf("%08lX", data->blocks[11]);	
}

static void interpret_block_12(const St25tbData* data) {
	furi_assert(data);

	usageDataBloc12 = furi_string_alloc_printf("%ld", data->blocks[12]);
	usageDataBloc12Hex = furi_string_alloc_printf("%08lX", data->blocks[12]);		
}

static void interpret_block_13(const St25tbData* data) {
    furi_assert(data);

	usageDataBloc13 = furi_string_alloc_printf("%ld", data->blocks[13]);
	usageDataBloc13Hex = furi_string_alloc_printf("%08lX", data->blocks[13]);	
}

static void interpret_block_14(const St25tbData* data) {
	furi_assert(data);

	uint8_t byte3 = (data->blocks[14] >> 8) & 0xFF;
	uint8_t byte4 = data->blocks[14] & 0xFF;

	uint32_t value_certif = (byte3 << 8) | byte4;
	
	certifData = furi_string_alloc_printf("%ld", value_certif);
	certifDataHex = furi_string_alloc_printf("%04lX", value_certif);

	FURI_LOG_D(TAG, "CERTIFICAT DE DISTRIBUTION: %ld", data->blocks[15]);
	
	uint8_t byte1 = (data->blocks[10] >> 24) & 0xFF;
	uint8_t byte2 = (data->blocks[10] >> 16) & 0xFF;
	uint32_t value_usageData = (byte1 << 8) | byte2;
	
	usageDataBloc14 = furi_string_alloc_printf("%ld", value_usageData);
	usageDataBloc14Hex = furi_string_alloc_printf("%04lX", value_usageData);	
}

static void interpret_block_15(const St25tbData* data) {
    furi_assert(data);

	certifDistr = furi_string_alloc_printf("%ld", data->blocks[15]);
	certifDistrHex = furi_string_alloc_printf("%08lX", data->blocks[15]);
}

static bool ticketing_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);

    const St25tbData* data = nfc_device_get_data(device, NfcProtocolSt25tb);

    const uint8_t ticketing_uid_prefix[] = {0xD0, 0x02, 0x33};

    if (memcmp(data->uid, ticketing_uid_prefix, sizeof(ticketing_uid_prefix)) == 0) {
        furi_string_cat_printf(parsed_data, "\e#%s\n", "Carte ticketing");
		furi_string_cat_printf(parsed_data, "%s\n", "UID");		
		furi_string_cat_printf(parsed_data, "%02X %02X %02X %02X %02X %02X %02X %02X\n",
                   data->uid[0], data->uid[1], data->uid[2], data->uid[3], data->uid[4], data->uid[5], data->uid[6], data->uid[7]);			   
    } else {
        FURI_LOG_D(TAG, "Mauvais type de carte détecté: %d", data->type);
        return false;
    }
	
	if (data->system_otp_block != 0xFFFFFFFF) {
        FURI_LOG_D(TAG, "mauvais bloc OTP du système");
        return false;
    }

	uint8_t blockCount = st25tb_get_block_count(data->type);

    for (int i = 0; i < blockCount; i++) {
		if (i == 255 && blockCount >= 255 && data->blocks[i] != 0xFFFFFFFF) {
            FURI_LOG_D(TAG, "Mauvais bloc OTP %d", i);
			return false;
        }
        switch (i) {
            case 0:
                interpret_block_0(data);
                break;
            case 1:
                interpret_block_1(data);
                break;
			case 2:
                interpret_block_2(data);
                break;
			case 3:
                interpret_block_3(data);
                break;
			case 4:
                interpret_block_4(data);
                break;
			case 5:
				interpret_block_5(data);
                break;
			case 6:
                interpret_block_6(data);
                break;
			case 7:
                interpret_block_7(data);
                break;
			case 8:
                interpret_block_8(data);
                break;
			case 9:
                interpret_block_9(data);
                break;
			case 10:
                interpret_block_10(data);
                break;
			case 11:
                interpret_block_11(data);
                break;
			case 12:
                interpret_block_12(data);
                break;
			case 13:
                interpret_block_13(data);
                break;
			case 14:
                interpret_block_14(data);
                break;	
			case 15:
                interpret_block_15(data);
                break;	
            default:
                break;
        }
    }
	
	furi_string_cat_printf(parsed_data, "Voyages restant: %s \n", furi_string_get_cstr(voyagesRestant));
    furi_string_cat_printf(parsed_data, "Rechargement restant: %s \n\n", furi_string_get_cstr(rechargementRestant));

	furi_string_cat_printf(parsed_data, "%s\n", "::::::::::::::AUTRES INFOS:::::::::::::::");		
	furi_string_cat_printf(parsed_data, "Code pays: %s \n", furi_string_get_cstr(codePays));
	furi_string_cat_printf(parsed_data, "Pays: %s \n", furi_string_get_cstr(pays));

	furi_string_cat_printf(parsed_data, "Voyages restant2: %s \n", furi_string_get_cstr(voyagesRestant2));
    furi_string_cat_printf(parsed_data, "Rechargement restant2: %s \n", furi_string_get_cstr(rechargementRestant2));

	furi_string_cat_printf(parsed_data, "%s\n", ":::::BLOC SÉDENTAIRE HEX:::::");	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(blocSedent));
	
	furi_string_cat_printf(parsed_data, "%s\n", "::::::::::BLOC SÉDENTAIRE::::::::::");		
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(blocSedentHex));

	furi_string_cat_printf(parsed_data, "%s\n", "::::::CERTIF. DONNÉES HEX::::::");	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(certifDataHex));

	furi_string_cat_printf(parsed_data, "%s\n", "::::::CERTIFICAT DONNÉES::::::");		
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(certifData));

	furi_string_cat_printf(parsed_data, "%s\n", ":::::::::::CERTIFICAT HEX:::::::::::::");	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(certifHex));
	
	furi_string_cat_printf(parsed_data, "%s\n", ":::::::::::::::::CERTIFICAT:::::::::::::::::");		
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(certif));

	furi_string_cat_printf(parsed_data, "%s\n", ":::::::CERTIF. DISTRIB HEX:::::::");	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(certifDistrHex));
	
	furi_string_cat_printf(parsed_data, "%s\n", ":::::::CERTIFICAT DISTRIB:::::::");		
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(certifDistr)); 
	
	furi_string_cat_printf(parsed_data, "%s\n", "::::::DONNÉES DISTRIB HEX:::::");	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc1Hex));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc2Hex));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc3Hex));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc4Hex));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc7Hex));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc8Hex));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc9Hex));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc10Hex));	
	
	furi_string_cat_printf(parsed_data, "%s\n", ":::::::::::DONNÉES DISTRIB::::::::::");		
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc1));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc2));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc3));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc4));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc7));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc8));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc9));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(distributionDataBloc10));	
	
	furi_string_cat_printf(parsed_data, "%s\n", "::::::::::DONNÉES UTI. HEX:::::::::::");
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc11Hex));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc12Hex));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc13Hex));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc14Hex));

	furi_string_cat_printf(parsed_data, "%s\n", ":::::DONNÉES UTILISATION:::::");	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc11));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc12));	
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc13));
	furi_string_cat_printf(parsed_data, "%s \n", furi_string_get_cstr(usageDataBloc14));			

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
