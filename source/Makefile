DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM
include $(DEVKITARM)/gba_rules

TARGET := aimlock
BUILD := build

CFLAGS := -g -Wall -O2 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections
LDFLAGS := -specs=gba.specs -mthumb -mthumb-interwork

OFILES := $(BUILD)/main.o

all: $(TARGET).gba

$(TARGET).gba: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	gbafix $@

$(TARGET).elf: $(OFILES)
	$(LD) $(LDFLAGS) -o $@ $(OFILES) -lgba

$(BUILD)/main.o: source/main.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) $(TARGET).elf $(TARGET).gba
