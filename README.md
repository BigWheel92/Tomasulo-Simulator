# Tomasulo-Simulator-in-C++-for-Windows

The Tomasulo-Simulator-in-C++-for-Windows simulates the tomasulo computer hardware algorithm. it shows the state after each step of execution. The code currently runs on Windows platform because it uses Window.h library.

The program requires an input file named "source.txt" which contains the information about:
 1. the number of reservation stations for add/sub, mul/div.
 2. the number of cycles of each instruction. 
 3. the total number of registers.
 4. the instructions to execute using Tomasulo Simulator.
 
 A sample input file is also present in the repository. The input file contains the following:
 
 
 #Enter the number of reservation stations and buffers below

Add_Sub_Reservation_Stations 3
Mul_Div_Reservation_Stations 2
Load_Buffers         3
Store_Buffers        1
 
#Enter the number of cycles of instructions
Add_Sub_Cycles 2
Mul_Cycles 10
Div_Cycles 40
Load_Store_Cycles 2


#Enter the total number of registers
REG 11

#Instructions go here. The number in the first line is the number of instructions
6
Add F2 34 R2
Store F2 45 R3
Mul  F0 F2 F4
Sub  F8 F6 F2
Div  F10 F0 F6
Add  F6 F8 F2
