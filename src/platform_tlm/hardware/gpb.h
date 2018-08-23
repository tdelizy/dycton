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
