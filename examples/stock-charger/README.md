## Using the stock charger
You may want to use the stock ROM for (offline) charging since it charges a bit more efficiently (with
less heat being generated). Ideally, this would be fixed directly in the custom ROM, but until then it
is possible to configure the bootloader to use the stock boot image when booting into charging mode:

1. Download the [stock ROM (`UL-K013-WW-12.10.1.36-user.zip`)](https://dlcdnets.asus.com/pub/ASUS/EeePAD/ME176C/UL-K013-WW-12.10.1.36-user.zip)
   and [the updated charger boot configuration (`charger.conf`)](charger.conf)
2. Extract `boot.img` from the downloaded ZIP.
3. Boot into Recovery, go to `Mount` and select `EFI System Partition (ESP)`.
4. Connect the tablet to your PC, and verify that it shows up in `adb devices`.
5.
    ```
    adb push boot.img /esp/asus-charger.img
    adb push charger.conf /esp/loader/entries/charger.conf
    ```
6. That's it! When you plug in the charger the next time it should boot into the stock charger UI. Yay!
