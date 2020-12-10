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

/* Simulation Helper
 *
 * imaginary component used for all simulation tricks, implemented as a SystemC
 * TLM module :
 * - allow logging start and end of allocation and free of objects
 * - allow to provide the embedded software with values that would have been
 *   statically defined in a real platform but are simulation parameters   
 * - enthropy generator stub for ECDSA application
 *
 */



#include "helper.h"
#include "offsets/helper.h"
#include "../iss/soclib_endian.h"
#include "../iss/mips32_wrapper.h"

#include "../platform_time.h"
#include "../address_map.h"

#define DBG(x) DY_DBG(x, PER_ID, HELPER_SUB_ID)

uint32_t alloc_addr = 0;
uint32_t alloc_size = 0;
uint32_t free_addr = 0;
uint32_t free_size = 0;
uint32_t alloc_site = 0;
uint32_t fallbacked = 0;
sc_core::sc_time alloc_start = sc_core::sc_time(0, sc_core::SC_PS);
sc_core::sc_time free_start = sc_core::sc_time(0, sc_core::SC_PS);


Helper::Helper(sc_core::sc_module_name name, Oracle * oracle, MIPS32Wrapper * iss_wrapper, const std::string &rng_file)
    : sc_core::sc_module(name),
    m_peq("m_peq")
{
    m_profile_len = 0;
    if(oracle != NULL)
        m_oracle = oracle;
    if(iss_wrapper != NULL)
        m_iss_wrapper = iss_wrapper;
    else {
        std::cerr << "no iss to link to helper, failure." << std::endl;
        abort();
    }

    m_rng_sim_file.open(rng_file, std::ios::binary | std::ios::in);
    if(!m_rng_sim_file.is_open()){
        std::cerr << "unable to open rng seed file, aborting..." << std::endl;
        abort();
    }
    
    target.register_b_transport(this, &Helper::b_transport);
    target.register_nb_transport_fw(this, &Helper::nb_transport_fw);
  	out.initialize(false);
    SC_THREAD(process);
}

void Helper::profile_set_len_val(uint32_t val)
{
    if(val){
        m_profile_len = val;
    } else {
        std::cerr << std::dec << "error in Helper::profile_set_len_val("<< val <<"): invalid size." << std::endl;
        abort();  
    }
}


void Helper::b_transport(tlm::tlm_generic_payload& pl, sc_core::sc_time& delay_time)
{
    assert(pl.get_response_status()==tlm::TLM_INCOMPLETE_RESPONSE);
    DBG(std::cout << "["<< name() <<"] stopping simulation by software trigger at time "
    << sc_core::sc_time_stamp().to_seconds() << " seconds" << std::endl);
    sc_core::sc_stop();
    pl.set_response_status(tlm::TLM_OK_RESPONSE);
}

tlm::tlm_sync_enum Helper::nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
    uint32_t a = 0;
    uint32_t d = 0;
    uint32_t oracle_response = 0xFFFF;
    assert(pl.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE);
    a = pl.get_address(); // get address
    if (a < 0x200){ // simple register range
        if (pl.get_command() == tlm::TLM_WRITE_COMMAND) {
            memcpy((char*)(&d), pl.get_data_ptr(), pl.get_data_length()); // get data (we don't expect data larger than sizeof(uint_32_t))
            d = uint32_machine_to_be(d);
            switch (a) {
            case STOP_SIM :
                std::cout << std::endl << "[" << name() << "] stopping simulation by software trigger at time "
                    << sc_core::sc_time_stamp().to_seconds() << " seconds with exit level = " << std::dec << d << std::endl;
                sc_core::sc_stop();
                break;
            case ALLOC_SIZE : // asked size (start of malloc call)
            	malloc_count++;
                DBG(std::cout << "[" << name() << "] malloc - req_start (" << std::hex << d << ")" << std::endl);
                alloc_size = d;
                total_size_allocated += alloc_size;
                alloc_start = sc_core::sc_time_stamp();
                break;
            case ALLOC_ADDR : // malloc returned address (end of malloc call)
                DBG(std::cout << "[" << name() << "] malloc - req_end (" << std::hex << d << ")" << std::endl);
                platlog->log_malloc(d, alloc_size, alloc_site, fallbacked);
                // monitor time spend in allocator and heap memory footprint
                malloc_cycles+= (uint64_t)(PROC_FREQ * (sc_core::sc_time_stamp() - alloc_start).to_seconds());
                fallbacked = 0;
                if(d < HEAP_BASE || d > HEAP_BASE+heap_size){
                    std::cerr << name() << "ERROR: malloc returned an address outside the heap ! (returned address = " << std::hex << d << ")" << std::endl;
                    // abort(); 
                    break;
                }
                if((d-HEAP_BASE)>heap_footprint){
                    heap_footprint = (d-HEAP_BASE);
                }
                break;
            case FREE_ADDR : // address to free (start of free call)
                DBG(std::cout << "[" << name() << "] free - req_start (" << std::hex << d << ")" << std::endl);
                free_addr = d;
                free_start = sc_core::sc_time_stamp();
                break;
            case FREE_SIZE : // free size (end of free call)
                DBG(std::cout << "[" << name() << "] free - req_end (" << std::hex << d << ")" << std::endl);
                platlog->log_free(free_addr);
                // monitor time spend in allocator
                free_cycles+= (uint64_t)(PROC_FREQ * (sc_core::sc_time_stamp() - free_start).to_seconds());
                break;
            case TIMED_EVENT : // register a timed event in the log, with prefix "2", the cycle count and the number given in parametter
                DBG(std::cout << "[" << name() << "] Timed Event: " << std::hex << d << ", at cycle "
                    << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                break;
            case ALLOC_FAIL_OFFSET :
                fallbacked = 1;
            	malloc_fallback++;
            	break;
            case PRINT_REGS:
                m_iss_wrapper->print_iss_regs();
                break;
            case RETURN_ADDR:
                alloc_site = d;
                DBG(std::cout << "[" << name() << "] Allocation Site: " << std::hex << d << ", at cycle "
                    << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                break;
            default :
                std::cerr << name() << "unrecognized address for writing data, see helper.cpp (" << a << ")" << std::endl;
                abort();
                break;
            }
        } else if (pl.get_command() == tlm::TLM_READ_COMMAND) {
            a = pl.get_address(); // get address
            switch (a) {
            case ALLOCATOR_STRATEGY: // read only : return the strategy number to use
                *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(Strategy);
                break;
            case ALLOCATOR_ORACLE_REQ: // read only : ask the oracle in which heap place the current request
                oracle_response = m_oracle->get_next_prediction();
                DBG(std::cout << "[" << name() << "] Oracle request, response = " << std::dec << oracle_response
                    << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                oracle_response = uint32_be_to_machine(oracle_response);
                *((uint32_t*)pl.get_data_ptr()) = oracle_response;
                break;
            case PROFILE_LEN: // read only : size of profile 
                DBG(std::cout << "[" << name() << "] profile length request, response = " << std::dec << m_profile_len
                    << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(m_profile_len);
                break;
            case ENTROPY_GENERATOR: // read only : urandom from seed file
                {
                    uint32_t rnd_ret = 206;
                    m_rng_sim_file.read((char*)&rnd_ret, sizeof(rnd_ret));
                    if(m_rng_sim_file.eof()){
                        std::cerr << name() << "error : no mere inputs for entropy!" << std::endl;
                        abort();
                    }
                    DBG(std::cout << "[" << name() << "] read urandom = " << std::dec << rnd_ret
                        << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                    *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(rnd_ret);
                    break;
                }
            case HEAP_COUNT_INFO:
                DBG(std::cout << "[" << name() << "] heap count info request, response = " << std::dec << dy_mem_arch->get_heap_count()
                    << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(dy_mem_arch->get_heap_count());
                break;
            case DATASET_INDEX:
                DBG(std::cout << "[" << name() << "] dataset index info request, response = " << std::dec << Dataset_index
                    << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(Dataset_index);
                break;
            case HEAP_ARCHI:
                DBG(std::cout << "[" << name() << "] heap architecture info request, response = " << std::dec << dy_mem_arch->get_architecture_id()
                    << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(dy_mem_arch->get_architecture_id());
                break;
            default :
                std::cerr << name() << "unrecognized address for reading data, see helper.cpp - @ = " << std::hex << a << std::endl;
                abort();
                break;
            }
        } else {
            std::cerr << name() << "command not supported." << std::endl;
            pl.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
            return tlm::TLM_COMPLETED;
        }
    } else { // address depends on requested data, access by range:
        if (pl.get_command() == tlm::TLM_WRITE_COMMAND) {
            std::cerr << name() << "unrecognized address for writing data, see helper.cpp (" << a << ")" << std::endl;
            abort();
        } else if (pl.get_command() == tlm::TLM_READ_COMMAND) {
            a = pl.get_address(); // get address
            if (a < HEAP_BASE_INFO + HELPER_RANGE_LEN) {
                uint32_t resp = 0;
                uint32_t target = (a - HEAP_BASE_INFO)/4;
                if (target < dy_mem_arch->get_heap_count()){
                    resp = dy_mem_arch->banks[target]->base_ad;
                    DBG(std::cout << "[" << name() << "] heap base address requested, response = " << std::dec << resp
                        << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                    *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(resp);
                } else {
                    std::cerr << name() << "asked for heap number " << std::dec << target << ", which does not exists." << std::endl;
                    abort();
                }
            } else if (a < HEAP_SIZE_INFO + HELPER_RANGE_LEN) {
                uint32_t resp = 0;
                uint32_t target = (a - HEAP_SIZE_INFO)/4;
                if (target < dy_mem_arch->get_heap_count()){
                    resp = dy_mem_arch->banks[target]->size;
                    DBG(std::cout << "[" << name() << "] heap size requested, response = " << std::dec << resp
                        << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
                    *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(resp);
                } else {
                    std::cerr << name() << "asked for heap number " << std::dec << target << ", which does not exists." << std::endl;
                    abort();
                }
            } else {
                std::cerr << name() << "unrecognized address for reading data, see helper.cpp - @ = " << std::hex << a << std::endl;
                abort();
            }
        } else {
            std::cerr << name() << "command not supported." << std::endl;
            pl.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
            return tlm::TLM_COMPLETED;
        }
    }
    m_peq.notify(pl);
    pl.set_response_status(tlm::TLM_OK_RESPONSE);
    return tlm::TLM_ACCEPTED;
}


void Helper::process()
{
    tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    tlm::tlm_sync_enum status;

    phase = tlm::TLM_COMPLETED;
    delay = sc_core::sc_time(0, sc_core::SC_PS);

    while (true) {
        // wait an incomming transaction
        wait(m_peq.get_event());
        // get the incomming transaction
        pl = m_peq.get_next_transaction();

        pl->set_response_status(tlm::TLM_OK_RESPONSE);
        // send response on backward path
        status = target->nb_transport_bw(*pl, phase, delay);
        if (status != tlm::TLM_COMPLETED) {
            std::cerr << name() << ": backward path failure." << std::endl;
            abort();
        }
    }

}