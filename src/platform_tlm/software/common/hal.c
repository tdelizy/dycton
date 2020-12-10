
/********************************************************************
 * Copyright (C) 2009, 2012 by Verimag                              *
 * Initial author: Matthieu Moy                                     *
 *                                                                  *
 * Inclusion and adaptation : Tristan Delizy, 2019                  *
 ********************************************************************/

/*
 * Now contains the stub and implementation for newlib syscalls
 * in this project we implemented "write" and "sbrk", the others are stubbed
 *
 * ref : https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html#sec_syscalls
 */


#include <sys/stat.h>
#include <sys/times.h>
#include <errno.h>
#include <stdio.h>
#undef errno
int errno;

#include "hal.h"
#include "libdycton.h"
#include "address_map.h"

extern Heap_ctx * alloc_context;

void _exit(int level)
{
#ifdef HELPER_BASE
    (*((int*)HELPER_BASE) = level);
#else
# warning No simhelper, exit will do an infinite loop
#endif
    while (1);
}



//----------Printing functions----------

void simple_print(char *s) {
    for (; *s; s++) {
        write_mem(0x40600000 + 0xc, (uint32_t)(*s));
    }
}

void simple_print_dec(uint32_t c) {
    write_mem(0x40600000 + 0x10, c);
}


void printL(char *s) {
    for (; *s; s++) {
        write_mem(0x40600000 + 0xc, (uint32_t)(*s));
    }
}

void print_decL(uint32_t c) {
    write_mem(0x40600000 + 0x10, c);
}

void print_hexL(long num) {
    // unsigned long num = number to be printed
    printL("0x");
    uint32_t base = 16;
    uint32_t i = 8;
    char outbuf[9];//highest possible hex is 8 F:s for 32 bit integers + a null character

    outbuf[8] = 0;//Null character
    for (i = 0; i < 8; i++) {
        outbuf[i] = '0';
    }

    i = 7;
    do {
        outbuf[i] = "0123456789ABCDEF"[num % base];
        i--;
        num = num / base;
    } while ( num > 0);

    printL(outbuf);
}

void print_addrL(uint32_t *addr) {
    //Print an adress and its contents
    printL("\n"); print_hexL((long)addr); printL(":"); print_hexL(*addr);
    printL("            ");
    print_decL((uint32_t)(addr)); printL(":"); print_decL(*addr);
}


//----------Newlib reentrant allocation functions wrappers ----------
// only needed to redirect internals newlib calls directly done to these reentrent versions

void *_malloc_r(struct _reent *reent, size_t nbytes)
{
    return malloc(nbytes);
}

void *_realloc_r(struct _reent *reent, void *aptr, size_t nbytes)
{
    return realloc(aptr, nbytes);
}

void * _calloc_r(struct _reent *reent, size_t num, size_t size)
{
    return (void *)calloc(num, size);
}

void _free_r(struct _reent *reent, void *aptr)
{
    free(aptr);
}


//----------Newlib reentrant lock / unlock stubs ----------
// /!\ THIS IS ONLY VALID ON MONO-THREADED PLATFORM !
/* code from newlib :
void
__malloc_lock (ptr)
     struct _reent *ptr;
{
#ifndef __SINGLE_THREAD__
  __lock_acquire_recursive (__malloc_recursive_mutex);
#endif
}

void
__malloc_unlock (ptr)
     struct _reent *ptr;
{
#ifndef __SINGLE_THREAD__
  __lock_release_recursive (__malloc_recursive_mutex);
#endif
}
*/

void
__malloc_lock (ptr)
     struct _reent *ptr;
{
  ((void) 0);
}
void
__malloc_unlock (ptr)
     struct _reent *ptr;
{
  ((void) 0);
}



//----------Newlib syscall stubs----------

int close(int file) {
#ifdef SYSCALL_PRINT
    simple_print("\nclose\n");
#endif
    return -1;
}

int execve(char *name, char **argv, char **env) {
#ifdef SYSCALL_PRINT
    simple_print("\nexecve\n");
#endif
    errno = ENOMEM;
    return -1;
}

int fork(void) {
#ifdef SYSCALL_PRINT
    simple_print("\nfork\n");
#endif
    errno = EAGAIN;
    return -1;
}

int fstat(int file, struct stat *st) {
#ifdef SYSCALL_PRINT
    simple_print("\nfstat\n");
#endif
    st->st_mode = S_IFCHR;
    return 0;
}

int getpid(void) {
#ifdef SYSCALL_PRINT
    simple_print("\ngetpid\n");
#endif
    return 1;
}

int isatty(int file) {
#ifdef SYSCALL_PRINT
    simple_print("\nisatty\n");
#endif
    return 1;
}

int kill(int pid, int sig) {
#ifdef SYSCALL_PRINT
    simple_print("\nkill\n");
#endif
    errno = EINVAL;
    return -1;
}

int link(char *old, char *new) {
#ifdef SYSCALL_PRINT
    simple_print("\nlink\n");
#endif
    errno = EMLINK;
    return -1;
}

int lseek(int file, int ptr, int dir) {
#ifdef SYSCALL_PRINT
    simple_print("\nlseek\n");
#endif
    return 0;
}

int open(const char *name, int flags, int mode) {
#ifdef SYSCALL_PRINT
    simple_print("\nopen\n");
#endif
    return -1;
}

int read(int file, char *ptr, int len) {
#ifdef SYSCALL_PRINT
    simple_print("\nread\n");
#endif
    return 0;
}


caddr_t sbrk(int incr) {
#ifdef SYSCALL_PRINT
    simple_print("\nsbrk\n");
    if(incr >= 0){
        simple_print("> increment param : + "); simple_print_dec((uint32_t)incr); simple_print("\n");
    }else{
        simple_print("> increment param : - "); simple_print_dec((uint32_t)((-1)*incr)); simple_print("\n");
    }
#endif
    unsigned long prev_heap_end;

    // init
    if (alloc_context->heap_end == 0) {
        alloc_context->heap_end = alloc_context->heap_base;
    }

    prev_heap_end = alloc_context->heap_end;
#ifdef SYSCALL_PRINT
    simple_print("==> prev_heap_end ="); simple_print_dec(prev_heap_end); simple_print("\n");
#endif

    if(alloc_context->heap_end + incr > (alloc_context->heap_base + alloc_context->heap_max_size)) {
#ifdef SYSCALL_PRINT
        simple_print("sbrk return 0 : not enougth space to extend this heap\n");
#endif
        return 0;
    }
    alloc_context->heap_end += incr;
#ifdef SYSCALL_PRINT
    simple_print("==> new_heap_end ="); simple_print_dec(alloc_context->heap_end); simple_print("\n");
    simple_print("TOTAL SBRKed MEM ="); simple_print_dec(alloc_context->heap_end - alloc_context->heap_base); simple_print("\n");
#endif
    return (caddr_t) prev_heap_end;
}



int stat(const char *file, struct stat *st) {
      st->st_mode = S_IFCHR;
      return 0;
    }

clock_t times(struct tms *buf) {
#ifdef SYSCALL_PRINT
    simple_print("\ntimes\n");
#endif
    return -1;
}

int unlink(char *name) {
#ifdef SYSCALL_PRINT
    simple_print("\nunlink\n");
#endif
    errno = ENOENT;
    return -1;
}

int wait(int *status) {
#ifdef SYSCALL_PRINT
    simple_print("\nwait\n");
#endif
    errno = ECHILD;
    return -1;
}

int write(int file, char *ptr, int len) {
    int printed;
#ifdef SYSCALL_PRINT
    simple_print("\nwrite\n");
#endif
    for (printed = 0; printed < len; printed++) {
        write_mem(0x40600000 + 4, (uint32_t)(*ptr++));
    }
    return len;
}