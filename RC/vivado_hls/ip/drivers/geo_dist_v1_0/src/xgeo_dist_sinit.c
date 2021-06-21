// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xgeo_dist.h"

extern XGeo_dist_Config XGeo_dist_ConfigTable[];

XGeo_dist_Config *XGeo_dist_LookupConfig(u16 DeviceId) {
	XGeo_dist_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XGEO_DIST_NUM_INSTANCES; Index++) {
		if (XGeo_dist_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XGeo_dist_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XGeo_dist_Initialize(XGeo_dist *InstancePtr, u16 DeviceId) {
	XGeo_dist_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XGeo_dist_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XGeo_dist_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

