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

#ifndef HELPER_H
#define HELPER_H


#include <fstream>
#include <cstdlib>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/peq_with_get.h"
#include <string>

#include "../log_trace.h"

// small code for reading from urandom
// from https://gist.github.com/mortenpi/9745042
template<class T>
T read_urandom()
{
    union {
        T value;
        char cs[sizeof(T)];
    } u;

    std::ifstream rfin("");
    rfin.read(u.cs, sizeof(u.cs));
    rfin.close();

    return u.value;
}



//forward declaration
class Oracle; 
class MIPS32Wrapper;

SC_MODULE(Helper)
{
    tlm_utils::simple_target_socket<Helper> target;
    sc_core::sc_out<bool> out; // stub for proc interrupt

    SC_HAS_PROCESS(Helper);
    Helper(sc_core::sc_module_name name, Oracle * oracle, MIPS32Wrapper * iss_wrapper, const std::string &rng_file);

    void b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & delay_time);
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);

    void profile_set_len_val(uint32_t val);

private:
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_peq;
    Oracle * m_oracle;
    uint32_t m_profile_len;
    MIPS32Wrapper * m_iss_wrapper;
    std::ifstream m_rng_sim_file;

    void process();
};

#endif
