/*
 * Copyright (c) 2013-2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_Storage.h"

#define ARM_STORAGE_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = { ARM_STORAGE_API_VERSION,
                                                  ARM_STORAGE_DRV_VERSION };

/* Driver Capabilities */
static const ARM_STORAGE_CAPABILITIES DriverCapabilities = {
    0, /* Asynchronous Mode */
    0, /* Supports EraseAll operation */
    0  /* Reserved */
};

//
// Functions
//

static ARM_DRIVER_VERSION ARM_Storage_GetVersion(void) {
    return DriverVersion;
}

static ARM_STORAGE_CAPABILITIES ARM_Storage_GetCapabilities(void) {
    return DriverCapabilities;
}

static int32_t ARM_Storage_Initialize(ARM_Storage_Callback_t callback) {}

static int32_t ARM_Storage_Uninitialize(void) {}

static int32_t ARM_Storage_PowerControl(ARM_POWER_STATE state) {
    switch(state) {
        case ARM_POWER_OFF:
            break;

        case ARM_POWER_LOW:
            break;

        case ARM_POWER_FULL:
            break;
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_Storage_ReadData(uint64_t addr, void * data, uint32_t size) {}

static int32_t ARM_Storage_ProgramData(uint64_t addr, const void * data, uint32_t size) {}

static int32_t ARM_Storage_Erase(uint64_t addr, uint32_t size) {}

static int32_t ARM_Storage_EraseAll(void) {}

static ARM_STORAGE_STATUS ARM_Storage_GetStatus(void) {}

static int32_t ARM_Storage_GetInfo(ARM_STORAGE_INFO * info) {}

static uint32_t ARM_Storage_ResolveAddress(uint64_t addr) {}

static int32_t ARM_Storage_GetNextBlock(const ARM_STORAGE_BLOCK * prev_block,
                                        ARM_STORAGE_BLOCK * next_block) {}

static int32_t ARM_Storage_GetBlock(uint64_t addr, ARM_STORAGE_BLOCK * block) {}

// End Storage Interface

extern ARM_DRIVER_STORAGE Driver_Storage0;
ARM_DRIVER_STORAGE Driver_Storage0 = { ARM_Storage_GetVersion,   ARM_Storage_GetCapabilities,
                                       ARM_Storage_Initialize,   ARM_Storage_Uninitialize,
                                       ARM_Storage_PowerControl, ARM_Storage_ReadData,
                                       ARM_Storage_ProgramData,  ARM_Storage_Erase,
                                       ARM_Storage_EraseAll,     ARM_Storage_GetStatus,
                                       ARM_Storage_GetInfo,      ARM_Storage_ResolveAddress,
                                       ARM_Storage_GetNextBlock, ARM_Storage_GetBlock };
