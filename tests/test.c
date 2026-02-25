#include "../include/vault_objects.h"
int main() {
    char hash_output[65]; 
    
    const char *test_data = "hello";
    size_t data_len = 5;

    VaultError result = vault_hash_content((const uint8_t *)test_data, data_len, hash_output);

    if (result == VAULT_OK) {
        printf("Test Basarili!\n");
        printf("Girdi: %s\n", test_data);
        printf("Hesaplanan Hash: %s\n", hash_output);
    } else {
        printf("Hata Olustu! Hata Kodu: %d\n", result);
    }

    return 0;
}