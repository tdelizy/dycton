#include "gpb.h"
#include <string>
#include <sstream>
#include <sys/time.h>
#include <iostream>
#include "../iss/soclib_endian.h"

#include "../log_trace.h"
#define DBG(x) DY_DBG(x, PER_ID, GPB_SUB_ID)
#define LOG(x) DY_LOG(x, PER_ID, GPB_SUB_ID)

GPBuffer::GPBuffer(sc_core::sc_module_name name, const char * data_name, const char * log_name, BufferDir bd)
    : sc_core::sc_module(name),
    m_peq("m_peq"),
    m_data_file(data_name, (bd==BUFF_IN) ? (std::fstream::in | std::ios::binary) : (std::fstream::out| std::ios::binary)), //c'est pas joli mais ça ouvre le fichier dans le bon mode
    m_log_file(log_name, std::fstream::out),
    m_dir(bd)
{
    target.register_nb_transport_fw(this, &GPBuffer::nb_transport_fw);
    SC_THREAD(process);

    int size = m_data_file.tellg();
    DBG(std::cout << "[" << name << "] instanciating (data=" << data_name << ", out =" << log_name <<", buffer direction = " << bd << std::endl);
    std::cout << "[" << name << "] instanciating (data=" << data_name << ", size =" << size << ")" << std::endl;

}

// Destructor
GPBuffer::~GPBuffer() {
  m_data_file.close();
  m_log_file.close();
}


// Read transactions
tlm::tlm_response_status GPBuffer::read(uint32_t a, uint32_t &d)
{
    std::string s;
    int buffer=0;
    bool eof = false;

    if(m_dir == BUFF_OUT){
        std::cout << name() << "General Output Buffer do not support read operation." << std::endl;
        return tlm::TLM_GENERIC_ERROR_RESPONSE;
    }

    // set response value to the next input from file

    // if eof, loop.
    /*
    if(!std::getline(m_data_file, s) && m_data_file.eof()){
        #ifdef TRACE_GPB
            std::cout << std::dec << "[" << name() << "] EOF, looping" << std::endl;
        #endif
        m_data_file.clear();
        m_data_file.seekg(0, std::ios::beg);
        std::getline(m_data_file, s);
    }
    */

    // not looping version
    if(m_data_file.eof())
        eof = true;

    buffer = m_data_file.get();

    // convert value to machine endianness
    if(eof){
        d = uint32_be_to_machine(EOF);
    }else{
        d = uint32_be_to_machine(buffer);
    }

    DBG(std::cout << std::dec << "[" << name() << "|R] time " << sc_core::sc_time_stamp().to_seconds() << ";" << std::hex << a << ";" << d << std::endl);
    LOG(m_log_file << sc_core::sc_time_stamp().to_seconds() << ";R;" << a << std::endl);

    return tlm::TLM_OK_RESPONSE;
}

// Write transactions
tlm::tlm_response_status GPBuffer::write(uint32_t a, uint32_t d)
{
    char buf = 0;

    if(m_dir == BUFF_IN){
        std::cout << name() << "General Input Buffer do not support write operation." << std::endl;
        return tlm::TLM_GENERIC_ERROR_RESPONSE;
    }

    if(d == (uint32_t)EOF)
        buf= EOF;
    else
        buf = uint32_machine_to_be(d);
    m_data_file.write(&buf,1);

    DBG(std::cout << std::dec << "[" << name() << "|W] time " << sc_core::sc_time_stamp().to_seconds() << ";" << std::hex << a << ";" << d << std::endl);
    LOG(m_log_file << sc_core::sc_time_stamp().to_seconds() << ";W;" << a << std::endl);

    return tlm::TLM_OK_RESPONSE;
}

tlm::tlm_sync_enum GPBuffer::nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
    DBG(std::cout << std::dec << "[" << name() << "] nb_transport_fw" << std::endl);
    m_peq.notify(pl);
    return tlm::TLM_ACCEPTED;
}


void GPBuffer::process()
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
    DBG(std::cout << std::dec << "[" << name() << "] process : event recieved" << std::endl);

        // get the incomming transaction
        pl = m_peq.get_next_transaction();

        // process the payload
        assert(pl->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE);
        switch (pl->get_command()) {
        case tlm::TLM_READ_COMMAND:
            read(pl->get_address(), *((uint32_t *)pl->get_data_ptr()));
            break;
        case tlm::TLM_WRITE_COMMAND:
            write(pl->get_address(), *((uint32_t *)pl->get_data_ptr()));
            break;
        default:
            std::cout << name() << "command not supported." << std::endl;
            pl->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
            break;
        }
        pl->set_response_status(tlm::TLM_OK_RESPONSE);

        // send response on backward path
        status = target->nb_transport_bw(*pl, phase, delay);
        if (status != tlm::TLM_COMPLETED) {
            std::cerr << name() << ": backward path failure." << std::endl;
            abort();
        }

    }

}