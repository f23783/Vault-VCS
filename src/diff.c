#include "../include/vault_cli.h"

VaultError vault_diff_compute(const char *old_text, size_t old_size,
                              const char *new_text, size_t new_size,
                              DiffResult *result){
    (void) old_text;
    (void) old_size;
    (void) new_text;
    (void) new_size;
    (void) result;
    return VAULT_OK;
}

void vault_diff_print(const DiffResult *result,
                      const char *old_path, const char *new_path){
    (void) result;
    (void) old_path;
    (void) new_path;
    return VAULT_OK;
}

void vault_diff_free(DiffResult *result){
    (void) result;
    return VAULT_OK;
}
