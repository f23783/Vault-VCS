#include "../include/vault_objects.h"

VaultError vault_hash_content(const uint8_t *data, size_t size,
                              char out_hash[VAULT_HASH_HEX_SIZE]){
    (void)data;
    (void)size;
    (void)out_hash;
    return VAULT_OK;
}

VaultError vault_object_write(VaultObjectType type,
                              const uint8_t *data, size_t size,
                              char out_hash[VAULT_HASH_HEX_SIZE]){
    (void)type;
    (void)data;
    (void)size;
    (void)out_hash;
    return VAULT_OK;
}

VaultError vault_object_read(const char hash[VAULT_HASH_HEX_SIZE],
                             uint8_t **out_data, size_t *out_size,
                             VaultObjectType *out_type){
    (void)hash;
    (void)out_data;
    (void)out_size;
    (void)out_type;
    return VAULT_OK;
}

int vault_object_exists(const char hash[VAULT_HASH_HEX_SIZE]){
    (void)hash;
    return 0;
}

VaultError vault_blob_write(const VaultBlob *blob,
                            char out_hash[VAULT_HASH_HEX_SIZE]){
    (void)blob;
    (void)out_hash;
    return VAULT_OK;
}

VaultError vault_tree_serialize(const VaultTree *tree,
                                uint8_t **out_data, size_t *out_size){
    (void)tree;
    (void)out_data;
    (void)out_size;
    return VAULT_OK;
}

VaultError vault_tree_deserialize(const uint8_t *data, size_t size,
                                  VaultTree *out_tree){
    (void)data;
    (void)size;
    (void)out_tree;
    return VAULT_OK;
}

void vault_tree_free(VaultTree *tree){
    (void)tree;
}
