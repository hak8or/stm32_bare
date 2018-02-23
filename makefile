# Various parts of the toolchain we need.
toolchain=arm-none-eabi-
CC=$(toolchain)gcc
LD=$(toolchain)ld
CP=$(toolchain)objcopy
OBJ=$(toolchain)objdump

# Output file
OUTPUT=firmware

# For OpenOCD, used to flash the IC.
INTERFACE=interface/stlink-v2-1.cfg
TARGET=target/stm32f3x.cfg

# Base directory for source
BASEDIR=src

# Assembly file for saying what the interrupt table is and
# the reset handler.
STARTUP=startup.s

# Flags to specify target, using thumb, enabling all errors,
# disabling all optimizations, and including debugging symbols.
CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -Os -g

# Entry point for our makefile
all: clean $(OUTPUT).bin

# Create the binary from our ELF file/
$(OUTPUT).bin: $(OUTPUT).elf
	$(CP) -O binary $< $@

# Compile and link our source.
$(OUTPUT).elf: $(BASEDIR)/$(STARTUP) main.cpp
	$(CC) -o $@ $(CFLAGS) -nostartfiles -Isrc/cmsis -Wl,-T$(BASEDIR)/linker_script.ld $^

# Dump the assembly, helpful for debugging.
dump_asm: $(OUTPUT).elf
	$(OBJ) -S $(OUTPUT).elf

# Delete the binary and .o files.
clean:
	rm -r -f $(OUTPUT).bin $(OUTPUT).elf *.o

# Have OCD flash the binary
flash: $(OUTPUT).bin
	openocd -f $(INTERFACE) -f $(TARGET) \
	-c init \
	-c "reset halt" \
	-c "flash probe 0" \
	-c "stm32f3x mass_erase 0" \
	-c "flash write_bank 0 ${OUTPUT}.bin 0" \
	-c "verify_image ${OUTPUT}.bin" \
	-c "reset halt" \
	-c shutdown

# Have OCD start the newly flashed code.
start: $(OUTPUT).bin
	openocd -f $(INTERFACE) -f $(TARGET) \
	-c init \
	-c "reset" \
	-c shutdown

# Compile, flash, run, and start, all in one command.
full: $(OUTPUT).bin
	make
	make flash
	make start
