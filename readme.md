# STM32 barebones setup
Barebones STM32 CLI based project for the nucle-F303RE board.

### Chip Specifications
MCU | STM32F303RET6
--- | ---
CPU Core | ARM®32-bit Cortex®-M4 CPU
Speed    | 72 Mhz
LED pin  | D13 or PA_5

Item     | Size                      | Map
--- | --- | ---
RAM      | 64 KB                     | 0x2000 0000 - 0x2001 0000
Flash    | 512 KB                    | 0x0800 0000 - 0x0808 0000

### Resources
- [Exception Vector](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABIFJFG.html)
- [Barebones example](http://wiki.seabright.co.nz/wiki/HelloSTM32.html)
- [Awesome explanation on SO of barebones example](http://electronics.stackexchange.com/questions/30736/stm32f2-makefile-linker-script-and-start-up-file-combination-without-commercia)
- [Even awesomer explanation of barebones example](http://fun-tech.se/stm32/linker/index.php)
- [Nucle-F303RE](https://developer.mbed.org/platforms/ST-Nucleo-F303RE/)
- [MCU Datasheet](http://www.st.com/web/catalog/mmc/FM141/SC1169/SS1576/LN1531/PF259246?s_searchtype=partnumber)
