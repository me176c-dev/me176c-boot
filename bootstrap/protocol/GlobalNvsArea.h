/** @file
  Copyright (c) 2004  - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:
  GlobalNvsArea.h

Abstract:
  Definition of the global NVS area protocol.  This protocol
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM code and ASL code.
  The format is derived from the ACPI reference code, version 0.95.

**/

#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
{ 0x74e1e48, 0x8132, 0x47a1, { 0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc }}

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

//
// Global NVS Area Protocol
//
typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  VOID *Area;
} EFI_GLOBAL_NVS_AREA_PROTOCOL;

#endif
