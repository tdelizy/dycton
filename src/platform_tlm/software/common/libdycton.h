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
#ifndef LIBDYCTON_H
#define LIBDYCTON_H

#include <stdlib.h>
#include <stdint.h>
#include "address_map.h"
#include "newlib_mem_manager.h"
#include "hal.h"

#ifndef EXIT_FAILURE        /* define exit() codes if not provided */
#define EXIT_FAILURE  1
#endif
#ifndef EXIT_WARNING
#define EXIT_WARNING  2
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS  0
#endif

#ifndef INTERNAL_SIZE_T
#define INTERNAL_SIZE_T size_t
#endif
#ifndef SIZE_SZ
#define SIZE_SZ                (sizeof(INTERNAL_SIZE_T))
#endif

#define MALLOC_EARLY_FAIL_IMPLEM // for handling multi-heap properly

#define NAV             128   /* number of bins */

//---------- types declaration from newlib allocator ----------
struct mallinfo {
    int arena;    /* total space allocated from system */
    int ordblks;  /* number of non-inuse chunks */
    int smblks;   /* unused -- always zero */
    int hblks;    /* number of mmapped regions */
    int hblkhd;   /* total space in mmapped regions */
    int usmblks;  /* unused -- always zero */
    int fsmblks;  /* unused -- always zero */
    int uordblks; /* total allocated space */
    int fordblks; /* total non-inuse space */
    int keepcost; /* top-most, releasable (via malloc_trim) space */
};

struct malloc_chunk
{
  INTERNAL_SIZE_T prev_size; /* Size of previous chunk (if free). */
  INTERNAL_SIZE_T size;      /* Size in bytes, including overhead. */
  struct malloc_chunk* fd;   /* double links -- used only if free. */
  struct malloc_chunk* bk;
};

typedef struct malloc_chunk* mchunkptr;

//----------Newlib memory management variables ----------
typedef struct malloc_chunk* mbinptr;


//----------heap context structure declaration ----------
typedef struct heap_context {
    mbinptr bins[NAV * 2 + 2];
    unsigned long trim_threshold;
    unsigned long top_pad;
    char* sbrk_base;
    unsigned long max_sbrked_mem;
    unsigned long max_total_mem;
    struct mallinfo current_mallinfo;
    unsigned long heap_base;
    unsigned long heap_max_size;
    unsigned long heap_end;
    unsigned long bfbs;
    unsigned int bfb_idx;
    unsigned int heap_id;
} Heap_ctx;



/* Errno support */
int * __errno ();


// typedef unsigned int size_t;
// typedef int ssize_t;
void init_multi_heap(void);
void * malloc(size_t sz);
void * realloc(void* ptr, size_t sz);
void * calloc(size_t num, size_t size);
void free(void*);

void exit(int);
void trap();

uint32_t get_heap_id(void);

#endif // LIBDYCTON_H
