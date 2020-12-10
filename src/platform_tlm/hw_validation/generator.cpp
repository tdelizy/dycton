/*

This file is part of the Dycton simulator.
This software aims to provide an environment for Dynamic Heterogeneous Memory 
Allocation for embedded devices study. It is build using SystemC / TLM.
It uses the MIPS32 ISS from the SocLib project (www.soclib.fr). 
It also use one SimSoc module (https://gforge.inria.fr/projects/simsoc/)
(originals athors credited in respective files)

Copyright (C) 2019  Tristan Delizy, CITI Lab, INSA de Lyon

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <systemc.h>

#include "validation_defines.h"
#include "../platform_time.h"
#include "generator.h"

#include "../log_trace.h"
#include "../platform_time.h"

using namespace sc_core;

const sc_core::sc_time PLATFORM_CYCLE = sc_core::sc_time (2500, sc_core::SC_PS);
const uint64_t PROC_FREQ = ((uint64_t))(1.0/(PLATFORM_CYCLE.to_seconds()));

Generator::Generator(sc_module_name name, ScatchPadMemory * dut)
                    : sc_module(name),
                    m_dut(dut),
                    m_peq("gen_peq")
{
	SC_THREAD(run_test_cases);
    m_test_case = -1;

    socket_0.register_nb_transport_bw(this, &Generator::nb_transport_bw_0);
    socket_1.register_nb_transport_bw(this, &Generator::nb_transport_bw_1);
    socket_2.register_nb_transport_bw(this, &Generator::nb_transport_bw_2);
}


void Generator::run_test_cases()
{
    std::cout <<"[DYCTON] running test case 00 ===============================================" << std::endl;
    test_case_00();
    std::cout <<"[DYCTON] done ===============================================================" << std::endl;
    reset_mem_state();
    std::cout <<"[DYCTON] running test case 01 ===============================================" << std::endl;
    test_case_01();
    std::cout <<"[DYCTON] done ===============================================================" << std::endl;
    reset_mem_state();
    std::cout <<"[DYCTON] running test case 02 ===============================================" << std::endl;
    test_case_02();
    std::cout <<"[DYCTON] done ===============================================================" << std::endl;
    reset_mem_state();
    std::cout <<"[DYCTON] running test case 03 ===============================================" << std::endl;
    test_case_03();
    std::cout <<"[DYCTON] done ===============================================================" << std::endl;

    sc_core::sc_stop();
}


/*
 * this test aims to ensure that all ports are able to read and write simultaneously in different cuts (TEST_00)
 */
void Generator::test_case_00(void)
{
    std::cout <<"[TEST_CASE_00] starting ..." << std::endl;
    std::cout <<"[TEST_CASE_00] this test aims to ensure that all ports are able to read and write simultaneously in different cuts." << std::endl;

    uint64_t test_start_time = ((uint64_t))(PROC_FREQ*sc_core::sc_time_stamp().to_seconds());
    uint64_t expected_rsp_times[3] = {0};
    uint32_t rsp_count = 0;

    tlm::tlm_generic_payload* pl;

    std::cout <<  "[TEST_CASE_00] testing simultaneous access on 3 cuts with READ operations." << std::endl;

    // compute the responses times if all ports works simultaneously:
    expected_rsp_times[0] = (uint64_t)(test_start_time + R_LAT);
    expected_rsp_times[1] = (uint64_t)(test_start_time + R_LAT);
    expected_rsp_times[2] = (uint64_t)(test_start_time + R_LAT);

    // send simultaneously 3 messages on 3 banks and check that they returns at correct times (read)
    gen_memory_transaction(0, CUT_0_ADDR, MAGIC_NB, 0);
    gen_memory_transaction(0, CUT_1_ADDR, MAGIC_NB+1, 1);
    gen_memory_transaction(0, CUT_2_ADDR, MAGIC_NB+2, 2);

    //wait the end of the 3 transactions
    while(rsp_count < 3){
        std::cout <<  "[TEST_CASE_00] waiting for responses ... "<< std::endl;
        wait(m_peq.get_event());
        pl = m_peq.get_next_transaction();
        while (pl!=NULL){
            rsp_count++;
            std::cout <<  "[TEST_CASE_00] recieved response." << std::endl;
            delete pl->get_data_ptr();
            delete pl;
            pl = m_peq.get_next_transaction();
        }
    }

    for(int i=0; i<3; i++){
        if(expected_rsp_times[i] != m_rsp_time[i]){
            std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
            << "] inconsistent response time from port "<< i << " on READ operation." << std::endl;
            abort();
        }
    }
    std::cout <<  "[TEST_CASE_00] all READ transaction returned at appropriate times." << std::endl;

    std::cout <<  "[TEST_CASE_00] testing simultaneous access on 3 cuts with WRITE operations." << std::endl;

    test_start_time = (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds());
    rsp_count = 0;

    // compute the responses times if all ports works simultaneously:
    expected_rsp_times[0] = (uint64_t)(test_start_time + W_LAT);
    expected_rsp_times[1] = (uint64_t)(test_start_time + W_LAT);
    expected_rsp_times[2] = (uint64_t)(test_start_time + W_LAT);

    // send simultaneously 3 messages on 3 banks and check that they returns at correct times (read)
    gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB+10, 0);
    gen_memory_transaction(1, CUT_1_ADDR, MAGIC_NB+11, 1);
    gen_memory_transaction(1, CUT_2_ADDR, MAGIC_NB+12, 2);

    //wait the end of the 3 transactions
    while(rsp_count < 3){
        std::cout <<  "[TEST_CASE_00] waiting for responses ... "<< std::endl;
        wait(m_peq.get_event());
        pl = m_peq.get_next_transaction();
        while (pl!=NULL){
            rsp_count++;
            std::cout <<  "[TEST_CASE_00] recieved response." << std::endl;
            delete pl->get_data_ptr();
            delete pl;
            pl = m_peq.get_next_transaction();
        }
    }

    for(int i=0; i<3; i++){
        if(expected_rsp_times[i] != m_rsp_time[i]){
            std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
            << "] inconsistent response time from port "<< i << " on WRITE operation." << std::endl;
            std::cerr << "[" << name() << "] expected cycle = " << expected_rsp_times[i] << ", response time = " << m_rsp_time[i] << std::endl;
            abort();
        }
    }
    std::cout <<  "[TEST_CASE_00] all WRITE transaction returned at appropriate times." << std::endl;
    std::cout <<  "[TEST_CASE_00] success" << std::endl;

}

/*
 * this test aims to ensure that all ports can access all cuts (TEST_01)
 */
void Generator::test_case_01(void)
{
    std::cout <<"[TEST_CASE_01] starting ..." << std::endl;
    std::cout <<"[TEST_CASE_01] this test aims to ensure that all ports can access all cuts." << std::endl;

    tlm::tlm_generic_payload* pl;
    uint32_t * buffer = new uint32_t(0);

    for(int port=0; port<3; port++){
        std::cout <<"[TEST_CASE_01] writing magic number into whole DUT using port " << port << std::endl;
        // write magic number in all DUT:
        for(int ad=0; ad<CUT_SIZE*3; ad+=4){
            gen_memory_transaction(1, ad, MAGIC_NB+port, port);
            wait(m_peq.get_event());
            pl = m_peq.get_next_transaction();
            delete pl->get_data_ptr();
            delete pl;
        }


        std::cout <<"[TEST_CASE_01] verifying values written by port " << port << std::endl;
        // read previously written magic number:
        for(int ad=0; ad<CUT_SIZE*3; ad+=4){
            gen_memory_transaction(0, ad, MAGIC_NB+port, port);
            wait(m_peq.get_event());
            pl = m_peq.get_next_transaction();
            memcpy(buffer,pl->get_data_ptr(),pl->get_data_length());
            delete pl->get_data_ptr();
            delete pl;
            if(*buffer != MAGIC_NB+port){
                std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
                << "] inconsistent response value from port "<< port << " on address " << ad
                << ", test failed to read previously written magic number" << std::endl;
                abort();
            }
        }
        std::cout <<"[TEST_CASE_01] successfully R/W all memory for port " << port << std::endl;
    }
    std::cout <<  "[TEST_CASE_01] all banks have been written and read from all ports." << std::endl;
    std::cout <<  "[TEST_CASE_01] success" << std::endl;
}

/*
 * this test aims to ensure that fixed priority between the spm ports is correctly implemented (TEST_02)
 * note : the lowest value is highest priority for port priority
 */
void Generator::test_case_02(void)
{
    std::cout <<"[TEST_CASE_02] starting ..." << std::endl;
    std::cout <<"[TEST_CASE_02] this test aims to ensure that fixed priority between the spm ports is correctly implemented." << std::endl;

    tlm::tlm_generic_payload* pl;

    // reset response time logging by port
    m_rsp_time[0]=0;
    m_rsp_time[1]=0;
    m_rsp_time[2]=0;

    std::cout <<"[TEST_CASE_02] initial request generation (one by port, targeting same cut)" << std::endl;

    // generates 1 requests by port targeting the same bank (writes with different magic numbers)
    std::cout <<"[TEST_CASE_02] port 0 request" << std::endl;
    gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB, 0);
    std::cout << "[TEST_CASE_02] port 1 request" << std::endl;
    gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB, 1);
    std::cout << "[TEST_CASE_02] port 2 request" << std::endl;
    gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB, 2);

    std::cout << "[TEST_CASE_02] flooding requests on highest priority port." << std::endl;

    // wait for a response and immediately emmit a new request from the highest priority port (a couple of times)
    for (int i = 0; i < 4; i++) {
        wait(m_peq.get_event());
        pl = m_peq.get_next_transaction();
        delete pl->get_data_ptr();
        delete pl;
        gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB, 2);
    }

    // ensure no response has been recieved on other ports (this test assumes that the 3rd port have the highest priority)
    if (m_rsp_time[0] || m_rsp_time[1]) {
        std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds())
        << "] messages emmited on port with low priorities have been processed and should not have been." << std::endl;
        abort();
    }

    // recieve the request responses still in progress (one by port)
    for (int i = 0; i < 3; i++) {
        wait(m_peq.get_event());
        pl = m_peq.get_next_transaction();
        delete pl->get_data_ptr();
        delete pl;
    }

    std::cout <<  "[TEST_CASE_02] transactions priorities have been respected: only transactions from the highest priority port have been processed." << std::endl;
    std::cout <<  "[TEST_CASE_02] success" << std::endl;
}


/*
 * this test aims to ensure that higher priority request send after is executed before request of lower priority (TEST_03)
 * note : the lowest value is highest priority for port priority
 */
void Generator::test_case_03(void)
{
    std::cout <<"[TEST_CASE_03] starting ..." << std::endl;
    std::cout <<"[TEST_CASE_03] this test aims to ensure that higher priority request send after is executed before request of lower priority." << std::endl;

    tlm::tlm_generic_payload* pl;

    // reset response time logging by port
    m_rsp_time[0]=0;
    m_rsp_time[1]=0;
    m_rsp_time[2]=0;

    // generates 1 request on port 1
    std::cout <<"[TEST_CASE_03] port 1 request at at cycle " <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
              << std::endl;
    gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB, 1);

    // wait third of write request process time
    wait((W_LAT/3) * PLATFORM_CYCLE);

    // generates 1 request on port 0
    std::cout <<"[TEST_CASE_03] port 0 request at cycle " <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
              << std::endl;
    gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB, 0);

    // wait third of write request process time
    wait((W_LAT/3) * PLATFORM_CYCLE);


    // generates 1 request on port 2 (highest priority)
    std::cout <<"[TEST_CASE_03] port 0 request at cycle " <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
              << std::endl;
    gen_memory_transaction(1, CUT_0_ADDR, MAGIC_NB, 2);

    // wait for the end of the first request
    wait(m_peq.get_event());
    pl = m_peq.get_next_transaction();
    delete pl->get_data_ptr();
    delete pl;

    // ensure it was the only request processed yet
    if(m_rsp_time[0] || m_rsp_time[2] || !m_rsp_time[1]){
        std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
        << "] the wrong request was processed." << std::endl;
        abort();
    }

    // wait for the end of the second request
    wait(m_peq.get_event());
    pl = m_peq.get_next_transaction();
    delete pl->get_data_ptr();
    delete pl;

    // ensure the highest priority request have been processed before the lowest.
    if( m_rsp_time[0] || !m_rsp_time[2]){
        std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
        << "] error: the request with lowest priority but send before was processed." << std::endl;
        abort();
    }

    std::cout <<  "[TEST_CASE_03] transactions priorities have been respected: the request with highest priority but send after was processed first." << std::endl;
    std::cout <<  "[TEST_CASE_03] success" << std::endl;
}


/*
 * generates the memory transaction on specified port
 * no check on address
 */
void Generator::gen_memory_transaction(bool write, uint32_t addr, uint32_t data, uint32_t port)
{
    tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    int* data_ptr;
    tlm::tlm_sync_enum status;
    int buf = addr;

    pl = new tlm::tlm_generic_payload();
    data_ptr = new int(0);

    *data_ptr = data;

    if(write){
        pl->set_command(tlm::TLM_WRITE_COMMAND);
    } else {
        pl->set_command(tlm::TLM_READ_COMMAND);
    }
    pl->set_address(addr);
    pl->set_data_ptr(reinterpret_cast<unsigned char*>(data_ptr));
    pl->set_data_length(4);
    pl->set_streaming_width(4); // = data_length to indicate no streaming
    pl->set_byte_enable_ptr(0);
    pl->set_dmi_allowed(false); // Mandatory initial value
    pl->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    phase = tlm::BEGIN_REQ;
    delay = sc_core::sc_time(0, sc_core::SC_PS);

    // send on specified interface
    switch(port){
        case 0:
            status = socket_0->nb_transport_fw(*pl, phase, delay);
            break;
        case 1:
            status = socket_1->nb_transport_fw(*pl, phase, delay);
            break;
        case 2:
            status = socket_2->nb_transport_fw(*pl, phase, delay);
            break;
        default:
            std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
            << "] port passed to gen_memory_transaction(...) doesn't exist" << std::endl;
            abort();
            break;
    }
    if (status != tlm::TLM_ACCEPTED) {
        std::cerr << "[" << name() << "] at [" <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds())
        << "] TLM error (addr=" << hex << buf << ")" << std::dec << std::endl;
        abort();
    }

}


/*
 * reset the SPM whole memory content to 0 (between test cases)
 */
void Generator::reset_mem_state(void)
{
    uint32_t * bank_ptr = 0;
    std::cout <<"[DYCTON] memory content reset" << std::endl;

    // then reset the memory content of all banks
    for (int i=0; i<3; i++){
        bank_ptr = m_dut->get_mem_ptr(i);
        std::memset(bank_ptr, 0, CUT_SIZE);
    }
}


// TLM-2 backward non-blocking transport method
tlm::tlm_sync_enum Generator::nb_transport_bw_0(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_time& delay)
{
    // std::cout <<  "port 0 got a response at " <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds()) << std::endl;
    m_rsp_time[0] = (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds());
    m_peq.notify(pl);
    return tlm::TLM_COMPLETED;
}


tlm::tlm_sync_enum Generator::nb_transport_bw_1(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_time& delay)
{
    // std::cout <<  "port 1 got a response at " <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds()) << std::endl;
    m_rsp_time[1] = (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds());
    m_peq.notify(pl);
    return tlm::TLM_COMPLETED;
}


tlm::tlm_sync_enum Generator::nb_transport_bw_2(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_time& delay)
{
    // std::cout <<  "port 2 got a response at " <<  (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds()) << std::endl;
    m_rsp_time[2] = (uint64_t)(PROC_FREQ*sc_core::sc_time_stamp().to_seconds());
    m_peq.notify(pl);
    return tlm::TLM_COMPLETED;
}