PROJECT := firmware
BUILD_DIR := ./build

LDSCRIPT = CubeMX/STM32F303RETx_FLASH.ld

C_DEFS =  \
-DSTM32F303xE

C_SOURCES = \
src/main.c \
CubeMX/system_stm32f3xx.c \
src/os.c \
src/allocator.c \
src/timers.c \
src/tcb.c \
src/synchro.c \
src/io.c \
src/sys.c

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
CPU = -mcpu=cortex-m4
FPU = 
#-mfpu=fpv4-sp-d16
FLOAT-ABI = 
#-mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

LIBS = -lc -lm -lnosys


# 
# Debug Build Settings
# 
DBG_BUILD_DIR := ./build/debug
DBG_OPT := -g -O0 -Wall -Wextra -Wnull-dereference -Wno-override-init -fdata-sections -ffunction-sections -fno-exceptions
DBG_CFLAGS := $(MCU) $(C_DEFS) $(C_INCLUDES) $(DBG_OPT) 
DBG_OBJ_FILES = $(addprefix $(DBG_BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
DBG_OBJ_FILES += $(addprefix $(DBG_BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
DBG_LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(DBG_BUILD_DIR)/$(PROJECT).map,--cref -Wl,--gc-sections

# 
# Release Build Settings
# 
REL_BUILD_DIR := ./build/release
REL_OPT := -Os -Wall -Wextra -Wnull-dereference -Wno-override-init -fdata-sections -ffunction-sections -fno-exceptions
REL_CFLAGS := $(MCU) $(C_DEFS) $(C_INCLUDES) $(REL_OPT) 
REL_OBJ_FILES = $(addprefix $(REL_BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
REL_OBJ_FILES += $(addprefix $(REL_BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.c $(sort $(dir $(ASM_SOURCES)))
REL_LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(REL_BUILD_DIR)/$(PROJECT).map,--cref -Wl,--gc-sections


.PHONY: all clean debug prep release remake
all: prep release

.SECONDARY: $(REL_OBJ_FILES)
.PRECIOUS: $(REL_BUILD_DIR)/%.elf

# 
# Release Rules
# 

.PHONY: release

.SECONDARY: $(REL_OBJ_FILES)
.PRECIOUS: $(REL_BUILD_DIR)/%.elf

release: $(REL_BUILD_DIR)/$(PROJECT).elf $(REL_BUILD_DIR)/$(PROJECT).asm $(REL_BUILD_DIR)/$(PROJECT).bin

$(REL_BUILD_DIR)/%.bin: $(REL_BUILD_DIR)/%.elf
	$(OBJCOPY) $< $@

$(REL_BUILD_DIR)/%.asm: $(REL_BUILD_DIR)/%.elf
	$(OBJDUMP) -S $< > $@

$(REL_BUILD_DIR)/$(PROJECT).elf: $(REL_OBJ_FILES) Makefile
	$(CC) $(REL_OBJ_FILES) $(REL_LDFLAGS) -o $@
	$(SIZE) $@

# $(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
# 	$(CC) $(CFLAGS) -c -o $@ $<

$(REL_BUILD_DIR)/%.o: %.c Makefile | $(REL_BUILD_DIR) 
	$(CC) -c $(REL_CFLAGS) -Wa,-a,-ad,-alms=$(REL_BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(REL_BUILD_DIR)/%.o: %.s Makefile | $(REL_BUILD_DIR)
	$(AS) -c $(REL_CFLAGS) $< -o $@

$(REL_BUILD_DIR):
	mkdir -p $(REL_BUILD_DIR)

# 
# Debug Rules
# 

.PHONY: debug
debug: $(DBG_BUILD_DIR)/$(PROJECT).elf $(DBG_BUILD_DIR)/$(PROJECT).asm $(DBG_BUILD_DIR)/$(PROJECT).bin

$(DBG_BUILD_DIR)/%.bin: $(DBG_BUILD_DIR)/%.elf
	$(OBJCOPY) $< $@

$(DBG_BUILD_DIR)/%.asm: $(DBG_BUILD_DIR)/%.elf
	$(OBJDUMP) -S $< > $@

$(DBG_BUILD_DIR)/$(PROJECT).elf: $(DBG_OBJ_FILES) Makefile
	$(CC) $(DBG_OBJ_FILES) $(DBG_LDFLAGS) -o $@
	$(SIZE) $@

# $(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
# 	$(CC) $(CFLAGS) -c -o $@ $<

$(DBG_BUILD_DIR)/%.o: %.c Makefile | $(DBG_BUILD_DIR) 
	$(CC) -c $(DBG_CFLAGS) -Wa,-a,-ad,-alms=$(DBG_BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(DBG_BUILD_DIR)/%.o: %.s Makefile | $(DBG_BUILD_DIR)
	$(AS) -c $(DBG_CFLAGS) $< -o $@

$(DBG_BUILD_DIR):
	mkdir -p $(DBG_BUILD_DIR)

prep:
	@mkdir -p $(DBG_BUILD_DIR) $(REL_BUILD_DIR)

# clean:
# 	rm -f $(DBG_BUILD_DIR) $(REL_BUILD_DIR) $(DBG_OBJ_FILES) $(REL_OBJ_FILES)
