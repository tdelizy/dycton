#ifndef HELPER_H_OFFSETS
#define HELPER_H_OFFSETS

#define STOP_SIM 0x0

#define ALLOC_SIZE 0x4 // write at this address at the begining of malloc call
#define ALLOC_ADDR 0x8 // write at this address just before returning the allocated mem zone

#define FREE_ADDR 0xC // write at this address at the begining of free call
#define FREE_SIZE 0x10 // write at this address just before returning from free

#define TIMED_EVENT 0x14 //for logging events (logs the time of write and the number written)

#define ALLOC_FAIL_OFFSET 0x18 // way to log if malloc fail on a heap and need to fallback to another

#define ALLOCATOR_STRATEGY 0x20 // read only : return the strategy number to use
#define ALLOCATOR_ORACLE_REQ 0x24 // read only : ask the oracle in which heap place the current request

#endif