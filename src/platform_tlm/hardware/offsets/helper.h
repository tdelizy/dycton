/*

This file is part of the Dycton simulator.
This software aims to provide an environment for Dynamic Heterogeneous Memory 
Allocation for embedded devices study. It is build using SystemC / TLM.
It uses the MIPS32 ISS from the SocLib project (www.soclib.fr). 
It also use one SimSoc module (https://gforge.inria.fr/projects/simsoc/)
(originals athors credited in respective files)

Copyright (C) 2019  Tristan Delizy, CITI Lab, INSA de Lyon

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef HELPER_H_OFFSETS
#define HELPER_H_OFFSETS

#define STOP_SIM 				(0x0)

#define ALLOC_SIZE 				(0x4) // write at this address at the begining of malloc call
#define ALLOC_ADDR 				(0x8) // write at this address just before returning the allocated mem zone

#define FREE_ADDR 				(0xC) // write at this address at the begining of free call
#define FREE_SIZE 				(0x10) // write at this address just before returning from free

#define TIMED_EVENT 			(0x14) //for logging events (logs the time of write and the number written)

#define ALLOC_FAIL_OFFSET 		(0x18) // way to log if malloc fail on a heap and need to fallback to another

#define ALLOCATOR_STRATEGY 		(0x20) // read only : return the strategy number to use
#define ALLOCATOR_ORACLE_REQ 	(0x24) // read only : ask the oracle in which heap place the current request

#define PRINT_REGS 				(0x30) // write to output (outside of simulation) the current processor registers values

#define RETURN_ADDR 			(0x40) // log the return address of a malloc call

#define PROFILE_LEN 			(0x50) // read value of profile list length

#define ENTROPY_GENERATOR 		(0x60) // read value from seed file

#define HEAP_COUNT_INFO 		(0x70) // simulation info, in real life we know that before compiling, but its a parameter for the simulator

#define DATASET_INDEX			(0x80) // simulation info, could also be a main input

#define HEAP_ARCHI				(0x90) // simulation info, in real life we know that before compiling, but its a parameter for the simulator




//---------------------------------------------------------------------------- range offsets
#define HELPER_RANGE_LEN		(0x100) // width of a range access

#define HEAP_BASE_INFO			(0x300) // address range acceded for offset + heap_nb * 4

#define HEAP_SIZE_INFO			(0x400) // address range acceded for offset + heap_nb * 4





#endif 