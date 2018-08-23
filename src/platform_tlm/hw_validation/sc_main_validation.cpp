#include <string>
#include <fstream>
#include <sys/time.h>

#include "generator.h"
#include "scratch_pad_memory.h"
#include "nb_bus.h"

#include "../platform_time.h"
#include "validation_defines.h"
#include "../log_trace.h"


/*
 * execution run all the design test and displays result, if something goes wrong it should be printed on std::err
 * normal outputs for test progression or success notification are done on std::out
 *
 *
 */

// definition of debug global variables
__DY_DBG_DEF


int sc_main(int, char **) {

    std::cout <<"[DYCTON] SPM hardware validation platform - TLM" << std::endl;
    std::cout <<"[DYCTON] platform components instantiation ==================================" << std::endl;


    dy_init_logtrace();
    // dy_enable_dbg(MEM_ID, ARBITRATED_SUB_ID); // spm banks
    // dy_enable_dbg(MEM_ID, SPM_SUB_ID); // spm


    std::fstream log_file("hw_validation_mem_access.log", std::fstream::out);

    /* Device Under Test (DUT) :
     *
     * the SPM component including 3 ports and 3 cuts.
     *
     * this test aims to ensure that :
     *  - all ports are able to read and write simultaneously in different cuts (TEST_00)
     *  - all ports can access all cuts (TEST_01)
     *  - fixed priority is correctly implemented (TEST_02)
     */

    unsigned int spm_sizes[] = {CUT_SIZE, CUT_SIZE, CUT_SIZE};
    unsigned int spm_r_lat[] = {R_LAT, R_LAT, R_LAT};
    unsigned int spm_w_lat[] = {W_LAT, W_LAT, W_LAT};
    unsigned int spm_ports = 3;
    unsigned int spm_banks = 3;
    unsigned int spm_prio[] = {2, 1, 0}; // the lowest value is highest priority

    ScatchPadMemory spm("spm_dycton", spm_ports, spm_banks, spm_sizes, spm_r_lat, spm_w_lat, spm_prio, log_file);


    /* Generator :
     *
     * this tlm components generates TLM memory transactions to test the DUT
     *
     * it has a function set_test(int) to choose test case
     *
     */

    Generator gen("test_generator", &spm);

    std::cout <<"[DYCTON] binding ============================================================" << std::endl;

    gen.socket_0.bind(spm.sockets[0]);
    gen.socket_1.bind(spm.sockets[1]);
    gen.socket_2.bind(spm.sockets[2]);

    spm.set_bank_base_address(0, CUT_0_ADDR);
    spm.set_bank_base_address(1, CUT_1_ADDR);
    spm.set_bank_base_address(2, CUT_2_ADDR);


    std::cout <<"[DYCTON] test start =========================================================" << std::endl;

    // start the simulation
    sc_core::sc_start();

    std::cout <<"[DYCTON] simulation end =====================================================" << std::endl;

    dy_deinit_logtrace();
	return 0;
}