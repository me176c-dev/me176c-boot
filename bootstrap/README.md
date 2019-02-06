# me176c-boot bootstrap
bootstrap handles some device-specific initialization for the ASUS MeMO Pad 7 (ME176C/X).
It's a simple EFI application that wraps the primary EFI bootloader.

## Features
- **Check if booted due to charger insertion:** In this case, `LoaderEntryOneShot` is set to `charger` to instruct
  the primary bootloader to boot into (offline) charging mode.
- **Install ACPI OEM6/GNVS table:** This is required for [me176c-acpi].
  See [Global NVS Area](https://github.com/me176c-dev/me176c-acpi#global-nvs-area).

## Build
android-efi is built with [Meson]:

```
meson . build
ninja -C build
```

By default, bootstrap will attempt to load `\systemd-bootx64.efi` as primary bootloader.
To change this, add a `-Dloader=\path\to\bootloader.efi` option when running `meson`.

## Usage
Use `build/bootstrap.efi` as primary EFI application: Copy it to `EFI/BOOT/bootx64.efi` on the ESP partition.
Copy your primary bootloader to the path configured above.

[me176c-acpi]: https://github.com/me176c-dev/me176c-acpi
[Meson]: https://mesonbuild.com/
