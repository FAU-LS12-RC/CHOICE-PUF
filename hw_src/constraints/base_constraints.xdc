create_pblock PUF_Block_Zone
add_cells_to_pblock [get_pblocks PUF_Block_Zone] [get_cells -hierarchical -regexp .*CHOICE_PUF.*gen.*/.*/.*/.*]
add_cells_to_pblock [get_pblocks PUF_Block_Zone] [get_cells {design_1_i/CHOICE_PUF_gen_0 design_1_i/xadc_wiz_0/U0/AXI_XADC_CORE_I/XADC_INST}]
resize_pblock [get_pblocks PUF_Block_Zone] -add {CLOCKREGION_X0Y1:CLOCKREGION_X0Y1}
set_property CONTAIN_ROUTING 1 [get_pblocks PUF_Block_Zone]
set_property EXCLUDE_PLACEMENT 1 [get_pblocks PUF_Block_Zone]
