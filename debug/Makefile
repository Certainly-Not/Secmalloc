SRC_DIR := src
BUILD_DIR := build
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
BUILD_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%,$(SRC_FILES))

SECMALLOC_OBJ = ../build/*.o
CC := gcc
CFLAGS := -g

$(BUILD_DIR)/%: $(SRC_DIR)/%.c $(LIB_FILE)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(SECMALLOC_OBJ) $@.o -o $@
	rm -f $(BUILD_DIR)/*.o
all: $(BUILD_FILES)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
