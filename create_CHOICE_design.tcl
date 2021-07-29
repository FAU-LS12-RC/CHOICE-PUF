#*****************************************************************************************
# @author: Streit Franz-Josef
# @date:   13.04.20
# @brief:  tcl script to generate CHOCIE HW/SW Design on the Digilent Zybo board 
#*****************************************************************************************


# Create project
set project_name "CHOICE_design"
create_project $project_name ./$project_name -part xc7z010clg400-1

set_property default_lib xil_defaultlib [current_project]
# Here we set the target FPGA architecture
set_property part xc7z010clg400-1 [current_project]
set_property sim.ip.auto_export_scripts 1 [current_project]
set_property simulator_language Mixed [current_project]
set_property target_language VHDL [current_project]

set proj_dir [get_property directory [current_project]]

## Here, we add all constraints 
add_files -fileset constrs_1 -norecurse "$proj_dir/../hw_src/constraints/base_constraints.xdc"
add_files -fileset constrs_1 -norecurse "$proj_dir/../hw_src/constraints/PUF_constraints.xdc"

## Here, we add all VHDL source files
add_files -norecurse "$proj_dir/../hw_src/hdl/CHOICE_PUF.vhd"
add_files -norecurse "$proj_dir/../hw_src/hdl/CHOICE_PUF_gen.vhd"
add_files -norecurse "$proj_dir/../hw_src/hdl/PUF_controller.vhd"
add_files -norecurse "$proj_dir/../hw_src/hdl/pattern_ctrl_unit.vhd" 
add_files -norecurse "$proj_dir/../hw_src/hdl/request_ctrl_unit.vhd"
add_files -norecurse "$proj_dir/../hw_src/hdl/tune_ctrl_unit.vhd"

## Here, we add all simulation data
add_files -fileset sim_1 -norecurse "$proj_dir/../hw_sim/PUF_controller_tb.vhd" 

## Here, we specifiy the IP repo. path
set_property  ip_repo_paths "$proj_dir/../hw_src/ip_repo" [current_project]

## Update ip catalog bevor creating block design
update_ip_catalog -rebuild -scan_changes

# Create block design
set bd_design_name "design_1"
create_bd_design $bd_design_name
source $proj_dir/../hw_src/bd/create_bd.tcl

# Generate the block design wrapper
make_wrapper -files [get_files $bd_design_name.bd] -top -import
set_property top ${bd_design_name}_wrapper [current_fileset] 

puts "INFO: Project created: $project_name"

update_compile_order -fileset sources_1
update_ip_catalog -rebuild -scan_changes
