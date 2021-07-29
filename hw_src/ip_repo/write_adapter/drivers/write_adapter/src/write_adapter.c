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
*  @brief baremetal software driver for the write adapter ipb 
*                                                                                
*                                                                                
**/

/***************************** Include Files *******************************/
#include "write_adapter.h"
/************************** Function Definitions ***************************/

write_adapter * init_write_adapter(u32 address) {
  write_adapter *control = (write_adapter*) address;
  return control;
}

u32 pop_data(write_adapter *control, void* data, pop_size size) {

    u32 lsb_word;
    s64 msb_word;
    switch (size) {
      case pop_32_bit:
        *(s32 *) data = control->pop_0;
        break;
      case pop_64_bit:
        lsb_word = control->pop_0;
        msb_word = control->pop_1;
        *(s64 *) data = (msb_word<<32 | lsb_word);
        break;
      case pop_128_bit:
      	  ((u32 *) data)[0] = control->pop_0;
    	  ((u32 *) data)[1] = control->pop_1;
    	  ((u32 *) data)[2] = control->pop_2;
    	  ((u32 *) data)[3] = control->pop_3;
      break;
      default:
        *(s32 *) data = control->pop_0;
        break;
    }
  
  return 0;
}

