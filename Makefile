# Compiler and tools
CC := aarch64-linux-gnu-gcc
LD := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy
DD := dd

# Source and object files
SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:.c=.o) src/start.o
TARGET := app_with_gap.bin

# Flags
CFLAGS := -c -nostdlib -nostartfiles -ffreestanding
LDFLAGS := -T src/linker.ld
DDFLAGS := if=/dev/zero bs=1 count=512 of=gap.bin

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o app.elf $(OBJECTS)
	$(OBJCOPY) -O binary app.elf app.bin
	$(DD) $(DDFLAGS)
	cat gap.bin app.bin > $(TARGET)

# To compile any .s and .c to .o
%.o: %.s %.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean rule
clean:
	rm -f $(OBJECTS) app.elf app.bin gap.bin $(TARGET)

.PHONY: all clean
