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

#ifndef ARBITRATED_MEM_H
#define ARBITRATED_MEM_H

#include <tlm>
#include "scratch_pad_memory.h"
#include "../log_trace.h"


typedef unsigned int ram_t;



class ArbitratedMem : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(ArbitratedMem);

public:

    ArbitratedMem( sc_core::sc_module_name name,
                   ScatchPadMemory* spm,
                   unsigned int bank_id,
                   pl_buffer** buf_ptr,
                   unsigned int nb_ports,
                   unsigned int size,
                   unsigned int r_latency,
                   unsigned int w_latency);

    ~ArbitratedMem();
    void trans_read_data(tlm::tlm_generic_payload &pl);
    void trans_write_data(tlm::tlm_generic_payload &pl);
    sc_core::sc_event& get_request_notification_event();
    void set_base_address(uint32_t ba) {m_bus_base_address = ba;}
    unsigned int get_base_address() {return m_bus_base_address;}

    uint64_t get_r_count(){return m_r_count;}
    uint64_t get_w_count(){return m_w_count;}

private:
    ScatchPadMemory* m_spm;
    sc_core::sc_event m_req_notif;
    pl_buffer** m_buffers;
    unsigned int m_bank_id;
    unsigned int m_nb_ports;
    unsigned int m_size;
    unsigned int m_r_lat;
    unsigned int m_w_lat;
    uint32_t m_bus_base_address;
    uint64_t m_r_count;
    uint64_t m_w_count;

    void memory_process();

public:
    /* The loader must have access to the storage */
    ram_t *m_content;

};

#endif