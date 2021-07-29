#!/usr/bin/env python3

import sys
import os
import re
import glob
import time
import serial
import serial.tools.list_ports
import subprocess
import select


def setUpChamber():
    print("Please enter the codesigns server the climate chamber is running on: ", end='')
    tcLoc = input()
    print("Use these climate chamber commands:")
    for cmd in list(tempControlCommands.keys()):
        print(cmd)

def serial_ports():
    global portPrefix
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
        portPrefix = "COM"
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
        portPrefix = "/dev/tty"
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
        portPrefix = "/dev/tty"
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def activePorts():
    global portsInUse
    print("Serial ports currently in use:")
    for i in range(len(portsInUse)):
      print(portsInUse[i])

def exitMain():
    global portsInUse
    msg = "exit\n"
    chars = []
    for c in msg:
        chars.append(ord(c))
        chars = list(map(int, chars))
    for i in range(len(portsInUse)):
        portsInUse[i].write(chars)
        portsInUse[i].flush()
        portsInUse[i].close
    portsInUse = []
    exit()

def configurePort(mytimeout=1):
   global portPrefix
   print("Please select serial port to connect to e.g., USB0 : ", end='')
   target_ports = input().split(" ")
   for port in target_ports:
       port = portPrefix + port
       #print(portPrefix, port," connection")
       try:
           tty = serial.Serial(port, baudrate=230400, bytesize=8,timeout=mytimeout)
           tty.reset_input_buffer() # just to make sure that buffers are empty
           tty.reset_output_buffer()
           portsInUse.append(tty)
       except (OSError, serial.SerialException):
           print("An error occurred when connecting to port " + port + ".\n")
           continue

def connectPort(mytimeout=1):
    ports = serial_ports()
    print("Currently available serial ports:")
    for port in ports:
        print(port)

    configurePort(mytimeout=1) 

def disconnectPort():
    print("Serial ports currently in use:")
    for port in ports:
        print(port)
    print("Please select serial ports to disconnect from: ", end='')
    target_port = input().split(" ")
    for port in target_port:
        port = portPrefix + port
        portsInUse[port].close
        del portsInUse[port]

def execScript():
    print("Please enter the path of the script: ", end='')
    path = input()
    script = open(path, "r")
    print("Enter a file name to create a log of the PUF measurement, leave empty if logging is not necessary: ", end='')
    logPath = input()
    if(logPath != ""):
        logFile = open(logPath, "w")
    cmds = script.readlines()
    ports = serial_ports()
    print("Currently available serial ports:")
    for port in ports:
        print(port)
    print("Please enter on how many devices the script shall be executed:")
    nr_device = int(input())
    for i in range(nr_device):
      configurePort(0.09)#set a really low timeout to maximize measurement speed
    
    print("Start executing script pls. be patient")
    start = time.time()
    for cmd in cmds:
        result = ""
        if(not cmd.endswith('\n')):
            msg = cmd + "\n"
        else:
            msg = cmd
        cmdSplit = cmd.split(' ')
        if (tempControlCommands.keys().__contains__(cmdSplit[0])):
            tcArgs = ["../CPP/tempchamber/build/./TCcontrol", "codesigns" + tcLoc + ".informatik.uni-erlangen.de", "2049", tempControlCommands[cmdSplit[0]]]
            if (len(cmdSplit) > 1):
                tcArgs.append(cmdSplit[1])
            tempControl = subprocess.Popen(tcArgs, stdout=subprocess.PIPE)
            line = tempControl.stdout.readline().decode("utf-8").rstrip()
            logFile.write("TCcontrol returned: " + line)
            continue
        chars = []
        for c in msg:
            chars.append(ord(c))
            chars = list(map(int, chars))
        for i in range(len(portsInUse)):
            portsInUse[i].write(chars)
            portsInUse[i].flush()
            result += str(portsInUse[i].readline())
            print("\r.")
            if (logPath != ""):#result != "" and 
                logFile.write("Executed command: " + cmd + "\n")
                logFile.write("Result of device of port " + str(portsInUse[i]) + ":\n")
                logFile.write(result.rstrip()+"\n")
    if (logPath != ""):#result != "" and 
        end = time.time()
        logFile.write("Measurement took me " + str(end-start) + " seconds\n")
    exitMain()

def parseCommand(cmd):
    global portsInUse
    argv = cmd.split(" ")
    if(FPGAcommands.keys().__contains__(argv[0])):
        if(len(portsInUse) == 0):
            if (FPGAcommands[argv[0]] is None):
                print("you're not connected to a serial port run cmd - connectPort - first")
                return
            else:
                FPGAcommands[argv[0]]()
                return
    elif(tempControlCommands.keys().__contains__(argv[0])):
        tcArgs = ["../CPP/tempchamber/build/./TCcontrol", "codesigns" + tcLoc + ".informatik.uni-erlangen.de", "2049", tempControlCommands[argv[0]]]
        if(len(argv) > 1):
            tcArgs.append(argv[1])
        tempControl = subprocess.Popen(tcArgs,  stdout=subprocess.PIPE)
        line = tempControl.stdout.readline().decode("utf-8").rstrip()
        print("TCcontrol returned: " + line)
        return
    else:
        if(len(ports)== 0):
            print("Invalid command use one of these:")
            for cmd in list(FPGAcommands.keys()) + list(tempControlCommands.keys()):
               print(cmd)
            return
    msg = (' '.join(argv) + "\n")
    chars = []
    for c in msg:
        chars.append(ord(c))
        chars = list(map(int, chars))
    portsInUse[0].write(chars)
    portsInUse[0].flush()
    result = ""
    line = "dummy"
    while not len(line) == 0:
        line = str(portsInUse[0].readline())
        if(FPGAcommands.keys().__contains__(argv[0])):
            result += line
        elif("Shutting down PUF Management System on the FPGA!!!" in line):
            print("Restart PUF Management System if you want to continue")
            if("exit" in str(argv[0])):
                return -1
            else:
                break
        else:
            print("the command ",argv[0]," is not supported")
            print("Use these FPGA commands:")
            showCommands()

    if(result != ""):
        print("Result of device of port " + str(portsInUse[0]) + ":")
        print(result.rstrip())
        

def showCommands():
    for cmd in list(FPGAcommands.keys()):
        print(cmd)

def main():

    print("Script to configure and readout CHOICE PUF on the FPGA and control the climate chamber")
    print("Use these FPGA commands:")
    showCommands()

    while(1):
        print("Please enter a command: ", end='')
        cmd = input()
        result = parseCommand(cmd)
        if(result == -1):
            exitMain()

# global variables
portsInUse = [] # Port to use for communication e.g., USB0
portPrefix = "" # path to serial com port depends on OS e.g., /dev/tty...
tcLoc = "75"    # climate chamber is currently running on codesigns75
tempControlCommands = {
        "TCsetTemp"     : "-s", 
        "TCgetTemp"     : "-g", 
        "TCsetState"    : "-r", 
        "TCgetState"    : "-q", 
        "TCinfo"        : "-i",
        "TCsetTempWait" : "-w"
        }

FPGAcommands = {
    # connects to the serial port the FPGA is connected
    "connectPort"    : connectPort,
    # shows active connections of serial ports with FPGAs
    "activePorts"    : activePorts,
    # disconnect active connections of serial ports with FPGAs
    "disconnectPort" : disconnectPort,           
    # automate measurements by running a measurement script
    "execScript"     : execScript,
    # configures a connection to the climate chamber
    "TempMeasurement": setUpChamber,
    # shows all files on the SD card
    "listFiles"      : None,
    # creates a file on the SD card, where all measurements are stored in a readable format
    "setTargetFile"  : None,
    # reads the measurements from file and sends them via serial com
    "sendFile"       : None,
    # allows to write a custom message to the measurements file on the FPGA
    "writeLine"      : None,
    # get current, max, and min chip temperature
    "getTemp"        : None,
    # get current chip voltage PINT, PAUX, PDRO
    "getVcc"         : None,
    "setTune"        : None,
    "setChoice"      : None,
    "setPattern"     : None,
    "getReadout"     : None,
    #prints all commands that are supported by these script
    "showCommands"   : showCommands,
    #prints all commands that are supported by the FPGA
    "help"           : None,
    #stop PUF and climate measurements
    "exit"           : exitMain
    }

if __name__ == "__main__": main()
