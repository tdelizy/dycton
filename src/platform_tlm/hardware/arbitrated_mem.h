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