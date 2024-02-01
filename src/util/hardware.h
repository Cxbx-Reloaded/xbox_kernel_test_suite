// SPDX-FileCopyrightText: none
// SPDX-License-Identifier: CC0-1.0
#pragma once

#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>
#include <string.h>

#include "util/output.h"

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    __asm__ __volatile__("inl %w1, %0"
                         : "=a"(ret)
                         : "Nd"(port));
    return ret;
}

static void getPICVersion(char pic_version[4])
{
    ULONG version;
    HalWriteSMBusValue(0x20, 0x01, FALSE, 0); // reset index to 0 for reading
    for (unsigned i = 0; i < 3; i++) {
        HalReadSMBusValue(0x20, 0x01, 0, &version);
        pic_version[i] = (char)version;
    }
    pic_version[3] = '\0';
}

static const char* getConsoleType(char pic_version[4])
{
    // See https://xboxdevwiki.net/Xboxen_Info
    if (pic_version[0] == 'A') {
        return "PROTOTYPE (UNKNOWN)";
    }
    else if (pic_version[0] == 'D' || pic_version[0] == 'B') {
        if (XboxHardwareInfo.Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) {
            return "DEVKIT or DEBUGKIT";
        }
        if (XboxHardwareInfo.Flags & XBOX_HW_FLAG_ARCADE) {
            // Base on Cxbx-Reloaded's chihiro lpc reader function.
            uint32_t type = inl(0x40F0); // Media Board Type reader
            if (type == 0x0000) {
                return "CHIHIRO (Type-1)";
            }
            else if (type == 0x0100) {
                return "CHIHIRO (Type-3)";
            }

            print("ERROR: Unknown return type from chihiro hardware: %08X", type);
            return "CHIHIRO (UNKNOWN)";
        }
    }
    else if (pic_version[0] == 'P') {
        if (strcmp(pic_version + 1, "01") == 0) {
            return "Retail 1.0";
        }
        if (strcmp(pic_version + 1, "05") == 0) {
            return "Retail 1.1";
        }
        if (strcmp(pic_version + 1, "11") == 0) {
            // NOTE: 0xD4 is Focus (1.4) video encoder software address.
            ULONG dummy_storage;
            if (HalReadSMBusValue(0xD4, 0x00, 0, &dummy_storage) == STATUS_SUCCESS) {
                return "Retail 1.4";
            }
            // TODO: Find out how to tell the difference between 1.2 and 1.3 revisions...
            return "Retail 1.2 or 1.3";
        }
        if (strcmp(pic_version + 1, "2L") == 0) {
            return "Retail 1.6";
        }
    }

    return "UNKNOWN";
}
