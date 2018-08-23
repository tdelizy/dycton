#ifndef ADDRESS_MAP_H
#define ADDRESS_MAP_H

/* strategies for allocation */
#define DEFAULT_STRATEGY 0
#define ORACLE_STRATEGY 1


/* memory technologies under test */
// #define MEM_FAST_RLAT (1)
// #define MEM_FAST_WLAT (1)

// #define MEM_SLOW_RLAT (10)
// #define MEM_SLOW_WLAT (10)

#define MEM_FAST_RLAT (1)
#define MEM_FAST_WLAT (3)

#define MEM_SLOW_RLAT (2)
#define MEM_SLOW_WLAT (30)


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


/* HEAP MEMORY ARCHITECTURE EXPERIMENTS */
/* The memories are ordered from the fastest to the slowest */
/* all latencies in cycles */
/* defined in function of memory footprint for experiment significance */

#if defined(jpeg)
#define APP_MEM_FOOTPRINT (235520)
#elif defined(h263)
#define APP_MEM_FOOTPRINT (1262320)
#elif defined(dijkstra)
#define APP_MEM_FOOTPRINT (10240)
#elif defined(patricia)
#define APP_MEM_FOOTPRINT (0)
#elif defined(json_parser) // 14 40 170
#define APP_MEM_FOOTPRINT (40960)
#elif defined(malloc_test)
#define APP_MEM_FOOTPRINT (131072)
#elif defined(jpg2000)
#define APP_MEM_FOOTPRINT (1705920)
#else
#define APP_MEM_FOOTPRINT (0x02000000) // wip (32Mo)
#endif

#define heap_front_align(x) (((x+8)/16)*16) //align heaps base on 16 Bytes

#if (DY_ARCH == 0) /*----------------------------------------------------- FAST = 100% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT // 100 % of memory footprint
#define HEAP_0_RLAT             MEM_FAST_RLAT
#define HEAP_0_WLAT             MEM_FAST_WLAT
#define HEAP_COUNT              (1)
#define HEAP_SIZE               (HEAP_0_SIZE)
#elif (DY_ARCH == 1) /*---------------------------------------------------- FAST = 75% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT*3/4 // 75 % of memory footprint
#define HEAP_0_RLAT             MEM_FAST_RLAT
#define HEAP_0_WLAT             MEM_FAST_WLAT
#define HEAP_1_BASE             heap_front_align(HEAP_0_BASE + HEAP_0_SIZE)
#define HEAP_1_SIZE             APP_MEM_FOOTPRINT // relax size of slow heap to ensure execution success
#define HEAP_1_RLAT             MEM_SLOW_RLAT
#define HEAP_1_WLAT             MEM_SLOW_WLAT
#define HEAP_COUNT              (2)
#define HEAP_SIZE               (HEAP_1_BASE + HEAP_1_SIZE - HEAP_BASE)
#elif (DY_ARCH == 2) /*---------------------------------------------------- FAST = 50% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT/2 // 50 % of memory footprint
#define HEAP_0_RLAT             MEM_FAST_RLAT
#define HEAP_0_WLAT             MEM_FAST_WLAT
#define HEAP_1_BASE             heap_front_align(HEAP_0_BASE + HEAP_0_SIZE)
#define HEAP_1_SIZE             APP_MEM_FOOTPRINT // relax size of slow heap to ensure execution success
#define HEAP_1_RLAT             MEM_SLOW_RLAT
#define HEAP_1_WLAT             MEM_SLOW_WLAT
#define HEAP_COUNT              (2)
#define HEAP_SIZE               (HEAP_1_BASE + HEAP_1_SIZE - HEAP_BASE)
#elif (DY_ARCH == 3) /*---------------------------------------------------- FAST = 25% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT/4 // 25 % of memory footprint
#define HEAP_0_RLAT             MEM_FAST_RLAT
#define HEAP_0_WLAT             MEM_FAST_WLAT
#define HEAP_1_BASE             heap_front_align(HEAP_0_BASE + HEAP_0_SIZE)
#define HEAP_1_SIZE             APP_MEM_FOOTPRINT // relax size of slow heap to ensure execution success
#define HEAP_1_RLAT             MEM_SLOW_RLAT
#define HEAP_1_WLAT             MEM_SLOW_WLAT
#define HEAP_COUNT              (2)
#define HEAP_SIZE               (HEAP_1_BASE + HEAP_1_SIZE - HEAP_BASE)
#elif (DY_ARCH == 4) /*---------------------------------------------------- FAST = 10% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT/10 // 10 % of memory footprint
#define HEAP_0_RLAT             MEM_FAST_RLAT
#define HEAP_0_WLAT             MEM_FAST_WLAT
#define HEAP_1_BASE             heap_front_align(HEAP_0_BASE + HEAP_0_SIZE)
#define HEAP_1_SIZE             APP_MEM_FOOTPRINT // relax size of slow heap to ensure execution success
#define HEAP_1_RLAT             MEM_SLOW_RLAT
#define HEAP_1_WLAT             MEM_SLOW_WLAT
#define HEAP_COUNT              (2)
#define HEAP_SIZE               (HEAP_1_BASE + HEAP_1_SIZE - HEAP_BASE)
#elif (DY_ARCH == 5) /*----------------------------------------------------- FAST = 5% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT/20 // 5 % of memory footprint
#define HEAP_0_RLAT             MEM_FAST_RLAT
#define HEAP_0_WLAT             MEM_FAST_WLAT
#define HEAP_1_BASE             heap_front_align(HEAP_0_BASE + HEAP_0_SIZE)
#define HEAP_1_SIZE             APP_MEM_FOOTPRINT // relax size of slow heap to ensure execution success
#define HEAP_1_RLAT             MEM_SLOW_RLAT
#define HEAP_1_WLAT             MEM_SLOW_WLAT
#define HEAP_COUNT              (2)
#define HEAP_SIZE               (HEAP_1_BASE + HEAP_1_SIZE - HEAP_BASE)
#elif (DY_ARCH == 6) /*----------------------------------------------------- FAST = 0% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT // 100 % of memory footprint slow (0% fast)
#define HEAP_0_RLAT             MEM_SLOW_RLAT
#define HEAP_0_WLAT             MEM_SLOW_WLAT
#define HEAP_COUNT              (1)
#define HEAP_SIZE               (HEAP_0_SIZE)
#elif (DY_ARCH == -2) /*----------------------------------------------------- FAST = 100%,  SLOW = 100% */
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             APP_MEM_FOOTPRINT // relax size of high heap
#define HEAP_0_RLAT             MEM_FAST_RLAT
#define HEAP_0_WLAT             MEM_FAST_WLAT
#define HEAP_1_BASE             heap_front_align(HEAP_0_BASE + HEAP_0_SIZE)
#define HEAP_1_SIZE             APP_MEM_FOOTPRINT // relax size of slow heap
#define HEAP_1_RLAT             MEM_SLOW_RLAT
#define HEAP_1_WLAT             MEM_SLOW_WLAT
#define HEAP_COUNT              (2)
#define HEAP_SIZE               (HEAP_1_BASE + HEAP_1_SIZE - HEAP_BASE)
#else /*--------------------------------------------------------------------*/
#define HEAP_BASE               heap_front_align(STACK_TOP)
#define HEAP_0_BASE             (HEAP_BASE)
#define HEAP_0_SIZE             (0x02000000) // 32Mo
#define HEAP_0_RLAT             (1)
#define HEAP_0_WLAT             (1)
#define HEAP_COUNT              (1)
#define HEAP_SIZE               (HEAP_0_SIZE)
#endif





// SPM access addresses from the bus
#define SPM_BASE                (0x00000000)
#define SPM_I_BASE              (SPM_BASE)
#define SPM_I_SIZE              (TEXT_SIZE)
#define SPM_D_BASE              (SPM_I_BASE + SPM_I_SIZE)
#define SPM_D_SIZE              (DATA_SIZE)
#define SPM_S_BASE              (SPM_D_BASE + DATA_SIZE)
#define SPM_S_SIZE              (STACK_SIZE)
#define SPM_GP_BASE             (HEAP_BASE)
#define SPM_GP_SIZE             (HEAP_SIZE)
#define SPM_SIZE                (SPM_I_SIZE + SPM_D_SIZE + SPM_GP_SIZE)

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

#endif


