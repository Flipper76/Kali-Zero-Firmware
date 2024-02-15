#include "filesystem_api_defines.h"

const char* filesystem_api_error_get_desc(FS_Error error_id) {
    const char* result = "erreur inconnue";
    switch(error_id) {
    case(FSE_OK):
        result = "OK";
        break;
    case(FSE_NOT_READY):
        result = "filesystem not ready";
        break;
    case(FSE_EXIST):
        result = "fichier/rep existe";
        break;
    case(FSE_NOT_EXIST):
        result = "fichier/rep inexistant";
        break;
    case(FSE_INVALID_PARAMETER):
        result = "Config invalide";
        break;
    case(FSE_DENIED):
        result = "access denied";
        break;
    case(FSE_INVALID_NAME):
        result = "nom/chemin invalide";
        break;
    case(FSE_INTERNAL):
        result = "erreur interne";
        break;
    case(FSE_NOT_IMPLEMENTED):
        result = "fonction non impl.";
        break;
    case(FSE_ALREADY_OPEN):
        result = "fichier ouvert";
        break;
    }
    return result;
}

bool file_info_is_dir(const FileInfo* file_info) {
    return (file_info->flags & FSF_DIRECTORY);
}