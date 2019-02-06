#include <efi.h>
#include <efilib.h>
#include "protocol/AcpiTable.h"
#include "protocol/GlobalNvsArea.h"

#include "acpi.h"
#include "gnvs.h"

#define AML_NAME_OP   0x08
#define AML_DWORD_OP  0x0c

static struct {
    struct ACPI_TABLE_HEADER header;
    UINT8 name_op;
    CHAR8 name[4];
    UINT8 data_op;
    UINT32 data;
} __attribute__((packed)) gnvs_table = {
    .header = {
        .signature = "OEM6",
        .length = sizeof(gnvs_table),
        .revision = 2,
        .oem_id = "ME176C",
        .oem_table_id = "GNVS",
        .creator_id = "BOOT",
    },
    .name_op = AML_NAME_OP,
    .name = "NVSA",
    .data_op = AML_DWORD_OP,
};

EFI_GUID gEfiAcpiTableProtocolGuid = EFI_ACPI_TABLE_PROTOCOL_GUID;
#define AcpiTableProtocol gEfiAcpiTableProtocolGuid

EFI_GUID gEfiGlobalNvsAreaProtocolGuid = EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID;
#define GlobalNvsAreaProtocol gEfiGlobalNvsAreaProtocolGuid

EFI_STATUS acpi_gnvs_install() {
    EFI_GLOBAL_NVS_AREA_PROTOCOL *gnvs_protocol;
    EFI_STATUS ret = LibLocateProtocol(&GlobalNvsAreaProtocol, (VOID**) &gnvs_protocol);
    if (ret) {
        return ret;
    }

    // Update ACPI table with proper offset
    gnvs_table.data = (UINT32) (UINTN) gnvs_protocol->Area;

    // Calculate checksum
    gnvs_table.header.checksum = (CHAR8) -acpi_calculate_table_checksum(&gnvs_table.header);

    EFI_ACPI_TABLE_PROTOCOL *acpi_table_protocol;
    ret = LibLocateProtocol(&AcpiTableProtocol, (VOID**) &acpi_table_protocol);
    if (ret) {
        return ret;
    }

    // Install ACPI table
    UINTN table_key;
    return uefi_call_wrapper(acpi_table_protocol->InstallAcpiTable, 4, acpi_table_protocol,
            &gnvs_table, sizeof(gnvs_table), &table_key);
}
