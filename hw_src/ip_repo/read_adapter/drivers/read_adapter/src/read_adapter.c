/*
* --------------------------------------------------------------------------     
* Copyright (c) 2017 Hardware-Software-Co-Design, Friedrich-                     
* Alexander-Universitaet Erlangen-Nuernberg (FAU), Germany.                      
* All rights reserved.                                                           
*                                                                                
*                                                                                
* This code and any associated documentation is provided "as is"                 
*                                                                                
* IN NO EVENT SHALL HARDWARE-SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-            
* UNIVERSITAET ERLANGEN-NUERNBERG (FAU) BE LIABLE TO ANY PARTY FOR DIRECT,       
* INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT            
* OF THE USE OF THIS CODE AND ITS DOCUMENTATION, EVEN IF HARDWARE-               
* SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET ERLANGEN-NUERNBERG        
* (FAU) HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. THE                  
* AFOREMENTIONED EXCLUSIONS OF LIABILITY DO NOT APPLY IN CASE OF INTENT          
* BY HARDWARE-SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET               
* ERLANGEN-NUERNBERG (FAU).                                                      
*                                                                                
* HARDWARE-SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET ERLANGEN-        
* NUERNBERG (FAU), SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT         
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          
* FOR A PARTICULAR PURPOSE.                                                      
*                                                                                
* THE CODE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND HARDWARE-              
* SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET ERLANGEN-                 
* NUERNBERG (FAU) HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,             
* UPDATES, ENHANCEMENTS, OR MODIFICATIONS.                                       
* -------------------------------------------------------------------------      
*                                                                                
*  @author Streit Franz-Josef                                                    
*  @date   05 Mai 2018                                                      
*  @version 0.1                                                                  
*  @brief baremetal software driver for the read adapter ipb 
*                                                                                
*                                                                                
**/


/***************************** Include Files *******************************/
#include "read_adapter.h"
/************************** Function Definitions ***************************/

read_adapter * init_read_adapter(u32 address) {
  read_adapter *control = (read_adapter*) address;
  return control;
}

u32 push_data(read_adapter *control, void *data, push_size size) {
  if (reg_en_read_adapt == control->use_reg) {
    switch (size) {
      case push_32_bit:
        control->lsb_push = *(s32 *) data;
        break;
      case push_64_bit:
        control->lsb_push = (*(s64 *) data & 0xFFFFFFFF);
        control->msb_push = (((*(s64 *) data) >> 32) & 0xFFFFFFFF);
        break;
      default:
        control->lsb_push = *(s32 *) data;
        break;
    }
  }
  return 0;
}

u32 set_reg_read_adapt(read_adapter *control, reg_ctrl_read_adapt reg) {
  control->use_reg = reg;
  return 0;
}

reg_ctrl_read_adapt get_reg_read_adapt(read_adapter *control) {
  return control->use_reg;
}



