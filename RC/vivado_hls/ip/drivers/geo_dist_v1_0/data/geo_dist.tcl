# ==============================================================
# Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2 (64-bit)
# Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
# ==============================================================
proc generate {drv_handle} {
    xdefine_include_file $drv_handle "xparameters.h" "XGeo_dist" \
        "NUM_INSTANCES" \
        "DEVICE_ID" \
        "C_S_AXI_GEO_DIST_IF_BASEADDR" \
        "C_S_AXI_GEO_DIST_IF_HIGHADDR"

    xdefine_config_file $drv_handle "xgeo_dist_g.c" "XGeo_dist" \
        "DEVICE_ID" \
        "C_S_AXI_GEO_DIST_IF_BASEADDR"

    xdefine_canonical_xpars $drv_handle "xparameters.h" "XGeo_dist" \
        "DEVICE_ID" \
        "C_S_AXI_GEO_DIST_IF_BASEADDR" \
        "C_S_AXI_GEO_DIST_IF_HIGHADDR"
}

