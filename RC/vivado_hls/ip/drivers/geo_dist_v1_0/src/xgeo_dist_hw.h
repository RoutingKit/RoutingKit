// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// geo_dist_if
// 0x00 : reserved
// 0x04 : reserved
// 0x08 : reserved
// 0x0c : reserved
// 0x10 : Data signal of lat_a_V
//        bit 19~0 - lat_a_V[19:0] (Read/Write)
//        others   - reserved
// 0x14 : reserved
// 0x18 : Data signal of lon_a_V
//        bit 19~0 - lon_a_V[19:0] (Read/Write)
//        others   - reserved
// 0x1c : reserved
// 0x20 : Data signal of lat_b_V
//        bit 19~0 - lat_b_V[19:0] (Read/Write)
//        others   - reserved
// 0x24 : reserved
// 0x28 : Data signal of lon_b_V
//        bit 19~0 - lon_b_V[19:0] (Read/Write)
//        others   - reserved
// 0x2c : reserved
// 0x30 : Data signal of result_V
//        bit 19~0 - result_V[19:0] (Read)
//        others   - reserved
// 0x34 : Control signal of result_V
//        bit 0  - result_V_ap_vld (Read/COR)
//        others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XGEO_DIST_GEO_DIST_IF_ADDR_LAT_A_V_DATA  0x10
#define XGEO_DIST_GEO_DIST_IF_BITS_LAT_A_V_DATA  20
#define XGEO_DIST_GEO_DIST_IF_ADDR_LON_A_V_DATA  0x18
#define XGEO_DIST_GEO_DIST_IF_BITS_LON_A_V_DATA  20
#define XGEO_DIST_GEO_DIST_IF_ADDR_LAT_B_V_DATA  0x20
#define XGEO_DIST_GEO_DIST_IF_BITS_LAT_B_V_DATA  20
#define XGEO_DIST_GEO_DIST_IF_ADDR_LON_B_V_DATA  0x28
#define XGEO_DIST_GEO_DIST_IF_BITS_LON_B_V_DATA  20
#define XGEO_DIST_GEO_DIST_IF_ADDR_RESULT_V_DATA 0x30
#define XGEO_DIST_GEO_DIST_IF_BITS_RESULT_V_DATA 20
#define XGEO_DIST_GEO_DIST_IF_ADDR_RESULT_V_CTRL 0x34

