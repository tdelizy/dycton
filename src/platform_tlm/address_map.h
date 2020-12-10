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

#ifndef ADDRESS_MAP_H
#define ADDRESS_MAP_H

/*
In this file : 
	- memory technologies under test timming definitions
	- platform address map definition
	- allocation strategy names and id for dispatcher definitions

*/


//------------------------------------------------------------------------------
// MEMORY TECHNOLOGIES DEFINITION
//------------------------------------------------------------------------------

/* memory technologies under test */

/* test memory */
// #define MEM_FAST_RLAT (1)
// #define MEM_FAST_WLAT (1)
// #define MEM_SLOW_RLAT (10)
// #define MEM_SLOW_WLAT (10)

/* tech eVaderis : fast MRAM vs dense MRAM */
#define MEM_FAST_RLAT (1)
#define MEM_FAST_WLAT (3)
#define MEM_SLOW_RLAT (2)
#define MEM_SLOW_WLAT (30)

/* tech intel SRAM vs RERAM*/
// #define MEM_FAST_RLAT (1)
// #define MEM_FAST_WLAT (1)
// #define MEM_SLOW_RLAT (4)
// #define MEM_SLOW_WLAT (4000)


//------------------------------------------------------------------------------
// ADDRESS_MAP
//------------------------------------------------------------------------------
/* base address required by MIPS processor */
#define RESET_BASE              (0xBFC00000)
#define RESET_SIZE              (0x00010000) // 64ko

#define EXCEP_BASE              (0x80000000)
#define EXCEP_SIZE              (0x00010000) // 64ko

#define DATA_BASE               (0x10000000)
#define DATA_SIZE               (0x00080000) // 512ko
#include "hardware/offsets/memory.h"

#define TEXT_BASE               (0x00400000)
#define TEXT_SIZE               (0x00080000) // 512kB

#define STACK_SIZE              (0x00080000) // 512ko
#define STACK_TOP               (DATA_BASE + DATA_SIZE + STACK_SIZE)
/*
 * CAUTION ! 
 *
 * SIZE OF SPM BANKS SHOULD ALWAYS BE ROUNDED TO 16
 * OTHERWISE MALLOC HEAPS MEMORY ZONES MAY BE UNALIGNED
 * (need to check that in sbrk (hal.c) and in DLMalloc (newlib_malloc.c))
 */

#define HEAP_BASE               (STACK_TOP)

// SPM access addresses from the bus
#define SPM_BASE                (0x00000000)
#define SPM_I_BASE              (SPM_BASE)
#define SPM_I_SIZE              (TEXT_SIZE)
#define SPM_D_BASE              (SPM_I_BASE + SPM_I_SIZE)
#define SPM_D_SIZE              (DATA_SIZE)
#define SPM_S_BASE              (SPM_D_BASE + DATA_SIZE)
#define SPM_S_SIZE              (STACK_SIZE)
#define SPM_GP_BASE             (HEAP_BASE)
// #define SPM_GP_SIZE             (HEAP_SIZE)
// #define SPM_SIZE                (SPM_I_SIZE + SPM_D_SIZE + SPM_GP_SIZE)

// Dense memory on the bus
#define DENSE_MEM_BASE          (0xE0000000)
#define DENSE_MEM_SIZE          (0x00600000) // 6Mo (needed for h263 benchmark)

#if defined(malloc_test) || defined(matmul) || defined(mergesort) || defined(dummy_memset)
#define TST_BASE                (DENSE_MEM_BASE)
#define TST_SIZE                (DENSE_MEM_SIZE)
#endif

#define HELPER_BASE             (0x41C00000)
#define HELPER_SIZE             (0x00001000)
#include "hardware/offsets/helper.h"

#if defined(lzw) || defined(moving_average) || defined(json_parser) || defined(jpeg)
#define IN_BUF_BASE             (0x42000000)
#define OUT_BUF_BASE            (0x43000000)
#endif

#define UART_BASEADDR           (0x40600000)
#define UART_SIZE               (0x00010000)
#include "hardware/offsets/uart.h"



//------------------------------------------------------------------------------
// ALLOCATION STRATEGY DEFINITIONS
//------------------------------------------------------------------------------
/* strategies for allocation */
#define DEFAULT_STRATEGY 				(0)
#define ORACLE_STRATEGY 				(1)
#define PROFILE_STRATEGY 	(2)
#define PROFILE_ENHANCED 				(3)
#define PROFILE_ILP 					(4)

// strategy names for argument parsing
#define DEFAULT_STRAT_STRING "default"
#define ORACLE_STRAT_STRING "oracle"
#define PROFILE_STRAT_STRING "profile"
#define PROFILE_ENHANCED_STRING "profile-enhanced"
#define PROFILE_ILP_STRING "profile-ilp"

#define STRAT_COUNT 5


//------------------------------------------------------------------------------
/* HEAP MEMORY UTILITIES */
//------------------------------------------------------------------------------

#define dycton_align_16(x) 	(((x+8)/16)*16) //align heaps base on 16 Bytes



#endif


