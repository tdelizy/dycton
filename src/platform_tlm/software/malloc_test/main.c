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

#include <stdio.h>
#include <stdlib.h>

#include "hal.h"
#include "libdycton.h"
#include "address_map.h"


#define SIM_END            (*((int*)HELPER_BASE)=0)


int main(void)
{
    printf("\n\n\n================== Basic test for DYCTON ==================\n");
    printf("(TDk @ CITI Lab)\n\n");

	char* test_mem = malloc(100 * sizeof(char));
	free(test_mem);
	test_mem = malloc(10 * sizeof(char));
	free(test_mem);
	test_mem = malloc(23);
	free(test_mem);
    test_mem = malloc(124);
    test_mem = malloc(240);
	test_mem = malloc(496);
	free(test_mem);



    printf("\n\ndone");
    printf("\n=========================== THE END ===========================\n");
    exit(0);
    return 0;
error:
    printf("\nFAILURE\n");
    exit(1);
    return 0;
}


 // size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);