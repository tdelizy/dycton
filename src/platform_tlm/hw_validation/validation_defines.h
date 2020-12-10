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

/*
 * Test platform address map and defines
 */

#define CUT_SIZE    (0x1000)
#define R_LAT       (1) // in platform cycles
#define W_LAT       (100) // in platform cycles

#define SPM_ADDRESS (0)
#define CUT_0_ADDR  (SPM_ADDRESS)
#define CUT_1_ADDR  (CUT_0_ADDR + CUT_SIZE)
#define CUT_2_ADDR  (CUT_1_ADDR + CUT_SIZE)

#define MAGIC_NB    (0xDEADBABA)
