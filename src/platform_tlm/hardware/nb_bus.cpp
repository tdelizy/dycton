//
// SimSoC Initial software, Copyright © INRIA 2007, 2008, 2009, 2010, 2011
// LGPL license version 3
//

// Adapted for LIBTLMPWT (Verimag/CNRS, 2012, 2013), by Claude Helmstetter

// Adapted for Dycton - CITI Lab INSA/INRIA 2017 - Tristan Delizy


#include "nb_bus.h"
#include <algorithm>
#include "../log_trace.h"

#define DBG(x) DY_DBG(x, BUS_ID, NULL_SUB_ID)

using namespace sc_core;
using namespace tlm;


NbBus::NbBus(sc_module_name name):
  target_sockets("target_sockets"),
  initiator_sockets("initiator_sockets"),
  intervals()
{
    DBG(std::cout << "[nb_bus]:bus init "<< std::endl);
  target_sockets.register_b_transport(this, &NbBus::b_transport);
  target_sockets.register_nb_transport_fw(this, &NbBus::nb_transport_fw);
  target_sockets.register_transport_dbg(this, &NbBus::transport_dbg);
  initiator_sockets.register_nb_transport_bw(this, &NbBus::nb_transport_bw);
}


void NbBus::before_end_of_elaboration() {
  // sort intervals
  std::sort(intervals.begin(),intervals.end());
  // initialize "previous" iterator
  previous = intervals.begin();
  // check intervals for overlaps
  for (size_t i = 1, ei = intervals.size(); i!=ei; ++i)
    if (intervals[i].last>=intervals[i-1].addr) {
      print_mmap();
      // ERROR("at least two intervals overlap in memory map.");
    }
  // if (trace_level.get()>=1)
  //   print_mmap();
}


void NbBus::bind_target(base_target_socket_type &port, uint64_t addr, uint64_t size)
{
  assert(addr<=addr+size-1);
  size_t n = intervals.size();
  assert((size_t)initiator_sockets.size()==n);
  initiator_sockets(port);
  intervals.push_back(Interval(n,addr,size));
}


void NbBus::b_transport(int id, tlm::tlm_generic_payload& pl, sc_time& delay)
{
    DBG(std::cout << "[" << name() << "]:call to b_transport "<< std::endl);
  assert(pl.get_response_status()==TLM_INCOMPLETE_RESPONSE);
  uint64_t addr = pl.get_address();
  size_t index;
  if (resolve(addr,index)) {
    pl.set_address(addr);
    initiator_sockets[index]->b_transport(pl, delay);
  } else {
    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
  }
}


tlm::tlm_sync_enum NbBus::nb_transport_fw( int id,
                                           tlm::tlm_generic_payload& pl,
                                           tlm::tlm_phase& phase,
                                           sc_time& delay)
{
    DBG(std::cout << "[" << name() << "]:call to nb_transport_fw (" << std::hex << pl.get_address() << ")" << std::endl);
    uint64_t addr = pl.get_address();
    size_t index;
    if (resolve(addr, index)) {
        route_extension* ext = 0;
        if (phase == tlm::BEGIN_REQ) {
            ext = new route_extension;
            ext->id = id;
            accessor(pl).set_extension(ext);
        }
        pl.set_address(addr);
        tlm::tlm_sync_enum status;
        status = initiator_sockets[index]->nb_transport_fw(pl, phase, delay);
        if (status == tlm::TLM_COMPLETED) {
            accessor(pl).clear_extension(ext);
            delete ext;
        }
        return status;
    } else {
        std::cerr << "[" << name() << "]:can't resolve address 0x" << std::hex << addr << std::endl;
        print_mmap(std::cerr);
        pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
        return TLM_COMPLETED;
    }
}


tlm::tlm_sync_enum NbBus::nb_transport_bw(int id, tlm::tlm_generic_payload& pl, tlm::tlm_phase& phase, sc_time& delay)
{
    DBG(std::cout << "[" << name() << "]:call to nb_transport_bw (addr=" << std::hex << pl.get_address() << ", id="<< id << ")" << std::endl);
    route_extension* ext = 0;
    accessor(pl).get_extension(ext);

    tlm::tlm_sync_enum status;
    uint64_t addr = 0;

    std::vector<NbBus::Interval>::iterator i = intervals.begin();

    while(i->index != (unsigned int)id && i != intervals.end()){
        i++;
    }
    addr = pl.get_address();
    addr += i->addr;
    // std::cout << "[" << name() << "] addr recovery (addr=" << std::hex << addr << ", index="<< i->index << ")" << std::endl;
    pl.set_address(addr);

    status = target_sockets[ext->id]->nb_transport_bw(pl, phase, delay);
    accessor(pl).clear_extension(ext);
    delete ext;
    return status;
}


unsigned int NbBus::transport_dbg(int id, tlm::tlm_generic_payload& pl)
{
  assert(pl.get_response_status()==TLM_INCOMPLETE_RESPONSE);
  uint64_t addr = pl.get_address();
  size_t index;
  if (resolve(addr,index)) {
    pl.set_address(addr);
    const unsigned int reply =
      initiator_sockets[index]->transport_dbg(pl);
    // INFO(3, "transport_dbg[" << reply << "]: " << pl);
    return reply;
  } else {
    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
    // INFO(3, "transport_dbg[0]: " << pl);
    return 0;
  }
}


void NbBus::print_mmap(std::ostream &os) {
  for (std::vector<NbBus::Interval>::iterator i = intervals.begin(), ei = intervals.end(); i!=ei; ++i) {
    os << "[" << name() << "]" << ": interval ["
       <<std::hex <<i->addr <<',' <<i->last+1
       <<") mapped to port #" << std::dec <<i->index;
    const sc_object *obj =
      dynamic_cast<const sc_object*>(initiator_sockets[i->index]);
    if (obj)
      os <<" \"" << obj->name() <<"\"";
    os <<std::endl;
  }
}


// return true if a target is found at this address
bool NbBus::resolve(uint64_t &addr, size_t &index)
{
  if (previous->addr<=addr && addr<=previous->last) {
    addr -= previous->addr;
    index = previous->index;
    return true;
  }
  std::vector<NbBus::Interval>::iterator i =
    lower_bound(intervals.begin(),
                intervals.end(),
                Interval(addr));
  if (i==intervals.end() || i->last<addr) {
    return false;
  } else {
    previous = i;
    addr -= i->addr;
    index = i->index;
    return true;
  }
}


// return true if a target is found at this address
bool NbBus::resolve(uint64_t &addr, size_t &index, uint64_t &base) {
  std::vector<NbBus::Interval>::iterator i =
    lower_bound(intervals.begin(),
                intervals.end(),
                Interval(addr));
  if (i==intervals.end() || i->last<addr) {
    return false;
  } else {
    addr -= i->addr;
    index = i->index;
    base = i->addr;
    return true;
  }
}