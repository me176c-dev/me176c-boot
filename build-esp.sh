#!/usr/bin/env bash
set -e

PARTITION_SIZE=33  # In MiB
PARTITION_ID=0x6b303133  # k013
PARTITION_LABEL=ESP

rm -rf build
mkdir build
cd build

# Build bootstrap
meson ../bootstrap bootstrap
ninja -C bootstrap

# Build systemd-boot
meson ../systemd-boot systemd-boot
ninja -C systemd-boot

# Build android-efi
meson ../android-efi android-efi
ninja -C android-efi

# Create new partition image
dd if=/dev/zero of=esp.img bs=1M count=$PARTITION_SIZE
loop_device=$(sudo losetup --find --show esp.img)

# Create FAT32 file system
sudo mkfs.fat -F32 -i $PARTITION_ID -n "$PARTITION_LABEL" "$loop_device"

# Mount file system
mkdir esp
sudo mount "$loop_device" esp

# Initial bootloader (for charger detection)
sudo mkdir -p esp/EFI/BOOT
sudo cp bootstrap/bootstrap.efi esp/EFI/BOOT/bootx64.efi

# Boot selection (systemd-boot)
sudo cp systemd-boot/systemd-bootx64.efi esp

# Android bootloader
sudo cp android-efi/android.efi esp

# Configuration
sudo cp -r ../loader esp

# Unmount and destroy loop device
sudo umount "$loop_device"
sudo losetup -d "$loop_device"

# Create ZIP
zip -9 esp.zip esp.img
