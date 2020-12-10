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

#include "memory.h"
#include "../platform_time.h"
#include <cstring>

#define DBG(x) DY_DBG(x, MEM_ID, MEM_SUB_ID)
#define LOG(x) DY_LOG(x, MEM_ID, MEM_SUB_ID)


// Constructor
Memory::Memory(sc_core::sc_module_name name,
               unsigned int size,
               unsigned int r_latency,
               unsigned int w_latency)
    : sc_module(name),
      m_peq("m_peq")
{
    m_size = size;
    m_r_lat = r_latency;
    m_w_lat = w_latency;
    DBG(std::cout << "[" << name << "] instanciating" << heap_base << std::endl);

    storage = new uint32_t[size / sizeof(uint32_t)];

    // Register callback
    target.register_nb_transport_fw(this, &Memory::nb_transport_fw);

    SC_THREAD(memory_process);
}

// Destructor
Memory::~Memory() {
    delete[] storage;
}


void Memory::trans_read_data(tlm::tlm_generic_payload &pl)
{
	DBG(std::cout << std::dec << "[" << name() << "] trans_read_data" << std::endl);
	m_r_count++;
    uint32_t addr = pl.get_address()/sizeof(uint32_t);
    assert(addr + pl.get_data_length() <= m_size);
    unsigned char* dp = (unsigned char*)pl.get_data_ptr();
    unsigned char* mp = (unsigned char*)(storage + addr);
    if(pl.get_byte_enable_ptr() != NULL){
        for (int i=3; (i+1)>0; i--){
            if(*(pl.get_byte_enable_ptr())&(1<<i)){
                *dp = *mp;
            }
            dp++;
            mp++;
        }
    } else {
        memcpy(pl.get_data_ptr(), (char*)(storage + addr), pl.get_data_length());
    }
    // platlog->log_access(pl.get_address()+m_bus_base_address, false);
    // introduce the memory read delay
    wait(m_r_lat * PLATFORM_CYCLE);
}

void Memory::trans_write_data(tlm::tlm_generic_payload &pl)
{
	DBG(std::cout << std::dec << "[" << name() << "] trans_write_data" << std::endl);
	m_w_count++;
    uint32_t addr = pl.get_address()/sizeof(uint32_t);
    assert(addr + pl.get_data_length() <= m_size);
    unsigned char* dp = (unsigned char*)pl.get_data_ptr();
    unsigned char* mp = (unsigned char*)(storage + addr);
    if(pl.get_byte_enable_ptr() != NULL){
        for (int i=3; (i+1)>0; i--){
            if(*(pl.get_byte_enable_ptr())&(1<<i))
                *mp = *dp;
            dp++;
            mp++;
        }
    } else {
        memcpy((char*)(storage + addr), pl.get_data_ptr(), pl.get_data_length());
    }
    // platlog->log_access(pl.get_address()+m_bus_base_address, true);
    // introduce the memory write delay
    wait(m_w_lat * PLATFORM_CYCLE);
}

void Memory::b_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay_time)
{
	DBG(std::cout << std::dec << "[" << name() << "] b_transport" << std::endl);
    assert(payload.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE);

    switch (payload.get_command()) {
    default:
        std::cout << name() << "command not supported." << std::endl;
        payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return;
    case tlm::TLM_WRITE_COMMAND:
        trans_write_data(payload);
        break;
    case tlm::TLM_READ_COMMAND:
        trans_read_data(payload);
        break;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE);

}

tlm::tlm_sync_enum Memory::nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
	DBG(std::cout << std::dec << "[" << name() << "] nb_transport_fw" << std::endl);
	unsigned int     len = pl.get_data_length();
	unsigned int     wid = pl.get_streaming_width();

	if (len > 4 || wid < len) {
		pl.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
		return tlm::TLM_COMPLETED;
	}

	m_peq.notify(pl);
	return tlm::TLM_ACCEPTED;
}

void Memory::memory_process()
{
	tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    tlm::tlm_sync_enum status;

    phase = tlm::TLM_COMPLETED;
    delay = sc_core::sc_time(0, sc_core::SC_PS);

	while (true) {
	DBG(std::cout << std::dec << "[" << name() << "] memory_process : waiting for event" << std::endl);
		// wait an incomming transaction
		wait(m_peq.get_event());

	DBG(std::cout << std::dec << "[" << name() << "] memory_process : event notified" << std::endl);

		// get the incomming transaction
		pl = m_peq.get_next_transaction();

		// process the payload
		assert(pl->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE);
        switch (pl->get_command()) {
        default:
            std::cout << name() << "command not supported." << std::endl;
            pl->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
            break;
        case tlm::TLM_WRITE_COMMAND:
            trans_write_data(*pl);
            break;
        case tlm::TLM_READ_COMMAND:
            trans_read_data(*pl);
            break;
        }
        pl->set_response_status(tlm::TLM_OK_RESPONSE);

		// send response on backward path
		status = target->nb_transport_bw(*pl, phase, delay);
		if (status != tlm::TLM_COMPLETED) {
			std::cerr << name() << ": backward path failure." << std::endl;
			abort();
		}

	}

}
