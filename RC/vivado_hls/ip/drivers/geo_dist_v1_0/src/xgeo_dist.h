// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XGEO_DIST_H
#define XGEO_DIST_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xgeo_dist_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Geo_dist_if_BaseAddress;
} XGeo_dist_Config;
#endif

typedef struct {
    u32 Geo_dist_if_BaseAddress;
    u32 IsReady;
} XGeo_dist;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XGeo_dist_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XGeo_dist_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XGeo_dist_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XGeo_dist_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XGeo_dist_Initialize(XGeo_dist *InstancePtr, u16 DeviceId);
XGeo_dist_Config* XGeo_dist_LookupConfig(u16 DeviceId);
int XGeo_dist_CfgInitialize(XGeo_dist *InstancePtr, XGeo_dist_Config *ConfigPtr);
#else
int XGeo_dist_Initialize(XGeo_dist *InstancePtr, const char* InstanceName);
int XGeo_dist_Release(XGeo_dist *InstancePtr);
#endif


void XGeo_dist_Set_lat_a_V(XGeo_dist *InstancePtr, u32 Data);
u32 XGeo_dist_Get_lat_a_V(XGeo_dist *InstancePtr);
void XGeo_dist_Set_lon_a_V(XGeo_dist *InstancePtr, u32 Data);
u32 XGeo_dist_Get_lon_a_V(XGeo_dist *InstancePtr);
void XGeo_dist_Set_lat_b_V(XGeo_dist *InstancePtr, u32 Data);
u32 XGeo_dist_Get_lat_b_V(XGeo_dist *InstancePtr);
void XGeo_dist_Set_lon_b_V(XGeo_dist *InstancePtr, u32 Data);
u32 XGeo_dist_Get_lon_b_V(XGeo_dist *InstancePtr);
u32 XGeo_dist_Get_result_V(XGeo_dist *InstancePtr);
u32 XGeo_dist_Get_result_V_vld(XGeo_dist *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
