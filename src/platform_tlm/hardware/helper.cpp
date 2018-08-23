#include "helper.h"
#include "offsets/helper.h"
#include "../iss/soclib_endian.h"

#include "../log_trace.h"
#include "../platform_time.h"
#include "../address_map.h"

#define DBG(x) DY_DBG(x, PER_ID, HELPER_SUB_ID)

uint32_t alloc_addr = 0;
uint32_t alloc_size = 0;
uint32_t free_addr = 0;
uint32_t free_size = 0;
sc_core::sc_time alloc_start = sc_core::sc_time(0, sc_core::SC_PS);
sc_core::sc_time free_start = sc_core::sc_time(0, sc_core::SC_PS);


Helper::Helper(sc_core::sc_module_name name, Oracle * oracle)
    : sc_core::sc_module(name),
    m_peq("m_peq")
{
    if(oracle != NULL)
        m_oracle = oracle;
    target.register_b_transport(this, &Helper::b_transport);
    target.register_nb_transport_fw(this, &Helper::nb_transport_fw);
  	out.initialize(false);
    SC_THREAD(process);
}

void Helper::b_transport(tlm::tlm_generic_payload& pl, sc_core::sc_time& delay_time)
{
    assert(pl.get_response_status()==tlm::TLM_INCOMPLETE_RESPONSE);
    DBG(std::cout << "["<< name() <<"] stopping simulation by software trigger at time "
    << sc_core::sc_time_stamp().to_seconds() << " seconds" << std::endl);
    sc_core::sc_stop();
    pl.set_response_status(tlm::TLM_OK_RESPONSE);
}

tlm::tlm_sync_enum Helper::nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
    uint32_t a = 0;
    uint32_t d = 0;
    uint32_t oracle_response = 0xFFFF;
    assert(pl.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE);

    if (pl.get_command() == tlm::TLM_WRITE_COMMAND) {
        a = pl.get_address(); // get address
        memcpy((char*)(&d), pl.get_data_ptr(), pl.get_data_length()); // get data (we don't expect data larger than sizeof(uint_32_t))
        d = uint32_machine_to_be(d);
        switch (a) {
        case STOP_SIM :
            std::cout << std::endl << "[" << name() << "] stopping simulation by software trigger at time "
                << sc_core::sc_time_stamp().to_seconds() << " seconds with exit level = " << std::dec << d << std::endl;
            sc_core::sc_stop();
            break;
        case ALLOC_SIZE : // asked size (start of malloc call)
        	malloc_count++;
            DBG(std::cout << "[" << name() << "] malloc - req_start (" << std::hex << d << ")" << std::endl);
            alloc_size = d;
            total_size_allocated += alloc_size;
            alloc_start = sc_core::sc_time_stamp();
            break;
        case ALLOC_ADDR : // malloc returned address (end of malloc call)
            DBG(std::cout << "[" << name() << "] malloc - req_end (" << std::hex << d << ")" << std::endl);
            platlog->log_malloc(d, alloc_size);
            // monitor time spend in allocator and heap memory footprint
            malloc_cycles+= (uint64_t)(PROC_FREQ * (sc_core::sc_time_stamp() - alloc_start).to_seconds());
            if((d-HEAP_BASE)>heap_footprint){
                heap_footprint = (d-HEAP_BASE);
            }
            break;
        case FREE_ADDR : // address to free (start of free call)
            DBG(std::cout << "[" << name() << "] free - req_start (" << std::hex << d << ")" << std::endl);
            free_addr = d;
            free_start = sc_core::sc_time_stamp();
            break;
        case FREE_SIZE : // free size (end of free call)
            DBG(std::cout << "[" << name() << "] free - req_end (" << std::hex << d << ")" << std::endl);
            platlog->log_free(free_addr);
            // monitor time spend in allocator
            free_cycles+= (uint64_t)(PROC_FREQ * (sc_core::sc_time_stamp() - free_start).to_seconds());
            break;
        case TIMED_EVENT : // register a timed event in the log, with prefix "2", the cycle count and the number given in parametter
            DBG(std::cout << "[" << name() << "] Timed Event: " << std::hex << d << ", at cycle "
                << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
            break;
        case ALLOC_FAIL_OFFSET :
        	malloc_fallback++;
        	break;
        default :
            std::cerr << name() << "unrecognized address for writing data, see helper.cpp (" << a << ")" << std::endl;
            abort();
            break;
        }
    } else if (pl.get_command() == tlm::TLM_READ_COMMAND) {
        a = pl.get_address(); // get address
        switch (a) {
        case ALLOCATOR_STRATEGY: // read only : return the strategy number to use
            *((uint32_t*)pl.get_data_ptr()) = uint32_be_to_machine(Strategy);
            break;
        case ALLOCATOR_ORACLE_REQ : // read only : ask the oracle in which heap place the current request
            oracle_response = m_oracle->get_next_prediction();
            DBG(std::cout << "[" << name() << "] Oracle request, response = " << std::dec << oracle_response
                << ", at cycle " << std::dec << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) << std::endl);
            oracle_response = uint32_be_to_machine(oracle_response);
            *((uint32_t*)pl.get_data_ptr()) = oracle_response;
            break;
        default :
            std::cerr << name() << "unrecognized address for reading data, see helper.cpp - @ = " << std::hex << a << std::endl;
            abort();
            break;
        }
    } else {
        std::cerr << name() << "command not supported." << std::endl;
        pl.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }
    m_peq.notify(pl);
    pl.set_response_status(tlm::TLM_OK_RESPONSE);
    return tlm::TLM_ACCEPTED;
}


void Helper::process()
{
    tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    tlm::tlm_sync_enum status;

    phase = tlm::TLM_COMPLETED;
    delay = sc_core::sc_time(0, sc_core::SC_PS);

    while (true) {
        // wait an incomming transaction
        wait(m_peq.get_event());
        // get the incomming transaction
        pl = m_peq.get_next_transaction();

        pl->set_response_status(tlm::TLM_OK_RESPONSE);
        // send response on backward path
        status = target->nb_transport_bw(*pl, phase, delay);
        if (status != tlm::TLM_COMPLETED) {
            std::cerr << name() << ": backward path failure." << std::endl;
            abort();
        }
    }

}