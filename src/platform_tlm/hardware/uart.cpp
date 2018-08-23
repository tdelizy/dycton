/********************************************************************
 * Copyright (C) 2012 by Verimag                                    *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

/*!
  \file uart.cpp
  \brief body for the file uart.h


*/

#include "uart.h"
#include "offsets/uart.h"
#include "../iss/soclib_endian.h"

#include "../log_trace.h"
#define DBG(x) DY_DBG(x, PER_ID, UART_SUB_ID)

#define USR_TXT_ON 		"\033[33m"
#define USR_TXT_OFF 	"\033[0m"

#define DBG_TXT_ON      "\033[31m"
#define DBG_TXT_OFF     "\033[0m"

uint32_t colors_in_uart_output = 1;

UART::UART(sc_core::sc_module_name name) : sc_core::sc_module(name), m_peq("m_peq")
{
    target.register_b_transport(this, &UART::b_transport);
    target.register_nb_transport_fw(this, &UART::nb_transport_fw);
    SC_THREAD(process);
}

tlm::tlm_response_status UART::write(uint32_t a, uint32_t d) {
    char c = uint32_be_to_machine(d);
    DBG(std::cout << "[" << name() << "] write( a = " << a << ", d = " << d << ")" << std::endl);
    if (colors_in_uart_output != 0) {
        switch (a) {
        case UART_STR_FIFO_WRITE:
            if (c == '\n') {
                std::cout << std::endl;
            } else {
                std::cout << USR_TXT_ON << c << USR_TXT_OFF;
            }
            break;
        case UART_STR_FIFO_WRITE_DBG:
            if (c == '\n') {
                std::cout << std::endl;
            } else {
                std::cout << DBG_TXT_ON << c << DBG_TXT_OFF;
            }
            break;
        case UART_DEC_FIFO_WRITE:
            std::cout << USR_TXT_ON << std::dec << uint32_be_to_machine(d) << std::hex << USR_TXT_OFF;
            break;
        case UART_DEC_FIFO_WRITE_DBG:
            std::cout << DBG_TXT_ON << std::dec << uint32_be_to_machine(d) << std::hex << DBG_TXT_OFF;
            break;
        default:
            SC_REPORT_ERROR(name(), "write register not implemented");
            return tlm::TLM_ADDRESS_ERROR_RESPONSE;
        }
    } else {
        switch (a) {
        case UART_STR_FIFO_WRITE:
        case UART_STR_FIFO_WRITE_DBG:
            if (c == '\n') {
                std::cout << std::endl;
            } else {
                std::cout << c;
            }
            break;
        case UART_DEC_FIFO_WRITE:
        case UART_DEC_FIFO_WRITE_DBG:
            std::cout << std::dec << uint32_be_to_machine(d)<< std::hex;
            break;
        default:
            SC_REPORT_ERROR(name(), "write register not implemented");
            return tlm::TLM_ADDRESS_ERROR_RESPONSE;
        }
    }
    wait(sc_core::SC_ZERO_TIME);
    return tlm::TLM_OK_RESPONSE;
}

tlm::tlm_response_status UART::read(uint32_t a, uint32_t & d) {
	(void)a;
	(void)d;
	SC_REPORT_ERROR(name(), "read not implemented");
	return tlm::TLM_ADDRESS_ERROR_RESPONSE;
}


void UART::b_transport(tlm::tlm_generic_payload &pl, sc_core::sc_time &delay_time)
{
	assert(pl.get_response_status()==tlm::TLM_INCOMPLETE_RESPONSE);
	if (pl.get_byte_enable_ptr()) {
		std::cerr << name() << "byte enable not supported."<< std::endl;
		pl.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
	} else {
		switch (pl.get_command()) {
		default:
			std::cout << name() <<"command not supported."<< std::endl;
			pl.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		return;
		case tlm::TLM_WRITE_COMMAND:
			write(pl.get_address(), *((uint32_t *)pl.get_data_ptr()));
		break;
	}
	pl.set_response_status(tlm::TLM_OK_RESPONSE);
	}
}

tlm::tlm_sync_enum UART::nb_transport_fw(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
    DBG(std::cout << std::dec << "[" << name() << "] nb_transport_fw" << std::endl);
    m_peq.notify(pl);
    return tlm::TLM_ACCEPTED;
}

void UART::process()
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

        // process the payload
        assert(pl->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE);
        switch (pl->get_command()) {
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