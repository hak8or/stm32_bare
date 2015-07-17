toolchain=arm-none-eabi-

CC=$(toolchain)gcc
LD=$(toolchain)ld
CP=$(toolchain)objcopy
OBJ=$(toolchain)objdump

OUTPUT=firmware

INTERFACE=interface/stlink-v2-1.cfg
TARGET=target/stm32f3x.cfg

STARTUP=startup.s

# Flags to specify target, using thumb, enabling all errors,
# disabling all optimizations, and whatever -g was.
CFLAGS = -mcpu=cortex-m4 -mthumb -Wall -O0 -g

# Entry point for our makefile
all: clean $(OUTPUT).bin flash run

# Create the binary from our ELF file/
$(OUTPUT).bin: $(OUTPUT).elf
	$(CP) -O binary $< $@

# Make the elf
$(OUTPUT).elf: $(STARTUP) main.cpp
	$(CC) -o $@ $(CFLAGS) -nostartfiles -Wl,-Tlinker_script.ld $^

dump_asm: $(OUTPUT).elf
	$(OBJ) -S $(OUTPUT).elf

clean:
	rm -r -f $(OUTPUT).bin $(OUTPUT).elf *.o

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

run: $(OUTPUT).bin
	openocd -f $(INTERFACE) -f $(TARGET) \
	-c init \
  -c "reset" \
	-c shutdown
