
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


    printf("=========================== THE END =============================\n");
    exit(0);
    return 0;

kp:
    printf("\nmemory error during test !");
    exit(1);
    return 0;

}

