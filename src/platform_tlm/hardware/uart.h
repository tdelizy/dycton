
/********************************************************************
 * Copyright (C) 2009, 2012 by Verimag                              *
 * Initial author: Matthieu Moy                                     *
 *                                                                  *
 * Inclusion and adaptation : Tristan Delizy, 2019                  *
 ********************************************************************/

/*!
  \file uart.h
  \brief UART module


*/
#ifndef UART_H
#define UART_H

#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/peq_with_get.h"

SC_MODULE(UART)
{
public:
    tlm_utils::simple_target_socket<UART> target;

    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_peq;

    void b_transport(tlm::tlm_generic_payload & pl, sc_core::sc_time & delay_time);
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload & pl, tlm::tlm_phase & phase, sc_core::sc_time & delay);

    tlm::tlm_response_status read(uint32_t a, uint32_t & d);
    tlm::tlm_response_status write(uint32_t a, uint32_t d);

    SC_HAS_PROCESS(UART);
    UART(sc_core::sc_module_name name);

    void process();

private:
};

#endif // UART_H
