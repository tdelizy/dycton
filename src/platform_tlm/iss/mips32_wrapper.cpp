/********************************************************************
 * project DYCTON 2017 - CITI Lab                                   *
 * Author: Tristan Delizy                                           *
 * Based on code from Matthieu Moy - Verimag 2009, 2012             *
 *******************************************************************/


#include "mips32_wrapper.h"
#include <iomanip>
#include <systemc.h>
#include "../address_map.h"
#include "../platform_time.h"
#include "../log_trace.h"

#define DBG(x) DY_DBG(x, WRAPPER_ID, NULL_SUB_ID)

static uint32_t bus_ireq_addr = 0xFFFFFFFF;
static uint32_t bus_dreq_read_addr = 0xFFFFFFFF;
static uint32_t bus_dreq_write_addr = 0xFFFFFFFF;

static struct soclib::common::Iss2::InstructionRequest i_req;
static struct soclib::common::Iss2::DataRequest d_req;

static sc_core::sc_event iss_req_trans_event;
static bool sim_d_resp_rdy = false;
static bool sim_i_resp_rdy = false;

const sc_core::sc_time PLATFORM_CYCLE = sc_core::sc_time (2500, sc_core::SC_PS);
const uint64_t PROC_FREQ = (uint64_t)(1.0/(PLATFORM_CYCLE.to_seconds()));


MIPS32Wrapper::MIPS32Wrapper( sc_core::sc_module_name name,
                              uint32_t ident,
                              uint32_t code_start,
                              uint32_t code_size,
                              uint32_t data_start,
                              uint32_t data_size,
                              uint32_t gp_start,
                              uint32_t spm_size)
                              : sc_core::sc_module(name),
                                irq("irq"),
                                m_code_start(code_start),
                                m_code_size(code_size),
                                m_data_start(data_start),
                                m_data_size(data_size),
                                m_gp_start(gp_start),
                                m_spm_size(spm_size),
                                m_i_peq("wrapper_i_peq"),
                                m_d_peq("wrapper_d_peq"),
                                m_iss((const char *)name, ident)
{
    DBG(std::cout << hex << "[" << name << "] instantiation"  << std::endl);
    DBG(std::cout << hex << "\tm_code_start : 0x" << m_code_start  << std::endl);
    DBG(std::cout << hex << "\tm_code_size : 0x" << m_code_size  << std::endl);
    DBG(std::cout << hex << "\tm_data_start : 0x" << m_data_start  << std::endl);
    DBG(std::cout << hex << "\tm_data_size : 0x" << m_data_size  << std::endl);
    DBG(std::cout << hex << "\tm_gp_start : 0x" << m_gp_start  << std::endl);
    DBG(std::cout << hex << "\tm_spm_size : 0x" << m_spm_size  << std::endl);

    m_iss.reset();
    m_cycle_count = 0;

    SC_THREAD(run_iss);
    SC_THREAD(run_sim);
    SC_METHOD(process_irq);
    dont_initialize();
    sensitive << irq;

    iss_t::CacheInfo cache_info;

    cache_info.has_mmu = false;
    cache_info.icache_line_size = 0;
    cache_info.icache_assoc = 0;
    cache_info.icache_n_lines = 0;
    cache_info.dcache_line_size = 0;
    cache_info.dcache_assoc = 0;
    cache_info.dcache_n_lines = 0;
    m_iss.setCacheInfo(cache_info);

  // verifying accessibility of the scratch pad memory
    if (m_code_start + spm_size >= data_start)
        assert(0 || "instruction access port overlaps with data access port");
    if (m_data_start + spm_size >= gp_start)
        assert(0 || "data access port overlaps with general purpose access port");

    i_socket.register_nb_transport_bw(this, &MIPS32Wrapper::nb_transport_bw_i);
    d_socket.register_nb_transport_bw(this, &MIPS32Wrapper::nb_transport_bw_d);
    b_socket.register_nb_transport_bw(this, &MIPS32Wrapper::nb_transport_bw_b);

}

uint64_t MIPS32Wrapper::get_cycles(void){
    return m_cycle_count;
}

void MIPS32Wrapper::process_irq(void) {
        std::cerr << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] process_irq sc_method called AND SHOULD NOT... dump & abort" << std::endl;
        m_iss.dump();
        std::cout << std::endl;
        abort();
    DBG(std::cout << "[" << name() << "]: IRQ signal changed to " << irq.read() << std::endl);
    if (irq.read()) {
        irq_duration = 10;
    }
}


void MIPS32Wrapper::request_fetch(uint32_t &addr)
{
    tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    int* data_ptr;
    unsigned char* byte_enable_ptr;
    tlm::tlm_sync_enum status;
    int buf = addr;

    pl = new tlm::tlm_generic_payload(); // /!\ we should/could not allocate and deallocate payload each time
    data_ptr = new int(0);
    byte_enable_ptr = new unsigned char(0);

    *byte_enable_ptr = 0xF; // fetch will always read 4 Bytes

    pl->set_command(tlm::TLM_READ_COMMAND);
    pl->set_data_ptr(reinterpret_cast<unsigned char*>(data_ptr));
    pl->set_data_length(4); // to be adapted w/ byte enable
    pl->set_streaming_width(4); // = data_length to indicate no streaming
    pl->set_byte_enable_ptr(byte_enable_ptr);
    pl->set_dmi_allowed(false); // Mandatory initial value
    pl->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    phase = tlm::BEGIN_REQ;
    delay = sc_core::sc_time(0, sc_core::SC_PS);



    // dispatch between SPM and BUS
    if(addr >= m_code_start && addr < (m_code_start + m_spm_size)){
        pl->set_address(addr - m_code_start);
        DBG(std::cout << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] request_fetch (i) (addr=" << hex << addr-m_code_start << ")" << std::dec << std::endl);
        status = i_socket->nb_transport_fw(*pl, phase, delay);
    } else {
        pl->set_address(addr);
        bus_ireq_addr = addr;
        DBG(std::cout << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] request_fetch (b) (addr=" << hex << addr << ")" << std::dec << std::endl);
        status = b_socket->nb_transport_fw(*pl, phase, delay);
    }
    if (status != tlm::TLM_ACCEPTED) {
        std::cerr << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] instruction request error (addr=" << hex << buf << ")" << std::dec << std::endl;
        abort();
    }
}


void MIPS32Wrapper::request_read_data(uint32_t &addr, soclib::common::Iss2::be_t be)
{
    tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    int* data_ptr;
    unsigned char* byte_enable_ptr;
    tlm::tlm_sync_enum status;
    int buf = addr;

    pl = new tlm::tlm_generic_payload(); // /!\ we should/could not allocate and deallocate payload each time
    data_ptr = new int(0);
    byte_enable_ptr = new unsigned char(0);

    *byte_enable_ptr = be;

    pl->set_command(tlm::TLM_READ_COMMAND);
    pl->set_data_ptr(reinterpret_cast<unsigned char*>(data_ptr));
    pl->set_data_length(4);
    pl->set_streaming_width(4); // = data_length to indicate no streaming
    pl->set_byte_enable_ptr(byte_enable_ptr);
    pl->set_dmi_allowed(false); // Mandatory initial value
    pl->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    phase = tlm::BEGIN_REQ;
    delay = sc_core::sc_time(0, sc_core::SC_PS);

    // dispatch between SPM and BUS
    // ! cannot access code from this interface from now ?
    if(addr >= m_data_start && addr < m_data_start + m_spm_size){
        pl->set_address(addr - m_data_start + m_code_size);
        DBG(std::cout << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] request_data_read (d) (addr=" << hex << addr << ")" << std::dec << std::endl);
        status = d_socket->nb_transport_fw(*pl, phase, delay);
    } else {
        pl->set_address(addr);
        bus_dreq_read_addr = addr;
        DBG(std::cout << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] request_data_read (b) (addr=" << hex << addr << ")" << std::dec << std::endl);
        status = b_socket->nb_transport_fw(*pl, phase, delay);
    }
    if (status != tlm::TLM_ACCEPTED) {
        std::cerr << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] data read error (addr=" << hex << buf << ")" << std::dec << std::endl;
        m_iss.dump();
        std::cout << std::endl;
        abort();
    }
}


void MIPS32Wrapper::request_write_data(uint32_t &addr, uint32_t &data, soclib::common::Iss2::be_t be)
{
    tlm::tlm_generic_payload* pl;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    int* data_ptr;
    unsigned char* byte_enable_ptr;
    tlm::tlm_sync_enum status;
    int buf = addr;

    if(addr == HELPER_BASE + TIMED_EVENT)
        m_iss.dump();

    pl = new tlm::tlm_generic_payload(); // /!\ we should not allocate and deallocate payload each time
    data_ptr = new int(0);
    byte_enable_ptr = new unsigned char(0);

    *data_ptr = data;
    *byte_enable_ptr = be;

    pl->set_command(tlm::TLM_WRITE_COMMAND);
    pl->set_address(addr);
    pl->set_data_ptr(reinterpret_cast<unsigned char*>(data_ptr));
    pl->set_data_length(4);
    pl->set_streaming_width(4); // = data_length to indicate no streaming
    pl->set_byte_enable_ptr(byte_enable_ptr);
    pl->set_dmi_allowed(false); // Mandatory initial value
    pl->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    phase = tlm::BEGIN_REQ;
    delay = sc_core::sc_time(0, sc_core::SC_PS);

    // dispatch between SPM and BUS
    // ! cannot access code from this interface from now ?
    if(addr >= m_data_start && addr < m_data_start + m_spm_size){
        pl->set_address(addr - m_data_start + m_code_size);
        DBG(std::cout << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] request_data_write (d) (addr=" << hex << addr << ")" << std::dec << std::endl);
        status = d_socket->nb_transport_fw(*pl, phase, delay);
    } else {
        pl->set_address(addr);
        bus_dreq_write_addr = addr;
        DBG(std::cout << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] request_data_write (b) (addr=" << hex << addr << ")" << std::dec << std::endl);
        status = b_socket->nb_transport_fw(*pl, phase, delay);
    }
    if (status != tlm::TLM_ACCEPTED) {
        std::cerr << "[" << name() << "] at [" << sc_time_stamp().to_seconds()
        << "] data write error (addr=" << hex << buf << ")" << std::dec << std::endl;
        m_iss.dump();
        std::cout << std::endl;
        abort();
    }
}



// we assume there that the buffer is of size pl.get_data_length
void MIPS32Wrapper::process_resp(tlm::tlm_generic_payload* pl, char* buffer)
{
    assert(pl->get_response_status() == tlm::TLM_OK_RESPONSE);
    if(buffer){
        memcpy(buffer,pl->get_data_ptr(),pl->get_data_length());
    } else if (pl->get_command() != tlm::TLM_WRITE_COMMAND){
        std::cerr << name() << ": NULL buffer in function call process_resp." << std::endl;
        abort();
    }
    delete pl->get_data_ptr();
    delete pl->get_byte_enable_ptr();
    delete pl;
}


// TLM-2 backward non-blocking transport method
tlm::tlm_sync_enum MIPS32Wrapper::nb_transport_bw_i(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_time& delay)
{
    uint32_t addr = pl.get_address();
    DBG(std::cout <<  "notif(i) [FETCH]" << std::hex << addr << std::endl);
    m_i_peq.notify(pl);
    return tlm::TLM_COMPLETED;
}


tlm::tlm_sync_enum MIPS32Wrapper::nb_transport_bw_d(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_time& delay)
{
    uint32_t addr = pl.get_address();
    DBG(std::cout << "notif(d) [DATA]" << std::hex << addr << std::endl);
    m_d_peq.notify(pl);
    return tlm::TLM_COMPLETED;
}


tlm::tlm_sync_enum MIPS32Wrapper::nb_transport_bw_b(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_time& delay)
{
    uint32_t addr = pl.get_address();

    // the dispatching between fetch and data from bus may be done via payload extention ? (this feels not safe)
    // ===> this is bad : it forces the interconnects to reconstruct original @ on bw path
    if((bus_ireq_addr) == (addr)){
        DBG(std::cout <<  "notif(i) [BUS]" << std::hex << addr << std::endl);
        m_i_peq.notify(pl);
        bus_ireq_addr = 0xFFFFFFFF;
    } else if ((bus_dreq_read_addr) == (addr)){
        DBG(std::cout <<  "notif(d) [BUS]" << std::hex << addr << std::endl);
        m_d_peq.notify(pl);
        bus_dreq_read_addr = 0xFFFFFFFF;
    } else if ((bus_dreq_write_addr) == (addr)){
        DBG(std::cout <<  "notif(d) [BUS]" << std::hex << addr << std::endl);
        m_d_peq.notify(pl);
        bus_dreq_write_addr = 0xFFFFFFFF;
    } else {
        std::cerr <<  "nb_transport_bw_b error : " << std::hex << addr << " || i= " <<  bus_ireq_addr << ", d_r= "<< bus_dreq_read_addr<< ", d_w= "<< bus_dreq_write_addr <<std::endl;
        abort();
    }
    return tlm::TLM_COMPLETED;
}

// sc_thread for simulator memory access generation
void MIPS32Wrapper::run_sim(void)
{
    uint32_t local_d_buf = 0;
    tlm::tlm_generic_payload* pl;
    bool data_write_in_progress = false;

    while(true){
        //--------------------------------------------------------------------------
        // wait that the ISS generates memory request
        wait(iss_req_trans_event);
        DBG(std::cout << "[run_sim] at [" << sc_time_stamp().to_seconds()
                  << "] iss_req_trans_event notified"<< std::endl);

        //--------------------------------------------------------------------------
        // generates the memory transactions
        if (i_req.valid) {
            DBG(std::cout << "[run_sim] at [" << sc_time_stamp().to_seconds()
            << "] run_sim instruction read transaction generation (addr=" << hex << i_req.addr << ")" << std::dec << std::endl);
            request_fetch(i_req.addr);
        }
        if (d_req.valid) {
            /* The ISS requested a memory access.*/
            /* First wait data interface to complete previous write if any*/
            if (data_write_in_progress){
                pl = m_d_peq.get_next_transaction(); // check we have not recieved the response before waiting
                if(pl == NULL){
                    DBG(std::cout <<  "[run_sim] d resp not recieved yet, waiting " << std::endl);
                    wait(m_d_peq.get_event()); // wait for data write response response (the moment the data memory can process a new request)
                    DBG(std::cout <<  "[run_sim] got it " << std::endl);
                    pl = m_d_peq.get_next_transaction();
                }
                process_resp(pl, NULL);
                data_write_in_progress = false;
            }
            switch (d_req.type) {
            case soclib::common::Iss2::DATA_READ:
                DBG(std::cout << "[run_sim] at [" << sc_time_stamp().to_seconds()
                << "] run_sim data read transaction generation (addr=" << hex << d_req.addr << ")" << std::dec << std::endl);
                request_read_data(d_req.addr, d_req.be);
                break;
            case soclib::common::Iss2::DATA_WRITE:
                DBG(std::cout << "[run_sim] at [" << sc_time_stamp().to_seconds()
                << "] run_sim data write transaction generation (addr=" << hex << d_req.addr << ", data=" << d_req.wdata
                << ")" << std::dec << std::endl);
                local_d_buf = uint32_be_to_machine(d_req.wdata);
                request_write_data(d_req.addr, local_d_buf, d_req.be);
                break;
            default:
                std::cerr << name() << ": run sim - data request not supported." << std::endl;
                abort();
                break;
            }
        }

    //--------------------------------------------------------------------------
    // wait responses, decouplate write transaction response from write request response.
       if(i_req.valid && d_req.valid){
            DBG(std::cout <<  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>(i&d) " << std::endl);
            if(d_req.type == soclib::common::Iss2::DATA_WRITE){
                sim_d_resp_rdy = true; // done before waiting actual response to decouplate memory write from request generation
                pl = m_i_peq.get_next_transaction(); // check we have not recieved the response before waiting (data write decoupling)
                if(pl == NULL){
                    wait(m_i_peq.get_event()); // wait for instruction response
                } else {
                    m_i_peq.notify(*pl);
                }
                // notify iss after fetch return, but wait for data response to generate next memory transaction
                sim_i_resp_rdy = true;
                sim_d_resp_rdy = true;
                data_write_in_progress = true;
            } else {
                pl = m_i_peq.get_next_transaction(); // check we have not recieved the response before waiting (data write decoupling)
                if(pl == NULL){
                    wait(m_i_peq.get_event() & m_d_peq.get_event()); // wait responses (i & d) from memory
                } else {
                    m_i_peq.notify(*pl);
                    wait(m_d_peq.get_event()); // wait responses (i & d) from memory
                }
                // the iss can now get the responses
                sim_d_resp_rdy = true;
                sim_i_resp_rdy = true;
            }
            DBG(std::cout <<  "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<(i&d) " << std::endl);
        } else if(i_req.valid) {
            DBG(std::cout <<  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>(i) " << std::endl);
            pl = m_i_peq.get_next_transaction(); // check we have not recieved the response before waiting (data write decoupling)
            if(pl == NULL){
                wait(m_i_peq.get_event()); // wait for instruction response
            } else {
                m_i_peq.notify(*pl);
            }
            sim_i_resp_rdy = true; // the i response can be used by the iss
            DBG(std::cout <<  "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<(i) " << std::endl);
        } else if(d_req.valid) {
            DBG(std::cout <<  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>(d) " << std::endl);
            if(d_req.type == soclib::common::Iss2::DATA_WRITE){
                sim_d_resp_rdy = true; // done before waiting actual response to decouplate memory write from request generation
            }
            wait(m_d_peq.get_event()); // wait for (actual) data response
            if(d_req.type == soclib::common::Iss2::DATA_READ){
                sim_d_resp_rdy = true; // the d read response can be used by the iss
            }
            DBG(std::cout <<  "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<(d) " << std::endl);
        }
    }
}



// main sc_thread of the wrapper for ISS encapsulation
void MIPS32Wrapper::run_iss(void)
{
    tlm::tlm_generic_payload* pl;

    struct soclib::common::Iss2::InstructionResponse i_resp;
    struct soclib::common::Iss2::DataResponse d_resp ;

    uint32_t i_req_addr_buf;
    struct soclib::common::Iss2::InstructionResponse i_resp_buf;

    uint32_t local_i_buf = 0;
    uint32_t local_d_buf = 0;

    uint32_t it = 0;

    bool rsp_proceed = true;

    wait(sc_core::sc_time(1, sc_core::SC_PS)); //init

    i_req.valid = false;
    i_req.addr = 0;

    d_req.valid = false;
    d_req.addr = 0;

    while (true) {

        if(!i_req.valid && !d_req.valid){
                DBG(std::cout << "[ run_iss] at [" << sc_time_stamp().to_seconds()
                << "] ISS getRequests" << std::dec << std::endl);
            // save the previous fetch target adress
            i_req_addr_buf = i_req.addr;
            // get the request from the ISS
            m_iss.getRequests(i_req, d_req);

            // std::cout << std::dec << (int)(((int)(d_req.be & (1<<0)))?1:0)<< (int)(((int)(d_req.be & (1<<1)))?1:0) << (int)(((int)(d_req.be & (1<<2)))?1:0) << (int)(((int)(d_req.be & (1<<3)))?1:0) << (int)(((int)(d_req.be & (1<<4)))?1:0) << (int)(((int)(d_req.be & (1<<5)))?1:0) << (int)(((int)(d_req.be & (1<<6)))?1:0) << (int)(((int)(d_req.be & (1<<7)))?1:0) << std::endl;

            /* bug fix : after some fetches, if the executeNCycles method is called for 1 internal iss cycle
            * the instruction is fetched 2 times, we then don't want to generate associated memory request
            * the problem only observed for "branch" instructions */
            if (i_req.valid && !d_req.valid) {
                if (i_req.addr == i_req_addr_buf) {
                DBG(std::cout << "[ run_iss] at [" << sc_time_stamp().to_seconds()
                << "] duplication instr. bug occured" << hex << i_req.addr << ")" << std::dec << std::endl);
                    // provide the ISS with last instruction fetched and invalid data response.
                    m_iss.executeNCycles(1, i_resp_buf, d_resp, it);
                    i_req.valid = false;
                    d_req.valid = false;
                    continue; // don't do anything else, skip to next processor requests
                }
            }

            i_resp.valid = false;
            d_resp.valid = false;
            sim_i_resp_rdy = false;
            sim_d_resp_rdy = false;
            rsp_proceed = false;

            if(i_req.valid || d_req.valid){
                iss_req_trans_event.notify(); // generates the memory transactions
            }

        }


        if(!rsp_proceed){
            if(i_req.valid && d_req.valid){
                if(sim_i_resp_rdy && sim_d_resp_rdy){
                DBG(std::cout << "[ run_iss] at [" << sc_time_stamp().to_seconds()
                << "] processing instruction and data response." << std::dec << std::endl);
                    // process instruction response
                    pl = m_i_peq.get_next_transaction();
                    process_resp(pl, (char*)&local_i_buf);
                    local_i_buf = uint32_machine_to_be(local_i_buf);
                    i_resp.valid = true;
                    i_resp.error = false;
                    i_resp.instruction = local_i_buf;
                    i_resp_buf.valid = i_resp.valid;
                    i_resp_buf.error = i_resp.error;
                    i_resp_buf.instruction = i_resp.instruction;

                    // process data response
                    if(d_req.type==soclib::common::Iss2::DATA_READ){
                        pl = m_d_peq.get_next_transaction();
                        process_resp(pl, (char*)&local_d_buf);
                        local_d_buf = uint32_machine_to_be(local_d_buf);
                        d_resp.rdata = local_d_buf;
                    }
                    d_resp.valid = true;
                    d_resp.error = false;
                    rsp_proceed = true; // proceeding response only once
                }
            } else if(i_req.valid){
                if(sim_i_resp_rdy){
                DBG(std::cout << "[ run_iss] at [" << sc_time_stamp().to_seconds()
                << "] processing instruction response." << std::dec << std::endl);
                    pl = m_i_peq.get_next_transaction();
                    process_resp(pl, (char*)&local_i_buf);
                    local_i_buf = uint32_machine_to_be(local_i_buf);
                    i_resp.valid = true;
                    i_resp.error = false;
                    i_resp.instruction = local_i_buf;
                    i_resp_buf.valid = i_resp.valid;
                    i_resp_buf.error = i_resp.error;
                    i_resp_buf.instruction = i_resp.instruction;
                    rsp_proceed = true; // proceeding response only once
                }
            } else if(d_req.valid){
                if(sim_d_resp_rdy){
                DBG(std::cout << "[ run_iss] at [" << sc_time_stamp().to_seconds()
                << "] processing data response." << std::dec << std::endl);
                    if(d_req.type==soclib::common::Iss2::DATA_READ){
                        pl = m_d_peq.get_next_transaction();
                        process_resp(pl, (char*)&local_d_buf);
                        local_d_buf = uint32_machine_to_be(local_d_buf);
                        d_resp.rdata = local_d_buf;
                    }
                    d_resp.valid = true;
                    d_resp.error = false;
                    rsp_proceed = true ;// proceeding response only once
                }
            }
        }

        // responses has been proceeded and will be transmitted
        // to the iss so next requests could possibly be ready next cycle
        // if not, the iss will return invalid requests until it is ready
        if(rsp_proceed){
            i_req.valid = false;
            d_req.valid = false;
        }

        // get the response to the ISS
        // if ( irq.read() ) it |= (1 << 0); //not used

        // std::cout << std::endl;
        // m_iss.dump();
        // std::cout << std::endl;

        // run the iss for one of its internal cycle
        m_iss.executeNCycles(1, i_resp, d_resp, it);

        // make time go forward for the equivalent time
        m_cycle_count++;
        wait(PLATFORM_CYCLE);
    }
}
