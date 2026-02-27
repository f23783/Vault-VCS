/*
 * ============================================================================
 *  main.c — Vault-VCS Giriş Noktası
 * ============================================================================
 *
 *  Bu dosya projenin ne kadar basit bir şekilde bir araya geldiğini gösterir.
 *  Tüm karmaşıklık header'larda tanımlanan fonksiyonların İÇİNDE gizlidir.
 *  main() fonksiyonu sadece:
 *    1. Argümanları parse et
 *    2. Doğru komutu çalıştır
 *    3. Temizle ve çık
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/vault_cli.h"

int main(int argc, char **argv)
{
    /* Hiç argüman yoksa yardım göster */
    if (argc < 2) {
        vault_cmd_help();
        return 1;
    }

    /* Argümanları parse et */
    VaultArgs args = {0};
    VaultError err = vault_parse_args(argc, argv, &args);
    if (err != VAULT_OK) {
        fprintf(stderr, "vault: invalid arguments. See 'vault help'.\n");
        return 1;
    }

    /* Komutu çalıştır */
    err = vault_dispatch(&args);

    /* Temizlik */
    vault_args_free(&args);

    if (err != VAULT_OK) {
        /* Hata mesajları zaten handler'lar içinde yazdırılıyor,
         * burada sadece çıkış kodunu ayarlıyoruz */
        return 1;
    }

    return 0;
}
