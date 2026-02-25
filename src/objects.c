#include "../include/vault_objects.h"
#include <stddef.h> 
#include <stdint.h>
#include <openssl/evp.h>

VaultError vault_hash_content(const uint8_t *data, size_t size,
                              char out_hash[VAULT_HASH_HEX_SIZE]){

    EVP_MD_CTX *context = EVP_MD_CTX_new();

    if(context == NULL){
        return VAULT_ERR_NOMEM;
     }

    if(EVP_DigestInit_ex(context, EVP_sha256(), NULL) != 1){
        EVP_MD_CTX_free(context);
        return VAULT_ERR_OPENSSL;
    }

    if (EVP_DigestUpdate(context, data, size) != 1){
        EVP_MD_CTX_free(context);
        return VAULT_ERR_OPENSSL;
    }
    unsigned char hash_result[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    if(EVP_DigestFinal_ex(context, hash_result, &hash_len)!= 1){
         EVP_MD_CTX_free(context);
         return VAULT_ERR_OPENSSL;
    }
    
    for (int i = 0; i < 32; i++){
        sprintf(&out_hash[i * 2], "%02x", hash_result[i]);
    }

    out_hash[64] = '\0';
    EVP_MD_CTX_free(context);

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
