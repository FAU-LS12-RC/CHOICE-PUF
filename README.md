# CHOICE-PUF
CHOICE, is a novel class of FPGA-based PUF designs with tunable uniqueness and reliability characteristics. By the use of addressable shift registers available on an FPGA, CHOICE provides a wide configuration space for adjusting a device-specific PUF response without sacrificing randomness. CHOICE can be used to design, prototype, and evaluate different PUF configurations by a hardware/software co-design on Xilinx Zynq Programmable System-on-Chip (PSoC) architectures.

A [paper](https://easychair.org/publications/preprint/2L3z) about CHOICE was presented at the [2021 International Conference on Field-Programmable Logic and Applications (FPL)](https://cfaed.tu-dresden.de/fpl2021/welcome-to-fpl2021). If you are using CHOICE and want to cite it, please reference the paper as follows: 

```bash
@inproceedings{streit2021choice,
  title={{CHOICE} – A Tunable {PUF}-Design for {FPGAs}},
  author={Streit, Franz-Josef and Kr{\"u}ger, Paul and Becher, Andreas and Schlumberger, Jens and Wildermann, Stefan and Teich, J{\"u}rgen},
  booktitle={31th International Conference on Field Programmable Logic and Applications (FPL)},
  year={2021},
  organization={IEEE}
}
```
# CHOICE-PUF Configuration Evaluation Framework
This project includes, a co-design for a Xilinx Zynq Programmable System-on-Chip (PSoC), where PUF configuration and readout routines are performed in [software](sw_src/CHOICE_PUF_SW/) on the CPU of the PSoC, while CHOICE PUF and configuration interfaces are implemented within the programmable logic of the [FPGA](hw_src/). The evaluation framework automates the tuning and response analysis. This framework consists of a host side and a client side, where the host side provides a user interface written in Python [[SerialComm.py](python_scripts/SerialComm.py)] to tune the PUF forming the client side within one or multiple boards and simultaneously read out their responses in an automated manner. The user interface allows various tuning configurations to be tested in a short time by sending setup commands to the client side via the serial UART interface. The client side written in C [software](sw_src/CHOICE_PUF_SW/) is in charge of interpreting these commands and then configuring the adressable shift registers within the programmable logic via a memory-mapped SW/HW interface.

You can write a configuration file similar to this [[PUFconfigScript.txt](python_scripts/PUFconfigScript.txt)] to setup the CHOICE PUF, readout PUF responses, readout chip informations such as on-chip temperature and supply voltage, we even added functionality to control a climate chamber.

## Authors
See the [AUTHORS](AUTHORS) file for details.
