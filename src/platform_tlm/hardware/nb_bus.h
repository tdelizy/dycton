//
// SimSoC Initial software, Copyright © INRIA 2007, 2008, 2009, 2010, 2011
// LGPL license version 3
//

// Adapted for LIBTLMPWT (Verimag/CNRS, 2012, 2013), by Claude Helmstetter

// Adapted for Dycton - CITI Lab INSA/INRIA 2017 - Tristan

#ifndef _NB_BUS_H_
#define _NB_BUS_H_

#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"
#include "tlm_utils/instance_specific_extensions.h"
#include <vector>
#include <stdint.h>

SC_MODULE(NbBus) {

    tlm_utils::multi_passthrough_target_socket<NbBus, 32> target_sockets;
    tlm_utils::multi_passthrough_initiator_socket<NbBus, 32> initiator_sockets;
    typedef tlm_utils::multi_passthrough_initiator_socket<NbBus, 32>::base_target_socket_type base_target_socket_type;

    NbBus(sc_core::sc_module_name name);
    void before_end_of_elaboration(); // prepare the memory map
    void bind_target(base_target_socket_type & port,
                     uint64_t base, uint64_t size);

    void b_transport(int id,
                     tlm::tlm_generic_payload & pl,
                     sc_core::sc_time & delay);

    // instance specific extenion used to identify Non-Blocking
    // transactions
    // We reuse code from
    // TLM-2009-07-15/unit_test/tlm/nb2b_adapter/src/nb2b_adapter.cpp
    struct route_extension:
        tlm_utils::instance_specific_extension<route_extension>
    {
        int id;
    };
    tlm_utils::instance_specific_extension_accessor accessor;

    tlm::tlm_sync_enum nb_transport_fw(int id,
                                       tlm::tlm_generic_payload & pl,
                                       tlm::tlm_phase & phase,
                                       sc_core::sc_time & delay);

    tlm::tlm_sync_enum nb_transport_bw(int id,
                                       tlm::tlm_generic_payload & pl,
                                       tlm::tlm_phase & phase,
                                       sc_core::sc_time & delay);

    unsigned int transport_dbg(int id,
                               tlm::tlm_generic_payload & pl);

    void print_mmap(std::ostream &os = std::cout);


protected:

    struct Interval {
        Interval(size_t i, uint64_t a, uint64_t s):
            index(i), addr(a), last(a + s - 1) {}
        Interval(uint64_t address): addr(address) {}
        bool operator<(const Interval &b) const {return addr > b.addr;}
        size_t index;
        uint64_t addr;
        uint64_t last;
        inline uint64_t size() const {return last + 1 - addr;}
    };

    std::vector<Interval> intervals;
    std::vector<NbBus::Interval>::iterator previous;

    bool resolve(uint64_t &addr, size_t &index);
    bool resolve(uint64_t &addr, size_t &index, uint64_t &base);
};

#endif // _NB_BUS_H_
