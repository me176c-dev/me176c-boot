// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2018 lambdadroid
 */

/*
 * Based on: (BSD-2-Clause)
 *   https://github.com/intel/kernelflinger/blob/f79f4b2f9f8760f191d48b1ff03ec15e75118a90/include/libkernelflinger/acpi.h
 *   https://github.com/intel/kernelflinger/blob/d8a2c52855a7d8965bd44901dc38bc375d6bcb05/libkernelflinger/acpi.c
 *
 * Copyright (c) 2013, Intel Corporation
 * All rights reserved.
 */

#include "acpi.h"
#include <efilib.h>

struct RSDP_TABLE {
    CHAR8   signature[8];       /* "RSD PTR " */
    CHAR8   checksum;           /* RSDP Checksum (bytes 0-19) */
    CHAR8   oem_id[6];          /* OEM ID String */
    CHAR8   revision;           /* ACPI Revision (0=1.0,2=2.0) */
    UINT32  rsdt_address;       /* 32-bit RSDT Pointer */
    UINT32  length;             /* RSDP Length */
    UINT64  xsdt_address;       /* 64-bit XSDT Pointer */
    CHAR8   extended_checksum;  /* RSDP Checksum (full) */
    CHAR8   reserved[3];        /* Reserved */
}  __attribute__((packed));

#define RSDP_SIGNATURE_SIZE (sizeof(((struct RSDP_TABLE*) 0)->signature))
static const char RSDP_SIGNATURE[RSDP_SIGNATURE_SIZE] = "RSD PTR ";

struct XSDT_TABLE {
    struct ACPI_TABLE_HEADER header;
    UINT64  entry[1];             /* Table Entries */
}  __attribute__((packed));

static const char XSDT_SIGNATURE[ACPI_TABLE_SIGNATURE_SIZE] = "XSDT";

static EFI_STATUS acpi_verify_checksum(struct ACPI_TABLE_HEADER *table) {
    CHAR8 *data = (CHAR8*) table;
    CHAR8 sum = 0;

    for (UINT32 i = 0; i < table->length; ++i) {
        sum += data[i];
    }

    return sum == 0 ? EFI_SUCCESS : EFI_CRC_ERROR;
}

static EFI_STATUS get_xsdt_table(struct XSDT_TABLE **xsdt) {
    EFI_GUID acpi_guid = ACPI_20_TABLE_GUID;
    struct RSDP_TABLE *rsdp;
    EFI_STATUS ret = LibGetSystemConfigurationTable(&acpi_guid, (VOID**) &rsdp);
    if (ret) {
        return ret;
    }

    if (CompareMem(rsdp->signature, RSDP_SIGNATURE, sizeof(RSDP_SIGNATURE))) {
        return EFI_COMPROMISED_DATA;
    }

    *xsdt = (struct XSDT_TABLE *) rsdp->xsdt_address;
    if (CompareMem((*xsdt)->header.signature, XSDT_SIGNATURE, sizeof(XSDT_SIGNATURE))) {
        return EFI_COMPROMISED_DATA;
    }

    ret = acpi_verify_checksum(&(*xsdt)->header);
    if (ret) {
        return ret;
    }

    return EFI_SUCCESS;
}

EFI_STATUS acpi_get_table(struct ACPI_TABLE_HEADER **table, CHAR8 signature[ACPI_TABLE_SIGNATURE_SIZE]) {
    struct XSDT_TABLE *xsdt;
    EFI_STATUS ret = get_xsdt_table(&xsdt);
    if (ret) {
        return ret;
    }

    UINTN table_count = (xsdt->header.length - sizeof(xsdt->header)) / sizeof(xsdt->entry[0]);
    for (UINTN i = 0; i < table_count; ++i) {
        struct ACPI_TABLE_HEADER *header = (struct ACPI_TABLE_HEADER*) xsdt->entry[i];
        if (!CompareMem(header->signature, signature, ACPI_TABLE_SIGNATURE_SIZE)) {
            *table = header;
            return acpi_verify_checksum(header);
        }
    }

    return EFI_NOT_FOUND;
}
