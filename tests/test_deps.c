#include <openssl/opensslv.h>
#include <stdio.h>
#include <openssl/sha.h>
#include <zlib.h>

int main(void) {
    printf("OpenSSL version: %s\n", OPENSSL_FULL_VERSION_STR);
    printf("zlib version: %s\n", zlibVersion());
    printf("Kurulum başarılı!\n");
    return 0;
}
