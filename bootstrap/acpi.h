// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2018 lambdadroid
 */

#ifndef BOOTSTRAP_ACPI_H
#define BOOTSTRAP_ACPI_H

/*
 * Based on: (BSD-2-Clause)
 *   https://github.com/intel/kernelflinger/blob/f79f4b2f9f8760f191d48b1ff03ec15e75118a90/include/libkernelflinger/acpi.h
 *
 * Copyright (c) 2013, Intel Corporation
 * All rights reserved.
 */

#include <efi.h>

/** Generic ACPI table header **/
struct ACPI_TABLE_HEADER {
    CHAR8   signature[4];       /* ASCII Table identifier */
    UINT32  length;             /* Length of the table, including the header */
    CHAR8   revision;           /* Revision of the structure */
    CHAR8   checksum;           /* Sum of all fields must be 0 */
    CHAR8   oem_id[6];          /* ASCII OEM identifier */
    CHAR8   oem_table_id[8];    /* ASCII OEM table identifier */
    UINT32  oem_revision;       /* OEM supplied revision number */
    CHAR8   creator_id[4];      /* Vendor ID of utility creator of the table */
    UINT32  creator_revision;   /* Revision of utility creator of the table */
}  __attribute__((packed));

#define ACPI_TABLE_SIGNATURE_SIZE (sizeof(((struct ACPI_TABLE_HEADER*)0)->signature))

EFI_STATUS acpi_get_table(struct ACPI_TABLE_HEADER **table, CHAR8 signature[ACPI_TABLE_SIGNATURE_SIZE]);

#define _acpi_table_has_field(table, member) (((UINTN) &(member) + sizeof(member)) <= (table)->header.length)
#define acpi_table_has_field(table, name) _acpi_table_has_field(table, ((typeof(table)) 0)->name)

#endif //BOOTSTRAP_ACPI_H
