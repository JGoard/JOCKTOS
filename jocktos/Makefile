PROJECT := main
BUILD_DIR := ./build

# toolchain settings
CROSS := arm-none-eabi-
CC := $(CROSS)gcc
OBJDUMP := $(CROSS)objdump
OBJCOPY := $(CROSS)objcopy

# gcc & ld compiler flags
OPT := -Og
FLAGS := -mcpu=cortex-m4 -mthumb
CFLAGS := $(FLAGS) $(OPT) -fno-common -g3 -Wall -Werror -Wextra
LDFLAGS := $(FLAGS) -Tlinker/minimal.ld -nostartfiles -nostdlib -lnosys

SRC_FILES := $(wildcard *.c)
OBJ_FILES := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

.SECONDARY: $(OBJ_FILES)
.PRECIOUS: $(BUILD_DIR)/%.elf

.PHONY: all
all: $(BUILD_DIR)/$(PROJECT).asm $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) $< -O binary $@

$(BUILD_DIR)/%.asm: $(BUILD_DIR)/%.elf
	$(OBJDUMP) -S $< > $@

$(BUILD_DIR)/%.elf: $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*
