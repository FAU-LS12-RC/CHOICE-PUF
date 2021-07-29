/*
 * CHOICE_PUF.c
 *
 *  Created on: Mar 18, 2021
 *      Author: Krüger, Streit
 */

#include "CHOICE_PUF.h"

/***********Basic defines and function prototypes************/
//Functions
static int startManager();
static int parseCommands(int argc, char* argv[]);
void xil_sprintf(char* s, const char *ctrl1, ...);

//Globals
FIL targetFile;
u16 currentTopTune = 32;
u16 currentBottomTune = 32;
u8 currentChoice[2] = { 3, 0 };
u64 currentTopPattern = 0x2AAAAAAAA;
u64 currentBottomPattern = 0x155555555;

//Macros
#define MAXINPUTBUFLEN 256
#define MAXARGS 32
/*****************************************************************/

int main() {
	initialization();
	startManager();
	while (1) {
		char command[MAXINPUTBUFLEN] = "";
		int c, i = 0;
		while ((c = inbyte()) != '\r' && c != '\n' && c != EOF && c != 0) {
			command[i++] = c;
			if(i==MAXINPUTBUFLEN)
				i=0;
		}
		command[i] = '\0';
		char* ptr;
		char* argv[MAXARGS];
		int argc = 0;

		// Use a synchronization mechanism to ensure UART buffers are empty
		if(command[0]==0)
		{
			xil_printf("Sync\n");
			continue;
		}

		ptr = strtok(command, " ");
		while (ptr && argc < MAXARGS - 1) {
			argv[argc++] = ptr;
			ptr = strtok(NULL, " ");
		}
		argv[argc] = 0;

		int result = parseCommands(argc, argv);
		if (result == -1) {
			xil_printf("Error!!! closing file\r\n");
			f_close(&targetFile);
			break;
		} else if (result == -2) {
			xil_printf("Goodbye\r\n");
			f_close(&targetFile);
			break;
		}
	}
	xil_printf("Shutting down PUF Management System on the FPGA!!!\r\n");
	return 0;
}

static int startManager() {
	xil_printf("PUF Management System started.\r\n");
	//might be required to have an initialization routine.
	return 0;
}

static int parseCommands(int argc, char* argv[]) {
	int ret_val = 0;
	float temps[3] = { };
	u32 output[4] = { };
	char *fn;
	FRESULT res;
	FILINFO fno;
	DIR dir;

	switch (keyfromstring(argv[0])) {

	case getTemp:
		getTemperatureData(temps);
		xil_printf("Current Temperature: %0d.%03d\r\n"
				"Max Temperature: %0d.%03d\r\n"
				"Min Temperature: %0d.%03d\r\n", (int) (temps[0]),
				XAdcFractionToInt(temps[0]), (int) (temps[1]),
				XAdcFractionToInt(temps[1]), (int) (temps[2]),
				XAdcFractionToInt(temps[2]));
		ret_val = 0;
		break;

	case getVcc:
		if (argc == 1) {
			float data[3][3] = { };
			getVoltageData(data[0], VCCPINT);
			getVoltageData(data[1], VCCPAUX);
			getVoltageData(data[2], VCCPDRO);
			xil_printf("Current Voltage at PINT: %0d.%03d\r\n"
					"Max Voltage: %0d.%03d\r\n"
					"Min Voltage: %0d.%03d\r\n", (int) data[0][0],
					XAdcFractionToInt(data[0][0]), (int) data[0][1],
					XAdcFractionToInt(data[0][1]), (int) data[0][2],
					XAdcFractionToInt(data[0][2]));
			xil_printf("Current Voltage at PAUX: %0d.%03d\r\n"
					"Max Voltage: %0d.%03d\r\n"
					"Min Voltage: %0d.%03d\r\n", (int) data[1][0],
					XAdcFractionToInt(data[1][0]), (int) data[1][1],
					XAdcFractionToInt(data[1][1]), (int) data[1][2],
					XAdcFractionToInt(data[1][2]));
			xil_printf("Current Voltage at PDRO: %0d.%03d\r\n"
					"Max Voltage: %0d.%03d\r\n"
					"Min Voltage: %0d.%03d\r\n", (int) data[2][0],
					XAdcFractionToInt(data[2][0]), (int) data[2][1],
					XAdcFractionToInt(data[2][1]), (int) data[2][2],
					XAdcFractionToInt(data[2][2]));
			ret_val = 0;
		} else {
			float data[3] = { };
			for (int i = 1; i < argc; i++) {
				int type;
				if (strcmp(argv[i], "PINT")) {
					type = VCCPINT;
					ret_val = 0;
				} else if (strcmp(argv[i], "PAUX")) {
					type = VCCPAUX;
					ret_val = 0;
				} else if (strcmp(argv[i], "PDRO")) {
					type = VCCPDRO;
					ret_val = 0;
				} else {
					xil_printf(
							"Unrecognized Argument.\r\nUsage: getVcc <PINT> <PAUX> <PDRO>\r\n");
					ret_val = -1;
				}
				if (ret_val == 0) {
					memset(data, 0, sizeof(float) * 3);
					getVoltageData(data, type);
					xil_printf("Current Voltage at %s: %0d.%03d\r\n"
							"Max Voltage: %0d.%03d\r\n"
							"Min Voltage: %0d.%03d\r\n", argv[i], (int) data[0],
							XAdcFractionToInt(data[0]), (int) data[1],
							XAdcFractionToInt(data[1]), (int) data[2],
							XAdcFractionToInt(data[2]));
				}
			}
		}
		break;

	case setTune:
		if (argc != 3) {
			//Usage Message
			xil_printf("wrong number of setTune arguments\r\n");
			ret_val = -1;
		} else {
			u16 toptune = (u16) strtol(argv[1], NULL, 16);
			u16 bottomtune = (u16) strtol(argv[2], NULL, 16);
			if (toptune == 0)
				toptune = currentTopTune;
			if (bottomtune == 0)
				bottomtune = currentBottomTune;
			setPUFTune(toptune, bottomtune);
			currentTopTune = toptune;
			currentBottomTune = bottomtune;
			ret_val = 0;
		}
		break;

	case setChoice:
		if (argc != 3) {
			//Usage Message
			xil_printf("wrong number of setChoice arguments\r\n");
			ret_val = -1;
		} else {
			u8 topChoice = (u8) strtol(argv[1], NULL, 10);
			u8 bottomChoice = (u8) strtol(argv[2], NULL, 10);
			setPUFChoice(topChoice, bottomChoice);
			currentChoice[0] = topChoice;
			currentChoice[1] = bottomChoice;
			ret_val = 0;
		}
		break;

	case setPattern:
		if (argc != 4) {
			//Usage Message
			xil_printf("wrong number of setPattern arguments\r\n");
			ret_val = -1;
		} else {
			u64 pattern_in = strtol(argv[2], NULL, 16);
			u64 pattern_init = strtoll(argv[3], NULL, 16) & 0xFFFFFFFF;
			u64 pattern = (pattern_in << 32) | pattern_init;
			if (strcmp(argv[1], "-t") == 0) {
				setPUFPattern(pattern, true, currentTopTune, currentBottomTune);
				currentTopPattern = pattern;
				ret_val = 0;
			} else if (strcmp(argv[1], "-b") == 0) {
				setPUFPattern(pattern, false, currentTopTune,
						currentBottomTune);
				currentBottomPattern = pattern;
				ret_val = 0;
			} else {
				//Usage Message
				xil_printf("wrong setPattern argument use either -t or -b\r\n");
				ret_val = -1;
			}
		}
		break;

	case getReadout: //TODO: Tuning Ausgabe nur SRL anpassen + andere Ausgaben
		if (argc < 2) {
			xil_printf("wrong number of getReadout arguments\r\n");
			ret_val = -1;
		} else {
			int i;
			for (i=1; i < argc - 1; i++) {
				if (strcmp(argv[i], "-tune") == 0) {
					char data[40] = "";
					xil_sprintf(data,
							"Current Tuning: Top - %02x | Bottom - %02x\r\n",
							currentTopTune, currentBottomTune);
					writeDataToFile(&targetFile, data, 40);
					ret_val = 0;
					//DEBUG
					//xil_printf("%s", data);
				} else if (strcmp(argv[i], "-ch") == 0) {
					char data[23] = "";
					xil_sprintf(data, "Current Choice: %i - %i\r\n",
							currentChoice[0], currentChoice[1]);
					writeDataToFile(&targetFile, data, 23);
					ret_val = 0;
				} else if (strcmp(argv[i], "-p") == 0) {
					char data[55] = "";
					xil_sprintf(data,
							"Current Pattern: Top - %01x%08x | Bottom - %01x%08x\r\n",
							(u32) (currentTopPattern >> 32),
							(u32) currentTopPattern,
							(u32) (currentBottomPattern >> 32),
							(u32) currentBottomPattern);
					writeDataToFile(&targetFile, data, 55);
					ret_val = 0;
				} else if (strcmp(argv[i], "-temp") == 0) {
					float temps[3] = { };
					getTemperatureData(temps);
					char data[53] = "";
					xil_sprintf(data,
							"Current Temperature: %0d.%03d Max: %0d.%03d Min: %0d.%03d\r\n",
							(int) temps[0], XAdcFractionToInt(temps[0]),
							(int) temps[0], XAdcFractionToInt(temps[0]),
							(int) temps[0], XAdcFractionToInt(temps[0]));
					writeDataToFile(&targetFile, data, 53);
					ret_val = 0;
					//DEBUG
					//xil_printf("%s", data);
				} else if (strcmp(argv[i], "-vcc") == 0) {
					float data[3][3] = { };
					getVoltageData(data[0], VCCPINT);
					getVoltageData(data[1], VCCPAUX);
					getVoltageData(data[2], VCCPDRO);
					char text[50] = "";
					xil_sprintf(text,
							"PINT Voltage: Current %0d.%03d Max %0d.%03d Min: %0d.%03d\r\n",
							(int) data[0][0], XAdcFractionToInt(data[0][0]),
							(int) data[0][1], XAdcFractionToInt(data[0][1]),
							(int) data[0][2], XAdcFractionToInt(data[0][2]));
					writeDataToFile(&targetFile, text, 50);
					//DEBUG
					//xil_printf("%s", text);
					xil_sprintf(text,
							"PAUX Voltage: Current %0d.%03d Max %0d.%03d Min: %0d.%03d\r\n",
							(int) data[1][0], XAdcFractionToInt(data[1][0]),
							(int) data[1][1], XAdcFractionToInt(data[1][1]),
							(int) data[1][2], XAdcFractionToInt(data[1][2]));
					writeDataToFile(&targetFile, text, 50);
					//DEBUG
					//xil_printf("%s", text);
					xil_sprintf(text,
							"PDRO Voltage: Current %0d.%03d Max %0d.%03d Min: %0d.%03d\r\n",
							(int) data[2][0], XAdcFractionToInt(data[2][0]),
							(int) data[2][1], XAdcFractionToInt(data[2][1]),
							(int) data[2][2], XAdcFractionToInt(data[2][2]));
					writeDataToFile(&targetFile, text, 50);
					ret_val = 0;
					//DEBUG
					//xil_printf("%s", text);
				} else {
					//Usage
					ret_val = -1;
				}
			}
			if (ret_val == 0) {
				int readsize = strtol(argv[i], NULL, 10);
				int readsizedigits = floor(log10(readsize - 1)) + 1;
				for (int i = 0; i < readsize; i++) {
					genPUFOutput(output);
					char result[41 + readsizedigits];
					memset(result, 0, 41 + readsizedigits);
					sprintf(result, "Read %*i: %08lx%08lx%08lx%08lx\r\n",
							readsizedigits, i, output[3], output[2], output[1],
							output[0]);
					writeDataToFile(&targetFile, result, 41 + readsizedigits);
					//DEBUG
					//xil_printf("%s\r\n", result);
				}
			}
		}
		break;

	case listFiles:
		res = f_opendir(&dir, "0:/"); /* Open the directory */
		if (res == FR_OK) {
			while (1) {
				res = f_readdir(&dir, &fno); /* Read a directory item */
				if (res != FR_OK || fno.fname[0] == 0)
					xil_printf(
							"Error occurred when reading file with listFiles\r\n");
				ret_val = -1;
				break; /* return on error or end of dir */
				if (fno.fname[0] == '.')
					continue; /* Ignore dot entry */
				fn = fno.fname;
				if (fno.fattrib & AM_DIR) { /* It is a directory */
					continue;
				} else { /* It is a file. */
					xil_printf(" %s%s\r\n", "0:/", fn);
					break;
				}
			}
			f_closedir(&dir);
			ret_val = 0;
		} else {
			xil_printf("Error occurred when opening file with listFiles\r\n");
			ret_val = -1;
		}
		break;

	case setTargetFile: {
		if (argc != 2) {
			//Usage
			xil_printf("wrong number of setTargetFile arguments\r\n");
			ret_val = -1;
		}
		char fullFileName[strlen(argv[1]) + 1];
		strcpy(fullFileName, argv[1]);
		char* fileName;
		char* fileExt;
		int ret = 0;
		fileName = strtok(argv[1], ".");
		fileExt = strtok(NULL, ".");
		if (strlen(fileName) <= 8 && strlen(fileName) > 0
				&& strlen(fileExt) <= 3 && strlen(fileExt) > 0) {
			f_close(&targetFile);
			ret = openFile(&targetFile, (char*) fullFileName);
			if (ret != FR_OK) {
				xil_printf("could not open file\r\n");
				ret_val = -1;
			} else {
				xil_printf("file created\r\n");
				ret_val = 0;
			}
		} else {
			//Wrong Filename Format
			xil_printf("wrong setTargetFile input\r\n");
			ret_val = -1;
		}
		}
		break;

	case writeLine:
		if (argc <= 1) {
			//Usage
			xil_printf("wrong number of writeLine arguments\r\n");
			ret_val = -1;
		} else {
			int length = 2 + argc - 2;
			for (int i = 1; i < argc; i++) {
				length += strlen(argv[i]);
			}
			char msg[length];
			memset(msg, 0, length);
			strcpy(msg, argv[1]);
			for (int i = 2; i < argc; i++) {
				strcat(msg, " ");
				strcat(msg, argv[i]);
			}
			strcat(msg, "\r\n");
			writeDataToFile(&targetFile, msg, length);
			ret_val = 0;
	}
		break;

	case sendFile: {
		FRESULT res = f_lseek(&targetFile, 0);
		if (res == FR_OK) {
			xil_printf("File transmission started.\r\n");
			UINT numBytesRead; //For debug purposes
			u8 readBuffer[128] = "";
			while (f_read(&targetFile, (void*) readBuffer, 127, &numBytesRead)
					== FR_OK && numBytesRead > 0) {
				//const char* test = readBuffer;
				xil_printf("%s", readBuffer);
				memset(readBuffer, 0, 128);
			}
			xil_printf("File transmission ended.\r\n");
			ret_val = 0;
		} else {
			xil_printf("File seek failed with sendFile.\r\n");
			ret_val = -1;
		}
	}
		break;

	case exit:
		// command to terminate PUF measurement was send
		ret_val = -2;
		break;

	case help:
		//Send info about command specified in the first argument
		xil_printf("Supported Commands:\r\n");
		int i;
		for (i = 0; i < NKEYS; i++) {
			xil_printf("%s\r\n", lookuptable[i].key);
		}
		xil_printf("Use \"help <command>\" to print this again\r\n");
		ret_val = 0;
		break;

	case BADKEY:
		ret_val = -1;
		xil_printf(
				"Command not recognized, type \"help\" for a list of supported commands.\r\n");
		break;

	default:
		ret_val = 0;
		xil_printf(
				"Command not recognized, type \"help\" for a list of supported commands.\r\n");
		break;
	}
	return ret_val;
}
