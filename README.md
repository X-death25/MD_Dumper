# MD Dumper

<img src="https://github.com/X-death25/MD_Dumper/blob/main/github_gfx/md_dumper_title.png" data-canonical-src="https://github.com/X-death25/MD_Dumper/blob/main/github_gfx/md_dumper_title.png" width="100%" /> 

MD-Dumper is an USB-C alternative of Krizz FlashKit programmer you can backup / restore your cartridge and save for use it in Flashcart or Emu.  
8 bit Master System and Game Gear dump is supported.  
You could also use included csv file for increase special game compatibility as well as extended flash memory support for third party cartridges.


Supported Features
-----

| Feature | Implemented |
| ------- | ----------- |
| ROM Dump up to 32 MEG  Classic              | :white_check_mark: |
| ROM Dump up to 128 MEG  Bankswitch          | :white_check_mark: |
| SRAM Backup Memory Classic                  | :white_check_mark: |
| SRAM Backup Memory Bankswitch               | :white_check_mark: |
| Rewrite Krikzz flashkit cart or compatible  | :white_check_mark: |
| Rewrite 64 MEG flash cartridge (bankswitch) | :arrows_counterclockwise: |
| ROM Dump in Sega Lock-on mode               | :white_check_mark: |
| ROM Dump in Super Street Fighter II mapper  | :white_check_mark: |
| Master System / MARK III ROM Dump           | :white_check_mark: |
| GAME GEAR ROM Dump                          | :white_check_mark: |
| Open-Everdrive support                      | :arrows_counterclockwise: |

## How to use it

You could find all documentation in the wiki here :

https://github.com/X-death25/MD_Dumper/wiki

## Preview

<img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/01.png" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/01.png" width="49%" /> <img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/card.jpg" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/card.jpg" width="49%" />

<img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview01.PNG" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview01.png" width="49%" /> <img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview02.PNG" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview02.png" width="49%" />

## Where i can buy it ?

https://www.tindie.com/products/xdeath/usb-megadrive-genesis-cartridge-readerwriter/

## Commands

### CLI Mode

Read mode :
```
-read auto  :  Auto Mode
-read bankswitch  :  Bankswitch Mode
-read manual (32|64|128|256|512|1024|2048|4096) (gg|md|sms) :  Manual Mode
-backup auto  :  Auto Mode
-backup bankswitch  :  Bankswitch Mode
-backup manual (8192|32768) (serial_spi|serial_i2c|parallel_sram):  Read Save Data
```

Write mode :
```
-erase_flash  :  Erase Flash Memory
-write_flash  :  Write Flash Memory
-erase_memory (serial_spi|serial_i2c|parallel_sram) :  Erase Save Memory
-restore_memory (serial_spi|serial_i2c|parallel_sram)  :  Write Save Memory
```

### GUI Mode
```
-gui|-gui_fr  : Using GUI Window (ENG|FR)
```

## Special thanks

- paulfertser for his code on how to use usb hid on stm32 , it's the first piece of my first version of md dumper https://github.com/paulfertser/stm32-tx-hid
- libopencm3 for the very cool STM32 dev lib.
- ichigobankai for some low level arm optimisation and the Master System / Game gear part. 
- Marv17 for a lot of hardware and software test.
- Jackobo (Akina Usagi) for the beautiful GUI.
- Sik for his wonderfull website about Megadrive hardware / dev info https://plutiedev.com/


## Licenses:

- Software : GPL v3
- Hardware : Firmware is closed source
- Documentation / WIKI : CC0 1.0


