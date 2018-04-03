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
3. Boot the tablet into Fastboot mode: On the stock ROM, press Volume Down + Power, and release the Power button
   once the backlight turns on.
4. Connect the tablet to your PC, and verify that it shows up in `fastboot devices`.
5. Flash the new EFI system partition extracted from the downloaded ZIP: `fastboot flash ESP esp.img`

### Troubleshooting
In some cases booting may fail or you make a mistake during the installation. In this case, first power off
the tablet by holding the Power button until it turns off.

#### Recovery
Even with the bootloader entirely broken, you can still boot into Fastboot mode using the rescue mode ("DNX mode").

1. Download the [stock ROM (`UL-K013-WW-12.10.1.36-user.zip`)](http://dlcdnet.asus.com/pub/ASUS/EeePAD/ME176C/UL-K013-WW-12.10.1.36-user.zip)
2. Extract `esp.zip` and `droidboot.img` from the downloaded ZIP.
3. Extract `EFI/Intel/efilinux.efi` from `esp.zip`.
4. Press Volume Up + Volume Down + Power until "Fastboot starting..." shows up on the display.
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
