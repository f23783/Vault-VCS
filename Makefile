# ===========================================================================
#  Vault-VCS Makefile
# ===========================================================================
#
#  Kullanım:
#    make          → Projeyi derle
#    make clean    → Derleme çıktılarını temizle
#    make test     → Testleri çalıştır
#    make valgrind → Bellek sızıntısı kontrolü
#
# ===========================================================================

CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -std=c11 -g
INCLUDES = -Iinclude
LIBS     = -lssl -lcrypto -lz    # OpenSSL + zlib

# Kaynak dosyaları (her üye kendi dosyasını ekler)
SRC_DIR  = src
SRCS     = $(SRC_DIR)/main.c \
           $(SRC_DIR)/objects.c \
           $(SRC_DIR)/index.c \
           $(SRC_DIR)/cli.c \
           $(SRC_DIR)/diff.c

OBJ_DIR  = build
OBJS     = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET   = vault

# ---- Kurallar -----------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# ---- Test & Debug -------------------------------------------------------

test: $(TARGET)
	@echo "=== Temel testler ==="
	./$(TARGET) init
	echo "merhaba dünya" > test.txt
	./$(TARGET) add test.txt
	./$(TARGET) commit -m "ilk commit"
	./$(TARGET) log
	@echo "=== Testler tamamlandı ==="

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) init

.PHONY: all clean test valgrind
