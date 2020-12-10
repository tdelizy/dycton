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

#ifndef SCRATCH_PAD_MEM_H
#define SCRATCH_PAD_MEM_H

#include <tlm>
#include "tlm_utils/simple_target_socket.h"

typedef unsigned int ram_t;

class ArbitratedMem;


class pl_buffer {
protected:
    unsigned int m_req_prio;
    unsigned int m_dest_bank;
    unsigned int m_orig_port;
    unsigned int m_bus_addr;
    tlm::tlm_generic_payload* m_req_ptr;

public:
    pl_buffer()
    {
        // std::cout << std::dec << "[pl_buffer] default constructor " << std::endl;
        m_req_ptr = NULL;
    }

    pl_buffer(unsigned int prio, unsigned int port)
    {
        // std::cout << std::dec << "[pl_buffer] constructor( " <<prio <<", " <<port << std::endl;
        m_req_ptr = NULL;
        m_req_prio = prio;
        m_orig_port = port;
    }

    // return true if request was accepted, false if the buffer is not empty
    bool set_req(tlm::tlm_generic_payload* new_req, unsigned int dest_bank)
    {
        if(m_req_ptr == NULL){
            m_req_ptr = new_req;
            m_dest_bank = dest_bank;
            m_bus_addr = new_req->get_address();
            return true;
        }
        return false;
    }

    unsigned int check_req(unsigned int asking_bank)
    {
        // std::cout << std::dec << "[CHECK_REQ] asking_bank =  " << asking_bank << " m_req_prio = " << m_req_prio << " m_req_ptr = " << m_req_ptr << std::endl;
        if(m_req_ptr && asking_bank == m_dest_bank)
            return m_req_prio;
        return 0xFFFFFFFF;
    }

    // if the request is not targetting the bank number passed in parameters (or no request), returns null
    // otherwise return pointer to payload.
    tlm::tlm_generic_payload* get_req(unsigned int asking_bank)
    {
        tlm::tlm_generic_payload * ret = NULL;
        if(m_req_ptr && asking_bank == m_dest_bank){
            ret = m_req_ptr;
            m_req_ptr = NULL;
        }
        return ret;
    }
    pl_buffer& operator=(const pl_buffer& other)
    {
        if (this != &other) { // self-assignment check
            m_req_prio = other.m_req_prio;
            m_dest_bank = other.m_dest_bank;
            m_req_ptr = other.m_req_ptr;
            m_orig_port = other.m_orig_port;
        }
        return *this;
    }
    unsigned int prio(){return m_req_prio;}
    unsigned int dest(){return m_dest_bank;}
    unsigned int orig(){return m_orig_port;}
    unsigned int addr(){return m_bus_addr;}
    bool empty(){return (m_req_ptr == NULL)?true:false;}

};



class ScatchPadMemory : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(ScatchPadMemory);

public:
    tlm_utils::simple_target_socket_tagged<ScatchPadMemory> *sockets;

	ScatchPadMemory( sc_core::sc_module_name name,
                unsigned int nb_ports,
                unsigned int nb_banks,
                unsigned int sizes[],
                unsigned int r_latencies[],
                unsigned int w_latencies[],
                unsigned int ports_prio[]);
	~ScatchPadMemory();

    unsigned int * get_mem_ptr(unsigned int bank_index);

    tlm::tlm_sync_enum nb_transport_fw(int id, tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);

    void set_bank_base_address(uint32_t bank, uint32_t base);
    uint64_t get_bank_read_count(uint32_t bank);
    uint64_t get_bank_write_count(uint32_t bank);


private:
    pl_buffer** m_buffers;
    ArbitratedMem **m_banks;
    unsigned int m_nb_banks;
    unsigned int m_nb_ports;
    unsigned int *m_sizes;
    unsigned int *m_priorities;
};


#endif // SCRATCH_PAD_MEM_H
