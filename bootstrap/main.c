// SPDX-License-Identifier: BSD-2-Clause
// Copyright (C) 2018 lambdadroid

#include <efi.h>
#include <efilib.h>

#include "rsci.h"
#include "gnvs.h"

#ifndef LOADER_PATH
#define LOADER_PATH  L"\\systemd-bootx64.efi"
#endif

// From gummiboot/systemd-boot
static EFI_GUID loader_guid = { 0x4a67b082, 0x0a4c, 0x41cf, {0xb6, 0xc7, 0x44, 0x0b, 0x29, 0xbb, 0x8c, 0x4f} };

#define LOADER_ENTRY_VARIABLE  L"LoaderEntryOneShot"
#define ENTRY_CHARGER          L"charger"

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *system_table) {
    InitializeLib(image, system_table);

    EFI_LOADED_IMAGE *loaded_image;
    EFI_STATUS err = uefi_call_wrapper(BS->OpenProtocol, 6, image, &LoadedImageProtocol, (VOID**) &loaded_image,
                                       image, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (err) {
        Print(L"Failed to open loaded image protocol\n");
        return err;
    }

    UINTN size = 0;
    if (uefi_call_wrapper(RT->GetVariable, 5, LOADER_ENTRY_VARIABLE, &loader_guid, NULL, &size, NULL) == EFI_NOT_FOUND) {
        // Check if booted by charger insertion
        if (rsci_is_charger_mode()) {
            err = uefi_call_wrapper(RT->SetVariable, 5, LOADER_ENTRY_VARIABLE, &loader_guid,
                                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                    sizeof(ENTRY_CHARGER), ENTRY_CHARGER);
            if (err) {
                Print(L"Failed to set charger loader entry: %r\n", err);
            }
        }
    }

    // Install ACPI table with global NVS area address
    err = acpi_gnvs_install();
    if (err) {
        Print(L"Failed to install ACPI GNVS table: %r\n", err);
    }

    EFI_DEVICE_PATH *next_image_path = FileDevicePath(loaded_image->DeviceHandle, LOADER_PATH);
    if (!next_image_path) {
        Print(L"Failed to get device path of '" LOADER_PATH "'\n");
        return EFI_INVALID_PARAMETER;
    }

    EFI_HANDLE next_image;
    err = uefi_call_wrapper(BS->LoadImage, 6, FALSE, image, next_image_path, NULL, 0, &next_image);
    FreePool(next_image_path);
    if (err) {
        Print(L"Failed to load next image from '" LOADER_PATH "'\n");
        goto out;
    }

    err = uefi_call_wrapper(BS->StartImage, 3, next_image, 0, NULL);
    if (err) {
        Print(L"Failed to start image\n");
        goto out;
    }

out:
    uefi_call_wrapper(BS->UnloadImage, 1, next_image);
    uefi_call_wrapper(BS->CloseProtocol, 4, image, &LoadedImageProtocol, image, NULL);
    return err;
}
