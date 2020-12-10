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

#ifndef GENERATOR_H
#define GENERATOR_H


#include "tlm_utils/peq_with_get.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "scratch_pad_memory.h"


struct Generator : sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(Generator);

public:
    tlm_utils::simple_initiator_socket<Generator> socket_0;
    tlm_utils::simple_initiator_socket<Generator> socket_1;
    tlm_utils::simple_initiator_socket<Generator> socket_2;

    Generator(sc_core::sc_module_name name, ScatchPadMemory * dut);

private:
    ScatchPadMemory * m_dut;
    int m_test_case;
    bool m_running;
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_peq;

    uint32_t m_rsp_time[3];

    void run_test_cases();
    void test_case_00(void);
    void test_case_01(void);
    void test_case_02(void);
    void test_case_03(void);

    void reset_mem_state(void);
    void gen_memory_transaction(bool write, uint32_t addr, uint32_t data, uint32_t port);
    tlm::tlm_sync_enum nb_transport_bw_0(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);
    tlm::tlm_sync_enum nb_transport_bw_1(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);
    tlm::tlm_sync_enum nb_transport_bw_2(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);
};

#endif
