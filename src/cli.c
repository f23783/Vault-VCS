#include "../include/vault_cli.h"
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <stdlib.h>

VaultError vault_parse_args(int argc, char **argv, VaultArgs *args){

    args->cmd= VAULT_CMD_UNKNOWN;
    args->targets = malloc(sizeof(char*) * 100);
    args->target_cnt = 0;

    for(int i = 1; i < argc; i++){
        if (strcmp(argv[i], "init") == 0) {
            args->cmd = VAULT_CMD_INIT;
        }
        else if (strcmp(argv[i], "add") == 0) {
            args->cmd = VAULT_CMD_ADD;
        }
        else if (strcmp(argv[i], "commit") == 0) {
            args->cmd = VAULT_CMD_COMMIT;
        }
        else if (strcmp(argv[i], "log") == 0) {
            args->cmd = VAULT_CMD_LOG;
        }
        else if (strcmp(argv[i], "status") == 0) {
            args->cmd = VAULT_CMD_STATUS;
        }
        else if (strcmp(argv[i], "checkout") == 0) {
            args->cmd = VAULT_CMD_CHECKOUT;
        }
        else if (strcmp(argv[i], "diff") == 0) {
            args->cmd = VAULT_CMD_DIFF;
        }
        else if (strcmp(argv[i], "help") == 0) {
            args->cmd = VAULT_CMD_HELP;
        }

        else if (strcmp(argv[i], "-m") == 0){
            if(i+1 < argc){
                strncpy(args->message, argv[i+1], sizeof(args->message) - 1);
                args->message[sizeof(args->message) - 1] = '\0';
                i++;
            }
            else{
                return 1;
            }
        }

        else if (strcmp(argv[i], "--author") == 0){
            if(i+1 < argc){
                strncpy(args->author, argv[i+1], sizeof(args->author) - 1);
                args->author[sizeof(args->author) - 1] ='\0';
                i++;
            }
            else{
                return 1;
            }
        }

        else if(strcmp(argv[i], "--verbose") == 0){
            args->verbose = 1;
        }

        else{
            if (argv[i][0] != '-' && args->cmd != VAULT_CMD_UNKNOWN && args->target_cnt < 100) { //i'ninci stringin ilk karakteri '-' değilse
                args->targets[args->target_cnt] = argv[i]; //argv[i] stringini args->targets dizisine kopyala
                args->target_cnt++;
            }
        }
    }

    return VAULT_OK;
}

VaultError vault_cmd_init(const VaultArgs *args){
   if (_mkdir(".vault") == 0){
        printf("Vault file created\n");
   }
   else {
        printf("Vault file is already exists or hasn't created");
        return 1;
   }
    if (_mkdir(".vault/objects") == 0){
        printf("Objects file created\n");
   }
    else {
        printf("Objects file is already exists or hasn't created");
        return 1;
    }
    FILE *f_head = fopen(".vault/HEAD", "w");
    if (f_head) {
    printf("Head file created\n");
    fclose(f_head);
    }
    else {
        printf("Head file could not open\n");
        return 1;
    }

    FILE *f_index = fopen(".vault/index", "w");
    if (f_index) {
    printf("Index file created\n");
    fclose(f_index);
    }
    else {
        printf("Index file could not open\n");
        return 1;
    }

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
    printf("Correct usage: vault <command> [<args>]\n\n"
    "Commands:\n"
    "       init       Create a new vault repository\n"
    "       add        Stage files for commit\n"
    "       commit     Record changes to the repository\n"
    "       log        Show commit history\n"
    "       status     Show working directory status\n"
    "       checkout   Restore a previous commit\n"
    "       diff       Show differences between versions\n");
}

void vault_args_free(VaultArgs *args){
    if (args->targets != NULL) {
            free(args->targets);
            args->targets = NULL;
    }
}


VaultError vault_dispatch(const VaultArgs *args){
    switch (args->cmd) {
        case VAULT_CMD_INIT:
            return vault_cmd_init(args);
        case VAULT_CMD_ADD:
            return vault_cmd_add(args);
        case VAULT_CMD_COMMIT:
            return vault_cmd_commit(args);
        case VAULT_CMD_LOG:
            return vault_cmd_log(args);
        case VAULT_CMD_STATUS:
            return vault_cmd_status(args);
        case VAULT_CMD_CHECKOUT:
            return vault_cmd_checkout(args);
        case VAULT_CMD_DIFF:
            return vault_cmd_diff(args);
        default:
            vault_cmd_help();
            return VAULT_OK;
    }
}
