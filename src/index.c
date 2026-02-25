#include "../include/vault_index.h"

VaultError vault_index_load(VaultIndex *idx){
    (void)idx;
    return VAULT_OK;
}

VaultError vault_index_save(const VaultIndex *idx){
    (void)idx;
    return VAULT_OK;
}

VaultError vault_index_add(VaultIndex *idx, const char *filepath){
    (void)idx;
    (void)filepath;
    return VAULT_OK;
}

VaultError vault_index_remove(VaultIndex *idx, const char *filepath){
    (void)idx;
    (void)filepath;
    return VAULT_OK;
}

int vault_index_find(const VaultIndex *idx, const char *filepath){
    (void)idx;
    (void)filepath;
    return 0;
}

VaultError vault_build_tree(const VaultIndex *idx,char out_tree_hash[VAULT_HASH_HEX_SIZE]){
    (void)idx;
    (void)out_tree_hash;
    return VAULT_OK;
}

VaultError vault_head_read(char out_hash[VAULT_HASH_HEX_SIZE]){
    (void)out_hash;
    return VAULT_OK;
}

VaultError vault_head_write(const char hash[VAULT_HASH_HEX_SIZE]){
    (void)hash;
    return VAULT_OK;
}

typedef void (*VaultStatusCallback)(const char *filepath, char status,void *ctx);

VaultError vault_status(const VaultIndex *idx,VaultStatusCallback callback,void *user_data){
    (void)idx;
    (void)callback;
    (void)user_data;
    return VAULT_OK;
}

void vault_index_free(VaultIndex *idx){
    (void)idx;
}
