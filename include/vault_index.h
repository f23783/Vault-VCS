/*
 * ============================================================================
 *  vault_index.h — Staging Area ve Tree Builder (Mantıksal Katman Arayüzü)
 * ============================================================================
 *
 *  Bu dosya "The Weaver" rolündeki kişinin implement edeceği fonksiyonları
 *  tanımlar. Index (staging area) yönetimi ve commit oluşturma mantığı
 *  bu katmandadır.
 *
 *  Temel sorumluluklar:
 *    - .vault/index dosyasını okuma/yazma
 *    - "vault add" ile dosyaları staging'e ekleme
 *    - Tree nesnelerini oluşturma (klasör yapısını tree'ye çevirme)
 *    - Commit nesnesi oluşturma ve parent zincirini yönetme
 *
 *  Bağımlılık: vault_objects.h (hash, read/write fonksiyonlarını kullanır)
 * ============================================================================
 */

#ifndef VAULT_INDEX_H
#define VAULT_INDEX_H

#include "vault_objects.h"
#include <time.h>

/* ---- Sabitler ----------------------------------------------------------- */

#define VAULT_INDEX_FILE  ".vault/index"
#define VAULT_HEAD_FILE   ".vault/HEAD"    /* Şu anki commit hash'ini tutar */
#define VAULT_MAX_PATH    1024

/* ---- Veri Yapıları ------------------------------------------------------ */

/*
 * IndexEntry: Staging area'daki tek bir dosya kaydı.
 *
 * "vault add main.c" dediğinde şu olur:
 *   1. main.c'nin içeriği okunur
 *   2. SHA-256 hash'i hesaplanır
 *   3. Blob olarak .vault/objects'e yazılır
 *   4. Bu struct'a dosya yolu ve hash kaydedilir
 *
 * Örnek:
 *   { .filepath = "src/main.c", .hash = "a1b2c3...", .mtime = 1719500000 }
 */
typedef struct {
    char filepath[VAULT_MAX_PATH];      /* Dosyanın repo kökünden göreceli yolu */
    char hash[VAULT_HASH_HEX_SIZE];     /* Dosyanın blob hash'i */
    long mtime;                         /* Son değişiklik zamanı (cache için) */
} IndexEntry;

/*
 * VaultIndex: Tüm staging area'yı temsil eder.
 * .vault/index dosyasının bellekteki hali.
 *
 * Dinamik dizi olarak yönetilir (tıpkı VaultTree gibi).
 */
typedef struct {
    IndexEntry *entries;    /* IndexEntry dizisi (dinamik) */
    size_t      count;      /* Mevcut kayıt sayısı */
    size_t      capacity;   /* Ayrılmış kapasite */
} VaultIndex;

/* ---- Index (Staging Area) Fonksiyonları --------------------------------- */

/*
 * vault_index_load:
 *   .vault/index dosyasını okuyup VaultIndex yapısına yükler.
 *   Dosya yoksa boş bir index döner (ilk kullanımda normal).
 *
 *   Parametreler:
 *     idx → Yüklenecek VaultIndex yapısı (çağıran oluşturur)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 *
 *   Örnek:
 *     VaultIndex idx;
 *     vault_index_load(&idx);
 *     printf("Staging'de %zu dosya var\n", idx.count);
 */
VaultError vault_index_load(VaultIndex *idx);

/*
 * vault_index_save:
 *   Bellekteki VaultIndex'i .vault/index dosyasına yazar.
 *   Atomik yazma kullanılmalı (geçici dosya → rename).
 *
 *   Index dosya formatı (her satır bir entry):
 *     "<hash> <mtime> <filepath>\n"
 *
 *   Örnek dosya içeriği:
 *     a1b2c3d4e5... 1719500000 src/main.c
 *     f6a7b8c9d0... 1719500100 include/utils.h
 */
VaultError vault_index_save(const VaultIndex *idx);

/*
 * vault_index_add:
 *   Bir dosyayı staging area'ya ekler.
 *
 *   Bu fonksiyon şunları yapmalı:
 *     1. Dosyayı diskten oku
 *     2. vault_hash_content() ile hash'le
 *     3. vault_blob_write() ile blob olarak kaydet
 *     4. Index'te aynı filepath varsa güncelle, yoksa yeni entry ekle
 *
 *   Parametreler:
 *     idx      → Güncellenecek index
 *     filepath → Eklenecek dosyanın yolu (repo köküne göreceli)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 *
 *   Örnek:
 *     vault_index_add(&idx, "src/main.c");
 *     vault_index_add(&idx, "README.md");
 *     vault_index_save(&idx);  // değişiklikleri diske yaz
 */
VaultError vault_index_add(VaultIndex *idx, const char *filepath);

/*
 * vault_index_remove:
 *   Bir dosyayı staging area'dan çıkarır.
 *   Dosyanın kendisi silinmez, sadece takipten çıkar.
 */
VaultError vault_index_remove(VaultIndex *idx, const char *filepath);

/*
 * vault_index_find:
 *   Verilen dosya yolunun index'teki konumunu bulur.
 *
 *   Dönüş: Entry'nin index'teki pozisyonu, bulunamazsa -1
 */
int vault_index_find(const VaultIndex *idx, const char *filepath);

/* ---- Tree Oluşturma ----------------------------------------------------- */

/*
 * vault_build_tree:
 *   Index'teki tüm dosya yollarından bir Tree hiyerarşisi oluşturur
 *   ve diske yazar.
 *
 *   Bu en karmaşık fonksiyonlardan biri. Şunu yapması gerekiyor:
 *
 *   Diyelim index şöyle:
 *     src/main.c    → hash_a
 *     src/utils.c   → hash_b
 *     README.md     → hash_c
 *
 *   Fonksiyon şu tree yapısını oluşturmalı:
 *
 *     Root Tree:
 *       040000 <src_tree_hash> src
 *       100644 <hash_c>        README.md
 *
 *     src/ Tree:
 *       100644 <hash_a> main.c
 *       100644 <hash_b> utils.c
 *
 *   Her alt-tree önce yazılır, hash'i alınır, üst tree'ye eklenir.
 *   Bu süreç özyinelemeli (recursive) olarak yapılır.
 *
 *   Parametreler:
 *     idx           → Mevcut index (dosya listesi)
 *     out_tree_hash → Root tree'nin hash'i (çıktı)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 */
VaultError vault_build_tree(const VaultIndex *idx,
                            char out_tree_hash[VAULT_HASH_HEX_SIZE]);

/* ---- Commit Oluşturma --------------------------------------------------- */

/*
 * vault_create_commit:
 *   Mevcut index'ten yeni bir commit oluşturur.
 *
 *   İşlem sırası:
 *     1. vault_build_tree() ile index'ten root tree oluştur
 *     2. vault_head_read() ile mevcut HEAD'i oku (parent olacak)
 *     3. VaultCommit struct'ını doldur
 *     4. Serialize edip nesne olarak yaz
 *     5. HEAD dosyasını yeni commit hash'iyle güncelle
 *
 *   Parametreler:
 *     idx         → Mevcut staging area
 *     author      → Yazar adı
 *     message     → Commit mesajı
 *     out_hash    → Yeni commit'in hash'i (çıktı)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 */
VaultError vault_create_commit(const VaultIndex *idx,
                               const char *author,
                               const char *message,
                               char out_hash[VAULT_HASH_HEX_SIZE]);

/* ---- HEAD Yönetimi ------------------------------------------------------ */

/*
 * vault_head_read:
 *   .vault/HEAD dosyasından şu anki commit hash'ini okur.
 *   Eğer henüz hiç commit yapılmamışsa out_hash[0] = '\0' olur.
 */
VaultError vault_head_read(char out_hash[VAULT_HASH_HEX_SIZE]);

/*
 * vault_head_write:
 *   .vault/HEAD dosyasına yeni commit hash'ini yazar.
 */
VaultError vault_head_write(const char hash[VAULT_HASH_HEX_SIZE]);

/* ---- Değişiklik Tespiti ------------------------------------------------- */

/*
 * vault_status:
 *   Çalışma dizinindeki dosyaları index ile karşılaştırır.
 *   Üç kategori tespit eder:
 *     - Değişmiş dosyalar (hash farklı)
 *     - Yeni dosyalar (index'te yok)
 *     - Silinmiş dosyalar (index'te var ama diskte yok)
 *
 *   Parametreler:
 *     idx       → Mevcut index
 *     callback  → Her farklılık için çağrılacak fonksiyon
 *     user_data → Callback'e geçirilecek ek veri (NULL olabilir)
 *
 *   Callback parametreleri:
 *     filepath → Dosya yolu
 *     status   → 'M' (modified), 'A' (added/new), 'D' (deleted)
 *     ctx      → user_data'nın kendisi
 */
typedef void (*VaultStatusCallback)(const char *filepath, char status,
                                    void *ctx);

VaultError vault_status(const VaultIndex *idx,
                        VaultStatusCallback callback,
                        void *user_data);

/* ---- Bellek Yönetimi ---------------------------------------------------- */

/*
 * vault_index_free:
 *   Index yapısını temizler. entries dizisini free() eder.
 */
void vault_index_free(VaultIndex *idx);

#endif /* VAULT_INDEX_H */
