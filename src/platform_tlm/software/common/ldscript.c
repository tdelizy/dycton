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

// this file is a .c in order to run c preprocessor on it to use defines from address_map and
// only have one definition of the stack top.

#define IN_LINKER
#include "address_map.h"
#undef IN_LINKER

// don't run preprocessor on the ALIGN() of ld.

#ifdef ALIGN
    #define _temp_ALIGN ALIGN
    #undef ALIGN
#endif

SECTIONS
{
   . = 0x80000000;
   .excep : {
        *(.excep)
        *(.excep.*)
   }
   . = 0xbfc00000;
   .reset : {
        *(.reset)
        *(.reset.*)
   }
   . = 0x00400000;
   .text : {
      *(.text.startup .text.startup.*)
      *(EXCLUDE_FILE (*libdycton.o *hal.o *newlib_malloc.o *exception.o *system.o) .text)
      system.o(.text)
      exception.o(.text)
      hal.o(.text)
      newlib_malloc.o(.text)
      libdycton.o(.text)
   }
   . = 0x10000000;
   .rodata : {
      *(.rodata)
      *(.rodata.*)
      . = ALIGN(4);
   }
   .data  : {
      *(.data)
   }
   .sdata : {
      *(.lit8)
      *(.lit4)
      *(.sdata)
   }
   _gp = .;
   . = ALIGN(4);
   _edata = .;
   .sbss  : {
      *(.sbss)
      *(.scommon)
   }
   .bss   : {
      *(.bss)
      *(COMMON)
   }
   . = ALIGN(4);
   _end = .;
   _stack = STACK_TOP-16;
}

#ifdef _temp_ALIGN
    #define ALIGN _temp_ALIGN
    #undef _temp_ALIGN
#endif