/*
 * CHOICE_PUF.h
 *
 *  Created on: Mar 18, 2021
 *      Author: Krüger, Streit
 */

#ifndef SRC_CHOICE_PUF_H_
#define SRC_CHOICE_PUF_H_

#include "xparameters.h"
#include "xplatform_info.h"
#include "xadcps.h"
#include "xsdps.h"
#include "xstatus.h"
#include "xtime_l.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "ff.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "math.h"

#include "read_adapter.h"
#include "write_adapter.h"

/***********Basic defines and function prototypes************/
#define XADC_DEVICE_ID 		XPAR_XADCPS_0_DEVICE_ID

static int XAdcFractionToInt(float FloatNum);
static XAdcPs XAdcInst;
static FATFS fatfs;
write_adapter *my_write_adapter;
read_adapter  *my_read_adapter;
XAdcPs *XAdcInstPtr;
/************************************************************/

/***********Additional defines and function prototypes************/

//Macros
#define PUF_REQ            0x01
#define PUF_TUNE_SET       0x02
#define PUF_CHOICE_SET     0x03
#define TOP_PATTERN_SET    0x04
#define BOTTOM_PATTERN_SET 0x05

#define TUNE_SIZE          0x1F

#define SET_SHIFT 60

// Here, we define all commands currently supported by the FPGA
#define BADKEY       -1
#define getTemp       1
#define getVcc        2
#define setTune       3
#define setChoice     4
#define setPattern    5
#define getReadout    6
#define listFiles     7
#define setTargetFile 8
#define sendFile      9
#define writeLine    10
#define help         11
#define exit         12

//Global variables
bool initPop;

typedef enum  {
  VCCPINT,
  VCCPAUX,
  VCCPDRO
} VoltageType;

// let's use a key/value data structure for all commands
typedef struct {
  char *key;
  int val;
} t_commands;

// here we define all commands accepted by the FPGA in a look-up table
static t_commands lookuptable[] = {
  {"getTemp",getTemp},
  {"getVcc",getVcc},
  {"setTune",setTune},
  {"setChoice",setChoice},
  {"setPattern",setPattern},
  {"getReadout",getReadout},
  {"listFiles",listFiles},
  {"setTargetFile",setTargetFile},
  {"sendFile",sendFile},
  {"writeLine",writeLine},
  {"help",help},
  {"exit",exit},
};

#define NKEYS (sizeof(lookuptable)/sizeof(t_commands))

//Functions
static void initialization(void);
static void getTemperatureData(float*);
static void getVoltageData(float*, VoltageType);
static int openFile(FIL* fp, const TCHAR* filename);
static void writeDataToFile(FIL* fp, char* dataString, size_t dataSize);
static void setPUFTune(u16 topSrlLength, u16 bottomSrlLength);
static void setPUFChoice(u8 top, u8 bottom);
static void setPUFPattern(u64 pattern, bool top, u16 topSrlLength,
    u16 bottomSrlLength);
static void genPUFOutput(u32* output);
//static unsigned int countSetBits(unsigned int n);
//static void usleep(unsigned int usecs);

/*****************************************************************/

static FATFS fatfs;

static void initialization(void) {

  //XADC initialization
  int Status;
  XAdcInstPtr = &XAdcInst;
  XAdcPs_Config *ConfigPtr;
  ConfigPtr = XAdcPs_LookupConfig(XADC_DEVICE_ID);
  if (ConfigPtr == NULL) {
    return XST_FAILURE;
  }
  XAdcPs_CfgInitialize(XAdcInstPtr, ConfigPtr, ConfigPtr->BaseAddress);

  Status = XAdcPs_SelfTest(XAdcInstPtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  XAdcPs_SetSequencerMode(XAdcInstPtr, XADCPS_SEQ_MODE_SAFE);

  //AXI initialization
  //init_platform();
  my_write_adapter = init_write_adapter(
      XPAR_WRITE_ADAPTER_0_S00_AXI_BASEADDR);
  my_read_adapter = init_read_adapter(XPAR_READ_ADAPTER_0_S00_AXI_BASEADDR);
  set_reg_read_adapt(my_read_adapter, reg_en_read_adapt);

  //SD-Card Initialization

  FRESULT Res;

  TCHAR *Path = "0:/";
  Res = f_mount(&fatfs, Path, 0);
  if (Res != FR_OK) {
    return XST_FAILURE;
  }

  /*Res = f_mkfs(Path, FM_FAT32, 0, work, sizeof work);
    if (Res != FR_OK) {
    return XST_FAILURE;
    }*/

  //Additional Initialization
  initPop = true;
}

static void getTemperatureData(float* data) {
  u32 TempRawData;

  TempRawData = XAdcPs_GetAdcData(XAdcInstPtr, XADCPS_CH_TEMP);
  data[0] = XAdcPs_RawToTemperature(TempRawData);

  TempRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr, XADCPS_MAX_TEMP);
  data[1] = XAdcPs_RawToTemperature(TempRawData);

  TempRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr, XADCPS_MIN_TEMP);
  data[2] = XAdcPs_RawToTemperature(TempRawData & 0xFFF0);
}

static void getVoltageData(float *data, VoltageType type) {
  u32 VccRawData;
  switch (type) {
    case VCCPINT: {
                    VccRawData = XAdcPs_GetAdcData(XAdcInstPtr, XADCPS_CH_VCCPINT);
                    data[0] = XAdcPs_RawToVoltage(VccRawData);

                    VccRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr,
                        XADCPS_MAX_VCCPINT);
                    data[1] = XAdcPs_RawToVoltage(VccRawData);

                    VccRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr,
                        XADCPS_MIN_VCCPINT);
                    data[2] = XAdcPs_RawToVoltage(VccRawData);
                    break;
                  }
    case VCCPAUX: {
                    VccRawData = XAdcPs_GetAdcData(XAdcInstPtr, XADCPS_CH_VCCPAUX);
                    data[0] = XAdcPs_RawToVoltage(VccRawData);

                    VccRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr,
                        XADCPS_MAX_VCCPAUX);
                    data[1] = XAdcPs_RawToVoltage(VccRawData);

                    VccRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr,
                        XADCPS_MIN_VCCPAUX);
                    data[2] = XAdcPs_RawToVoltage(VccRawData);
                    break;
                  }
    case VCCPDRO: {
                    VccRawData = XAdcPs_GetAdcData(XAdcInstPtr, XADCPS_CH_VCCPDRO);
                    data[0] = XAdcPs_RawToVoltage(VccRawData);

                    VccRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr,
                        XADCPS_MAX_VCCPDRO);
                    data[1] = XAdcPs_RawToVoltage(VccRawData);

                    VccRawData = XAdcPs_GetMinMaxMeasurement(XAdcInstPtr,
                        XADCPS_MIN_VCCPDRO);
                    data[2] = XAdcPs_RawToVoltage(VccRawData);
                    break;
                  }
  }
}

//static unsigned int countSetBits(unsigned int n) {
//	unsigned int count = 0;
//	while (n) {
//		count += n & 1;
//		n >>= 1;
//	}
//	return count;
//}

static int openFile(FIL* fp, const TCHAR* filename) {
  //FRESULT res = f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
  //if (res == FR_NO_FILE) {
  //Create new File
  FRESULT res = f_open(fp, filename,
      FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
  if (res == FR_NO_FILE) {
    xil_printf("Error no File %s\r\n", filename);
    return -1;
    //res = FR_OK; //f_open returns FR_NO_FILE even if FA_CREATE_ALWAYS is set.
  }
  if (res != FR_OK) {
    xil_printf("Error making the File %s\r\n", filename);
    return -1;
  }
  res = f_lseek(fp, 0);
  if (res) {
    xil_printf("Error setting the pointer inside the File %s to 0\r\n",
        filename);
    return -1;
  }
  return 0;
}

static void writeDataToFile(FIL* fp, char* dataString, size_t dataSize) {
  UINT NumBytesWritten; //For debug purposes
  u8 writeBuffer[dataSize] __attribute__ ((aligned(32)));
  for (int i = 0; i < dataSize; i++) {
    writeBuffer[i] = dataString[i];
  }
  FRESULT res = f_write(fp, (const void*) writeBuffer, dataSize,
      &NumBytesWritten);
  if (res != FR_OK) {
    xil_printf("Error writing to a File\r\n");
  }
}

static void setPUFTune(u16 topTune, u16 bottomTune) {
  u64 tune_set = 0;
  if (--topTune > 31)
    topTune = 31; //Max SRL Tune 0b11111
  if (--bottomTune > 31)
    bottomTune = 31; //Max SRL Tune 0b11111
  tune_set = tune_set | (topTune << 5) | bottomTune;
  tune_set |= (((u64) PUF_TUNE_SET) << SET_SHIFT);

  push_data(my_read_adapter, &tune_set, push_64_bit);
}

static void setPUFChoice(u8 top, u8 bottom) {
  u64 choice_set = 0;
  if (top > 3)
    top = 3;
  if (bottom > 3)
    bottom = 3;
  if (bottom == top) {
    top = 3;
    bottom = 0;
    xil_printf("Invalid SR choice, default choice is 3-0.\r\n");
  }
  choice_set = choice_set | (top << 2) | bottom;
  choice_set |= (((u64) PUF_CHOICE_SET) << SET_SHIFT);

  push_data(my_read_adapter, &choice_set, push_64_bit);
}

static void setPUFPattern(u64 pattern, bool top, u16 topTune, u16 bottomTune) { //TODO: Patternfehler! 0 => 1 => 0
  if (--topTune > 31)
    topTune = 31; //Max SRL Tune 0b11111
  if (--bottomTune > 31)
    bottomTune = 31; //Max SRL Tune 0b11111

  u64 pattern_set = pattern & 0x1FFFFFFFF; //Take first 33 Bits - 32 Bit Pattern + 1 Bit Shift in Pattern
  //Convert Pattern into a correct one
  if (top) {
    //Top Tune
    pattern_set &= ~((u64) 0x1 << (31 - topTune));
    pattern_set |= ((u64) 0x1 << (32 - topTune));
    pattern_set |= (((u64) TOP_PATTERN_SET) << SET_SHIFT);
  } else {
    pattern_set |= ((u64) 0x1 << (31 - bottomTune));
    pattern_set &= ~((u64) 0x1 << (32 - bottomTune));
    pattern_set |= (((u64) BOTTOM_PATTERN_SET) << SET_SHIFT);
  }

  push_data(my_read_adapter, &pattern_set, push_64_bit);
}

static void genPUFOutput(u32* output) {	//TODO: If Tuning is changed after the pattern was set prevent the output from generating before pattern is set again
  u64 data_in = (((u64) PUF_REQ) << 60);
  push_data(my_read_adapter, &data_in, push_64_bit);
  if (initPop) {
    pop_data(my_write_adapter, output, pop_128_bit);
    //initPop = false;
  }
  //usleep(200);
  pop_data(my_write_adapter, output, pop_128_bit);
}

/****************************************************************************/
/**
 *
 * This function converts the fraction part of the given floating point number
 * (after the decimal point)to an integer.
 *
 * @param	FloatNum is the floating point number.
 *
 * @return	Integer number to a precision of 3 digits.
 *
 * @note
 * This function is used in the printing of floating point data to a STDIO device
 * using the xil_printf function. The xil_printf is a very small foot-print
 * printf function and does not support the printing of floating point numbers.
 *
 *****************************************************************************/
int XAdcFractionToInt(float FloatNum) {
  float Temp;

  Temp = FloatNum;
  if (FloatNum < 0) {
    Temp = -(FloatNum);
  }

  return (((int) ((Temp - (float) ((int) Temp)) * (1000.0f))));
}

//static void usleep(unsigned int usecs) {
//	int i, j;
//	for (j = 0; j < usecs; j++) {
//		for (i = 0; i < 15; i++)
//			asm("nop");
//	}
//}

int keyfromstring(char *key)
{
    int i;
    for (i=0; i < NKEYS; i++) {
        if (strcmp(lookuptable[i].key, key) == 0)
            return lookuptable[i].val;
    }
    return BADKEY;
}
#endif /* SRC_CHOICE_PUF_H_ */
