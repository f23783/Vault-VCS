#include "../include/vault_cli.h"

VaultError vault_parse_args(int argc, char **argv, VaultArgs *args){
    (void) argc;
    (void) argv;
    (void) args;
    return VAULT_OK;
}

VaultError vault_cmd_init(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}

VaultError vault_cmd_add(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}

VaultError vault_cmd_commit(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}

VaultError vault_cmd_log(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}

VaultError vault_cmd_status(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}

VaultError vault_cmd_checkout(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}

VaultError vault_cmd_diff(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}

void vault_cmd_help(void){
}

void vault_args_free(VaultArgs *args){
    (void) args;
}

VaultError vault_dispatch(const VaultArgs *args){
    (void) args;
    return VAULT_OK;
}
