#include "../log_trace.h"




// Constructor
PlatformLogSystem::PlatformLogSystem( sc_core::sc_module_name name, std::fstream& log_acc_stream,  std::fstream& log_obj_stream)
    : sc_module(name), m_log_acc_file(log_acc_stream), m_log_obj_file(log_obj_stream)
{
    std::cout << "instantiating logtrace system" << std::endl;
    m_log_acc_file << LOG_COMMENT_STR << MEM_LOG_ACC_HEADERS << std::endl;
    m_log_acc_file << LOG_COMMENT_STR << "temporal_resolution=" << LOG_FLUSH_RESOLUTION_CYCLES << std::endl;
    m_log_acc_file << LOG_COMMENT_STR << "spacial_resolution=" << LOG_ADDR_AGGREG_RESOLUTION << std::endl;

    m_log_obj_file << LOG_COMMENT_STR << MEM_LOG_OBJ_HEADERS << std::endl;

    m_heap_accesses_outside_obj = 0;
    m_invalid_free_calls = 0;
    m_current_heap_action = 0;
    m_r_access_count = 0;
    m_w_access_count = 0;
    // systemC thread declaration
    SC_THREAD(log_process);
}

// Destructor
PlatformLogSystem::~PlatformLogSystem()
{
    m_log_acc_file.close();
}



void PlatformLogSystem::log_process()
{
    // wait every LOG_FLUSH_RESOLUTION_CYCLES cycles and flush aggregated log to this memory.
    while (true) {
        wait(LOG_FLUSH_RESOLUTION_CYCLES * PLATFORM_CYCLE);
        log_flush();
    }
}

void PlatformLogSystem::log_access(uint32_t addr, bool write_op)
{
    // aggregated access log construction
    uint32_t aggregated_ddr = addr - addr % LOG_ADDR_AGGREG_RESOLUTION;
    std::map<uint32_t, AggregatedAccesses*>::iterator it_acc = m_log_acc_map.find(aggregated_ddr);
    if (it_acc == m_log_acc_map.end()) {
        m_log_acc_map[aggregated_ddr] = new AggregatedAccesses(write_op);
    } else {
        write_op ? it_acc->second->w++ : it_acc->second->r++;
    }

    // heap object access monitoring
    // Returns an iterator pointing to the first element that is greater than key.
    std::map<uint32_t, LoggedObj*>::iterator it_obj = m_log_obj_map.upper_bound(addr);
    if(m_log_obj_map.size()==0){
        m_heap_accesses_outside_obj++;
        return;
    }
    unsigned int last_obj_end = m_log_obj_map.rbegin()->first + m_log_obj_map.rbegin()->second->size;
    if ((it_obj != m_log_obj_map.end() && it_obj != m_log_obj_map.begin()) || ((last_obj_end) > addr && it_obj == m_log_obj_map.end())) {
        it_obj = std::prev(it_obj); // we take the previous one
        if (addr < (it_obj->first + it_obj->second->size) && addr >= it_obj->first ) {
            write_op ? it_obj->second->w++ :  it_obj->second->r++;
        }
    } else {
        m_heap_accesses_outside_obj++;
    }
}

// systemc thread called by the simulation core every LOG_FLUSH_RESOLUTION_CYCLES platform cycles
void PlatformLogSystem::log_flush(bool auto_flush)
{
    // "time_cycles;r_count;w_count;address"
    for (auto const &ent : m_log_acc_map) {
        m_log_acc_file
                << ((uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) - (auto_flush ? LOG_FLUSH_RESOLUTION_CYCLES : 0)) << ";"
                << ent.second->r << ";"
                << ent.second->w << ";"
                << ent.first << std::endl;
        delete ent.second;
    }
    m_log_acc_map.clear();
}

void PlatformLogSystem::log_malloc(uint32_t addr, uint32_t size)
{
    if (m_log_obj_map.find(addr) != m_log_obj_map.end()) {
        std::cerr << name() << ": overlap in heap allocation ! " << std::endl;
        // abort();
    }
    m_log_obj_map[addr] = new LoggedObj((uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()), size, m_current_heap_action);
    m_current_heap_action++;
}

unsigned int dbg_untouched_obj = 0;

void PlatformLogSystem::log_free(uint32_t addr)
{
    // std::cout << "PlatformLogSystem::log_free::IN"<< std::endl;
    std::map<uint32_t, LoggedObj*>::iterator it_obj = m_log_obj_map.find(addr);
    if ( it_obj == m_log_obj_map.end()) {
        m_invalid_free_calls++;
    } else {
        if((it_obj->second->r + it_obj->second->w )== 0)
            dbg_untouched_obj++;
        // "address;size;malloc_date_cycles;lifespan_cycles;r_count;w_count"
        m_log_obj_file
                << it_obj->first << ";"
                << it_obj->second->size << ";"
                << it_obj->second->malloc_date << ";"
                << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) - it_obj->second->malloc_date << ";"
                << it_obj->second->r << ";"
                << it_obj->second->w << ";"
                << it_obj->second->alloc_order << ";"
                << m_current_heap_action << std::endl;
        m_current_heap_action++;

        m_r_access_count += it_obj->second->r;
        m_w_access_count += it_obj->second->w;

        delete it_obj->second; // delete obj descriptor
        m_log_obj_map.erase(it_obj);
    }
    // std::cout << "PlatformLogSystem::log_free::OUT"<< std::endl;
}

void PlatformLogSystem::log_unfreed_at_end()
{
    for (auto const &ent : m_log_obj_map) {
        if((ent.second->r + ent.second->w )== 0)
            dbg_untouched_obj++;
        m_log_obj_file
                << ent.first << ";"
                << ent.second->size << ";"
                << ent.second->malloc_date << ";"
                << (uint64_t)(PROC_FREQ * sc_core::sc_time_stamp().to_seconds()) - ent.second->malloc_date << ";"
                << ent.second->r << ";"
                << ent.second->w << ";"
                << ent.second->alloc_order << ";"
                << m_current_heap_action << std::endl;
        m_current_heap_action++;
        delete ent.second;
    }
    m_log_obj_map.clear();
}

void PlatformLogSystem::log_flush_end()
{
    log_flush(false);
    log_unfreed_at_end();
    // std::cout<< "[invalid free calls : "<<m_invalid_free_calls<< "]" << std::endl;
    // std::cout<< "[dbg_untouched_obj : "<<dbg_untouched_obj<<  "]" <<std::endl;
    // std::cout<< "[m_heap_accesses_outside_obj : "<<m_heap_accesses_outside_obj<< "]" << std::endl;
    // std::cout<< "[heap read access count : "<<m_r_access_count<< "]" << std::endl;
    // std::cout<< "[heap write access count : "<<m_w_access_count<< "]" << std::endl;
    // std::cout<< "[heap read / write access ratio : "<<(float)m_r_access_count/(float)m_w_access_count<< "]" << std::endl;


}



Oracle::Oracle(std::fstream& oracle_file): m_oracle_file(oracle_file)
{
    m_oracle_count = 0;
}

Oracle::~Oracle()
{
    m_oracle_file.close();
}

unsigned int Oracle::get_next_prediction()
{
    if (m_oracle_file.eof()) {
        std::cerr << "ERROR : asking oracle prediction but no more omen aviable in oracle file, aborting." << std::endl;
        abort();
    }
    unsigned int buf = 0xDEADDEAD;

    m_oracle_file >> buf;

    if (buf == 0xDEADDEAD) {
        std::cerr << "ERROR : unable to read from oracle file, aborting." << std::endl;
        abort();
    }
    // std::cout << "oracle ("<< m_oracle_count<< ") = " << buf<< std::endl;
    m_oracle_count++;
    return buf;
}

