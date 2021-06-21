// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xgeo_dist.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XGeo_dist_CfgInitialize(XGeo_dist *InstancePtr, XGeo_dist_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Geo_dist_if_BaseAddress = ConfigPtr->Geo_dist_if_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XGeo_dist_Set_lat_a_V(XGeo_dist *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGeo_dist_WriteReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LAT_A_V_DATA, Data);
}

u32 XGeo_dist_Get_lat_a_V(XGeo_dist *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGeo_dist_ReadReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LAT_A_V_DATA);
    return Data;
}

void XGeo_dist_Set_lon_a_V(XGeo_dist *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGeo_dist_WriteReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LON_A_V_DATA, Data);
}

u32 XGeo_dist_Get_lon_a_V(XGeo_dist *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGeo_dist_ReadReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LON_A_V_DATA);
    return Data;
}

void XGeo_dist_Set_lat_b_V(XGeo_dist *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGeo_dist_WriteReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LAT_B_V_DATA, Data);
}

u32 XGeo_dist_Get_lat_b_V(XGeo_dist *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGeo_dist_ReadReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LAT_B_V_DATA);
    return Data;
}

void XGeo_dist_Set_lon_b_V(XGeo_dist *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGeo_dist_WriteReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LON_B_V_DATA, Data);
}

u32 XGeo_dist_Get_lon_b_V(XGeo_dist *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGeo_dist_ReadReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_LON_B_V_DATA);
    return Data;
}

u32 XGeo_dist_Get_result_V(XGeo_dist *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGeo_dist_ReadReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_RESULT_V_DATA);
    return Data;
}

u32 XGeo_dist_Get_result_V_vld(XGeo_dist *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGeo_dist_ReadReg(InstancePtr->Geo_dist_if_BaseAddress, XGEO_DIST_GEO_DIST_IF_ADDR_RESULT_V_CTRL);
    return Data & 0x1;
}

