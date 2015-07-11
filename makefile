toolchain=arm-none-eabi-

CC=$(toolchain)gcc
LD=$(toolchain)ld
CP=$(toolchain)objcopy
OBJ=$(toolchain)objdump

OUTPUT=firmware

STARTUP=startup.s

# Flags to specify target, using thumb, enabling all errors,
# disabling all optimizations, and whatever -g was.
CFLAGS = -mcpu=cortex-m4 -mthumb -Wall -O0 -g

# Entry point for our makefile
all: $(OUTPUT).bin

# Create the binary from our ELF file/
$(OUTPUT).bin: $(OUTPUT).elf
	$(CP) -O binary $< $@

# Make the elf
$(OUTPUT).elf: $(STARTUP) main.cpp
	$(CC) -o $@ $(CFLAGS) -nostartfiles -Wl,-Tlinker_script.ld $^

dump_asm:
	$(OBJ) -S $(OUTPUT).elf

clean:
	rm -r -f $(OUTPUT).bin $(OUTPUT).elf *.o
