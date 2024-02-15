# Compiler and tools
CC := aarch64-linux-gnu-gcc
LD := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy
DD := dd

# Source and object files
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o) start.o
TARGET := app_with_gap.bin

# Flags
CFLAGS := -c -nostdlib -nostartfiles
LDFLAGS := -T linker.ld
DDFLAGS := if=/dev/zero bs=1 count=512 of=gap.bin

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o app.elf $(OBJECTS)
	$(OBJCOPY) -O binary app.elf app.bin
	$(DD) $(DDFLAGS)
	cat gap.bin app.bin > $(TARGET)

# To compile any .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Special rule for start.S
start.o: start.S
	$(CC) $(CFLAGS) -o start.o start.S

# Clean rule
clean:
	rm -f $(OBJECTS) app.elf app.bin gap.bin $(TARGET)

.PHONY: all clean
