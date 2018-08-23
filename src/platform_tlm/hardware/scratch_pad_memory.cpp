#include "scratch_pad_memory.h"
#include "../iss/soclib_endian.h"
#include <string>
#include <sstream>
#include "arbitrated_mem.h"

#include "../log_trace.h"
#define DBG(x) DY_DBG(x, MEM_ID, SPM_SUB_ID)

// return the internal storage pointer for loading memory before simulation
unsigned int * ScatchPadMemory::get_mem_ptr(unsigned int bank_index)
{
	if(bank_index < m_nb_banks){
		return m_banks[bank_index]->m_content;
	}
	return NULL;
}

// Constructor
ScatchPadMemory::ScatchPadMemory(   sc_core::sc_module_name name,
                                    unsigned int nb_ports,
                                    unsigned int nb_banks,
                                    unsigned int sizes[],
                                    unsigned int r_latencies[],
                                    unsigned int w_latencies[],
                                    unsigned int ports_prio[])
                        			: sc_module(name)
{
	DBG(std::cout << "[" << name << "] instanciating : " << nb_ports << " ports, " << nb_banks << " banks."<<std::endl);

	// params allocation and init
	m_nb_banks = nb_banks;
	m_nb_ports = nb_ports;

	sockets = new tlm_utils::simple_target_socket_tagged<ScatchPadMemory>[m_nb_ports];
	m_banks = new ArbitratedMem *[m_nb_banks];
    m_buffers = new pl_buffer *[m_nb_ports];

    m_sizes = new unsigned int[m_nb_banks];
    m_priorities = new unsigned int[m_nb_banks];;

	for(unsigned int i=0; i<m_nb_banks; i++) {
		std::string bank_name = "";
		bank_name += static_cast<const char*>(name);
		bank_name += "::bank::";
		bank_name += static_cast<std::ostringstream*>( &(std::ostringstream() << i) )->str();
		DBG(std::cout << bank_name << "(" << sizes[i] << " octets) " << std::endl);
		m_banks[i] = new ArbitratedMem(bank_name.c_str(), this, i, m_buffers, m_nb_ports, sizes[i], r_latencies[i], w_latencies[i]);
        m_sizes[i] = sizes[i];
	}

	for(unsigned int i=0; i<m_nb_ports; i++) {
		std::string port_name = "";
		port_name += static_cast<const char*>(name);
		port_name += "::port::";
		port_name += static_cast<std::ostringstream*>( &(std::ostringstream() << i) )->str();
		DBG(std::cout << port_name  << std::endl);

        sockets[i].register_nb_transport_fw(this, &ScatchPadMemory::nb_transport_fw, i);
        m_priorities[i] = ports_prio[i];
        m_buffers[i] = new pl_buffer(m_priorities[i], i);
	}
}


// Destructor
ScatchPadMemory::~ScatchPadMemory() {

	for(unsigned int i=0; i<m_nb_banks; i++) {
		delete m_banks[i];
	}

	for(unsigned int i=0; i<m_nb_ports; i++) {
        delete m_buffers[i];
	}

	delete [] m_banks;
	delete [] sockets;
    delete [] m_buffers;
    delete [] m_priorities;
    delete [] m_sizes;

}



tlm::tlm_sync_enum ScatchPadMemory::nb_transport_fw(int id, tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
    uint64_t addr = pl.get_address();
    uint32_t bank = 0;
    uint32_t b_offset = 0;
    uint32_t port = id;

    // compute bank number and address bank offset
    while (bank < m_nb_banks - 1 && addr > (b_offset + m_sizes[bank]-4)) {
        b_offset += m_sizes[bank];
        bank++;
    }

    // Check if the address is within memory bounds and determine accessed bank & @offset of that bank
    if ((addr - b_offset) > m_sizes[bank]) {
        std::cerr << name() << ": Write access outside memory range! (@"
                  << std::hex << addr << ", bank=" << std::dec << bank << ", bank_offset=" << std::hex << b_offset << ")" << std::endl;
        pl.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    } else {

        DBG(std::cout << std::dec << "[" << name() << "] --> nb_transport_fw port =" << port <<", addr =" << std::hex << addr << ", bank = " << bank << ", offset = " << b_offset << std::endl);
        // check if the buffer of this port is empty and fill buffer with memory request
        // (for now just abort if buffer is not empty...)
        if(!m_buffers[port]->set_req(&pl, bank)){
            std::cerr << name() << ": buffer is not empty !" << std::endl;
            abort();
        }

        // modify address to internal bank address
        // done after buffer construction so it can save the original bus address
        pl.set_address(addr - b_offset);

        // notify bank that request is pending
        m_banks[bank]->get_request_notification_event().notify();
        return tlm::TLM_ACCEPTED;
    }
}


void ScatchPadMemory::set_bank_base_address(uint32_t bank, uint32_t base)
{
    if(bank < m_nb_banks)
        m_banks[bank]->set_base_address(base);
    else
    	assert(0);
}


uint64_t ScatchPadMemory::get_bank_read_count(uint32_t bank)
{
    if(bank < m_nb_banks)
        return m_banks[bank]->get_r_count();
    else
    	assert(0);
}

uint64_t ScatchPadMemory::get_bank_write_count(uint32_t bank)
{
    if(bank < m_nb_banks)
        return m_banks[bank]->get_w_count();
    else
    	assert(0);
}
