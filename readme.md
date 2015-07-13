# STM32 barebones setup
Barebones STM32 CLI based project for the nucle-F303RE board.

## Chip Specifications
MCU | STM32F303RET6
--- | ---
CPU Core | ARM®32-bit Cortex®-M4 CPU
Speed    | 72 Mhz
LED pin  | D13 or PA_5

Item     | Size                      | Map
--- | --- | ---
RAM      | 64 KB                     | 0x2000 0000 - 0x2001 0000
Flash    | 512 KB                    | 0x0800 0000 - 0x0808 0000

## Resources
- [Exception Vector](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABIFJFG.html)
- [Barebones example](http://wiki.seabright.co.nz/wiki/HelloSTM32.html)
- [Awesome explanation on SO of barebones example](http://electronics.stackexchange.com/questions/30736/stm32f2-makefile-linker-script-and-start-up-file-combination-without-commercia)
- [Even awesomer explanation of barebones example](http://fun-tech.se/stm32/linker/index.php)
- [Nucle-F303RE](https://developer.mbed.org/platforms/ST-Nucleo-F303RE/)
- [MCU Datasheet](http://www.st.com/web/catalog/mmc/FM141/SC1169/SS1576/LN1531/PF259246?s_searchtype=partnumber)

## Getting started
Getting everything setup might present some difficulty depending on how familiar you are with gdb, openocd, and debugging in general for embedded applications. On the high level, you have openocd talking to the STM32 chip over SWD using the on board ST-Link V2. You can actually telnet into the openocd server on port 4444 and do basic functions like flashing the chip, reseting, halting, etc. To compile, flash, and run, just running ```make``` will suffice.

#### Cross Compiler
Most distro's have an ARM cross compiler in their repository (Arch has [one](https://www.archlinux.org/packages/community/x86_64/arm-none-eabi-gcc/)), but for whatever reason you may want to get an ARM maintained version of GCC [here](https://launchpad.net/gcc-arm-embedded) manually, which is what I am using.
First we need to make sure our cross compiler is accessible by either your path variable or symlinking it to /usr/bin. I added this to my .zshrc so the toolchain is visible via the path variable.
```bash
# Gotta add our arm compiler stuff to path.
PATH="$PATH:/home/hak8or/Desktop/arm_embedded/gcc-arm-none-eabi-4_9-2015q2/bin"
```
The makefile has most of what we need, with ```reset && make clean && make && make dump_asm``` compiling the project and showing the objdump.

#### Flashing
To program the chip, we just run ```make flash```.

#### GDB
And now for GDB, which may be problematic depending on which distro you are using since GDB seems to require the 32 bit ncurses library. For example, I am using Arch which requires the following changes:
```bash
yaourt -S ncurses        # Install ncurses
vim /etc/pacman.conf     # Enable multilib by uncommenting both [multilib]
                         # and the include path.
yaourt -Syu              # Get the multilib mirror.
youart -S lib32-ncurses  # Install the 32 bit library.
```

Start up openocd using ```openocd -f interface/stlink-v2-1.cfg -f target/stm32f3x.cfg``` and to start up gdb do ```arm-none-eabi-gdb --tui --eval-command="target remote localhost:3333" firmware.elf```. This loads the elf file including all it's metadata into gdb and talks to openocd over port 3333. Here is a list of commands you can run in GDB, through make sure to use help often because GDB offers a *ton* of functionality.

Commands           | Description
---                | ---
```s```            | Single step source C/C++.
```mon reg```      | Dumps the registers.
```mon reg pc```   | Dumps a specific register, like PC.
```display foo```  | Dumps the contents of variable foo.
```info frame```   | Dumps the current stack frame.
```layout split``` | Display the C/C++ source and assembly view.
```layout regs```  | Display the registers and source
```stepi```        | Single step assembly.
```break 5```      | Set a breakpoint on C/C++ source line 5.
```run```          | Run forever or till we hit a breakpoint.
