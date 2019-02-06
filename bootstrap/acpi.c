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
    struct {
        CHAR8   signature[8];       /* "RSD PTR " */
        CHAR8   checksum;           /* RSDP Checksum (bytes 0-19) */
        CHAR8   oem_id[6];          /* OEM ID String */
        CHAR8   revision;           /* ACPI Revision (0=1.0,2=2.0) */
        UINT32  rsdt_address;       /* 32-bit RSDT Pointer */
    } v1;
    UINT32  length;             /* RSDP Length */
    UINT64  xsdt_address;       /* 64-bit XSDT Pointer */
    CHAR8   extended_checksum;  /* RSDP Checksum (full) */
    CHAR8   reserved[3];        /* Reserved */
}  __attribute__((packed));

#define RSDP_SIGNATURE_SIZE (sizeof(((struct RSDP_TABLE*) 0)->v1.signature))
static const char RSDP_SIGNATURE[RSDP_SIGNATURE_SIZE] = "RSD PTR ";

struct XSDT_TABLE {
    struct ACPI_TABLE_HEADER header;
    UINT64  entry[1];             /* Table Entries */
}  __attribute__((packed));

static const char XSDT_SIGNATURE[ACPI_TABLE_SIGNATURE_SIZE] = "XSDT";

static CHAR8 acpi_calculate_checksum(const CHAR8 *data, UINT32 len) {
    CHAR8 sum = 0;
    for (UINT32 i = 0; i < len; ++i) {
        sum += data[i];
    }
    return sum;
}

CHAR8 acpi_calculate_table_checksum(const struct ACPI_TABLE_HEADER *table) {
    return acpi_calculate_checksum((const CHAR8*) table, table->length);
}

static inline EFI_STATUS acpi_verify_checksum(const CHAR8 *data, UINT32 len) {
    return acpi_calculate_checksum(data, len) == 0 ? EFI_SUCCESS : EFI_CRC_ERROR;
}

static inline EFI_STATUS acpi_verify_table_checksum(const struct ACPI_TABLE_HEADER *table) {
    return acpi_calculate_table_checksum(table) == 0 ? EFI_SUCCESS : EFI_CRC_ERROR;
}

static EFI_STATUS get_xsdt_table(struct XSDT_TABLE **xsdt) {
    EFI_GUID acpi_guid = ACPI_20_TABLE_GUID;
    struct RSDP_TABLE *rsdp;
    EFI_STATUS ret = LibGetSystemConfigurationTable(&acpi_guid, (VOID**) &rsdp);
    if (ret) {
        return ret;
    }

    if (CompareMem(rsdp->v1.signature, RSDP_SIGNATURE, sizeof(RSDP_SIGNATURE))) {
        return EFI_COMPROMISED_DATA;
    }
    if (rsdp->v1.revision < 2) {
        return EFI_UNSUPPORTED;
    }

    ret = acpi_verify_checksum((const CHAR8*) rsdp, sizeof(rsdp->v1));
    if (ret) {
        return ret;
    }

    ret = acpi_verify_checksum((const CHAR8*) rsdp, sizeof(*rsdp));
    if (ret) {
        return ret;
    }

    *xsdt = (struct XSDT_TABLE *) rsdp->xsdt_address;
    if (CompareMem((*xsdt)->header.signature, XSDT_SIGNATURE, sizeof(XSDT_SIGNATURE))) {
        return EFI_COMPROMISED_DATA;
    }

    ret = acpi_verify_table_checksum(&(*xsdt)->header);
    if (ret) {
        return ret;
    }

    return EFI_SUCCESS;
}

EFI_STATUS acpi_get_table(struct ACPI_TABLE_HEADER **table, const CHAR8 signature[ACPI_TABLE_SIGNATURE_SIZE]) {
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
            return acpi_verify_table_checksum(header);
        }
    }

    return EFI_NOT_FOUND;
}
