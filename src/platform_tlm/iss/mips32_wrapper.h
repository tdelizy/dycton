/********************************************************************
 * project DYCTON 2017 - CITI Lab                                   *
 * Author: Tristan Delizy                                           *
 * Based on code from Matthieu Moy - Verimag 2009, 2012             *
 *******************************************************************/


#ifndef MIPS32_WRAPPER_H
#define MIPS32_WRAPPER_H

#include "mips32.h"
#include "iss2.h"
#include "soclib_endian.h"
#include "arithmetics.h"
#include "tlm_utils/peq_with_get.h"
#include "tlm_utils/simple_initiator_socket.h"

/*!
  Wrapper for MicroBlaze ISS using the BASIC protocol.
*/
struct MIPS32Wrapper : sc_core::sc_module {
  tlm_utils::simple_initiator_socket<MIPS32Wrapper> i_socket;
  tlm_utils::simple_initiator_socket<MIPS32Wrapper> d_socket;
  tlm_utils::simple_initiator_socket<MIPS32Wrapper> b_socket;
  sc_core::sc_in<bool> irq;

protected:
  SC_HAS_PROCESS(MIPS32Wrapper);

public:
  MIPS32Wrapper(  sc_core::sc_module_name name,
                  uint32_t ident,
                  uint32_t code_start,
                  uint32_t code_size,
                  uint32_t data_start,
                  uint32_t data_size,
                  uint32_t gp_start,
                  uint32_t spm_size);

  uint64_t get_cycles(void);

private:
  int irq_duration;
  uint32_t m_code_start;
  uint32_t m_code_size;
  uint32_t m_data_start;
  uint32_t m_data_size;
  uint32_t m_gp_start;
  uint32_t m_spm_size;

  uint64_t m_cycle_count;

  tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_i_peq;
  tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_d_peq;
  typedef soclib::common::Mips32ElIss iss_t;

  void run_iss(void);
  void run_sim(void);
  void process_irq(void);

  void request_fetch(uint32_t &addr);
  void request_read_data(uint32_t &addr, soclib::common::Iss2::be_t be = 0);
  void request_write_data(uint32_t &addr, uint32_t &data, soclib::common::Iss2::be_t be = 0);
  void process_resp(tlm::tlm_generic_payload* pl, char* buffer);

  tlm::tlm_sync_enum nb_transport_bw_i(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);
  tlm::tlm_sync_enum nb_transport_bw_d(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);
  tlm::tlm_sync_enum nb_transport_bw_b(tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_core::sc_time& delay);

  iss_t m_iss;
};

#endif // MIPS32_WRAPPER_H
