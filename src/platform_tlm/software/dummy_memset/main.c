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
#include <string.h>
#include "libdycton.h"
#include "address_map.h"
#include "hal.h"



#define TST_VAL 	(0XDEADBABA)




int main(void)
{
    volatile unsigned char * addr = 0;
    printf("\n\n================== Dummy memset test for DYCTON ==================\n");
    printf("(TDk @ CITI Lab)\n");

    addr = (volatile unsigned char *)malloc(200);
    printf("malloc return value (allocated address): 0x%X\n", (int)addr);

    printf("writing magic number in target memory\n");
    memset((void *)TST_BASE, TST_VAL, TST_SIZE);
    printf("...ok.\n");


    printf("verifying written value\n") ;
    for (addr = (unsigned char *)TST_BASE; addr < (unsigned char *)(TST_BASE + TST_SIZE) ; addr++) {
        if (*addr != (unsigned char)TST_VAL)
            goto kp;
    }
    printf("...ok.\n");

    uint32_t * addr = (uint32_t *)PROFILE_ADDR;
    printf("profile addr value = 0X%x\n", (int)PROFILE_ADDR);
    printf("content:\n");
    while ((uint32_t)addr < DENSE_MEM_BASE+DENSE_MEM_SIZE){
        printf("0x%x\n", *addr);
        addr++;

    }


    printf("=========================== THE END =============================\n");
    exit(0);
    return 0;

kp:
    printf("\nmemory error during test !");
    exit(1);
    return 0;

}

