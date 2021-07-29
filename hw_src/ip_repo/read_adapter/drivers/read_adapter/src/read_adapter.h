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

#ifndef READ_ADAPTER_H
#define READ_ADAPTER_H

#ifdef __cplusplus                                                               
extern "C" {                                                                     
#endif

#define BAREMETAL

#ifdef BAREMETAL
  /****************** Include Files ********************/
#include "xil_types.h"
#include "xil_printf.h"
#else
  /****************** Include Files ********************/
#include <linux/types.h>

  typedef u8 __u8;
  typedef s8 __s8;
  typedef u16 __u16;
  typedef s16 __s16;
  typedef u32 __u32;
  typedef s32 __s32;

#define READ_ADAPTER_S00_AXI_SLV_REG0_OFFSET 0
#define READ_ADAPTER_S00_AXI_SLV_REG1_OFFSET 4
#define READ_ADAPTER_S00_AXI_SLV_REG2_OFFSET 8
#define READ_ADAPTER_S00_AXI_SLV_REG3_OFFSET 12

#define REG_WRITE(base,reg,val) iowrite32((u32)(val),((u32*)(base))+(reg))
#define REG_READ(base,reg) ioread32(((u32*)(base))+(reg))

#endif

  typedef enum {
    reg_en_read_adapt = 0x0001, 
    reg_nen_read_adapt = 0x0000,
  }reg_ctrl_read_adapt;

  typedef enum {
    push_32_bit,
    push_64_bit
  }push_size;

  typedef struct {
    volatile u32 lsb_push;
    volatile u32 msb_push;
    volatile u16 use_reg;
  }read_adapter;

  /** @brief Initialization of read_adapter driver
   *
   *  @param set hw address of device.
   *  @return pointer to read_adapter struct.
   */
  read_adapter * init_read_adapter(u32 address);

  /** @brief Use read_adapter to write values to the system
   *
   *  @param control struct of read_adapter
   *  @param pointer to DMA instance
   *  @param generic void pointer to data
   *  @param package length
   *  @param distinguish between <= 32 and 64 bit data types
   *  @return Return if succeeded.
   */
  u32 push_data(read_adapter *control, void *data, push_size size);

  /** @brief Use read_adapter to enable register or DMA interface
   *
   *  @param control struct of read_adapter
   *  @param hw address of read DMA
   *  @param pointer to DMA instance
   *  @param ID of DMA instance
   *  @param set transfer from register or DMA interface
   *  @return Return if succeeded.
   */
  u32 set_reg_read_adapt(read_adapter *control, reg_ctrl_read_adapt reg);

  /** @brief Use read_adapter to read interface settings
   *
   * @param control struct of read_adapter
   * @param read use_reg register to get current settings
   * @return register content
   */
  reg_ctrl_read_adapt get_reg_read_adapt(read_adapter *control);

#ifdef __cplusplus                                                               
}                                                                                
#endif 

#endif // READ_ADAPTER_H





