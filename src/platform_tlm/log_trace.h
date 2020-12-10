/*

This file is part of the Dycton simulator.
This software aims to provide an environment for Dynamic Heterogeneous Memory 
Allocation for embedded devices study. It is build using SystemC / TLM.
It uses the MIPS32 ISS from the SocLib project (www.soclib.fr). 
It also use one SimSoc module (https://gforge.inria.fr/projects/simsoc/)
(originals athors credited in respective files)

Copyright (C) 2019  Tristan Delizy, CITI Lab, INSA de Lyon

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef LOG_TRACE_H
#define LOG_TRACE_H

#include <string>
#include <tlm>
#include <map>
#include "platform_time.h"

//==============================================================================
// Simulation global variables
//==============================================================================

extern uint32_t colors_in_uart_output;

class MemArchi;
extern MemArchi* dy_mem_arch;
extern const uint32_t heap_base;
extern uint32_t heap_end;
extern uint32_t heap_size;
extern uint32_t heap_footprint;

extern uint32_t Strategy;

extern uint32_t malloc_fallback;
extern uint32_t malloc_count;
extern uint64_t malloc_cycles;
extern uint64_t free_cycles;
extern uint64_t total_size_allocated;

extern uint32_t Dataset_index;

//==============================================================================
// defines
//==============================================================================

//------------------------------------------------------------------------------
// debug traces activation defines
#define WRAPPER_ID 0
//------------------------------------------------------------------------------
#define MEM_ID 1
    #define SPM_SUB_ID 0
    #define ARBITRATED_SUB_ID 1
    #define MEM_SUB_ID 2
//------------------------------------------------------------------------------
#define BUS_ID 2
//------------------------------------------------------------------------------
#define PER_ID 3
    #define UART_SUB_ID 0
    #define HELPER_SUB_ID 1
    #define GPB_SUB_ID 2
//------------------------------------------------------------------------------
#define ISS_ID 4
//------------------------------------------------------------------------------
#define SCMAIN_ID 5
//------------------------------------------------------------------------------
    #define NULL_SUB_ID 0
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// names and definitions for logging
#define LOG_NAME(x) (std::string()+"../logs/"+x+".log").c_str()

#define MEM_LOG_ACC_HEADERS "time_cycles;r_count;w_count;address"
#define MEM_LOG_OBJ_HEADERS "addr;size;malloc_date_cycles;lifespan_cycles;r_count;w_count;alloc_order;free_order;alloc_site;fallback"

#define LOG_COMMENT_STR "#" // default value from numpy

#define RESET_NAME ".reset"
#define EXCEPT_NAME ".except"
#define SPM_CODE_NAME ".text"
#define SPM_DATA_NAME ".data + stack"
#define SPM_GP_NAME "heap"
#define DENSE_MEM_NAME "main memory"

#if defined(h263)
#define LOG_FLUSH_RESOLUTION_CYCLES (10000000) // in cycles
#define LOG_ADDR_AGGREG_RESOLUTION (10240) // in bytes
#else
#define LOG_FLUSH_RESOLUTION_CYCLES (500000) // in cycles
#define LOG_ADDR_AGGREG_RESOLUTION (1024) // in bytes
#endif



//==============================================================================
// classes
//==============================================================================

// log control by simulated hw module

class LogTraceModule {
    public:
        LogTraceModule(int nb) { this->s = new bool[nb]; }
        ~LogTraceModule() { delete[] this->s; }
        bool* s;
};

template <size_t NB>
class LogTraceModDef {
    public:
        LogTraceModDef() { for(unsigned int i=0; i<NB; i++){this->m[i]= NULL; } }
        LogTraceModule* m[NB];
};


// Memory Architecture Descriptor (MAD)

class MemBank {
    public:
        MemBank(std::string n, unsigned int st, unsigned int sz, unsigned int rl, unsigned int wl)
            { this->name = n; this->base_ad = st; this->size = sz; this->rlat = rl; this->wlat = wl;
                std::cout << "creating interval : "<< this->name << " " << this->base_ad << " " << this->size << std::endl; }
        std::string name;
        unsigned int base_ad;
        unsigned int size;
        unsigned int rlat;
        unsigned int wlat;
};

class MemArchi {
    public:
        MemBank** banks;
        MemArchi(int nb, int arch): size(nb), index(0), simID(arch) { this->banks = new MemBank*[nb]; }
        ~MemArchi(){ delete[] this->banks; }
        void add(std::string n, unsigned int st, unsigned int sz, unsigned int rl, unsigned int wl){
            if(index<this->size)
                this->banks[index++]=new MemBank(n, st, sz, rl, wl);
            else{
                std::cout << "ERROR in address map construction"<< std::endl;
                abort();
            }
        }
        void print(std::ostream &os){
            for(int i=0;i<this->index;i++){
                os << std::dec << this->banks[i]->name << ":" <<
                this->banks[i]->base_ad << ":" << this->banks[i]->size << ":" <<
                this->banks[i]->rlat << ":" << this->banks[i]->wlat << std::endl;
            }
        }
        uint32_t get_heap_count(){return this->index;}
        void set_architecture_id(int id){this->simID = id;}
        int get_architecture_id(){return this->simID;}
    private:
        int size, index, simID;
};



// log aggregation in memory
class AggregatedAccesses {
    public:
        AggregatedAccesses()
            {this->r = 0; this->w = 0;}
        AggregatedAccesses(bool write_op)
            {this->r = 0; this->w = 0; write_op ? this->w++ :this->r++;}
        uint32_t r;
        uint32_t w;
};


// object moritoring
class LoggedObj {
    public:
        LoggedObj(uint64_t date, uint32_t sz, uint32_t ra, uint32_t order, uint32_t fb)
            {this->r = 0; this->w = 0; size = sz; malloc_date = date; alloc_site = ra; this->alloc_order = order; this->fallback = fb;}
        uint32_t r;
        uint32_t w;
        uint32_t size;
        uint32_t alloc_site;
        uint64_t malloc_date; // in platform cycles
        uint32_t alloc_order; // ordering relative to malloc/free calls
        uint32_t fallback; // does this object has been fallbacked into slow ?
};

// log module
class PlatformLogSystem : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(PlatformLogSystem);

public:

    PlatformLogSystem( sc_core::sc_module_name name, std::fstream& log_acc_stream,  std::fstream& log_obj_stream);
    ~PlatformLogSystem();

    void log_access(uint32_t addr, bool write_op);

    void log_malloc(uint32_t addr, uint32_t size, uint32_t alloc_site, uint32_t fallback);
    void log_free(uint32_t addr);

    void log_flush_end();

    uint64_t get_r_count(){return m_r_access_count;}
    uint64_t get_w_count(){return m_w_access_count;}

private:
    // Log system for aggregating access to heap objects
    std::fstream& m_log_acc_file;
    std::fstream& m_log_obj_file;
    std::map<uint32_t, AggregatedAccesses * > m_log_acc_map;
    std::map<uint32_t, LoggedObj * > m_log_obj_map;
    uint32_t m_heap_accesses_outside_obj;
    uint32_t m_invalid_free_calls;
    uint32_t m_current_heap_action;
    uint64_t m_r_access_count;
    uint64_t m_w_access_count;

    void log_flush(bool auto_flush = true);
    void log_unfreed_at_end();
    void log_process();
};

// oracle for strategy precomputed
class Oracle
{
public:
    Oracle(std::fstream& oracle_file);
    ~Oracle();
    unsigned int get_next_prediction(void);
private:
    std::fstream& m_oracle_file;
    uint32_t m_oracle_count;
};


//==============================================================================
// log and dbg trace globals
//==============================================================================
// defined in sc_main.cpp
extern PlatformLogSystem *platlog;

extern LogTraceModule* log_wrp;
extern LogTraceModule* log_mem;
extern LogTraceModule* log_bus;
extern LogTraceModule* log_per;

extern LogTraceModDef<4> dy_log;
extern bool dy_log_switch;

extern LogTraceModule* dbg_wrp;
extern LogTraceModule* dbg_mem;
extern LogTraceModule* dbg_bus;
extern LogTraceModule* dbg_per;
extern LogTraceModule* dbg_iss;
extern LogTraceModule* dbg_scmain;

extern LogTraceModDef<6> dy_dbg;
extern bool dy_dbg_switch;

extern Oracle * Tiresias;

//==============================================================================
// utilities : inlines and macros
//==============================================================================
// must be called at sc_main start
inline void dy_init_logtrace()
{
    dy_dbg.m[WRAPPER_ID] = dbg_wrp;
    dy_dbg.m[MEM_ID] = dbg_mem;
    dy_dbg.m[BUS_ID] = dbg_bus;
    dy_dbg.m[PER_ID] = dbg_per;
    dy_dbg.m[ISS_ID] = dbg_iss;
    dy_dbg.m[SCMAIN_ID] = dbg_scmain;

    dy_log.m[WRAPPER_ID] = log_wrp;
    dy_log.m[MEM_ID] = log_mem;
    dy_log.m[BUS_ID] = log_bus;
    dy_log.m[PER_ID] = log_per;
}

inline void dy_deinit_logtrace()
{
    delete dbg_wrp;
    delete dbg_mem;
    delete dbg_bus;
    delete dbg_per;
    delete dbg_iss;
    delete dbg_scmain;
    delete log_wrp;
    delete log_mem;
    delete log_bus;
    delete log_per;
}

inline void dy_dump_dbgtrace_conf()
{
    std::cout << "dycton dbg conf :" << std::endl;
    std::cout << "wrapper: " << dy_dbg.m[WRAPPER_ID]->s[0] << std::endl;
    std::cout << "mem::spm: " << dy_dbg.m[MEM_ID]->s[0] << std::endl;
    std::cout << "mem::arbitrated: " << dy_dbg.m[MEM_ID]->s[1] << std::endl;
    std::cout << "mem::memory: " << dy_dbg.m[MEM_ID]->s[2] << std::endl;
    std::cout << "bus: " << dy_dbg.m[BUS_ID]->s[0] << std::endl;
    std::cout << "per::uart: " << dy_dbg.m[PER_ID]->s[0] << std::endl;
    std::cout << "per::helper: " << dy_dbg.m[PER_ID]->s[1] << std::endl;
    std::cout << "per::gpb: " << dy_dbg.m[PER_ID]->s[2] << std::endl;
    std::cout << "iss: " << dy_dbg.m[ISS_ID]->s[0] << std::endl;
    std::cout << "scmain: " << dy_dbg.m[SCMAIN_ID]->s[0] << std::endl;
}

// debug macro : x is "executed" only if dbg is active for concerned submodule
#define DY_DBG(x, mod, submod)  do { if(dy_dbg_switch && dy_dbg.m[mod]->s[submod]){x;} } while(0)

// logging macro : x is "executed" only if dbg is active for concerned submodule
#define DY_LOG(x, mod, submod)  do { if(dy_log_switch && dy_log.m[mod]->s[submod]){x;} } while(0)

// logging macro : x is "executed" only if dbg is active for addresses inside heap range
#define DY_LOG_HEAP_ONLY(a, x, mod, submod)  do { if((unsigned int)a >= (unsigned int)heap_base && (unsigned int)a < (unsigned int)heap_end ){x;} } while(0)

// turn on DBG (no conf modification)
inline void dy_dbg_on() {dy_dbg_switch = true;}

// turn off DBG (no conf modification)
inline void dy_dbg_off() {dy_dbg_switch = false;}

// turn on LOG (no conf modification)
inline void dy_log_on() {dy_log_switch = true;}

// turn off LOG (no conf modification)
inline void dy_log_off() {dy_log_switch = false;}

// activate debug for concerned submodule
inline void dy_enable_dbg(int m, int s) { dy_dbg.m[m]->s[s] = true; }

// deactivate debug for concerned submodule
inline void dy_disable_dbg(int m, int s) { dy_dbg.m[m]->s[s] = false; }

// activate debug for concerned submodule
inline void dy_enable_log(int m, int s) { dy_log.m[m]->s[s] = true; }

// activate debug for concerned submodule
inline void dy_disable_log(int m, int s) { dy_log.m[m]->s[s] = false; }

// debug globals definition macro (must be done in cpp file)

#define __DY_DBG_DEF  LogTraceModDef<4> dy_log; \
                    bool dy_log_switch = true; \
                    LogTraceModDef<6> dy_dbg; \
                    bool dy_dbg_switch = true; \
                    LogTraceModule* log_wrp = new LogTraceModule(1); \
                    LogTraceModule* log_mem = new LogTraceModule(3); \
                    LogTraceModule* log_bus = new LogTraceModule(1); \
                    LogTraceModule* log_per = new LogTraceModule(3); \
                    LogTraceModule* dbg_wrp = new LogTraceModule(1); \
                    LogTraceModule* dbg_mem = new LogTraceModule(3); \
                    LogTraceModule* dbg_bus = new LogTraceModule(1); \
                    LogTraceModule* dbg_per = new LogTraceModule(3); \
                    LogTraceModule* dbg_iss = new LogTraceModule(1); \
                    LogTraceModule* dbg_scmain = new LogTraceModule(1);


#endif //LOG_TRACE_H