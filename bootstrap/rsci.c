// SPDX-License-Identifier: BSD-2-Clause
// Copyright (C) 2018 lambdadroid

/*
 * Based on: (BSD-2-Clause)
 *   https://github.com/intel/kernelflinger/blob/f79f4b2f9f8760f191d48b1ff03ec15e75118a90/include/libkernelflinger/acpi.h
 *   https://github.com/intel/kernelflinger/blob/4119291fc90db063e5b9047ed36baed7e19afcc4/include/libkernelflinger/power.h
 * Copyright (c) 2013, Intel Corporation
 */

#include "rsci.h"
#include "acpi.h"

#include <efilib.h>

static const CHAR8 RSCI_SIGNATURE[ACPI_TABLE_SIGNATURE_SIZE] = "RSCI";

struct RSCI_TABLE {
    struct ACPI_TABLE_HEADER header;
    CHAR8   wake_source;        /* How system woken up from S4 or S5 */
    CHAR8   reset_source;       /* How system was reset */
    CHAR8   reset_type;         /* Identify type of reset */
    CHAR8   shutdown_source;    /* How system was last shutdown */
    UINT32  indicators;         /* Bitmap with additional info */
}  __attribute__((packed));

/* Wake sources */
enum wake_source {
    WAKE_NOT_APPLICABLE,
    WAKE_BATTERY_INSERTED,
    WAKE_USB_CHARGER_INSERTED,
    WAKE_ACDC_CHARGER_INSERTED,
    WAKE_POWER_BUTTON_PRESSED,
    WAKE_RTC_TIMER,
    WAKE_BATTERY_REACHED_IA_THRESHOLD,
    WAKE_ERROR = -1,
};

static enum wake_source rsci_get_wake_source() {
    struct RSCI_TABLE *rsci;
    EFI_STATUS ret = acpi_get_table((struct ACPI_TABLE_HEADER**) &rsci, RSCI_SIGNATURE);
    if (ret) {
        Print(L"Failed to get RSCI table: %r\n", ret);
        return WAKE_ERROR;
    }

    if (!acpi_table_has_field(rsci, wake_source)) {
        Print(L"RSCI table does not contain wake source\n");
        return WAKE_ERROR;
    }

    return (enum wake_source) rsci->wake_source;
}

BOOLEAN rsci_is_charger_mode() {
    enum wake_source ws = rsci_get_wake_source();
    return ws == WAKE_USB_CHARGER_INSERTED || ws == WAKE_ACDC_CHARGER_INSERTED;
}
