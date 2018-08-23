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
