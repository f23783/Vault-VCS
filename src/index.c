#define  _GNU_SOURCE
#include "../include/vault_index.h"
#include "../include/vault_objects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VaultError vault_index_load(VaultIndex *idx){
    FILE *fptr;
    fptr = fopen(VAULT_INDEX_FILE, "r");
    idx->capacity = 2;
    idx->entries = (IndexEntry *)malloc(idx->capacity * sizeof(IndexEntry));
    char *line = NULL;
    size_t capacity = 0;
    ssize_t readed;

    if (fptr == NULL) {
        return VAULT_ERR_IO;
    }

    while ((readed = getline(&line, &capacity, fptr)) != -1)  {
        if (idx->count >= idx->capacity) {
            idx->capacity *= 2;
            IndexEntry *temp = (IndexEntry *)realloc(idx->entries, idx->capacity * sizeof(IndexEntry));
            if (temp == NULL) {
                free(line);
                fclose(fptr);
                return VAULT_ERR_NOMEM;
            }
            idx->entries = temp;
        }

        char *hash_tok = strtok(line, " ");
        char *mtime_tok = strtok(NULL, " ");
        char *filepath_tok = strtok(NULL, "\n");

        if (hash_tok && mtime_tok && filepath_tok) {
            strncpy(idx->entries[idx->count].hash, hash_tok, VAULT_HASH_HEX_SIZE - 1);
            idx->entries[idx->count].hash[VAULT_HASH_HEX_SIZE - 1] = '\0';
            idx->entries[idx->count].mtime = atol(mtime_tok);
            strncpy(idx->entries[idx->count].filepath, filepath_tok, VAULT_MAX_PATH - 1);
            idx->entries[idx->count].filepath[VAULT_MAX_PATH - 1] = '\0';
            idx->count++;
        }
    }

    free(line);
    fclose(fptr);
    return VAULT_OK;
}

VaultError vault_index_save(const VaultIndex *idx){
    FILE *fptr;

    fptr = fopen("index_new", "w");

    if (fptr == NULL) {
        return VAULT_ERR_IO;
    }
    for (int i = 0; i < idx->count; i++) {
        fprintf(fptr, "%s %ld %s\n", idx->entries[i].hash, idx->entries[i].mtime, idx->entries[i].filepath);
    }

    fclose(fptr);

    if (rename("index_new", ".vault/index") != 0) {
        return VAULT_ERR_IO;
    }
    return VAULT_OK;
}

VaultError vault_index_add(VaultIndex *idx, const char *filepath){
    char hash[VAULT_HASH_HEX_SIZE];
    FILE *fptr = fopen(filepath, "r");
    if (fptr == NULL) {
        fclose(fptr);
        return VAULT_ERR_IO;
    }
    else {
        if (vault_index_find(idx, filepath) == -1) {

        }
        else {
            size_t capacity = 1024;
            size_t length = 0;
            char *content = (char *)malloc(capacity);

            int ch;
            while ((ch = fgetc(fptr)) != EOF) {
                if (length >= capacity) {
                    capacity *= 2;
                    char *temp = (char *)realloc(content, capacity);
                    if (temp == NULL) {
                        free(content);
                        fclose(fptr);
                        return VAULT_ERR_NOMEM;
                    }
                    content = temp;
                }
                content[length++] = (char)ch;
            }
            content[length] = '\0';

            vault_hash_content((uint8_t *)content, capacity, hash);
            free(content);
            fclose(fptr);
            int index_location = vault_index_find(idx, filepath);//todo muhtemelen aşağıdaki -1 buglar
            if (index_location != -1 && strcmp(idx->entries[index_location - 1].hash, hash) == 0) {
                return VAULT_OK;
            }
            else if (index_location != -1 && strcmp(idx->entries[index_location - 1].hash, hash) != 0) {
                strcpy(idx->entries[index_location - 1].hash, hash);
            }
            else if (index_location != -1) {
                idx->count ++;
                if (idx->count >= idx->capacity) {
                    idx->capacity *= 2;
                    IndexEntry *temp = (IndexEntry *)realloc(idx->entries, idx->capacity * sizeof(IndexEntry));
                    if (temp == NULL) {
                        return VAULT_ERR_NOMEM;
                    }
                    idx->entries = temp;
                }
                strcpy(idx->entries[idx->count].filepath, filepath);
                strcpy(idx->entries[idx->count].hash, hash);
                idx->entries[idx->count].mtime = time(NULL);
            }
        }
    }

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
