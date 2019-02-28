## Booting into main OS on charger insertion
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
