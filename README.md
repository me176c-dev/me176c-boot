# me176c-boot [![Build Status](https://travis-ci.com/me176c-dev/me176c-boot.svg?branch=master)](https://travis-ci.com/me176c-dev/me176c-boot)
me176c-boot is a custom, unlocked bootloader for the ASUS MeMO Pad 7 (ME176C(X)). It consists out of a
[fork of systemd-boot](https://github.com/me176c-dev/systemd-boot-me176c), a simple UEFI boot manager
and [android-efi](https://github.com/me176c-dev/android-efi), a simple bootloader for Android™ boot images.

## Features
- Boot into Android, Recovery and Fastboot
- Unlocked bootloader, can boot into custom ROMs
- Completely open-source and custom built for ASUS MeMO Pad (ME176C(X))
- Flexible configuration to add further boot targets (e.g. Linux dual boot)

## Installation
0. Make sure that you are running Android 5.0 (Lollipop).
1. Download and unpack `esp.zip` from [the latest release](https://github.com/me176c-dev/me176c-boot/releases).
2. Make sure you have [Fastboot installed](https://wiki.lineageos.org/adb_fastboot_guide.html).
3. Boot the tablet into Fastboot mode: On the stock ROM, press `Volume Down` + `Power`, and release the `Power` button
   once the backlight turns on.
4. Connect the tablet to your PC, and verify that it shows up in `fastboot devices`.
5. Flash the new EFI System Partition (ESP) extracted from the downloaded ZIP: `fastboot flash ESP esp.img`
6. **Recommended:** Configure the bootloader to [use the stock charger for offline mode charging](/examples/stock-charger)

## Usage
By default, the bootloader will boot into the main Android system.
You can recognize the bootloader by the green Android robot that is shown on boot.

Press `Volume Down` while booting to show the boot menu. Navigate through the menu with the `Volume Down` key.
Press `Volume Up` to select an option. The `Power` button is **not** working inside the boot menu.

### Configuration
You can access the (systemd-boot) configuration files from Recovery:
go to `Mount` and select `EFI System Partiton (ESP)`.
Then you can access the ESP using ADB at `/esp`.

Here are some links with more information how to configure the bootloader:
- [`systemd-boot` on ArchLinux Wiki](https://wiki.archlinux.org/index.php/Systemd-boot)
- [android-efi README](https://github.com/me176c-dev/android-efi#systemd-boot)
  (`android` boot type is supported in me176c-boot)

#### Booting from other partitions
There is an additional `volume` option in me176c-boot that allows booting from
other partitions on the internal storage, based on their GPT partition UUID.
The partition needs to be formatted as FAT32.

#### Examples
- [Using the stock charger](/examples/stock-charger) (recommended)
- [Booting into main OS on charger insertion](/examples/charger-main-os)
- [Dual/multi boot](/examples/multi-boot) (e.g. Android and Linux)

### Limitations
The "BIOS" of the tablet can boot from the internal storage and USB(-OTG).
It does not seem to be able to boot directly from external SD cards.
However, it is possible to keep most of the system on the external SD card
by placing only the kernel (the boot partition) on the internal storage.

The EFI System Partition (ESP) is severely limited in its size (only 16 MiB),
which is usually not enough to keep more than one Linux kernel. That's why
me176c-boot supports booting from other partitions on the internal storage.

#### Setting up an additional ESP partition
There is an unneeded APD (ASUS Product Demo) partition on the tablet, which has
a reasonable amount of storage (~300 MiB) and is therefore ideal as additional
ESP partition.
It contains a few demo product images/media files that are only used for the
"demo mode" on the stock ROM.

1. Boot into TWRP recovery and make a backup of the `APD (ASUS Product Demo)`
   partition. Store in it a safe place in case you want to restore it.
2. Wipe the APD partition, and change its file system to FAT32.
3. Place the EFI application (e.g. Linux) on the APD partition, and use it
   in a boot target using the `volume` option:

    ```
    # Partition UUID of the APD partition
    volume  80868086-8086-8086-8086-000000000007
    ```

### Troubleshooting
In some cases booting may fail or you make a mistake during the installation. In this case, first power off
the tablet by holding the `Power` button until it turns off.

#### Recovery
Even with the bootloader entirely broken, you can still boot into Fastboot mode using the rescue mode ("DNX mode").

1. Download the [stock ROM (`UL-K013-WW-12.10.1.36-user.zip`)](https://dlcdnets.asus.com/pub/ASUS/EeePAD/ME176C/UL-K013-WW-12.10.1.36-user.zip)
2. Extract `esp.zip` and `droidboot.img` from the downloaded ZIP.
3. Extract `EFI/Intel/efilinux.efi` from `esp.zip`.
4. Press `Volume Up` + `Volume Down` + `Power` until "Fastboot starting..." shows up on the display.
5. Connect the tablet to your PC, and verify that it shows up in `fastboot devices`.
6.
    ```
    fastboot flash osloader efilinux.efi
    fastboot boot droidboot.img
    ```

## Building
1. `git clone --recursive https://github.com/me176c-dev/me176c-boot.git`
2. `./build-esp.sh` (some commands require root, make sure you can use `sudo`)
3. `esp.img` and `esp.zip` are in the `build` directory

### Project setup
This repository contains 3 separate components:

- **bootstrap** (`EFI/BOOT/bootx64.efi`): See [bootstrap](bootstrap/README.md).
- **systemd-boot** (`systemd-bootx64.efi`):
  A submodule that points to the current version of the [systemd-boot](https://www.freedesktop.org/wiki/Software/systemd/systemd-boot/) fork.
- **android-efi**: (`android.efi`):
  The bootloader that boots into Android boot images.
