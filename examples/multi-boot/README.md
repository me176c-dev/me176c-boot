## Dual/multi boot
It is possible to set up dual/multi boot to have two or more installations on
the tablet at the same time (e.g. Android and Linux). This works by setting up
additional boot targets and making sure that the installations use separate
partitions.

You almost certainly need to
[set up an additional ESP partition](/#setting-up-an-additional-esp-partition).
Otherwise there will be barely enough space for a single installation.

### Internal Storage
The internal storage is partitioned using a standard Android partition layout,
e.g. a `boot`, `recovery`, `system` and `data` partition. Avoid re-partitioning
when possible: there are `reserved` partitions that may be needed for the device
to boot and it won't be easy to return to Android if you change the partition
layout.

If you don't want Android, you can use the `data` partition as root partition.
Otherwise, if anything, shrink the `data` partition and add new partitions at the end.
This should not prevent Android from booting.

Use the additional ESP partition (APD) as boot partition.

### External Storage (SD card)
As documented in ["Limitations" (README)](/#readme) it is not possible to
boot directly from an external SD card. However, it is possible to place only
the kernel (or boot partition) on the internal storage and have the actual root
file system on an external SD card.

<table>
<thead><tr><th>Advantages</th><th>Disadvantages</th></tr></thead>
<tbody><tr>
<td>

- No need to re-partition the internal storage
- More available storage (can generally use any size, I tested up to 128 GB)
- Multiple installations possible

</td><td>

- Slower (the SD card reader performance is quite limited)
- Still need kernel (or boot partition) on internal storage

</td>
</tbody></tr>
</table>

- Use a GUID Partition Table (GPT) on the SD card
- Setup at least a root partition for your system
- Use the additional ESP partition (APD) as boot partition
- Multi boot with Android:
  - Change the partition type GUID of the root partition
    - e.g. `4F68BCE3-E8CD-4DB1-96E7-FBCAF984B709` (Linux x86_64 root)
    - The default - Linux filesystem (`0FC63DAF-8483-4772-8E79-3D69D8477DE4`) -
      will appear as external storage on Android
  - Consider adding a small partition at the beginning of the disk to share
    files between Android/Linux

### Example setup (Linux)
The notes above are independent should apply to any OS.
This section shows a specific Linux dual boot setup on the external SD card
with Android on the internal storage.

- Android is installed normally
- Partitioning on external SD card:
  - ~8 GiB Linux filesystem partition for file sharing with Android (FAT32/EXT4/F2FS)
  - `/` Linux x86_64 root (`4F68BCE3-E8CD-4DB1-96E7-FBCAF984B709`) partition,
    EXT4/F2FS (F2FS is much faster)
- `/boot` on APD partition on internal storage, FAT32
- Boot target (`/esp/loader/entries/linux.conf`):
    ```
    title    Arch Linux
    volume   80868086-8086-8086-8086-000000000007
    linux    /vmlinuz-linux-me176c
    initrd   /intel-ucode-byt-t-c0.img
    initrd   /acpi-me176c.img
    initrd   /initramfs-linux-me176c.img
    options  root=PARTUUID=... rw
    ```
