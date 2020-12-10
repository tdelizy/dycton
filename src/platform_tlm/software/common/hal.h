/********************************************************************
 * Copyright (C) 2009, 2012 by Verimag                              *
 * Initial author: Matthieu Moy                                     *
 *                                                                  *
 * Inclusion and adaptation : Tristan Delizy, 2019                  *
 ********************************************************************/

/*!
  \file hal.h
  \brief Harwdare Abstraction Layer : implementation for simulator.


*/
#ifndef HAL_H
#define HAL_H

#include <stdint.h>

typedef char *caddr_t;


// #define DYCTON_DBG
#ifdef DYCTON_DBG
// trace the syscall used by newlib
#define SYSCALL_PRINT
#endif

#ifndef EOF
  #define EOF (-1)
#endif
#ifndef NULL
  #define NULL (0)
#endif
#ifndef ERANGE
  #define ERANGE (12345678)
#endif

#define read_mem(a)     *((volatile uint32_t *)(a))
#define write_mem(a,d)  do { *((volatile uint32_t *)(a))=(uint32_t)(d); } while(0)
#define wait_for_irq()  do { irq_received = 0; while(irq_received == 0) {}} while (0)
#define cpu_relax()     (void)0

void _exit(int level);

void simple_print(char *s);
void simple_print_dec(uint32_t c);

int errno;


// debug print macros
#ifdef DYCTON_DBG
  #define print_addr(x) print_addrL(x)
  #define print_hex(x) print_hexL(x)
  #define print_dec(x) print_decL(x)
  #define print(x) printL(x)
#else
  #define print_addr(x) do {} while (0)
  #define print_hex(x) do {} while (0)
  #define print_hex(x) do {} while (0)
  #define print_dec(x) do {} while (0)
  #define print(x) do {} while (0)
  #define print_dec(x) do {} while (0)
#endif

void printL(char *addr);
void print_decL(uint32_t c);
void print_addrL(uint32_t *addr);
void print_hexL(long num);


#ifdef assert
  #undef assert
#endif
#define assert(x)                                                       \
do {                                                                    \
    if ( !(x) ) {                                                       \
        print("ASSERTION FAILED: (");print(#x);print(")\n");            \
        exit(42);                                                       \
    }                                                                   \
} while (0)

#endif /* HAL_H */
