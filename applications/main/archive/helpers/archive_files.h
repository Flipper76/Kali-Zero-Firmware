#pragma once

#include <m-array.h>
#include <furi.h>
#include <m-algo.h>
#include <storage/storage.h>
#include "toolbox/path.h"
#include <xtreme/xtreme.h>

#define FAP_MANIFEST_MAX_ICON_SIZE 32

typedef enum {
    ArchiveFileTypeIButton,
    ArchiveFileTypeNFC,
    ArchiveFileTypeSubGhz,
    ArchiveFileTypeLFRFID,
    ArchiveFileTypeInfrared,
    ArchiveFileTypeSubghzPlaylist,
    ArchiveFileTypeSubghzRemote,
    ArchiveFileTypeInfraredRemote,
    ArchiveFileTypeBadKb,
    ArchiveFileTypeU2f,
    ArchiveFileTypeApplication,
    ArchiveFileTypeJS,
    ArchiveFileTypeSearch,
    ArchiveFileTypeUpdateManifest,
    ArchiveFileTypeDiskImage,	
    ArchiveFileTypeFolder,
    ArchiveFileTypeUnknown,
    ArchiveFileTypeAppOrJs,
    ArchiveFileTypeLoading,
} ArchiveFileTypeEnum;

typedef struct {
    FuriString* path;
    ArchiveFileTypeEnum type;
    uint8_t* custom_icon_data;
    FuriString* custom_name;
    bool fav;
    bool is_app;
} ArchiveFile_t;

static void ArchiveFile_t_init(ArchiveFile_t* obj) {
    obj->path = furi_string_alloc();
    obj->type = ArchiveFileTypeUnknown;
    obj->custom_icon_data = NULL;
    obj->custom_name = furi_string_alloc();
    obj->fav = false;
    obj->is_app = false;
}

static void ArchiveFile_t_init_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    obj->path = furi_string_alloc_set(src->path);
    obj->type = src->type;
    if(src->custom_icon_data) {
        obj->custom_icon_data = malloc(FAP_MANIFEST_MAX_ICON_SIZE);
        memcpy(obj->custom_icon_data, src->custom_icon_data, FAP_MANIFEST_MAX_ICON_SIZE);
    } else {
        obj->custom_icon_data = NULL;
    }
    obj->custom_name = furi_string_alloc_set(src->custom_name);
    obj->fav = src->fav;
    obj->is_app = src->is_app;
}

static void ArchiveFile_t_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    furi_string_set(obj->path, src->path);
    obj->type = src->type;
    if(src->custom_icon_data) {
        obj->custom_icon_data = malloc(FAP_MANIFEST_MAX_ICON_SIZE);
        memcpy(obj->custom_icon_data, src->custom_icon_data, FAP_MANIFEST_MAX_ICON_SIZE);
    } else {
        obj->custom_icon_data = NULL;
    }
    furi_string_set(obj->custom_name, src->custom_name);
    obj->fav = src->fav;
    obj->is_app = src->is_app;
}

static void ArchiveFile_t_clear(ArchiveFile_t* obj) {
    furi_string_free(obj->path);
    if(obj->custom_icon_data) {
        free(obj->custom_icon_data);
        obj->custom_icon_data = NULL;
    }
    furi_string_free(obj->custom_name);
}

static int ArchiveFile_t_cmp(const ArchiveFile_t* a, const ArchiveFile_t* b) {
    if(kalizero_settings.sort_dirs_first) {
        if(a->type == ArchiveFileTypeFolder && b->type != ArchiveFileTypeFolder) {
            return -1;
        }
        if(a->type != ArchiveFileTypeFolder && b->type == ArchiveFileTypeFolder) {
            return 1;
        }
    }

    return furi_string_cmpi(
        furi_string_empty(a->custom_name) ? a->path : a->custom_name,
        furi_string_empty(b->custom_name) ? b->path : b->custom_name);
}

#define M_OPL_ArchiveFile_t()                 \
    (INIT(API_2(ArchiveFile_t_init)),         \
     SET(API_6(ArchiveFile_t_set)),           \
     INIT_SET(API_6(ArchiveFile_t_init_set)), \
     CLEAR(API_2(ArchiveFile_t_clear)),       \
     CMP(API_6(ArchiveFile_t_cmp)),           \
     SWAP(M_SWAP_DEFAULT),                    \
     EQUAL(API_6(M_EQUAL_DEFAULT)))

ARRAY_DEF(files_array, ArchiveFile_t)

ALGO_DEF(files_array, ARRAY_OPLIST(files_array, M_OPL_ArchiveFile_t()))

void archive_set_file_type(ArchiveFile_t* file, const char* path, bool is_folder, bool is_app);
bool archive_get_items(void* context, const char* path);
void archive_file_append(const char* path, const char* format, ...)
    _ATTRIBUTE((__format__(__printf__, 2, 3)));
void archive_delete_file(void* context, const char* format, ...)
    _ATTRIBUTE((__format__(__printf__, 2, 3)));
FS_Error archive_copy_rename_file_or_dir(
    void* context,
    const char* src_path,
    FuriString* dst_path,
    bool copy,
    bool find_name);
