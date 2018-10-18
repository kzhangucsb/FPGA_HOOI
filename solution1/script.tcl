############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project ttm1
set_top ttm
add_files ttm1/ttm.cpp
open_solution "solution1"
set_part {xcku5p-ffvb676-2-e}
create_clock -period 10 -name default
#source "./ttm1/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -format ip_catalog
