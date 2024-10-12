#include <furi.h>
#include <storage/storage.h>
#include "main.h"
#include "mtp.h"
#include "usb_desc.h"
#include "utils.h"

#define BLOCK_SIZE 65536

// Supported operations (example, add as needed)
uint16_t supported_operations[] = {
    MTP_OP_GET_DEVICE_INFO,
    MTP_OP_OPEN_SESSION,
    MTP_OP_CLOSE_SESSION,
    MTP_OP_GET_STORAGE_IDS,
    MTP_OP_GET_STORAGE_INFO,
    MTP_OP_GET_NUM_OBJECTS,
    MTP_OP_GET_OBJECT_HANDLES,
    MTP_OP_GET_OBJECT_INFO,
    MTP_OP_GET_OBJECT,
    MTP_OP_SEND_OBJECT_INFO,
    MTP_OP_SEND_OBJECT,
    MTP_OP_DELETE_OBJECT,
    MTP_OP_GET_DEVICE_PROP_DESC,
    MTP_OP_GET_DEVICE_PROP_VALUE,
    MTP_OP_GET_OBJECT_PROPS_SUPPORTED,
    MTP_OP_SET_OBJECT_PROP_VALUE};

uint16_t supported_object_props[] = {
    MTP_PROP_STORAGE_ID,
    MTP_PROP_OBJECT_FORMAT,
    MTP_PROP_OBJECT_FILE_NAME,
};

// Supported device properties (example, add as needed)
uint16_t supported_device_properties[] = {
    0xd402, // Device friendly name
};

uint16_t supported_playback_formats[] = {
    MTP_FORMAT_UNDEFINED,
    MTP_FORMAT_ASSOCIATION,
};

void merge_path(char* target, char* base, char* name) {
    // implement this way since strcat is unavailable

    char* ptr = target;
    strcpy(target, base);
    ptr += strlen(base);
    strcpy(ptr, "/");
    ptr++;
    strcpy(ptr, name);
}

// temporary storage for buffer
MTPDataPersistence persistence;

void mtp_handle_bulk(AppMTP* mtp, uint8_t* buffer, uint32_t length) {
    UNUSED(mtp);
    if(persistence.left_bytes > 0) {
        FURI_LOG_I("MTP", "Left bytes: %ld", persistence.left_bytes);
        handle_mtp_data_packet(mtp, buffer, length, 1);
        return;
    }

    if(length < 12) {
        FURI_LOG_E("MTP", "Invalid MTP packet");
        return;
    }

    struct MTPHeader* header = (struct MTPHeader*)buffer;
    uint16_t type = header->type;

    if(type == MTP_TYPE_COMMAND) {
        struct MTPContainer* container = (struct MTPContainer*)buffer;
        handle_mtp_command(mtp, container);
    } else if(type == MTP_TYPE_DATA) {
        if(header->len > length) {
            persistence.left_bytes = header->len;
        }
        handle_mtp_data_packet(mtp, buffer, length, 0);
    } else if(type == MTP_TYPE_RESPONSE) {
        handle_mtp_response(mtp, header);
    } else {
        FURI_LOG_W("MTP", "Unsupported MTP packet type: %d", type);
    }
}

void setup_persistence(struct MTPContainer* container) {
    persistence.transaction_id = container->header.transaction_id;
    memcpy(persistence.params, container->params, sizeof(uint32_t) * 5);
}

void handle_mtp_data_packet(AppMTP* mtp, uint8_t* buffer, int32_t length, bool cont) {
    UNUSED(mtp);
    uint8_t* ptr = buffer;
    if(!cont) {
        struct MTPHeader* header = (struct MTPHeader*)buffer;

        if(header->transaction_id != persistence.transaction_id) {
            // the params value can not be trusted.
            // reset the params
            for(int i = 0; i < 5; i++) {
                persistence.params[i] = 0;
            }
        }

        persistence.global_buffer = NULL;
        uint32_t transaction_id = header->transaction_id;
        persistence.op = header->op;

        FURI_LOG_I("MTP", "Handling MTP data: 0x%04x", header->op);
        FURI_LOG_I("MTP", "Transaction ID: %ld", transaction_id);
        FURI_LOG_I("MTP", "Length: %ld", length);

        ptr = (uint8_t*)buffer + sizeof(struct MTPHeader);

        switch(header->op) {
        case MTP_OP_SEND_OBJECT_INFO:
        case MTP_OP_SET_OBJECT_PROP_VALUE: {
            persistence.global_buffer = malloc(sizeof(uint8_t) * 256);
            persistence.buffer_offset = 0;
            break;
        }
        case MTP_OP_SEND_OBJECT: {
            persistence.buffer_offset = 0;
            persistence.left_bytes = header->len - sizeof(struct MTPHeader);
        }
        }
    }

    // for speicific operations.
    if(persistence.op == MTP_OP_SEND_OBJECT) {
        uint32_t handle = persistence.prev_handle;
        if(handle == 0) {
            FURI_LOG_E("MTP", "Invalid handle for MTP_OP_SEND_OBJECT");
            send_mtp_response(
                mtp, 3, MTP_RESP_INVALID_OBJECT_HANDLE, persistence.transaction_id, NULL);
            return;
        }

        char* path = get_path_from_handle(mtp, handle);
        int bytes_length = length - (ptr - buffer);
        int offset = persistence.buffer_offset;

        FURI_LOG_I("MTP", "Writing to file: %s with offset %02x", path, offset);
        if(persistence.current_file == NULL) {
            persistence.current_file = storage_file_alloc(mtp->storage);
            if(!storage_file_open(persistence.current_file, path, FSAM_WRITE, FSOM_OPEN_EXISTING)) {
                FURI_LOG_E("MTP", "Failed to open file: %s", path);
                send_mtp_response(
                    mtp, 3, MTP_RESP_INVALID_OBJECT_HANDLE, persistence.transaction_id, NULL);
                storage_file_free(persistence.current_file);
                persistence.current_file = NULL;
                return;
            }
        }

        File* file = persistence.current_file;
        // no need since the file is already opened
        // storage_file_seek(file, offset, SEEK_SET);

        storage_file_write(file, ptr, bytes_length);

        persistence.buffer_offset += bytes_length;
        persistence.left_bytes -= bytes_length;

        if(persistence.left_bytes <= 0) {
            send_mtp_response(mtp, 3, MTP_RESP_OK, persistence.transaction_id, NULL);
            storage_file_close(file);
            storage_file_free(file);

            persistence.current_file = NULL;
        }
    }

    if(persistence.global_buffer != NULL) {
        memcpy(persistence.global_buffer + persistence.buffer_offset, ptr, length);
        persistence.buffer_offset += length;

        if(persistence.left_bytes > 0) {
            persistence.left_bytes -= length;
        }

        FURI_LOG_I("MTP", "Buffer offset: %ld", persistence.buffer_offset);
        FURI_LOG_I("MTP", "Left bytes: %ld", persistence.left_bytes);

        if(persistence.left_bytes == 0) {
            handle_mtp_data_complete(mtp);
        }
    }
}

void handle_mtp_data_complete(AppMTP* mtp) {
    // dump the buffer
    print_bytes("MTP Data", persistence.global_buffer, persistence.buffer_offset);

    switch(persistence.op) {
    case MTP_OP_SEND_OBJECT_INFO: {
        // parse the object info - persistence.global_buffer doesn't contain MTPHeader
        struct ObjectInfoHeader* info = (struct ObjectInfoHeader*)(persistence.global_buffer);

        uint8_t* ptr = persistence.global_buffer + sizeof(struct ObjectInfoHeader);
        ptr += 12;

        // dump the ptr
        print_bytes(
            "MTP FileName", ptr, persistence.buffer_offset - sizeof(struct ObjectInfoHeader));

        bool is_dir = info->format == MTP_FORMAT_ASSOCIATION;
        char* name = NULL;
        if(CheckMTPStringHasUnicode(ptr)) {
            // unicode isn't supported
            name = is_dir ? "New Folder" : "New File";
        } else {
            name = ReadMTPString(ptr);
        }

        // if the name is blank, generate random name
        if(*name == 0) {
            char* random_name = malloc(sizeof(char) * 10);
            strcpy(random_name, "file_");
            int random = rand() % 1000;
            char random_str[4];
            itoa(random, random_str, 10);
            strcpy(random_name + 5, random_str);
            name = random_name;
        }

        FURI_LOG_I("MTP", "Creating object: %s", name);

        uint32_t storage_id = persistence.params[0];
        uint32_t parent = persistence.params[1];

        char* base_path = get_base_path_from_storage_id(storage_id);
        if(base_path == NULL) {
            FURI_LOG_E("MTP", "Invalid storage ID: %ld", storage_id);
            send_mtp_response(
                mtp, 3, MTP_RESP_INVALID_STORAGE_ID, persistence.transaction_id, NULL);
            break;
        }

        if(parent != 0xffffffff) {
            base_path = get_path_from_handle(mtp, parent);
            if(base_path == NULL) {
                FURI_LOG_E("MTP", "Invalid parent handle: %ld", parent);
                send_mtp_response(
                    mtp, 3, MTP_RESP_INVALID_OBJECT_HANDLE, persistence.transaction_id, NULL);
                break;
            }
        }

        char* full_path = malloc(sizeof(char) * 256);
        merge_path(full_path, base_path, name);

        FURI_LOG_I("MTP", "Format: %04x", info->format);

        if(is_dir) {
            if(!storage_dir_exists(mtp->storage, full_path)) {
                if(!storage_simply_mkdir(mtp->storage, full_path)) {
                    FURI_LOG_E("MTP", "Failed to create directory: %s", full_path);
                    send_mtp_response(
                        mtp, 3, MTP_RESP_GENERAL_ERROR, persistence.transaction_id, NULL);
                    free(full_path);
                    break;
                }
            }
        } else {
            if(!storage_file_exists(mtp->storage, full_path)) {
                // create file
                File* file = storage_file_alloc(mtp->storage);
                if(!storage_file_open(file, full_path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
                    FURI_LOG_E("MTP", "Failed to create file: %s", full_path);
                    send_mtp_response(
                        mtp, 3, MTP_RESP_GENERAL_ERROR, persistence.transaction_id, NULL);
                    storage_file_free(file);
                    free(full_path);
                    break;
                }

                storage_file_close(file);
                storage_file_free(file);

                persistence.prev_handle = issue_object_handle(mtp, full_path);
            }
        }

        uint32_t handle = issue_object_handle(mtp, full_path);
        persistence.params[2] = handle;

        free(name);
        free(full_path);
        send_mtp_response(mtp, 3, MTP_RESP_OK, persistence.transaction_id, persistence.params);

        break;
    }
    default:
        FURI_LOG_W("MTP", "Unsupported MTP operation in bulk transfer: 0x%04x", persistence.op);
        send_mtp_response(mtp, 3, MTP_RESP_UNKNOWN, persistence.transaction_id, NULL);
        break;
    }

    free(persistence.global_buffer);
    persistence.global_buffer = NULL;
}

void handle_mtp_response(AppMTP* mtp, struct MTPHeader* header) {
    UNUSED(mtp);
    FURI_LOG_I("MTP", "Handling MTP response: 0x%04x", header->op);
    FURI_LOG_I("MTP", "Transaction ID: %ld", header->transaction_id);
    FURI_LOG_I("MTP", "Has Data: %d", persistence.global_buffer != NULL);
    FURI_LOG_I("MTP", "Data length: %ld", persistence.buffer_offset);
}

void handle_mtp_command(AppMTP* mtp, struct MTPContainer* container) {
    uint16_t mtp_op = container->header.op;
    FURI_LOG_I("MTP", "Handling MTP operation: 0x%04x", mtp_op);

    switch(mtp_op) {
    case MTP_OP_GET_DEVICE_INFO:
        FURI_LOG_I("MTP", "GetDeviceInfo operation");
        send_device_info(mtp, container->header.transaction_id);
        // Process the GetDeviceInfo operation
        break;
    case MTP_OP_OPEN_SESSION:
    case MTP_OP_CLOSE_SESSION:
        //FURI_LOG_I("MTP", "Open/CloseSession operation (STUB)");
        send_mtp_response(mtp, 3, MTP_RESP_OK, container->header.transaction_id, NULL);
        break;
    case MTP_OP_GET_STORAGE_IDS:
        FURI_LOG_I("MTP", "GetStorageIDs operation");
        send_storage_ids(mtp, container->header.transaction_id);
        break;
    case MTP_OP_GET_STORAGE_INFO: {
        FURI_LOG_I("MTP", "GetStorageInfo operation");
        uint8_t* info = malloc(sizeof(uint8_t) * 256);
        int length = GetStorageInfo(mtp, container->params[0], info);
        send_mtp_response_buffer(
            mtp,
            MTP_TYPE_DATA,
            MTP_OP_GET_STORAGE_INFO,
            container->header.transaction_id,
            info,
            length);
        send_mtp_response_buffer(
            mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, container->header.transaction_id, NULL, 0);
        free(info);
        break;
    }
    case MTP_OP_GET_OBJECT_HANDLES:
        FURI_LOG_I("MTP", "GetObjectHandles operation");
        if(container->params[1]) {
            send_mtp_response_buffer(
                mtp,
                MTP_TYPE_RESPONSE,
                MTP_RESP_SPEC_BY_FORMAT_UNSUPPORTED,
                container->header.transaction_id,
                NULL,
                0);
            break;
        } else {
            uint8_t* buffer = malloc(sizeof(uint8_t) * 256);
            int length = GetObjectHandles(mtp, container->params[0], container->params[2], buffer);
            send_mtp_response_buffer(
                mtp,
                MTP_TYPE_DATA,
                MTP_OP_GET_OBJECT_HANDLES,
                container->header.transaction_id,
                buffer,
                length);

            send_mtp_response_buffer(
                mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, container->header.transaction_id, NULL, 0);
            free(buffer);
        }
        break;
    case MTP_OP_GET_OBJECT_INFO: {
        FURI_LOG_I("MTP", "GetObjectInfo operation");
        uint8_t* buffer = malloc(sizeof(uint8_t) * 512);
        int length = GetObjectInfo(mtp, container->params[0], buffer);

        if(length < 0) {
            send_mtp_response(
                mtp,
                MTP_TYPE_RESPONSE,
                MTP_RESP_INVALID_OBJECT_HANDLE,
                container->header.transaction_id,
                NULL);
            break;
        }

        send_mtp_response_buffer(
            mtp,
            MTP_TYPE_DATA,
            MTP_OP_GET_OBJECT_INFO,
            container->header.transaction_id,
            buffer,
            length);
        free(buffer);

        send_mtp_response_buffer(
            mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, container->header.transaction_id, NULL, 0);
        break;
    }
    case MTP_OP_GET_OBJECT_PROPS_SUPPORTED: {
        FURI_LOG_I("MTP", "GetObjectPropsSupported operation");
        uint8_t* buffer = malloc(sizeof(uint8_t) * 256);
        uint32_t count = sizeof(supported_object_props) / sizeof(uint16_t);
        memcpy(buffer, &count, sizeof(uint32_t));
        memcpy(buffer + sizeof(uint32_t), supported_object_props, sizeof(uint16_t) * count);

        int length = sizeof(uint32_t) + sizeof(uint16_t) * count;
        send_mtp_response_buffer(
            mtp,
            MTP_TYPE_DATA,
            MTP_OP_GET_OBJECT_PROPS_SUPPORTED,
            container->header.transaction_id,
            buffer,
            length);
        free(buffer);
        send_mtp_response_buffer(
            mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, container->header.transaction_id, NULL, 0);
        break;
    }
    case MTP_OP_DELETE_OBJECT:
        FURI_LOG_I("MTP", "DeleteObject operation");
        if(DeleteObject(mtp, container->params[0])) {
            send_mtp_response(mtp, 3, MTP_RESP_OK, container->header.transaction_id, NULL);
        } else {
            send_mtp_response(
                mtp, 3, MTP_RESP_INVALID_OBJECT_HANDLE, container->header.transaction_id, NULL);
        }
        break;

    case MTP_OP_GET_DEVICE_PROP_VALUE:
        FURI_LOG_I("MTP", "GetDevicePropValue operation");
        send_device_prop_value(mtp, container->header.transaction_id, container->params[0]);
        // Process the GetDevicePropValue operation
        break;
    case MTP_OP_GET_DEVICE_PROP_DESC:
        FURI_LOG_I("MTP", "GetDevicePropDesc operation");
        send_device_prop_desc(mtp, container->header.transaction_id, container->params[0]);
        // Process the GetDevicePropDesc operation
        break;
    // Handle bulk transfer specific operations
    case MTP_OP_SEND_OBJECT_INFO:
        FURI_LOG_I("MTP", "SendObjectInfo operation");
        setup_persistence(container);
        break;
    case MTP_OP_SEND_OBJECT:
        FURI_LOG_I("MTP", "SendObject operation");
        setup_persistence(container);
        break;
    case MTP_OP_GET_OBJECT: {
        FURI_LOG_I("MTP", "GetObject operation");
        GetObject(mtp, container->header.transaction_id, container->params[0]);

        break;
    }
    // Handle other bulk operations here...
    default:
        FURI_LOG_W("MTP", "Unsupported MTP operation in bulk transfer: 0x%04x", mtp_op);
        send_mtp_response(mtp, 3, MTP_RESP_UNKNOWN, container->header.transaction_id, NULL);
        break;
    }
}

void send_storage_ids(AppMTP* mtp, uint32_t transaction_id) {
    uint32_t count;
    uint32_t storage_ids[3];
    GetStorageIDs(mtp, storage_ids, &count);

    FURI_LOG_I("MTP", "Sending storage IDs: %ld storages", count);
    uint32_t payload[3] = {count, storage_ids[0], storage_ids[1]};
    send_mtp_response_buffer(
        mtp,
        MTP_TYPE_DATA,
        MTP_OP_GET_STORAGE_IDS,
        transaction_id,
        (uint8_t*)payload,
        sizeof(uint32_t) * (count + 1));
    send_mtp_response_buffer(mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, transaction_id, NULL, 0);
}

void send_device_info(AppMTP* mtp, uint32_t transaction_id) {
    uint8_t* response = malloc(sizeof(uint8_t) * 256);
    int length = BuildDeviceInfo(response);
    send_mtp_response_buffer(
        mtp, MTP_TYPE_DATA, MTP_OP_GET_DEVICE_INFO, transaction_id, response, length);
    send_mtp_response_buffer(mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, transaction_id, NULL, 0);
    free(response);
}

void send_device_prop_value(AppMTP* mtp, uint32_t transaction_id, uint32_t prop_code) {
    uint8_t* response = malloc(sizeof(uint8_t) * 256);
    int length = GetDevicePropValue(prop_code, response);
    send_mtp_response_buffer(
        mtp, MTP_TYPE_DATA, MTP_OP_GET_DEVICE_PROP_VALUE, transaction_id, response, length);
    send_mtp_response_buffer(mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, transaction_id, NULL, 0);
    free(response);
}

void send_device_prop_desc(AppMTP* mtp, uint32_t transaction_id, uint32_t prop_code) {
    uint8_t* response = malloc(sizeof(uint8_t) * 256);
    int length = GetDevicePropDesc(prop_code, response);
    send_mtp_response_buffer(
        mtp, MTP_TYPE_DATA, MTP_OP_GET_DEVICE_PROP_DESC, transaction_id, response, length);
    send_mtp_response_buffer(mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, transaction_id, NULL, 0);
    free(response);
}

char* get_path_from_handle(AppMTP* mtp, uint32_t handle) {
    FileHandle* current = mtp->handles;
    while(current != NULL) {
        if(current->handle == handle) {
            return current->path;
        }
        current = current->next;
    }
    return NULL;
}

uint32_t issue_object_handle(AppMTP* mtp, char* path) {
    int handle = 0;
    int length = strlen(path);
    char* path_store = malloc(sizeof(char) * (length + 1));
    strcpy(path_store, path);

    if(mtp->handles == NULL) {
        mtp->handles = malloc(sizeof(FileHandle));
        mtp->handles->handle = handle;
        mtp->handles->path = path_store;
        mtp->handles->next = NULL;
        return handle;
    }

    FileHandle* current = mtp->handles;
    if(strcmp(current->path, path) == 0) {
        return current->handle;
    }

    while(current->next != NULL) {
        if(strcmp(current->path, path) == 0) {
            return current->handle;
        }
        current = current->next;
        handle++;
    }

    current->next = malloc(sizeof(FileHandle));
    current = current->next;
    handle++;

    current->handle = handle;
    current->path = path_store;
    current->next = NULL;
    return handle;
}

char* get_base_path_from_storage_id(uint32_t storage_id) {
    if(storage_id == INTERNAL_STORAGE_ID) {
        return STORAGE_INT_PATH_PREFIX;
    } else if(storage_id == EXTERNAL_STORAGE_ID) {
        return STORAGE_EXT_PATH_PREFIX;
    }
    return NULL;
}

int list_and_issue_handles(
    AppMTP* mtp,
    uint32_t storage_id,
    uint32_t association,
    uint32_t* handles) {
    Storage* storage = mtp->storage;
    char* base_path = get_base_path_from_storage_id(storage_id);
    if(base_path == NULL) {
        base_path = "";
    }

    File* dir = storage_file_alloc(storage);
    if(association == 0xffffffff) {
        // count the objects in the root directory
        storage_dir_open(dir, base_path);
    } else {
        char* path = get_path_from_handle(mtp, association);
        FURI_LOG_I("MTP", "Association path: %s", path);
        if(path == NULL) {
            return 0;
        }
        storage_dir_open(dir, path);
        base_path = path;
    }

    int count = 0;
    FileInfo fileinfo;
    char* file_name = malloc(sizeof(char) * 256);
    char* full_path = malloc(sizeof(char) * 256);
    while(storage_dir_read(dir, &fileinfo, file_name, 256)) {
        if(file_info_is_dir(&fileinfo)) {
            FURI_LOG_I("MTP", "Found directory: %s", file_name);
        } else {
            FURI_LOG_I("MTP", "Found file: %s", file_name);
        }

        merge_path(full_path, base_path, file_name);
        FURI_LOG_I("MTP", "Full path: %s", full_path);

        uint32_t handle = issue_object_handle(mtp, full_path);
        if(handles != NULL) {
            handles[count] = handle;
        }
        count++;
    }

    FURI_LOG_I("MTP", "Getting number of objects in storage %ld", storage_id);
    FURI_LOG_I("MTP", "Base path: %s", base_path);
    FURI_LOG_I("MTP", "Association: %ld", association);
    FURI_LOG_I("MTP", "Number of objects: %d", count);

    storage_dir_close(dir);
    storage_file_free(dir);
    free(file_name);
    free(full_path);
    return count;
}

struct GetObjectContext {
    File* file;
};

int GetObject_callback(void* ctx, uint8_t* buffer, int length) {
    struct GetObjectContext* obj_ctx = (struct GetObjectContext*)ctx;
    return storage_file_read(obj_ctx->file, buffer, length);
}

void GetObject(AppMTP* mtp, uint32_t transaction_id, uint32_t handle) {
    char* path = get_path_from_handle(mtp, handle);
    if(path == NULL) {
        send_mtp_response(
            mtp, MTP_TYPE_RESPONSE, MTP_RESP_INVALID_OBJECT_HANDLE, transaction_id, NULL);
        return;
    }

    FURI_LOG_I("MTP", "Getting object: %s", path);

    Storage* storage = mtp->storage;
    File* file = storage_file_alloc(storage);
    if(!storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E("MTP", "Failed to open file: %s", path);
        send_mtp_response(
            mtp, MTP_TYPE_RESPONSE, MTP_RESP_INVALID_OBJECT_HANDLE, transaction_id, NULL);
        return;
    }

    uint32_t size = storage_file_size(file);

    struct GetObjectContext ctx = {
        .file = file,
    };

    send_mtp_response_stream(
        mtp, MTP_TYPE_DATA, MTP_OP_GET_OBJECT, transaction_id, &ctx, GetObject_callback, size);

    send_mtp_response(mtp, MTP_TYPE_RESPONSE, MTP_RESP_OK, transaction_id, NULL);

    storage_file_close(file);
    storage_file_free(file);
}

int GetObjectInfo(AppMTP* mtp, uint32_t handle, uint8_t* buffer) {
    ObjectInfoHeader* header = (ObjectInfoHeader*)buffer;
    uint8_t* ptr = buffer + sizeof(ObjectInfoHeader);

    char* path = get_path_from_handle(mtp, handle);
    if(path == NULL) {
        return -1;
    }

    FURI_LOG_I("MTP", "Getting object info for handle %ld", handle);
    FURI_LOG_I("MTP", "Path: %s", path);

    header->protection_status = 0;

    Storage* storage = mtp->storage;
    File* file = storage_file_alloc(storage);
    uint16_t length;

    FileInfo fileinfo;
    FS_Error err = storage_common_stat(storage, path, &fileinfo);
    if(err != FSE_OK) {
        FURI_LOG_E("MTP", "Failed to get file info: %s", filesystem_api_error_get_desc(err));
        return -1;
    }

    if(memcmp(path, STORAGE_INT_PATH_PREFIX, strlen(STORAGE_INT_PATH_PREFIX)) == 0) {
        FURI_LOG_I("MTP", "Object in Internal storage");
        header->storage_id = INTERNAL_STORAGE_ID;
    } else if(memcmp(path, STORAGE_EXT_PATH_PREFIX, strlen(STORAGE_EXT_PATH_PREFIX)) == 0) {
        FURI_LOG_I("MTP", "Object in External storage");
        header->storage_id = EXTERNAL_STORAGE_ID;
    } else {
        return -1;
    }

    if(file_info_is_dir(&fileinfo)) {
        FURI_LOG_I("MTP", "Directory");

        header->format = MTP_FORMAT_ASSOCIATION;
        header->association_type = 0x0001; // Generic folder
    } else {
        FURI_LOG_I("MTP", "Undefined type");

        header->format = MTP_FORMAT_UNDEFINED;
    }

    header->compressed_size = fileinfo.size;

    header->thumb_format = 0;
    header->thumb_compressed_size = 0;
    header->thumb_pix_width = 0;
    header->thumb_pix_height = 0;
    header->image_pix_width = 0;
    header->image_pix_height = 0;
    header->image_bit_depth = 0;

    /*
    // is on root directory (/int or /ext)
    if(strchr(path + 1, '/') == NULL) {
        header->parent_object = 0;
    } else {
        char* parent_path = malloc(sizeof(char) * 256);
        strcpy(parent_path, path);
        char* last_slash = strrchr(parent_path, '/');
        *last_slash = '\0';
        header->parent_object = issue_object_handle(mtp, parent_path);

        free(parent_path);
    }
    */

    char* file_name = strrchr(path, '/');
    FURI_LOG_I("MTP", "File name: %s", file_name + 1);

    WriteMTPString(ptr, file_name + 1, &length);
    ptr += length;

    // get created
    WriteMTPString(ptr, "20240608T010702", &length);
    ptr += length;

    // get last modified
    WriteMTPString(ptr, "20240608T010702", &length);
    ptr += length;

    // get keywords
    WriteMTPString(ptr, "", &length);
    ptr += length;

    storage_file_free(file);

    return ptr - buffer;
}

bool CheckMTPStringHasUnicode(uint8_t* buffer) {
    uint8_t* base = buffer;
    int len = *base;

    uint16_t* ptr = (uint16_t*)(base + 1);

    for(int i = 0; i < len; i++) {
        if(ptr[i] > 0x7F) {
            return true;
        }
    }

    return false;
}

char* ReadMTPString(uint8_t* buffer) {
    int len16 = *(uint8_t*)buffer;
    if(len16 == 0) {
        return "";
    }

    char* str = malloc(sizeof(char) * len16);

    uint8_t* base = buffer + 1;
    uint16_t* ptr = (uint16_t*)base;

    for(int i = 0; i < len16; i++) {
        str[i] = *ptr++;
    }

    return str;
}

int GetNumObjects(AppMTP* mtp, uint32_t storage_id, uint32_t association) {
    return list_and_issue_handles(mtp, storage_id, association, NULL);
}

int GetObjectHandles(AppMTP* mtp, uint32_t storage_id, uint32_t association, uint8_t* buffer) {
    uint8_t* ptr = buffer;
    uint16_t length;

    UNUSED(length);

    // For now, just return a single object handle
    int count = GetNumObjects(mtp, storage_id, association);

    *(uint32_t*)ptr = count;
    ptr += sizeof(uint32_t);

    uint32_t* handles = (uint32_t*)ptr;
    length = list_and_issue_handles(mtp, storage_id, association, handles);
    ptr += length * sizeof(uint32_t);

    return ptr - buffer;
}

int GetDevicePropValue(uint32_t prop_code, uint8_t* buffer) {
    uint8_t* ptr = buffer;
    uint16_t length;

    switch(prop_code) {
    case 0xd402:
        WriteMTPString(ptr, "Flipper Zero", &length);
        ptr += length;
        break;
    default:
        // Unsupported property
        break;
    }

    return ptr - buffer;
}

int GetDevicePropDesc(uint32_t prop_code, uint8_t* buffer) {
    uint8_t* ptr = buffer;
    uint16_t length;

    switch(prop_code) {
    case 0xd402:
        // Device friendly name
        *(uint16_t*)ptr = prop_code;
        ptr += 2;

        // type is string
        *(uint16_t*)ptr = 0xffff;
        ptr += 2;

        // read-only
        *(uint16_t*)ptr = 0x0000;
        ptr += 2;

        length = GetDevicePropValue(prop_code, ptr);
        ptr += length;

        length = GetDevicePropValue(prop_code, ptr);
        ptr += length;

        // no-form
        *(uint16_t*)ptr = 0x0000;
        ptr += 2;
        break;
    default:
        // Unsupported property
        break;
    }

    return ptr - buffer;
}

void send_mtp_response_stream(
    AppMTP* mtp,
    uint16_t resp_type,
    uint16_t resp_code,
    uint32_t transaction_id,
    void* callback_context,
    int (*callback)(void* ctx, uint8_t* buffer, int length),
    uint32_t length) {
    int chunk_idx = 0;

    size_t buffer_available = MTP_MAX_PACKET_SIZE;
    uint8_t* buffer = malloc(sizeof(uint8_t) * buffer_available);
    uint8_t* ptr = buffer;

    uint32_t sent_length = 0;
    FURI_LOG_I("MTP", "Sending MTP response stream: %ld bytes", length);

    do {
        buffer_available = MTP_MAX_PACKET_SIZE;
        ptr = buffer; // reset the pointer

        if(chunk_idx == 0) {
            struct MTPHeader* hdr = (struct MTPHeader*)buffer;
            hdr->len = length + sizeof(*hdr);
            hdr->type = resp_type;
            hdr->op = resp_code;
            hdr->transaction_id = transaction_id;

            ptr += sizeof(*hdr);
            buffer_available -= sizeof(*hdr);
        }

        FURI_LOG_I("MTP", "Remaining bytes for packet: %d", buffer_available);

        int read_bytes = callback(callback_context, ptr, buffer_available);
        uint32_t usb_bytes = (ptr - buffer) + read_bytes;
        FURI_LOG_I("MTP", "USB packet size: %ld", usb_bytes);

        usbd_ep_write(mtp->dev, MTP_EP_IN_ADDR, buffer, usb_bytes);

        sent_length += usb_bytes;
        chunk_idx++;

        FURI_LOG_I(
            "MTP",
            "Sent chunk %d (currently sent: %ld/%ld)",
            chunk_idx,
            sent_length - sizeof(struct MTPHeader),
            length);
    } while(sent_length < length + sizeof(struct MTPHeader));

    free(buffer);
}

int send_mtp_response_buffer_callback(void* ctx, uint8_t* buffer, int size) {
    struct MTPResponseBufferContext* context = (struct MTPResponseBufferContext*)ctx;
    if(context->buffer == NULL || context->size == 0) {
        return 0;
    }

    uint32_t remaining = context->size - context->sent;
    uint32_t to_send = size;
    if(remaining < to_send) {
        to_send = remaining;
    }

    memcpy(buffer, context->buffer + context->sent, to_send);
    context->sent += to_send;

    return to_send;
}

void send_mtp_response_buffer(
    AppMTP* mtp,
    uint16_t resp_type,
    uint16_t resp_code,
    uint32_t transaction_id,
    uint8_t* buffer,
    uint32_t size) {
    struct MTPResponseBufferContext* ctx = malloc(sizeof(struct MTPResponseBufferContext));
    ctx->buffer = buffer;
    ctx->size = size;
    ctx->sent = 0;

    send_mtp_response_stream(
        mtp, resp_type, resp_code, transaction_id, ctx, send_mtp_response_buffer_callback, size);
    free(ctx);
}

void send_mtp_response(
    AppMTP* mtp,
    uint16_t resp_type,
    uint16_t resp_code,
    uint32_t transaction_id,
    uint32_t* params) {
    uint32_t response[5] = {0};

    if(params != NULL) {
        memcpy(response, params, sizeof(uint32_t) * 5);
    }

    send_mtp_response_buffer(
        mtp, resp_type, resp_code, transaction_id, (uint8_t*)response, sizeof(response));
}

int mtp_handle_class_control(AppMTP* mtp, usbd_device* dev, usbd_ctlreq* req) {
    UNUSED(dev);

    int value = -1;
    uint32_t handle = req->wIndex;
    uint8_t* buffer = req->data;
    uint32_t size = req->wLength;

    UNUSED(handle);
    UNUSED(buffer);
    UNUSED(size);

    switch(req->bRequest) {
    case MTP_REQ_CANCEL:
        // Handle Cancel request
        value = 0;
        break;
    case MTP_REQ_GET_EXT_EVENT_DATA:
        // Handle GetExtEventData request
        value = 0;
        break;
    case MTP_REQ_RESET:
        // Handle Reset request
        mtp->state = MTPStateReady;
        value = 0;
        break;
    case MTP_REQ_GET_DEVICE_STATUS:
        // Handle GetDeviceStatus request
        struct mtp_device_status* status = (struct mtp_device_status*)req->data;
        status->wLength = sizeof(*status);
        status->wCode = (mtp->state == MTPStateBusy) ? MTP_RESP_DEVICE_BUSY : MTP_RESP_OK;
        value = status->wLength;
        break;
    default:
        // Unsupported request
        break;
    }

    return value;
}
int BuildDeviceInfo(uint8_t* buffer) {
    uint8_t* ptr = buffer;
    uint16_t length;

    // Standard version
    *(uint16_t*)ptr = 100;
    ptr += sizeof(uint16_t);

    // Vendor extension ID
    *(uint32_t*)ptr = MTP_VENDOR_EXTENSION_ID;
    ptr += sizeof(uint32_t);

    // Vendor extension version
    *(uint16_t*)ptr = MTP_VENDOR_EXTENSION_VERSION;
    ptr += sizeof(uint16_t);

    // Vendor extension description
    WriteMTPString(ptr, "microsoft.com: 1.0;", &length);
    ptr += length;

    // Functional mode
    *(uint16_t*)ptr = MTP_FUNCTIONAL_MODE;
    ptr += sizeof(uint16_t);

    // Operations supported
    length = sizeof(supported_operations) / sizeof(uint16_t);
    *(uint32_t*)ptr = length; // Number of supported operations
    ptr += sizeof(uint32_t);

    for(int i = 0; i < length; i++) {
        *(uint16_t*)ptr = supported_operations[i];
        ptr += sizeof(uint16_t);
    }

    // Supported events (example, add as needed)
    *(uint32_t*)ptr = 0; // Number of supported events
    ptr += sizeof(uint32_t);

    length = sizeof(supported_device_properties) / sizeof(uint16_t);
    *(uint32_t*)ptr = length; // Number of supported device properties
    ptr += sizeof(uint32_t);

    for(int i = 0; i < length; i++) {
        *(uint16_t*)ptr = supported_device_properties[i];
        ptr += sizeof(uint16_t);
    }

    // Supported capture formats (example, add as needed)
    *(uint32_t*)ptr = 0; // Number of supported capture formats
    ptr += sizeof(uint32_t);

    // Supported playback formats (example, add as needed)
    length = sizeof(supported_playback_formats) / sizeof(uint16_t);
    *(uint32_t*)ptr = length; // Number of supported playback formats
    ptr += sizeof(uint32_t);

    for(int i = 0; i < length; i++) {
        *(uint16_t*)ptr = supported_playback_formats[i];
        ptr += sizeof(uint16_t);
    }

    // Manufacturer
    WriteMTPString(ptr, USB_MANUFACTURER_STRING, &length);
    ptr += length;

    // Model
    WriteMTPString(ptr, USB_DEVICE_MODEL, &length);
    ptr += length;

    // Device version
    WriteMTPString(ptr, MTP_DEVICE_VERSION, &length);
    ptr += length;

    // Serial number
    WriteMTPString(ptr, MTP_DEVICE_SERIAL, &length);
    ptr += length;

    return ptr - buffer;
}

void GetStorageIDs(AppMTP* mtp, uint32_t* storage_ids, uint32_t* count) {
    SDInfo sd_info;
    FS_Error err = storage_sd_info(mtp->storage, &sd_info);
    storage_ids[0] = INTERNAL_STORAGE_ID;
    if(err != FSE_OK) {
        FURI_LOG_E("MTP", "SD Card not found");
        *count = 1; // We have only one storage
        return;
    }

    storage_ids[1] = EXTERNAL_STORAGE_ID;
    *count = 2; // We have two storages: internal and external
}

int GetStorageInfo(AppMTP* mtp, uint32_t storage_id, uint8_t* buf) {
    MTPStorageInfoHeader* info = (MTPStorageInfoHeader*)buf;
    uint8_t* ptr = buf + sizeof(MTPStorageInfoHeader);
    uint16_t length;

    info->free_space_in_objects = 20ul;
    info->filesystem_type = 0x0002; // Generic hierarchical
    info->access_capability = 0x0000; // Read-write
    FURI_LOG_I("MTP", "Getting storage info for storage ID %04lx", storage_id);

    if(storage_id == INTERNAL_STORAGE_ID) {
        // Fill in details for internal storage
        info->storage_type = 0x0003; // Fixed RAM

        // Fill in details for internal storage
        uint64_t total_space;
        uint64_t free_space;
        FS_Error err = storage_common_fs_info(
            mtp->storage, STORAGE_INT_PATH_PREFIX, &total_space, &free_space);
        if(err != FSE_OK) {
            info->max_capacity = 0;
            info->free_space_in_bytes = 0;
        } else {
            info->max_capacity = total_space / BLOCK_SIZE;
            info->free_space_in_bytes = free_space / BLOCK_SIZE;
        }

        WriteMTPString(ptr, "Internal Storage", &length);
        ptr += length;

        WriteMTPString(ptr, "INT_STORAGE", &length);
        ptr += length;
    } else if(storage_id == EXTERNAL_STORAGE_ID) {
        SDInfo sd_info;
        FS_Error err = storage_sd_info(mtp->storage, &sd_info);

        // Fill in details for internal storage
        info->storage_type = 0x0004; // Removable RAM

        if(err != FSE_OK) {
            info->max_capacity = 0;
            info->free_space_in_bytes = 0;

            FURI_LOG_E("MTP", "SD Card not found");
        } else {
            // Fill in details for external storage
            info->max_capacity = (uint64_t)sd_info.kb_total * 1024 / BLOCK_SIZE;
            info->free_space_in_bytes = (uint64_t)sd_info.kb_free * 1024 / BLOCK_SIZE;
        }

        WriteMTPString(ptr, "SD Card", &length);
        ptr += length;

        WriteMTPString(ptr, "SD_CARD", &length);
        ptr += length;
    }

    // try to convert into big endian???
    //info->max_capacity = __builtin_bswap64(info->max_capacity);
    //info->free_space_in_bytes = __builtin_bswap64(info->free_space_in_bytes);

    return ptr - buf;
}

// Microsoft-style UTF-16LE string:
void WriteMTPString(uint8_t* buffer, const char* str, uint16_t* length) {
    uint8_t* ptr = buffer;
    uint8_t str_len = strlen(str);

    FURI_LOG_I("MTP", "Writing MTP string: %s", str);
    FURI_LOG_I("MTP", "String length: %d", str_len);

    // extra handling for empty string
    if(str_len == 0) {
        *ptr = 0x00;

        // that's it!
        *length = 1;
        return;
    }

    *ptr = str_len + 1; // Length byte (number of characters including the null terminator)
    ptr++;
    while(*str) {
        *ptr++ = *str++;
        *ptr++ = 0x00; // UTF-16LE encoding (add null byte for each character)
    }
    *ptr++ = 0x00; // Null terminator (UTF-16LE)
    *ptr++ = 0x00;

    FURI_LOG_I("MTP", "String byte length: %d", ptr - buffer);
    *length = ptr - buffer;
}

void WriteMTPBEString(uint8_t* buffer, const char* str, uint16_t* length) {
    uint8_t* ptr = buffer;
    uint8_t str_len = strlen(str);
    *ptr++ = str_len + 1; // Length byte (number of characters including the null terminator)
    while(*str) {
        *ptr++ = 0x00; // UTF-16BE encoding (add null byte for each character)
        *ptr++ = *str++;
    }
    *ptr++ = 0x00; // Null terminator (UTF-16LE)
    *ptr++ = 0x00;
    *length = ptr - buffer;
}

bool DeleteObject(AppMTP* mtp, uint32_t handle) {
    UNUSED(mtp);
    FURI_LOG_I("MTP", "Deleting object %ld", handle);

    char* path = get_path_from_handle(mtp, handle);
    if(path == NULL) {
        return false;
    }

    FileInfo fileinfo;
    FS_Error err = storage_common_stat(mtp->storage, path, &fileinfo);

    if(err == FSE_OK) {
        if(file_info_is_dir(&fileinfo)) {
            if(!storage_simply_remove_recursive(mtp->storage, path)) {
                return false;
            }
        } else {
            if(storage_common_remove(mtp->storage, path) != FSE_OK) {
                return false;
            }
        }

        return true;
    }

    return false;
}
