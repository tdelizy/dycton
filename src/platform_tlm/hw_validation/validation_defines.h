/*
 * Test platform address map and defines
 */

#define CUT_SIZE    (0x1000)
#define R_LAT       (1) // in platform cycles
#define W_LAT       (100) // in platform cycles

#define SPM_ADDRESS (0)
#define CUT_0_ADDR  (SPM_ADDRESS)
#define CUT_1_ADDR  (CUT_0_ADDR + CUT_SIZE)
#define CUT_2_ADDR  (CUT_1_ADDR + CUT_SIZE)

#define MAGIC_NB    (0xDEADBABA)
