#ifndef HELPER_H
#define HELPER_H

#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/peq_with_get.h"




class Oracle; //forward declaration



SC_MODULE(Helper)
{
    tlm_utils::simple_target_socket<Helper> target;
    sc_core::sc_out<bool> out; // stub for proc interrupt

    SC_HAS_PROCESS(Helper);
    Helper(sc_core::sc_module_name name, Oracle * oracle);

    void b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & delay_time);
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);

private:
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_peq;
    Oracle * m_oracle;
    void process();
};

#endif
