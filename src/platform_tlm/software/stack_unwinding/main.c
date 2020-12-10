
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libdycton.h"
#include "address_map.h"
#include "hal.h"

#define PRINT_ISS_REGS do {write_mem(HELPER_BASE+PRINT_REGS,0);} while(0)

void print_regs(void)
{
    unsigned int _sp = 0;
    // unsigned int _fp = 0;
    unsigned int _ra = 0;
    asm volatile ("sw $sp, %0" : "=m"(_sp));
    // asm volatile ("sw $s8, %0" : "=m"(_fp));
    asm volatile ("sw $ra, %0" : "=m" (_ra));
    PRINT_ISS_REGS;
    printf("\n- - - - - - - - - - - - - - - - - - - - - \n");
    printf("sp = 0x%x\n", _sp);
    // printf("fp = 0x%x\n", _fp);
    printf("ra = 0x%lx\n", _ra);
    printf("- - - - - - - - - - - - - - - - - - - - - \n");

    return;
}

void print_stack(void)
{
    // get stack pointer value
    unsigned int _sp = 0;
    asm volatile ("sw $sp, %0" : "=m"(_sp));

    // get a pointer to the stack top (declared in address_map.h)
    unsigned int * unwinding_ptr = (unsigned int *)STACK_TOP;

    // print the stack
    printf("\n_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \n");
    printf("addr\t\tval\n");
    while(unwinding_ptr >= (unsigned int*) _sp){
        printf("0x%x\t0x%x\n", (unsigned int)unwinding_ptr, *unwinding_ptr);
        unwinding_ptr--;
    }
    printf("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \n");

}


int f1(int a, int b)
{
    uint32_t ra = __builtin_return_address(0);
    printf("f1 - RA = 0x%lx\n", ra);
    int res = 0;
    int stackfiller[10];
    memset(&stackfiller, 0xDC, 10*sizeof(int));

    for(int i=2; i<(a+b); i++){
        res++;
        if(i<8)
            stackfiller[i]=0x22;
    }

    print_stack();
    print_regs();

    return res;
}

int f2(int a)
{
    uint32_t ra = __builtin_return_address(0);
    printf("f2 - RA = 0x%lx\n", ra);
    int res = 0;
    int stackfiller[10];
    memset(&stackfiller, 0xFE, 10*sizeof(int));

    for(int i=2; i<10; i++){
        res++;
        if(i<8)
            stackfiller[i]=0x33;
    }
    f1(stackfiller[2], 1);

    return res;
}


int f3(int a)
{
    uint32_t ra = __builtin_return_address(0);
    printf("f3 - RA = 0x%lx\n", ra);
    int stackfiller[10];
    memset(&stackfiller, 0xBA, 10*sizeof(int));
    stackfiller[2] = a;
    f2(stackfiller[2]);
    return 0;
}

int main(void)
{
    uint32_t ra = __builtin_return_address(0);
    printf("\n\n================== Stack Unwinding test for DYCTON ==================\n");
    printf("(TDk @ CITI Lab)\n");

    printf("main - RA = 0x%lx\n", ra);

    f3(6);

    printf("=========================== THE END =============================\n");
    exit(0);
    return 0;
}
