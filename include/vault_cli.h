/*
 * ============================================================================
 *  vault_cli.h — Komut Satırı Arayüzü (Commander Katmanı)
 * ============================================================================
 *
 *  Bu dosya "The Commander" rolündeki kişinin implement edeceği fonksiyonları
 *  tanımlar. Kullanıcının terminalde yazdığı komutları parse edip, doğru
 *  katmana yönlendirir.
 *
 *  Desteklenen komutlar:
 *    vault init                     → Yeni repo oluştur
 *    vault add <dosya>              → Dosyayı staging'e ekle
 *    vault commit -m "mesaj"        → Commit oluştur
 *    vault log                      → Commit geçmişini göster
 *    vault status                   → Değişiklikleri listele
 *    vault checkout <hash>          → Eski commit'e dön
 *    vault diff <dosya>             → Dosya farklarını göster
 *    vault diff <hash1> <hash2>     → İki commit arası farklar
 *
 *  Bağımlılık: vault_objects.h, vault_index.h
 * ============================================================================
 */

#ifndef VAULT_CLI_H
#define VAULT_CLI_H

#include "vault_index.h"

/* ---- Komut Tipleri ------------------------------------------------------ */

/*
 * Kullanıcının girdiği komutu temsil eden enum.
 * CLI parser önce string'i bu enum'a çevirir,
 * sonra ilgili handler fonksiyonunu çağırır.
 */
typedef enum {
    VAULT_CMD_INIT,         /* vault init */
    VAULT_CMD_ADD,          /* vault add <dosya> */
    VAULT_CMD_COMMIT,       /* vault commit -m "mesaj" */
    VAULT_CMD_LOG,          /* vault log */
    VAULT_CMD_STATUS,       /* vault status */
    VAULT_CMD_CHECKOUT,     /* vault checkout <hash> */
    VAULT_CMD_DIFF,         /* vault diff ... */
    VAULT_CMD_HELP,         /* vault help */
    VAULT_CMD_UNKNOWN       /* Tanınmayan komut */
} VaultCommand;

/* ---- Parsed Argümanlar -------------------------------------------------- */

/*
 * VaultArgs: Parse edilmiş komut satırı argümanlarını tutar.
 *
 * Örnek: "vault commit -m 'İlk commit'"
 *   → cmd     = VAULT_CMD_COMMIT
 *   → message = "İlk commit"
 *
 * Örnek: "vault add src/main.c src/utils.c"
 *   → cmd        = VAULT_CMD_ADD
 *   → targets    = {"src/main.c", "src/utils.c"}
 *   → target_cnt = 2
 */
typedef struct {
    VaultCommand  cmd;              /* Hangi komut çalıştırılacak */
    char          message[512];     /* -m flag'i ile verilen commit mesajı */
    char          author[128];      /* --author flag'i (opsiyonel, ortam değişkeninden alınabilir) */
    char        **targets;          /* Dosya yolları veya hash'ler listesi */
    int           target_cnt;       /* targets dizisindeki eleman sayısı */
    int           verbose;          /* -v flag'i: ayrıntılı çıktı */
} VaultArgs;

/* ---- CLI Parser --------------------------------------------------------- */

/*
 * vault_parse_args:
 *   main() fonksiyonundan gelen argc/argv'yi parse edip
 *   VaultArgs yapısına dönüştürür.
 *
 *   Parametreler:
 *     argc → Argüman sayısı (main'den gelen)
 *     argv → Argüman dizisi (main'den gelen)
 *     args → Parse sonucu (çıktı)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 *
 *   Örnek:
 *     // argv = {"vault", "commit", "-m", "ilk commit"}
 *     VaultArgs args;
 *     vault_parse_args(argc, argv, &args);
 *     // args.cmd = VAULT_CMD_COMMIT
 *     // args.message = "ilk commit"
 */
VaultError vault_parse_args(int argc, char **argv, VaultArgs *args);

/* ---- Komut Handler'ları ------------------------------------------------- */

/*
 * vault_cmd_init:
 *   Yeni bir vault reposu oluşturur.
 *
 *   Şunları yapar:
 *     1. .vault/ dizinini oluştur
 *     2. .vault/objects/ dizinini oluştur
 *     3. Boş .vault/index dosyası oluştur
 *     4. .vault/HEAD dosyasını oluştur (boş, henüz commit yok)
 *
 *   Eğer .vault/ zaten varsa hata ver veya uyarı göster.
 */
VaultError vault_cmd_init(const VaultArgs *args);

/*
 * vault_cmd_add:
 *   Dosyaları staging area'ya ekler.
 *
 *   Her target dosya için vault_index_add() çağırır.
 *   Dosya bulunamazsa hata mesajı yazdırır ama diğer dosyalara devam eder.
 *
 *   Örnek çıktı:
 *     $ vault add src/main.c README.md
 *     added: src/main.c
 *     added: README.md
 */
VaultError vault_cmd_add(const VaultArgs *args);

/*
 * vault_cmd_commit:
 *   Yeni bir commit oluşturur.
 *
 *   Kontroller:
 *     - Index boş mu? → "nothing to commit" hatası
 *     - Mesaj verilmiş mi? → "-m flag'i gerekli" hatası
 *
 *   Başarılı olursa:
 *     $ vault commit -m "Proje yapısı oluşturuldu"
 *     [main a1b2c3d] Proje yapısı oluşturuldu
 *      2 files changed
 */
VaultError vault_cmd_commit(const VaultArgs *args);

/*
 * vault_cmd_log:
 *   Commit geçmişini ters kronolojik sırayla gösterir.
 *   HEAD'den başlayıp parent zincirini takip eder.
 *
 *   Çıktı formatı:
 *     commit a1b2c3d4e5f6...
 *     Author: Arda
 *     Date:   Mon Jun 17 14:30:00 2025
 *
 *         Proje yapısı oluşturuldu
 *
 *     commit f6a7b8c9d0e1...
 *     Author: Arda
 *     Date:   Mon Jun 17 12:00:00 2025
 *
 *         İlk commit
 */
VaultError vault_cmd_log(const VaultArgs *args);

/*
 * vault_cmd_status:
 *   Çalışma dizininin durumunu gösterir.
 *
 *   Çıktı formatı:
 *     Changes not staged for commit:
 *       modified: src/main.c
 *       deleted:  old_file.c
 *
 *     Untracked files:
 *       new_file.c
 */
VaultError vault_cmd_status(const VaultArgs *args);

/*
 * vault_cmd_checkout:
 *   Verilen commit hash'ine göre çalışma dizinini geri yükler.
 *
 *   ⚠️ DİKKAT: Bu tehlikeli bir işlem!
 *
 *   İşlem sırası:
 *     1. Hedef commit'i oku → tree hash'ini al
 *     2. Tree'yi recursive olarak çöz
 *     3. Çalışma dizinindeki mevcut dosyaları temizle
 *     4. Tree'deki blob'ları dosya olarak geri yaz
 *     5. Index'i güncelle
 *     6. HEAD'i güncelle
 *
 *   Güvenlik: Kaydedilmemiş değişiklik varsa uyarı ver!
 *     "Error: uncommitted changes. Commit or discard them first."
 */
VaultError vault_cmd_checkout(const VaultArgs *args);

/*
 * vault_cmd_diff:
 *   İki versiyon arasındaki farkları gösterir.
 *
 *   Kullanım 1: vault diff <dosya>
 *     → Dosyanın mevcut hali ile index'teki hali arasındaki fark
 *
 *   Kullanım 2: vault diff <hash1> <hash2>
 *     → İki commit arasındaki farklar
 *
 *   Çıktı formatı (basitleştirilmiş diff):
 *     --- a/src/main.c
 *     +++ b/src/main.c
 *     @@ -3,5 +3,7 @@
 *      #include <stdio.h>
 *     -int old_func() {
 *     +int new_func() {
 *     +    // yeni yorum
 */
VaultError vault_cmd_diff(const VaultArgs *args);

/* ---- Diff Engine (Dahili) ----------------------------------------------- */

/*
 * DiffLine: Diff çıktısındaki tek bir satırı temsil eder.
 *
 * op değerleri:
 *   ' ' → Değişmeyen satır (context)
 *   '+' → Eklenen satır
 *   '-' → Silinen satır
 */
typedef struct {
    char  op;           /* '+', '-', veya ' ' */
    char *text;         /* Satır içeriği */
    int   line_old;     /* Eski dosyadaki satır numarası (-1 ise yeni satır) */
    int   line_new;     /* Yeni dosyadaki satır numarası (-1 ise silinen satır) */
} DiffLine;

typedef struct {
    DiffLine *lines;    /* Diff satırları dizisi */
    size_t    count;    /* Satır sayısı */
    size_t    capacity; /* Ayrılmış kapasite */
} DiffResult;

/*
 * vault_diff_compute:
 *   İki metin arasındaki farkları hesaplar.
 *   Basit bir LCS (Longest Common Subsequence) algoritması kullanılabilir.
 *
 *   Parametreler:
 *     old_text  → Eski dosya içeriği
 *     old_size  → Eski dosya boyutu
 *     new_text  → Yeni dosya içeriği
 *     new_size  → Yeni dosya boyutu
 *     result    → Diff sonucu (çıktı, çağıran free etmeli)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 */
VaultError vault_diff_compute(const char *old_text, size_t old_size,
                              const char *new_text, size_t new_size,
                              DiffResult *result);

/*
 * vault_diff_print:
 *   DiffResult'ı terminale formatlanmış şekilde yazdırır.
 *   (Yeşil/kırmızı ANSI renkleri opsiyonel bonus)
 */
void vault_diff_print(const DiffResult *result,
                      const char *old_path, const char *new_path);

/*
 * vault_diff_free:
 *   DiffResult yapısını temizler.
 */
void vault_diff_free(DiffResult *result);

/* ---- Yardımcı ----------------------------------------------------------- */

/*
 * vault_cmd_help:
 *   Kullanım bilgisini yazdırır.
 *
 *   Çıktı:
 *     usage: vault <command> [<args>]
 *
 *     Commands:
 *       init       Create a new vault repository
 *       add        Stage files for commit
 *       commit     Record changes to the repository
 *       log        Show commit history
 *       status     Show working directory status
 *       checkout   Restore a previous commit
 *       diff       Show differences between versions
 */
void vault_cmd_help(void);

/*
 * vault_args_free:
 *   VaultArgs yapısındaki targets dizisini temizler.
 */
void vault_args_free(VaultArgs *args);

/* ---- Ana Dağıtıcı (Dispatcher) ----------------------------------------- */

/*
 * vault_dispatch:
 *   Parse edilmiş argümanlara göre doğru handler'ı çağırır.
 *   main() fonksiyonundan çağrılacak tek fonksiyon budur.
 *
 *   Kullanım:
 *     int main(int argc, char **argv) {
 *         VaultArgs args;
 *         if (vault_parse_args(argc, argv, &args) != VAULT_OK) {
 *             vault_cmd_help();
 *             return 1;
 *         }
 *         VaultError err = vault_dispatch(&args);
 *         vault_args_free(&args);
 *         return (err == VAULT_OK) ? 0 : 1;
 *     }
 */
VaultError vault_dispatch(const VaultArgs *args);

#endif /* VAULT_CLI_H */
