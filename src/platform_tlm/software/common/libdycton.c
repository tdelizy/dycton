/*
 *
 * SOCLIB_GPL_HEADER_BEGIN
 *
 * This file is part of SoCLib, GNU GPLv2.
 *
 * SoCLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * SOCLIB_GPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: nipo
 */

/* Interation and usage in the dycton project : 
 *
 * most of the initial code was replaced, now contains the implementation of the
 * dispatcher taking placement decisions at runtime.
 * It also contains variables declaration of the malloc from newlib (DLMalloc)
 * adapted for multi-heap.
 * this decision is taken inside the dispatch_and_allocate function
 *
 * Tristan Delizy, 2019
 */




#include <string.h>

#include "libdycton.h"
#if defined(jpeg)
#include "jpeg_alloc_site_profile.h"
#elif defined(json_parser)
#include "json_parser_alloc_site_profile.h"
#elif defined(dijkstra)
#include "dijkstra_alloc_site_profile.h"
#elif defined(jpg2000)
#include "jpg2000_alloc_site_profile.h"
#elif defined(h263)
#include "h263_alloc_site_profile.h"
#elif defined(ecdsa)
#include "ecdsa_alloc_site_profile.h"
#else
#warning "no profile header : profile strategy unusable, see libdycton.c"
#define GENERATED_SWITCH_PROFILE(a, b) do{exit(454545);}while(0)
#define ASSERT_PROFILE_CONSISTENCY(a) do{exit(454545);}while(0)
#endif
#include "hal.h"
#include "../../address_map.h"

#define ALLOC_REQ_START(x) do {write_mem(HELPER_BASE+ALLOC_SIZE,x);} while(0)
#define ALLOC_REQ_END(x) do {write_mem(HELPER_BASE+ALLOC_ADDR,x);} while(0)

#define FREE_REQ_START(x) do {write_mem(HELPER_BASE+FREE_ADDR,x);} while(0)
#define FREE_REQ_END(x) do {write_mem(HELPER_BASE+FREE_SIZE,x);} while(0)

#define ALLOC_GRANULARITY ((uint32_t)sizeof(uint32_t)) // size in octets

#define ALLOC_INTERNAL_FAIL(x) do {write_mem(HELPER_BASE+ALLOC_FAIL_OFFSET,x);} while(0)

#define ALLOC_RETURN_ADDR(x) do {write_mem(HELPER_BASE+RETURN_ADDR,x);} while(0) // must be used between alloc start and alloc end.

#define PROFILE_LIST_LENGTH read_mem(HELPER_BASE+PROFILE_LEN)

#define HEAP_X_BASE(x) read_mem(HELPER_BASE + HEAP_BASE_INFO + (x*4))

#define HEAP_X_SIZE(x) read_mem(HELPER_BASE + HEAP_SIZE_INFO + (x*4))

#define HEAP_COUNT read_mem(HELPER_BASE + HEAP_COUNT_INFO)

#define ARCH_INDEX read_mem(HELPER_BASE + HEAP_ARCHI)



//----------Newlib memory management types and defines ----------
#ifndef DEFAULT_SBRK_BASE_INIT
#define DEFAULT_SBRK_BASE_INIT (char*)(-1)
#endif
#ifndef DEFAULT_TRIM_THRESHOLD
#define DEFAULT_TRIM_THRESHOLD (128L * 1024L)
#endif
#ifndef DEFAULT_TOP_PAD
#define DEFAULT_TOP_PAD (0)
#endif
#ifndef DEAULT_MAX_SBRKED_MEM_INIT
#define DEAULT_MAX_SBRKED_MEM_INIT (0)
#endif
#ifndef DEFAULT_MAX_TOTAL_MEM_INIT
#define DEFAULT_MAX_TOTAL_MEM_INIT (0)
#endif
#ifndef DEFAULT_CURRENT_MALLINFO_INIT
#define DEFAULT_CURRENT_MALLINFO_INIT {0}
#endif

#define bin_at_dycton(x,i)      ((mbinptr)((char*)&(x.bins[2*(i) + 2]) - 2*SIZE_SZ))
#define IAV(x,i)  bin_at_dycton(x,i), bin_at_dycton(x,i)

#ifndef DEFAULT_BIN_ARRAY_INIT
#define DEFAULT_BIN_ARRAY_INIT(x) {                                                         \
    0, 0,                                                                                   \
    IAV(x,0),   IAV(x,1),   IAV(x,2),   IAV(x,3),   IAV(x,4),   IAV(x,5),   IAV(x,6),   IAV(x,7),           \
    IAV(x,8),   IAV(x,9),   IAV(x,10),  IAV(x,11),  IAV(x,12),  IAV(x,13),  IAV(x,14),  IAV(x,15),          \
    IAV(x,16),  IAV(x,17),  IAV(x,18),  IAV(x,19),  IAV(x,20),  IAV(x,21),  IAV(x,22),  IAV(x,23),          \
    IAV(x,24),  IAV(x,25),  IAV(x,26),  IAV(x,27),  IAV(x,28),  IAV(x,29),  IAV(x,30),  IAV(x,31),          \
    IAV(x,32),  IAV(x,33),  IAV(x,34),  IAV(x,35),  IAV(x,36),  IAV(x,37),  IAV(x,38),  IAV(x,39),          \
    IAV(x,40),  IAV(x,41),  IAV(x,42),  IAV(x,43),  IAV(x,44),  IAV(x,45),  IAV(x,46),  IAV(x,47),          \
    IAV(x,48),  IAV(x,49),  IAV(x,50),  IAV(x,51),  IAV(x,52),  IAV(x,53),  IAV(x,54),  IAV(x,55),          \
    IAV(x,56),  IAV(x,57),  IAV(x,58),  IAV(x,59),  IAV(x,60),  IAV(x,61),  IAV(x,62),  IAV(x,63),          \
    IAV(x,64),  IAV(x,65),  IAV(x,66),  IAV(x,67),  IAV(x,68),  IAV(x,69),  IAV(x,70),  IAV(x,71),          \
    IAV(x,72),  IAV(x,73),  IAV(x,74),  IAV(x,75),  IAV(x,76),  IAV(x,77),  IAV(x,78),  IAV(x,79),          \
    IAV(x,80),  IAV(x,81),  IAV(x,82),  IAV(x,83),  IAV(x,84),  IAV(x,85),  IAV(x,86),  IAV(x,87),          \
    IAV(x,88),  IAV(x,89),  IAV(x,90),  IAV(x,91),  IAV(x,92),  IAV(x,93),  IAV(x,94),  IAV(x,95),          \
    IAV(x,96),  IAV(x,97),  IAV(x,98),  IAV(x,99),  IAV(x,100), IAV(x,101), IAV(x,102), IAV(x,103),         \
    IAV(x,104), IAV(x,105), IAV(x,106), IAV(x,107), IAV(x,108), IAV(x,109), IAV(x,110), IAV(x,111),         \
    IAV(x,112), IAV(x,113), IAV(x,114), IAV(x,115), IAV(x,116), IAV(x,117), IAV(x,118), IAV(x,119),         \
    IAV(x,120), IAV(x,121), IAV(x,122), IAV(x,123), IAV(x,124), IAV(x,125), IAV(x,126), IAV(x,127)}
#endif

#define SIZE_SZ                (sizeof(INTERNAL_SIZE_T))
#ifndef MALLOC_ALIGNMENT
#define MALLOC_ALIGN           8
#define MALLOC_ALIGNMENT       (SIZE_SZ < 4 ? 8 : (SIZE_SZ + SIZE_SZ))
#else
#define MALLOC_ALIGN           MALLOC_ALIGNMENT
#endif
#define MALLOC_ALIGN_MASK      (MALLOC_ALIGNMENT - 1)
#define MINSIZE                (sizeof(struct malloc_chunk))

/* pad request bytes into a usable size */

#define request2size(req) \
 (((unsigned long)((req) + (SIZE_SZ + MALLOC_ALIGN_MASK)) < \
  (unsigned long)(MINSIZE + MALLOC_ALIGN_MASK)) ? ((MINSIZE + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK)) : \
   (((req) + (SIZE_SZ + MALLOC_ALIGN_MASK)) & ~(MALLOC_ALIGN_MASK)))



#define HEAP_CTX_STATIC_INIT(x) {                                                           \
    DEFAULT_BIN_ARRAY_INIT(multi_heap_ctx[x]),                                              \
    DEFAULT_TRIM_THRESHOLD,                                                                 \
    DEFAULT_TOP_PAD,                                                                        \
    DEFAULT_SBRK_BASE_INIT,                                                                 \
    DEAULT_MAX_SBRKED_MEM_INIT,                                                             \
    DEFAULT_MAX_TOTAL_MEM_INIT,                                                             \
    DEFAULT_CURRENT_MALLINFO_INIT,                                                          \
    0,                                                                         \
    0,                                                                         \
    0,                                                                                      \
    0,                                                                         \
    0,                                                                                      \
    x                                                                                       \
}
 
 


//---------- multi-heap and dispatcher variables declaration ----------

Heap_ctx multi_heap_ctx[2] = {HEAP_CTX_STATIC_INIT(0), HEAP_CTX_STATIC_INIT(1)};
Heap_ctx * alloc_context = &(multi_heap_ctx[0]);

uint32_t fast_bytes_occupied = 0;
uint32_t last_fast_bytes_occupied = 0;
uint32_t max_fast_bytes_occupied = 0;
uint32_t low_thr = 33; // percentage as an int
uint32_t high_thr = 66; // percentage as an int
uint32_t discarded_obj_count = 0;
uint32_t p_occ = 0;

uint32_t delta_5percent = 0;


// stub to call at start
// in real life these init would be static dependent on HW architecture
// but in simulation, to facilitate parameters exploration,
// those information are not known at compile time
void init_multi_heap(void)
{
#ifdef DYCTON_DBG
    print("init_multi_heap\n");
#endif
    multi_heap_ctx[0].heap_base = HEAP_X_BASE(0);
    multi_heap_ctx[0].heap_max_size = HEAP_X_SIZE(0);
    multi_heap_ctx[0].bfbs = HEAP_X_SIZE(0);

    if (HEAP_COUNT >= 2){
        multi_heap_ctx[1].heap_base = HEAP_X_BASE(1);
        multi_heap_ctx[1].heap_max_size = HEAP_X_SIZE(1);
        multi_heap_ctx[1].bfbs = HEAP_X_SIZE(1);
    }
    max_fast_bytes_occupied = HEAP_X_SIZE(0);
    delta_5percent = HEAP_X_SIZE(0)/20;
}

// implementations of the different placement strategies 
void * dispatch_and_allocate(size_t sz, uint32_t caller_address)
{
    void *returnPointer = NULL;
    uint32_t oracle_rsp = 0xFFFF;


    // workaround : init here
    if(!(multi_heap_ctx[0].heap_base)){
        init_multi_heap();
    }


    // dispatch following current strategy
    switch (*(uint32_t*)(HELPER_BASE + ALLOCATOR_STRATEGY)) {
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    case DEFAULT_STRATEGY:
        // greedy : try to allocate in first / fastest heap (/!\ speed ordering assumption)
        alloc_context = &(multi_heap_ctx[0]);
        returnPointer = newlib_malloc_r(_impure_ptr, sz);

        // fallback to other heap in case of failure (implem for 2 heaps)
        if (!returnPointer) {
            ALLOC_INTERNAL_FAIL(0);
            alloc_context = &(multi_heap_ctx[1]);
            returnPointer = newlib_malloc_r(_impure_ptr, sz);
        }
        break;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    case ORACLE_STRATEGY:
        oracle_rsp = *(uint32_t*)(HELPER_BASE + ALLOCATOR_ORACLE_REQ);
        if (oracle_rsp >= HEAP_COUNT) {
            #ifdef DYCTON_DBG
            print("heap count = ");print_dec(HEAP_COUNT); print("\n");
            print("oracle strategy inconsistent with current architecture.\n");
            print("aborting...\n");
            #endif
            exit(30);
        }
        alloc_context = &(multi_heap_ctx[oracle_rsp]);
        returnPointer = newlib_malloc_r(_impure_ptr, sz);
        // fallback to other heap in case of failure (implem for 2 heaps)
        if (!returnPointer) {
            #ifdef DYCTON_DBG
            print("not enougth space to follow oracle strategy, fallback on slow memory.");
            #endif
            ALLOC_INTERNAL_FAIL(0);
            alloc_context = &(multi_heap_ctx[(1-oracle_rsp)]);
            returnPointer = newlib_malloc_r(_impure_ptr, sz);
            if (!returnPointer){
                exit(31);
            }
        }
        break;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    case PROFILE_STRATEGY:
        ASSERT_PROFILE_CONSISTENCY(caller_address); // deactivated by default, see {app}_profile.h
        GENERATED_SWITCH_PROFILE(caller_address,PROFILE_LIST_LENGTH);
        break; 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    case PROFILE_ENHANCED:
        ASSERT_PROFILE_CONSISTENCY(caller_address); // deactivated by default, see {app}_profile.h
        switch (ARCH_INDEX) {
            case 5:
                GEN_SW_PROF_ENHANCED_A5(caller_address,PROFILE_LIST_LENGTH);
                break; 
            case 4:
                GEN_SW_PROF_ENHANCED_A4(caller_address,PROFILE_LIST_LENGTH);
                break; 
            case 3:
                GEN_SW_PROF_ENHANCED_A3(caller_address,PROFILE_LIST_LENGTH);
                break; 
            case 2:
                GEN_SW_PROF_ENHANCED_A2(caller_address,PROFILE_LIST_LENGTH);
                break; 
            case 1:
                GEN_SW_PROF_ENHANCED_A1(caller_address,PROFILE_LIST_LENGTH);
                break; 
            default:
#ifdef DYCTON_DBG
                print("PROFILE_STRATEGY_ENHANCED : architecture not supported.\n");
                print("aborting...\n");
#endif
                exit(34);
            }
        break; 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    case PROFILE_ILP:
        ASSERT_PROFILE_CONSISTENCY(caller_address); // deactivated by default, see {app}_profile.h
        switch (ARCH_INDEX) {
            case 5:
                GEN_SW_PROF_ILP_A5(caller_address);
                break; 
            case 4:
                GEN_SW_PROF_ILP_A4(caller_address);
                break; 
            case 3:
                GEN_SW_PROF_ILP_A3(caller_address);
                break; 
            case 2:
                GEN_SW_PROF_ILP_A2(caller_address);
                break; 
            case 1:
                GEN_SW_PROF_ILP_A1(caller_address);
                break; 
            default:
#ifdef DYCTON_DBG
                print("PROFILE_STRATEGY_ILP : architecture not supported.\n");
                print("aborting...\n");
#endif
                exit(34);
            }
        break; 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    default:
#ifdef DYCTON_DBG
        print("unknown strategy number.\n");
        print("aborting...\n");
#endif
        exit(33);
    }

    return returnPointer;
}


//---------- classical memory managment primitives handling multi heap ----------
// the lalloc interface dispatch requests between multiple heaps
void *malloc(size_t sz)
{
    void *returnPointer = NULL;
    uint32_t caller_address = (uint32_t)__builtin_return_address(0);

#ifdef DYCTON_DBG
    print("dycton malloc("); print_dec((int)sz); print(") padded_sz = "); print_dec((int)request2size(sz)); print("\n");
#endif
    ALLOC_REQ_START(sz);
    ALLOC_RETURN_ADDR(caller_address);

    returnPointer = dispatch_and_allocate(sz, caller_address);

    ALLOC_REQ_END(returnPointer);
    return returnPointer;
}



// unneficient realloc implementation
// copy back the full size, should not break anything (as the added memory should not be initialized ...)
// CAUTION: This would be a major security issue in a real system ! 
void *realloc(void* ptr, size_t sz)
{
    void *returnPointer = NULL;
    uint32_t caller_address = (uint32_t)__builtin_return_address(0);

#ifdef DYCTON_DBG
    print("dycton realloc("); print_dec((int)ptr); print(", "); print_dec((int)sz); print(")\n");
#endif
    ALLOC_REQ_START(sz);
    ALLOC_RETURN_ADDR(caller_address);

    returnPointer = dispatch_and_allocate(sz, caller_address);
    
    if (!ptr) {
        ALLOC_REQ_END(returnPointer);
        return returnPointer;
    } else {
        memcpy(returnPointer, ptr, sz);
        ALLOC_REQ_END(returnPointer);
        free(ptr); // do not overlap in time different operation logs of memory allocator
        return returnPointer;
    }
}




void* calloc(size_t num, size_t size)
{
    void *returnPointer = NULL;
    uint32_t caller_address = (uint32_t)__builtin_return_address(0);

#ifdef DYCTON_DBG
    print("dycton calloc("); print_dec((int)num); print(", "); print_dec((int)size); print(")\n");
#endif

    volatile const uint32_t sz = size * num;

    ALLOC_REQ_START(sz);
    ALLOC_RETURN_ADDR(caller_address);

    returnPointer = dispatch_and_allocate(sz, caller_address);

    memset(returnPointer, 0, sz);
    ALLOC_REQ_END(returnPointer);
    return returnPointer;

}

void free(void* ptr)
{
    uint32_t free_sz = 0;
#ifdef DYCTON_DBG
    print("dycton free("); print_dec((int)ptr); print(")");
#endif
    FREE_REQ_START(ptr);

    for (int i = 0; i < (HEAP_COUNT); i++)
    {   if ((int)ptr >= multi_heap_ctx[i].heap_base && (int)ptr <= multi_heap_ctx[i].heap_end ) {
            alloc_context = &(multi_heap_ctx[i]);
            free_sz = newlib_free_r(_impure_ptr, (void*)ptr);
            if(i == 0){
                // print(" sz = "); print_dec((int)free_sz); print("\n");
                fast_bytes_occupied -= free_sz;
            }
            FREE_REQ_END(0);
            return;
        }
    }

    // free can be called (and in fact is called) with invalid values, we should not care
    FREE_REQ_END(1);
}



//---------- stubs and utils ----------
/* Errno support */
int errno;

int * __errno ()
{
    return &errno;
}

/* From newlib for reentrancy support
 */
/*
 * All references to struct _reent are via this pointer.
 * Internally, newlib routines that need to reference it should use _REENT.
 */
extern struct _reent *_impure_ptr __attribute__((__section__(".sdata")));


void exit(int level)
{
    _exit(level);
    while (1); // stop asking questions and let gcc be happy.
}

void trap()
{
#if defined(__mips__)
# if __mips >= 32
    asm volatile("teq $0, $0");
# else
    asm volatile("break");
# endif
#elif defined(PPC)
    asm volatile("trap");
#elif defined(__sparc__)
    asm volatile("ta 0");
#elif defined(__lm32__)
    asm volatile("break");
#elif defined(__arm__)
    asm volatile("swi 1");
#else
# warning No trap
#endif
}

