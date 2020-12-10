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

#include "arbitrated_mem.h"
#include "../platform_time.h"
#include "../iss/soclib_endian.h"
#include <cstring>

#define DBG(x) DY_DBG(x, MEM_ID, ARBITRATED_SUB_ID)
// #define LOG(x) DY_LOG(x, MEM_ID, ARBITRATED_SUB_ID)
// only log access to memory bank if its part of the heap
#define LOG(x) DY_LOG_HEAP_ONLY(m_bus_base_address, x, MEM_ID, ARBITRATED_SUB_ID)

// Constructor
ArbitratedMem::ArbitratedMem(   sc_core::sc_module_name name,
                                ScatchPadMemory* spm,
                                unsigned int bank_id,
                                pl_buffer** buf_ptr,
                                unsigned int nb_ports,
                                unsigned int size,
                                unsigned int r_latency,
                                unsigned int w_latency)
                                : sc_module(name)
{
    std::cout << std::dec << "[" << name << "] instanciation with params:" << std::endl;
    std::cout << std::dec << "\tsize" << size << std::endl;
    std::cout << std::dec << "\tr_latency" << r_latency << std::endl;
    std::cout << std::dec << "\tw_latency" << w_latency << std::endl;

    // init specific values and create memory
    m_spm = spm;
    m_bank_id = bank_id;
    m_buffers = buf_ptr;
    m_size = size;
    m_r_lat = r_latency;
    m_w_lat = w_latency;
    m_nb_ports = nb_ports;

    m_content = new ram_t[size / sizeof(uint32_t)];

    // systemC thread declaration
    SC_THREAD(memory_process);
}

// Destructor
ArbitratedMem::~ArbitratedMem() {
    delete [] m_content;
}


void ArbitratedMem::trans_read_data(tlm::tlm_generic_payload &pl)
{
    DBG(std::cout << std::dec << "[" << name() << "] trans_read_data" << std::endl);
    m_r_count++;
    uint32_t addr = pl.get_address()/sizeof(uint32_t);
    assert(addr + pl.get_data_length() <= m_size);
    unsigned char* dp = (unsigned char*)pl.get_data_ptr();
    unsigned char* mp = (unsigned char*)(m_content + addr);
    if(pl.get_byte_enable_ptr() != NULL){
        for (int i=3; (i+1)>0; i--){
            if(*(pl.get_byte_enable_ptr())&(1<<i)){
                *dp = *mp;
            }
            dp++;
            mp++;
        }
    } else {
        memcpy(pl.get_data_ptr(), (char*)(m_content + addr), pl.get_data_length());
    }
    LOG(platlog->log_access(pl.get_address()+m_bus_base_address, false));
    // introduce the memory read delay
    wait(m_r_lat * PLATFORM_CYCLE);
}

void ArbitratedMem::trans_write_data(tlm::tlm_generic_payload &pl)
{
    DBG(std::cout << std::dec << "[" << name() << "] trans_write_data" << std::endl);
	m_w_count++;
    uint32_t addr = pl.get_address()/sizeof(uint32_t);
    assert(addr + pl.get_data_length() <= m_size);
    unsigned char* dp = (unsigned char*)pl.get_data_ptr();
    unsigned char* mp = (unsigned char*)(m_content + addr);
    if(pl.get_byte_enable_ptr() != NULL){
        for (int i=3; (i+1)>0; i--){
            if(*(pl.get_byte_enable_ptr())&(1<<i)){
                *mp = *dp;
            }
            dp++;
            mp++;
        }
    } else {
        memcpy((char*)(m_content + addr), pl.get_data_ptr(), pl.get_data_length());
    }
    LOG(platlog->log_access(pl.get_address()+m_bus_base_address, true));
    // introduce the memory write delay
    wait(m_w_lat * PLATFORM_CYCLE);
}


sc_core::sc_event& ArbitratedMem::get_request_notification_event()
{
    DBG(std::cout << std::dec << "[" << name() << "] get_request_notification_event" << std::endl);
    return m_req_notif;
}

void ArbitratedMem::memory_process()
{
    tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    tlm::tlm_sync_enum status;

    phase = tlm::TLM_COMPLETED;
    delay = sc_core::sc_time(0, sc_core::SC_PS);

    while (true) {
        unsigned int prio;
        unsigned int target;
        unsigned int temp;

        // wait an incomming transaction
        DBG(std::cout << std::dec << "[" << name() << "] memory_process : waiting for event" << std::endl);
        wait(m_req_notif);
        DBG(std::cout << std::dec << "[" << name() << "] memory_process : event notified" << std::endl);

        // process requests until buffers are empty (after the end of the last request processing)
        while (true) {
            // execute at the end of the deltacycle
            wait(sc_core::SC_ZERO_TIME);
            prio = 0xFFFFFFFF;
            target = 0xFFFFFFFF;
            temp = 0xFFFFFFFF;
            // get the incomming transaction of highest priority
            for (unsigned int i = 0; i < m_nb_ports; i++) {
                temp = m_buffers[i]->check_req(m_bank_id);
                if (temp < prio) {
                    target = i;
                    prio = temp;
                }
            }

            // if target is not set then we can sleep this sc_thread until next m_req_notif
            if (target > m_nb_ports){
                break;
            }
            DBG(std::cout << std::dec << "[" << name() << "] memory_process : processing request from port " << target << std::endl);
            pl = m_buffers[target]->get_req(m_bank_id);


            // process the payload
            if (pl->get_data_length() > 4 ) {
                std::cerr << name() << ": data length > 4 Bytes are not supported by the memory" << std::endl;
                abort();
            }
            assert(pl->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE);
            switch (pl->get_command()) {
            default:
                std::cout << name() << "command not supported." << std::endl;
                pl->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
                break;
            case tlm::TLM_WRITE_COMMAND:
                trans_write_data(*pl);
                pl->set_response_status(tlm::TLM_OK_RESPONSE);
                break;
            case tlm::TLM_READ_COMMAND:
                trans_read_data(*pl);
                pl->set_response_status(tlm::TLM_OK_RESPONSE);
                break;
            }

            // restore consistent bus address
            pl->set_address(m_buffers[target]->addr());

            // send response on backward path
            status = m_spm->sockets[m_buffers[target]->orig()]->nb_transport_bw(*pl, phase, delay);
            if (status != tlm::TLM_COMPLETED) {
                std::cerr << name() << ": backward path failure." << std::endl;
                abort();
            }
        }

    }

}

