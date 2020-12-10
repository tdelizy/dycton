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

#ifndef MEMORY_H
#define MEMORY_H

// #include <tlm>
#include "tlm_utils/peq_with_get.h"
#include "tlm_utils/simple_target_socket.h"
#include "../log_trace.h"

class Memory : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(Memory);

public:
    tlm_utils::simple_target_socket<Memory> target;

    Memory( sc_core::sc_module_name name,
            unsigned int size,
            unsigned int r_latency,
            unsigned int w_latency);
    ~Memory();

    void b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & delay_time);
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);
    void set_base_address(uint32_t ba) {m_bus_base_address = ba;}
    unsigned int get_base_address() {return m_bus_base_address;}

    uint64_t get_r_count(){return m_r_count;}
    uint64_t get_w_count(){return m_w_count;}

    /* The loader must have access to the storage */
    uint32_t *storage;

private:
    unsigned int m_size;
    unsigned int m_r_lat;
    unsigned int m_w_lat;
    uint32_t m_bus_base_address;
    uint64_t m_r_count;
    uint64_t m_w_count;

    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_peq;

    void trans_write_data(tlm::tlm_generic_payload & pl);
    void trans_read_data(tlm::tlm_generic_payload & pl);
    void memory_process();
};


#endif
