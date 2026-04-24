
# project name
TARGET = stm32f411_second_project

BUILD_DIR = build
SRC_DIR = src
LIB_DIR = lib

INC_DIRS = $(SRC_DIR)
INC_DIRS += $(LIB_DIR)/cmsis/Core/Include
INC_DIRS += $(LIB_DIR)/cmsis/Device/Include
INC_DIRS += $(LIB_DIR)/hal/Inc
INC_DIRS += $(LIB_DIR)/startup


# all c files

C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")

# HAL_SOURCES = $(LIB_DIR)/hal/stm32f4xx_hal.c
# HAL_SOURCES += $(LIB_DIR)/hal/stm32f4xx_hal_gpio.c
# HAL_SOURCES += $(LIB_DIR)/hal/stm32f4xx_hal_rpio.c
# HAL_SOURCES += $(LIB_DIR)/hal/stm32f4xx_hal_rcc.c

# init system file
SYSTEM_SOURCE = $(LIB_DIR)/startup/system_stm32f4xx.c

# assembler startup file
ASM_SOURCES = $(LIB_DIR)/startup/startup_stm32f411xe.s

# C_SOURCES += $(HAL_SOURCES) $(SYSTEM_SOURCE)
C_SOURCES += $(SYSTEM_SOURCE)

# compiler, tools, flags

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# CPU and FPU for Cortex-M4F
CPU_FLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Optimization: -os for size, -o0 - for debug

OPT = -O0 -g

CFLAGS = $(CPU_FLAGS) $(OPT) -c -std=gnu11 -Wall -ffunction-sections -fdata-sections
CFLAGS += $(addprefix -I,$(INC_DIRS))
CFLAGS += -DUSE_HAL_DRIVER -DSTM32F411xE # macros for HAL
CFLAGS += -fstack-protector-all # If the buffer overflows on the stack, the program will notice this and go to the error handler.
CFLAGS += -Isrc

LINKER_SCRIPT = $(LIB_DIR)/linker/stm32f411xe.ld

#linking
LDFLAGS = $(CPU_FLAGS) -T $(LINKER_SCRIPT) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
LDFLAGS += --specs=nano.specs --specs=nosys.specs


# object files

# for c files
C_OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))

# for ASM files

ASM_OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(ASM_SOURCES:.s=.o)))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# vpath for compiler
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# build rules
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

# build dir creation

$(BUILD_DIR):
	mkdir -p $@

# c files compiling
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "  CC    $<"
	$(CC) $(CFLAGS) -c $< -o $@

# asm files compiling
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo "  AS    $<"
	$(CC) $(CFLAGS) -c $< -o $@

# linking
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	@echo "  LD    $@"
	$(CC) $^ $(LDFLAGS) -o $@
	$(SIZE) --format=berkeley $@

# firmware creation
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# device firmware
flash: all
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

# usb dfu
dfu: all
	-dfu-util -a 0 -s 0x08000000:leave -R -D $(BUILD_DIR)/$(TARGET).bin

CHECK_TOOL = cppcheck

CHECK_FLAGS = --enable=all --suppress=missingInclude --suppress=missingIncludeSystem  --error-exitcode=1

.PHONY: check

check:
	$(CHECK_TOOL) $(CHECK_FLAGS) src/

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean flash dfu

