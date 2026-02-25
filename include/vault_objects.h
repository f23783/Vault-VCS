/*
 * ============================================================================
 *  vault_objects.h — Vault Nesne Modeli (Fiziksel Katman Arayüzü)
 * ============================================================================
 *
 *  Bu dosya "The Architect" rolündeki kişinin implement edeceği fonksiyonların
 *  İMZALARINI tanımlar. Diğer üyeler bu fonksiyonları çağıracak ama içlerini
 *  bilmek zorunda değil.
 *
 *  Temel sorumluluklar:
 *    - SHA-256 hash üretimi
 *    - Blob/Tree/Commit nesnelerini diske yazma (zlib sıkıştırmalı)
 *    - Diskten okuma (zlib açma)
 *    - .vault/objects/ dizin yapısını yönetme
 *
 *  Kullanılan kütüphaneler: OpenSSL (SHA-256), zlib (sıkıştırma)
 * ============================================================================
 */

#ifndef VAULT_OBJECTS_H
#define VAULT_OBJECTS_H

#include <stddef.h>  /* size_t için */
#include <stdint.h>  /* uint8_t gibi sabit boyutlu tipler için */

/* ---- Sabitler ----------------------------------------------------------- */

/*
 * SHA-256 hash'i 64 hex karakter + '\0' = 65 byte.
 * Her yerde aynı boyutu kullanmak için sabitle tanımlıyoruz.
 */
#define VAULT_HASH_HEX_SIZE 65

/*
 * .vault dizini altındaki objects klasöründe hash'in ilk 2 karakteri
 * alt klasör ismi olarak kullanılır (Git'teki gibi).
 *   Örnek: hash = "a1b2c3d4..."
 *          Yol  = .vault/objects/a1/b2c3d4...
 */
#define VAULT_OBJECTS_DIR ".vault/objects"

/* ---- Nesne Tipleri ------------------------------------------------------ */

/*
 * Vault'ta üç tür nesne var. Diske yazarken nesnenin başına
 * bu tip bilgisi eklenir, böylece okurken ne olduğunu anlarız.
 *
 *   "blob <boyut>\0<içerik>"    → Dosya içeriği
 *   "tree <boyut>\0<içerik>"    → Klasör listesi
 *   "commit <boyut>\0<içerik>"  → Anlık görüntü kaydı
 */
typedef enum {
    VAULT_OBJ_BLOB,
    VAULT_OBJ_TREE,
    VAULT_OBJ_COMMIT
} VaultObjectType;

/* ---- Veri Yapıları ------------------------------------------------------ */

/*
 * Blob: Bir dosyanın ham içeriğini temsil eder.
 *
 * Örnek kullanım:
 *   VaultBlob blob;
 *   blob.data = dosya_icerigi;    // "hello world\n"
 *   blob.size = 12;
 *   blob_write(&blob, hash_out);  // → diske yazar, hash'i döner
 */
typedef struct {
    uint8_t *data;      /* Dosyanın ham byte içeriği */
    size_t   size;      /* İçeriğin byte cinsinden boyutu */
} VaultBlob;

/*
 * TreeEntry: Tree nesnesinin içindeki tek bir satır.
 * Bir klasördeki her dosya/alt-klasör için bir TreeEntry var.
 *
 * Örnek:
 *   { .mode = "100644", .name = "main.c", .hash = "a1b2c3..." }
 *   { .mode = "040000", .name = "src",    .hash = "d4e5f6..." }
 *
 * mode değerleri:
 *   "100644" → normal dosya (blob)
 *   "040000" → alt klasör (başka bir tree)
 */
typedef struct {
    char mode[8];                       /* Dosya modu: "100644" veya "040000" */
    char name[256];                     /* Dosya/klasör adı */
    char hash[VAULT_HASH_HEX_SIZE];    /* Bu girişin işaret ettiği nesne hash'i */
} VaultTreeEntry;

/*
 * Tree: Bir klasörün anlık görüntüsü.
 * İçinde birden fazla TreeEntry tutar.
 *
 * Bellekte dinamik dizi olarak yönetilir:
 *   tree.entries    → malloc ile ayrılmış dizi
 *   tree.count      → şu anki eleman sayısı
 *   tree.capacity   → dizinin toplam kapasitesi
 */
typedef struct {
    VaultTreeEntry *entries;    /* TreeEntry dizisi (dinamik) */
    size_t          count;      /* Dizideki mevcut eleman sayısı */
    size_t          capacity;   /* Dizinin ayrılmış kapasitesi */
} VaultTree;

/*
 * Commit: Projenin belirli bir andaki tam durumunu kaydeder.
 *
 * Örnek:
 *   tree_hash   = "abc123..."   → root tree'nin hash'i
 *   parent_hash = "def456..."   → bir önceki commit (ilk commit'te boş)
 *   author      = "Arda"
 *   message     = "İlk commit: proje yapısı oluşturuldu"
 *   timestamp   = 1719500000    → Unix epoch zamanı
 */
typedef struct {
    char    tree_hash[VAULT_HASH_HEX_SIZE];     /* Root tree nesnesinin hash'i */
    char    parent_hash[VAULT_HASH_HEX_SIZE];   /* Önceki commit hash (ilk commit'te "\0") */
    char    author[128];                         /* Yazar adı */
    char    message[512];                        /* Commit mesajı */
    long    timestamp;                           /* Unix zaman damgası */
} VaultCommit;

/* ---- Hata Kodları ------------------------------------------------------- */

/*
 * Tüm fonksiyonlar bu hata kodlarından birini döner.
 * Başarılıysa VAULT_OK (0), değilse negatif bir değer.
 */
typedef enum {
    VAULT_OK            =  0,   /* İşlem başarılı */
    VAULT_ERR_IO        = -1,   /* Dosya okuma/yazma hatası */
    VAULT_ERR_HASH      = -2,   /* SHA-256 hesaplama hatası */
    VAULT_ERR_COMPRESS  = -3,   /* zlib sıkıştırma/açma hatası */
    VAULT_ERR_NOMEM     = -4,   /* Bellek ayırma (malloc) başarısız */
    VAULT_ERR_NOTFOUND  = -5,   /* Nesne disktte bulunamadı */
    VAULT_ERR_CORRUPT   = -6    /* Nesne bozuk veya okunamıyor */
} VaultError;

/* ---- Fonksiyon İmzaları (Architect'in implement edeceği) --------------- */

/*
 * vault_hash_content:
 *   Verilen byte dizisinin SHA-256 hash'ini hesaplar.
 *
 *   Parametreler:
 *     data     → Hash'lenecek ham veri
 *     size     → Verinin boyutu (byte)
 *     out_hash → Sonucun yazılacağı buffer (en az VAULT_HASH_HEX_SIZE)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 *
 *   Örnek:
 *     char hash[VAULT_HASH_HEX_SIZE];
 *     vault_hash_content("hello", 5, hash);
 *     // hash = "2cf24dba5fb0a30e26e83b2ac5b9e29e..."
 */
VaultError vault_hash_content(const uint8_t *data, size_t size,
                              char out_hash[VAULT_HASH_HEX_SIZE]);

/*
 * vault_object_write:
 *   Bir nesneyi (blob, tree veya commit) zlib ile sıkıştırıp
 *   .vault/objects/<ilk2>/<kalan> yoluna yazar.
 *
 *   Parametreler:
 *     type     → Nesne tipi (BLOB, TREE, COMMIT)
 *     data     → Nesnenin ham içeriği
 *     size     → İçeriğin boyutu
 *     out_hash → Yazılan nesnenin hash'i (çıktı)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 *
 *   Not: Bu fonksiyon "atomik yazma" yapmalı:
 *        Önce geçici dosyaya yaz, sonra rename() ile taşı.
 *        Böylece yarıda kalan yazma işlemleri veriyi bozmaz.
 */
VaultError vault_object_write(VaultObjectType type,
                              const uint8_t *data, size_t size,
                              char out_hash[VAULT_HASH_HEX_SIZE]);

/*
 * vault_object_read:
 *   Hash'i verilen nesneyi diskten okuyup, sıkıştırmayı açıp döner.
 *
 *   Parametreler:
 *     hash     → Okunacak nesnenin hash'i
 *     out_data → Okunan veri (malloc ile ayrılır, ÇAĞIRAN free() YAPMALI)
 *     out_size → Okunan verinin boyutu (çıktı)
 *     out_type → Nesnenin tipi (çıktı)
 *
 *   Dönüş: VAULT_OK veya hata kodu
 *
 *   ⚠️ Bellek yönetimi: Bu fonksiyon out_data için bellek ayırır.
 *      Çağıran taraf kullanım sonrası free(out_data) yapmalıdır!
 */
VaultError vault_object_read(const char hash[VAULT_HASH_HEX_SIZE],
                             uint8_t **out_data, size_t *out_size,
                             VaultObjectType *out_type);

/*
 * vault_object_exists:
 *   Verilen hash'e sahip bir nesnenin diskte olup olmadığını kontrol eder.
 *
 *   Dönüş: 1 = var, 0 = yok
 */
int vault_object_exists(const char hash[VAULT_HASH_HEX_SIZE]);

/* ---- Yardımcı Fonksiyonlar ---------------------------------------------- */

/*
 * vault_blob_write:
 *   Bir VaultBlob yapısını nesne olarak diske yazar.
 *   (vault_object_write etrafında kolaylık wrapper'ı)
 */
VaultError vault_blob_write(const VaultBlob *blob,
                            char out_hash[VAULT_HASH_HEX_SIZE]);

/*
 * vault_tree_serialize / vault_tree_deserialize:
 *   Tree nesnesini byte dizisine çevirir / byte dizisinden geri yükler.
 *
 *   Serileştirme formatı (her satır bir entry):
 *     "<mode> <hash> <name>\n"
 *
 *   Örnek:
 *     "100644 a1b2c3d4... main.c\n040000 e5f6a7b8... src\n"
 */
VaultError vault_tree_serialize(const VaultTree *tree,
                                uint8_t **out_data, size_t *out_size);

VaultError vault_tree_deserialize(const uint8_t *data, size_t size,
                                  VaultTree *out_tree);

/*
 * vault_commit_serialize / vault_commit_deserialize:
 *   Commit nesnesini byte dizisine çevirir / geri yükler.
 *
 *   Serileştirme formatı:
 *     "tree <tree_hash>\n"
 *     "parent <parent_hash>\n"     (ilk commit'te bu satır olmaz)
 *     "author <author> <timestamp>\n"
 *     "\n"
 *     "<commit mesajı>"
 */
VaultError vault_commit_serialize(const VaultCommit *commit,
                                  uint8_t **out_data, size_t *out_size);

VaultError vault_commit_deserialize(const uint8_t *data, size_t size,
                                    VaultCommit *out_commit);

/* ---- Bellek Yönetimi ---------------------------------------------------- */

/*
 * Dinamik olarak ayrılmış tree yapısını temizler.
 * tree->entries için free() çağırır ve count/capacity sıfırlar.
 */
void vault_tree_free(VaultTree *tree);

#endif /* VAULT_OBJECTS_H */
