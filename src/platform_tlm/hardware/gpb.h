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

#ifndef GPB_H
#define GPB_H

#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/peq_with_get.h"

SC_MODULE(GPBuffer) {
public:
    enum BufferDir {
        BUFF_IN,
        BUFF_OUT
    };

    tlm_utils::simple_target_socket<GPBuffer> target;
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_peq;

    SC_HAS_PROCESS(GPBuffer);
    GPBuffer(   sc_core::sc_module_name name,
                const char * data_name = "GPBuffer_dycton.data",
                const char * log_name = "GPBuffer_dycton.log",
                BufferDir bd = BUFF_IN);
    ~GPBuffer();

    tlm::tlm_response_status read(uint32_t a, uint32_t & d);
    tlm::tlm_response_status write(uint32_t a, uint32_t d);

    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload & pl, tlm::tlm_phase & phase, sc_core::sc_time & delay);

    void process();

private:
    // data file
    std::fstream m_data_file;
    // log file
    std::fstream m_log_file;
    // buffer direction
    const BufferDir m_dir;
};

#endif
