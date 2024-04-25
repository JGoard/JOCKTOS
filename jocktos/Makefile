PROJECT := firmware
BUILD_DIR := ./build

LDSCRIPT = CubeMX/STM32F303RETx_FLASH.ld

C_DEFS =  \
-DSTM32F303xE

C_SOURCES = \
src/main.c \
CubeMX/system_stm32f3xx.c \
src/os.c \
src/timers.c \
src/tcb.c \
src/synchro.c

C_INCLUDES = \
-ICubeMX/ \
-Iinc

ASM_SOURCES = \
CubeMX/startup_stm32f303xe.s

# toolchain settings
CROSS := arm-none-eabi-
CC := $(CROSS)gcc
AS = $(CROSS)gcc -x assembler-with-cpp
OBJDUMP := $(CROSS)objdump
OBJCOPY := $(CROSS)objcopy
SIZE := $(CROSS)size

# gcc & ld compiler flags
OPT := -g
CPU = -mcpu=cortex-m4
FPU = 
#-mfpu=fpv4-sp-d16
FLOAT-ABI = 
#-mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

CFLAGS := $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fno-exceptions

LIBS = -lc -lm -lnosys
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map,--cref -Wl,--gc-sections

OBJ_FILES = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJ_FILES += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

.SECONDARY: $(OBJ_FILES)
.PRECIOUS: $(BUILD_DIR)/%.elf

.PHONY: all
all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).asm $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) $< $@

$(BUILD_DIR)/%.asm: $(BUILD_DIR)/%.elf
	$(OBJDUMP) -S $< > $@

$(BUILD_DIR)/$(PROJECT).elf: $(OBJ_FILES) Makefile
	$(CC) $(OBJ_FILES) $(LDFLAGS) -o $@
	$(SIZE) $@

# $(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
# 	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*
