# me176c-boot
me176c-boot is a custom, unlocked bootloader for the ASUS MeMO Pad 7 (ME176C/X). It consists out of a
[fork of systemd-boot](https://github.com/me176c-dev/systemd-boot-me176c), a simple UEFI boot manager
and [android-efi](https://github.com/me176c-dev/android-efi), a simple bootloader for Android™ boot images.

## Features
- Boot into Android, Recovery and Fastboot
- Unlocked bootloader, can boot into custom ROMs
- Completely open-source and custom built for ASUS MeMO Pad (ME176C/X)
- Flexible configuration to add further boot targets (e.g. Linux dual boot - coming soon)

## Installation
1. Download and unpack `esp.zip` from [the latest release](https://github.com/me176c-dev/me176c-boot/releases).
2. Make sure you have [Fastboot installed](https://wiki.lineageos.org/adb_fastboot_guide.html).
3. Boot the tablet into Fastboot mode: On the stock ROM, press `Volume Down` + `Power`, and release the `Power` button
   once the backlight turns on.
4. Connect the tablet to your PC, and verify that it shows up in `fastboot devices`.
5. Flash the new EFI system partition extracted from the downloaded ZIP: `fastboot flash ESP esp.img`

## Usage
By default, the bootloader will boot into the main Android system.
You can recognize the bootloader by the green Android robot that is shown on boot.

Press `Volume Down` while booting to show the boot menu. Navigate through the menu with the `Volume Down` key.
Press `Volume Up` to select an option. The `Power` button is **not** working inside the boot menu.

### Configuration
You can access the configuration files from Recovery, go to `Mount` and select `EFI System Partiton (ESP)`.
Then you can access the ESP using ADB at `/esp`.

Here are some links with more information how to configure the bootloader:
- [`systemd-boot` on ArchLinux Wiki](https://wiki.archlinux.org/index.php/Systemd-boot)
- [android-efi README](https://github.com/me176c-dev/android-efi#readme)

#### Using the stock charger
You may want to use the stock ROM for (offline) charging since it charges a bit more efficiently (with
less heat being generated). Ideally, this would be fixed directly in the custom ROM, but until then it
is possible to configure the bootloader to use the stock boot image when booting into charging mode:

1. Download the [stock ROM (`UL-K013-WW-12.10.1.36-user.zip`)](http://dlcdnet.asus.com/pub/ASUS/EeePAD/ME176C/UL-K013-WW-12.10.1.36-user.zip)
   and [the updated charger boot configuration (`charger.conf`)](https://gist.github.com/lambdadroid/6de9d6ac316277c8d6966a0e7b5f954d/raw/charger.conf)
2. Extract `boot.img` from the downloaded ZIP.
3. Boot into Recovery, go to `Mount` and select `EFI System Partition (ESP)`.
4. Connect the tablet to your PC, and verify that it shows up in `adb devices`.
5.
    ```
    adb push boot.img /esp/asus-charger.img
    adb push charger.conf /esp/loader/entries/charger.conf
    ```
6. That's it! When you plug in the charger the next time it should boot into the stock charger UI. Yay!

#### Booting into main OS on charger insertion
If you want to disable the separate charging mode on your tablet and always boot into the main OS,
you can simply copy the primary boot configuration, and use it for the charger as well:

1. Boot into Recovery, go to `Mount` and select `EFI System Partition (ESP)`.
2. Connect the tablet to your PC, and verify that it shows up in `adb devices`.
3. Download the configuration for your primary boot configuration (e.g. `android.conf`):

    ```
    adb pull /esp/loader/entries/android.conf
    ```
4. Rename it to `charger.conf`.
5. Open it in a text editor and change the `title` to `Charger`.
6. Copy it back to your device:

    ```
    adb push charger.conf /esp/loader/entries/charger.conf
    ```

### Troubleshooting
In some cases booting may fail or you make a mistake during the installation. In this case, first power off
the tablet by holding the `Power` button until it turns off.

#### Recovery
Even with the bootloader entirely broken, you can still boot into Fastboot mode using the rescue mode ("DNX mode").

1. Download the [stock ROM (`UL-K013-WW-12.10.1.36-user.zip`)](http://dlcdnet.asus.com/pub/ASUS/EeePAD/ME176C/UL-K013-WW-12.10.1.36-user.zip)
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
