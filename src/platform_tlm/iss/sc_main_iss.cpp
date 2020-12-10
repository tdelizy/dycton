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


#include <string>
#include <fstream>
#include <sys/time.h>
#include "systemc.h"

#include "mips32_wrapper.h"
#include "memory.h"
#include "scratch_pad_memory.h"
#include "nb_bus.h"
#include "helper.h"
#include "uart.h"
#include "gpb.h"

#include "../address_map.h"
#include "../platform_time.h"

#include "../log_trace.h"

#include "../elf-loader/loader/include/loader.h"
#include "../elf-loader/loader/include/exception.h"

// application specific includes
// contains static array init, don't include anywhere else
#if defined(jpeg)
    #define TARGET_APP "jpeg"
    #include "../software/jpeg/dycton_jpeg_app.h"
#elif defined(h263)
    #define TARGET_APP "h263"
    #include "../software/h263/dycton_h263_app.h"
#elif defined(dijkstra)
    #define TARGET_APP "dijkstra"
    #include "../software/dijkstra/dycton_dijkstra_app.h"
#elif defined(patricia)
    #define TARGET_APP "patricia"
    #include "../software/patricia/dycton_patricia_app.h"
#elif defined(json_parser)
    #define TARGET_APP "json_parser"
    #include "../software/json_parser/dycton_json_parser_app.h"
#elif defined(jpg2000)
    #define TARGET_APP "jpg2000"
    #include "../software/jpg2000/dycton_jpg2000_app.h"
#elif defined(ecdsa)
    #define TARGET_APP "ecdsa"
    #include "../software/ecdsa/dycton_ecdsa_app.h"
#else
    #define TARGET_APP "debug"
    #include "../software/json_parser/dycton_json_parser_app.h"
#endif



namespace soclib {
namespace common {
extern bool elf_load(const std::string &filename,
                     soclib::common::Loader &loader);
}
};




void usage(void);
void retrieve_result_if_needed(unsigned int * base_pointer, unsigned int ds_sz,  unsigned int ds_stream_sz);
void mem_init_jpg2000(unsigned int * internal_pointer, std::string dataset_path, unsigned int ds_sz);
void mem_init_h263(char * internal_pointer_char, std::string dataset_path, unsigned int ds_sz);
void mem_init_jpeg(unsigned int * internal_pointer, std::string dataset_path, unsigned int ds_sz);
void mem_init_dijkstra(unsigned int * internal_pointer, std::string dataset_path, unsigned int ds_sz);
void mem_init_patricia(unsigned int * internal_pointer);
void mem_init_mergesort(unsigned int * internal_pointer);
void mem_init_matmul(unsigned int * internal_pointer);


// definition of debug global variables
__DY_DBG_DEF

const uint32_t heap_base = (HEAP_BASE);
uint32_t heap_end = 0;
uint32_t heap_size = 0;
uint32_t heap_footprint = 0;
uint64_t total_size_allocated = 0;
PlatformLogSystem * platlog;
uint64_t malloc_cycles = 0;
uint64_t free_cycles = 0;
Oracle * Tiresias;
uint32_t Strategy = 0;
int Architecture = 0;
uint32_t Dataset_index = 0;
uint32_t malloc_count = 0;
uint32_t malloc_fallback = 0;
std::string input_strat_filename;
std::string prof_len_str;
uint32_t profile_len = 0;
MemArchi* dy_mem_arch = NULL;


std::string Strat_string_array[STRAT_COUNT] = {
    DEFAULT_STRAT_STRING,
    ORACLE_STRAT_STRING,
    PROFILE_STRAT_STRING,
    PROFILE_ENHANCED_STRING,
    PROFILE_ILP_STRING
};


#define DBG(x) DY_DBG(x, SCMAIN_ID, NULL_SUB_ID)



//------------------------------------------------------------------------------
// sc_main : simulator main routine
//      instantiate platform
//      fill up memory
//      run simulation
//      retrieve results
//------------------------------------------------------------------------------
int sc_main(int argc, char *argv[]) {

//------------------------------------------------------------------------------
// INPUT PARAMETER PARSING
//------------------------------------------------------------------------------
	std::cout << "\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
	std::cout << "Dycton simulator - MIPS32 SoC simulator with heterogeneous memory subsystem" << std::endl;
	std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
	std::cout << "Author: Tristan Delizy" << std::endl;
	std::cout << "Contact: tdelizy@insa-lyon.fr" << std::endl;
	std::cout << "Public repo: https://gitlab.inria.fr/citi-lab/dycton" << std::endl;
	std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;

    // command line parsing
    int option = 0;
    std::string strat_name;
    std::string dataset_path;
    std::string dataset_ref;
    while ((option = getopt (argc, argv, "d:a:s:f:p:ch")) != -1)
        switch (option)
        {
        case 'd':
            Dataset_index = std::stoi(optarg);
            // std::cout << "dataset : " << optarg << std::endl;
            break;
        case 'a':
            Architecture = std::stoi(optarg);
            // std::cout << "Archi : " << optarg << std::endl;
            break;
        case 's':
            strat_name = optarg;
            // std::cout << "strategie : " << optarg << std::endl;
            break;
        case 'f':
            // std::cout << "file path : " << optarg << std::endl;
            input_strat_filename = optarg;
            break;
        case 'p':
            // std::cout << "profile length : " << optarg << std::endl;
            profile_len = std::stoi(optarg);
            break;
        case 'c':
            // std::cout << "colors in UART output disabled." << std::endl;
            colors_in_uart_output = 0;
            break;
        case 'h':
        	usage();
        	return 0;
        	break;
        default:
            std::cerr << "Error: invalid command line argument: "<< option << std::endl;
        	usage();
            abort();
            break;
        }

    for(int i=0; i<STRAT_COUNT; i++){
        if(strat_name == Strat_string_array[i]){
            Strategy = i;
            break;
        }

    }
#ifdef ecdsa // workaround : ecdsa does not have filenames but just .h embedded messages
    dataset_ref = std::string(*(dataset_array[Dataset_index]));
    if(dataset_ref.length() >= 60){
        dataset_ref = dataset_ref.substr(0, 60).append("...");
    }
    std::cout << "ecdsa dataset size : " << std::to_string(std::strlen(*dataset_array[Dataset_index])) << std::endl;
#else
    dataset_ref = std::string(dataset_array[Dataset_index]);
#endif

    if(Dataset_index >= DATASET_COUNT){
        std::cerr << "Error: dataset index out of range, aborting..." << std::endl;
        abort();
    }

    // parameters check
    if(Architecture > 6 || Architecture < -2){
        std::cerr << "Error: unknown architecture, aborting..." << std::endl;
        abort();
    }
    if(Strategy >= STRAT_COUNT){
        std::cerr << "Error: unknown strategy, aborting..." << std::endl;
        abort();
    }
    if(Strategy != DEFAULT_STRATEGY && (Architecture == 0 || Architecture == 6 || Architecture == -1)){
        std::cerr << "Error: Strategy unaviable for single heap, aborting..." << std::endl;
        abort();
    }
    if((Strategy == PROFILE_ENHANCED || Strategy == PROFILE_STRATEGY)&& profile_len == 0){
        std::cerr << "Error: invalid profile length (option -p, mandatory for profile strategy)" << std::endl;
        abort();
    }


    // print simulation information for logging
    std::cout << "placement strategy : " << Strat_string_array[Strategy] << " (" << Strategy << ")" << std::endl;
    std::cout << "memory architecture : " << Architecture << std::endl;
    std::cout << "application : " << TARGET_APP << std::endl;
    std::cout << "dataset index : " << Dataset_index << std::endl;
    std::cout << "dataset filename: " << dataset_ref << std::endl;
    std::cout << "target dataset application mem footprint: " << dataset_footprint[Dataset_index] << std::endl;




//------------------------------------------------------------------------------
// SIMULATION ENVIRONMENT INITIALISATION
//------------------------------------------------------------------------------


    // wall clock time logging struct declaration
    struct timeval real_time_start;
    struct timeval real_time_end;

    // first initialize log and trace system and configure here
    // what is going to be output on std::out and which module will generates log.
    // see platform_tlm/log_trace.h
    std::fstream access_log_file("../logs/mem_access.log", std::fstream::out);
    std::fstream timing_file("../logs/cycles", std::fstream::out);
    std::fstream object_log_file("../logs/heap_objects.log", std::fstream::out);
    std::fstream oracle_file(input_strat_filename,  std::fstream::in);
    platlog = new PlatformLogSystem( "dycton_log", access_log_file, object_log_file);
    Tiresias = new Oracle(oracle_file);

    dy_init_logtrace();
    dy_enable_dbg(SCMAIN_ID, NULL_SUB_ID);
    // dy_enable_dbg(PER_ID, HELPER_SUB_ID);
    // dy_enable_dbg(MEM_ID, ARBITRATED_SUB_ID); // spm banks
    // dy_enable_dbg(MEM_ID, SPM_SUB_ID); // spm
    // dy_enable_dbg(MEM_ID, MEM_SUB_ID); // main mem
    // dy_enable_dbg(WRAPPER_ID, NULL_SUB_ID); // iss wrapper

    // dy_enable_log(MEM_ID, MEM_SUB_ID); // main mem
    dy_enable_log(MEM_ID, ARBITRATED_SUB_ID); //spm banks
    // dy_dump_dbgtrace_conf(); // print the platform debug configuration




//------------------------------------------------------------------------------
// MEMORY ARCHITECTURE CONSTRUCTION
//------------------------------------------------------------------------------

    // declare the MAD (Memory Architecture Desciptor)
    // this is used along the sc_main to construct the different simulated HW modules
    // then it is used to connect them through the bus and finally it is used in the
    // dispatcher (embedded software), through the helper to get the number of heap
    // in the current simulation.
    dy_mem_arch = new MemArchi(2, Architecture);
    uint32_t app_footprint = dataset_footprint[Dataset_index]; // get the correct heap size
    uint32_t fast_heap_sz = 0;
    uint32_t next_heap_ad = 0;
    unsigned int spm_sizes[] = {SPM_I_SIZE, SPM_D_SIZE, SPM_S_SIZE, 0, 0};
    unsigned int spm_r_lat[] = {1, 1, 1, 0, 0};
    unsigned int spm_w_lat[] = {1, 1, 1, 0, 0};
    unsigned int spm_prio[] = {4, 3, 2, 1, 0};
    unsigned int spm_tot_size = 0;
    unsigned int spm_ports = 0;
    unsigned int spm_banks = 0;

    // handle 2 heaps case
    if ( Architecture != 0 && Architecture != 6 && Architecture != -1 ){
        switch (Architecture){
            case -2:
                fast_heap_sz = 2*app_footprint; // 200 % of fast and 200% of slow ("ideal" 2 heaps mem archi)
                break;
            case 1:
                fast_heap_sz = (app_footprint * 3) / 4 ; // 75 % of fast
                break;
            case 2:
                fast_heap_sz = app_footprint / 2; // 50 % of fast
                break;
            case 3:
                fast_heap_sz = app_footprint / 4; // 25 % of fast
                break;
            case 4:
                fast_heap_sz = app_footprint / 10; // 10 % of fast
                break;
            case 5:
                fast_heap_sz = app_footprint / 20; // 5 % of fast
                break;
            default:
                std::cerr << "error in memory architecture construction, aborting..." << std::endl;
                abort();
        }

        // ensure size of heap is a mulltiple of 16 (DLMalloc requirement)
        fast_heap_sz = dycton_align_16(fast_heap_sz);

        dy_mem_arch->add("HEAP_0", HEAP_BASE, fast_heap_sz, MEM_FAST_RLAT, MEM_FAST_WLAT); // fast heap first
        next_heap_ad = HEAP_BASE + fast_heap_sz;
        dy_mem_arch->add("HEAP_1", next_heap_ad, dycton_align_16(app_footprint), MEM_SLOW_RLAT, MEM_SLOW_WLAT); // slow heap

        // HW parameters
        spm_sizes[3] = dy_mem_arch->banks[0]->size;
        spm_sizes[4] = dy_mem_arch->banks[1]->size;
        spm_tot_size = SPM_I_SIZE + SPM_D_SIZE + SPM_S_SIZE + dy_mem_arch->banks[0]->size + dy_mem_arch->banks[1]->size;
        spm_r_lat[3] = dy_mem_arch->banks[0]->rlat;
        spm_r_lat[4] = dy_mem_arch->banks[1]->rlat;
        spm_w_lat[3] = dy_mem_arch->banks[0]->wlat;
        spm_w_lat[4] = dy_mem_arch->banks[1]->wlat;
        spm_ports = 3;
        spm_banks = 5;

        heap_end = dy_mem_arch->banks[1]->base_ad + dy_mem_arch->banks[1]->size;

    } else { // single heap architecture otherwise
        switch (Architecture){
            case -1:
                dy_mem_arch->add("HEAP_0", HEAP_BASE, 0x02000000, 1, 1); // 32Mo of ideal memory
                break;
            case 0:
                dy_mem_arch->add("HEAP_0", HEAP_BASE, app_footprint, MEM_FAST_RLAT, MEM_FAST_WLAT); // only fast
                break;
            case 6:
                dy_mem_arch->add("HEAP_0", HEAP_BASE, app_footprint, MEM_SLOW_RLAT, MEM_SLOW_WLAT); // only slow
                break;
            default:
                std::cerr << "error in memory architecture construction, aborting..." << std::endl;
                abort();
        }

        // HW parameters
        spm_sizes[3] = dy_mem_arch->banks[0]->size;
        spm_tot_size = SPM_I_SIZE + SPM_D_SIZE + SPM_S_SIZE + dy_mem_arch->banks[0]->size;
        spm_r_lat[3] = dy_mem_arch->banks[0]->rlat;
        spm_w_lat[3] = dy_mem_arch->banks[0]->wlat;
        spm_ports = 3;
        spm_banks = 4;

        heap_end = dy_mem_arch->banks[0]->base_ad + dy_mem_arch->banks[0]->size;
    }
    heap_size = heap_end - heap_base;

    dy_mem_arch->print(std::cout);

//------------------------------------------------------------------------------
// SIMULATED PLATFORM COMPONENTS INSTANTIATION
//------------------------------------------------------------------------------
    DBG(std::cout <<"[DYCTON] platform components instantiation ============================" << std::endl);
    DBG(std::cout <<"PLATFORM FREQUENCY : " << PROC_FREQ << "Hz" << std::endl);
    // ISS and wrapper instanciation
    // workaround : for iss wrapper stack is at top of data. its the case in
    // our memory architecture but not in the same bank, but don't need to tell it to the iss
    MIPS32Wrapper cpu("MyWrap", 0, TEXT_BASE, TEXT_SIZE, DATA_BASE, SPM_D_SIZE + SPM_S_SIZE, SPM_GP_BASE, spm_tot_size);

    // Scratch Pad Memory instanciation
    ScatchPadMemory spm("spm_dycton", spm_ports, spm_banks, spm_sizes, spm_r_lat, spm_w_lat, spm_prio);

    // Bus (TLM non-blocking communication implementation)
    NbBus bus("bus");

    // dense memory on the bus
    unsigned int dense_mem_r_lat = 1;
    unsigned int dense_mem_w_lat = 1;
    Memory dense_mem("dense_mem", DENSE_MEM_SIZE, dense_mem_r_lat, dense_mem_w_lat);

    // helpers memories and components
    Memory reset("reset_mem_range", RESET_SIZE, 0, 0);
    Memory except("except_mem_range", EXCEP_SIZE, 0, 0);
    Helper helper("helper", Tiresias, &cpu, "../rng_sim.txt");
    UART uart("uart");


//------------------------------------------------------------------------------
// SIMULATED PLATFORM MEMORY FILL
//------------------------------------------------------------------------------

    DBG(std::cout <<"[DYCTON] signal declaration ===========================================" << std::endl);

    sc_core::sc_signal<bool> cpu_irq("cpu_irq"); // stub


    DBG(std::cout <<"[DYCTON] memory content loading =======================================" << std::endl);

    // buffers reading from a file on host machine, if needed by the application
#ifdef moving_average
    GPBuffer in_buf("Input_buffer", "../software/moving_average/datasets/gpb.input", "../logs/gib.log", GPBuffer::BUFF_IN);
    GPBuffer out_buf("Output_buffer", "../software/moving_average/datasets/gpb.output", "../logs/gob.log", GPBuffer::BUFF_OUT);
#endif
#ifdef lzw
    GPBuffer in_buf("Input_buffer", "../software/lzw/datasets/lzw.in", "../logs/gib.log", GPBuffer::BUFF_IN);
    GPBuffer out_buf("Output_buffer", "../software/lzw/datasets/lzw.out", "../logs/gob.log", GPBuffer::BUFF_OUT);
#endif
#ifdef json_parser
    GPBuffer in_buf("Input_buffer", dataset_ref.c_str(), "../logs/gib.log", GPBuffer::BUFF_IN);
    GPBuffer out_buf("Output_buffer", "../software/json_parser/gpb.output", "../logs/gob.log", GPBuffer::BUFF_OUT);
#endif


    // Load the program sections into memory
    soclib::common::Loader::register_loader("elf", soclib::common::elf_load);
    unsigned int * internal_pointer = 0;

    try {
        soclib::common::Loader loader("../software/a.out");

        loader.load(reset.storage, RESET_BASE, RESET_SIZE);
        for (int i = 0; i < RESET_SIZE / 4; i++) {
            reset.storage[i] = uint32_be_to_machine(reset.storage[i]);
        }

        loader.load(except.storage, EXCEP_BASE, EXCEP_SIZE);
        for (int i = 0; i < EXCEP_SIZE / 4; i++) {
            except.storage[i] = uint32_be_to_machine(except.storage[i]);
        }

        internal_pointer = spm.get_mem_ptr(0);
        loader.load(internal_pointer, TEXT_BASE, TEXT_SIZE);
        for (int i = 0; i < TEXT_SIZE / 4; i++) {
            internal_pointer[i] = uint32_be_to_machine(internal_pointer[i]);
        }

        internal_pointer = spm.get_mem_ptr(1);
        loader.load(internal_pointer, DATA_BASE, DATA_SIZE);
        for (int i = 0; i < DATA_SIZE / 4; i++) {
            internal_pointer[i] = uint32_be_to_machine(internal_pointer[i]);
        }
    } catch (soclib::exception::RunTimeError &e) {
        std::cerr << "unable to load ELF file in memory:" << std::endl;
        std::cerr << e.what() << std::endl;
        abort();
    }

    // now filling application specific data on dense memory on the bus
    internal_pointer = dense_mem.storage;

#ifdef matmul
    mem_init_matmul(internal_pointer);
#endif
#ifdef mergesort
    mem_init_mergesort(internal_pointer);
#endif
#ifdef patricia
    mem_init_patricia(internal_pointer);
#endif
#ifdef dijkstra
    unsigned int dataset_size = dataset_in_size[Dataset_index];
    dataset_path = dataset_ref.c_str();
    mem_init_dijkstra(internal_pointer, dataset_path, dataset_size);
#endif
#ifdef jpeg
    unsigned int dataset_size = dataset_in_size[Dataset_index];
    dataset_path = dataset_ref.c_str();
    mem_init_jpeg(internal_pointer, dataset_path, dataset_size);
#endif
#ifdef h263
    char * internal_pointer_char = (char *)dense_mem.storage;
    unsigned int dataset_size = dataset_in_size[Dataset_index];
    dataset_path = dataset_ref.c_str();
    mem_init_h263(internal_pointer_char, dataset_path, dataset_size);
#endif
#ifdef jpg2000
    unsigned int dataset_size = dataset_in_size[Dataset_index];
    dataset_path = dataset_ref.c_str();
    mem_init_jpg2000(internal_pointer, dataset_path, dataset_size);
#endif


    // seting up the profile length specified
    if(profile_len > 0){
        DBG(std::cout << "setting up profile length : "<< std::dec << profile_len << std::endl);
        helper.profile_set_len_val(profile_len);
    }



//------------------------------------------------------------------------------
// BUS ADDRESS PLATFORM MAPPING / COMPONENTS BINDING
//------------------------------------------------------------------------------
    DBG(std::cout <<"[DYCTON] mapping ======================================================" << std::endl);

    // initiators
    cpu.i_socket.bind(spm.sockets[0]);
    cpu.d_socket.bind(spm.sockets[1]);
    cpu.b_socket.bind(bus.target_sockets);

    // interrupts (stub)
    helper.out(cpu_irq);
    cpu.irq(cpu_irq);

#if defined(lzw) || defined(moving_average) || defined(json_parser)
    bus.bind_target(in_buf.target,  IN_BUF_BASE,            sizeof(uint32_t));
    bus.bind_target(out_buf.target, OUT_BUF_BASE,           sizeof(uint32_t));
#endif

    bus.bind_target(reset.target, RESET_BASE, RESET_SIZE);
    reset.set_base_address(RESET_BASE);

    bus.bind_target(except.target, EXCEP_BASE, EXCEP_SIZE);
    except.set_base_address(EXCEP_BASE);

    bus.bind_target(spm.sockets[2], SPM_BASE, spm_tot_size);
    spm.set_bank_base_address(0, TEXT_BASE);
    spm.set_bank_base_address(1, DATA_BASE);
    spm.set_bank_base_address(2, DATA_BASE+DATA_SIZE);

    // spm heap banks base address set
    spm.set_bank_base_address(3, dy_mem_arch->banks[0]->base_ad);
    if ( Architecture != 0 && Architecture != 6 && Architecture != -1 )
        spm.set_bank_base_address(4, dy_mem_arch->banks[1]->base_ad);


    bus.bind_target(dense_mem.target, DENSE_MEM_BASE, DENSE_MEM_SIZE);
    dense_mem.set_base_address(DENSE_MEM_BASE);

    bus.bind_target(uart.target, UART_BASEADDR, UART_SIZE);

    bus.bind_target(helper.target, HELPER_BASE, HELPER_SIZE);


    std::fstream dy_mem_arch_file("../logs/memory_architecture", std::fstream::out);
    dy_mem_arch->print(dy_mem_arch_file);



//------------------------------------------------------------------------------
// SIMULATION RUN, RESULT RETRIEVEING AND PRINTING
//------------------------------------------------------------------------------
    DBG(std::cout <<"[DYCTON] sim start ====================================================" << std::endl);

    // log wall clock time of simulation start
    gettimeofday(&real_time_start,NULL);

    // start the simulation
    sc_core::sc_start();

    // flush elements not logged down file logs yet
    platlog->log_flush_end();

    // log wall clock time of simulation end
    gettimeofday(&real_time_end, NULL);

    uint64_t total_cycle_count = cpu.get_cycles();

    uint64_t code_r = spm.get_bank_read_count(0);
    uint64_t code_w = spm.get_bank_write_count(0);
    uint64_t data_r = spm.get_bank_read_count(1);
    uint64_t data_w = spm.get_bank_write_count(1);
    uint64_t stack_r = spm.get_bank_read_count(2);
    uint64_t stack_w = spm.get_bank_write_count(2);

    uint64_t heap_0_r = spm.get_bank_read_count(3);
    uint64_t heap_0_w = spm.get_bank_write_count(3);

	uint64_t heap_1_r = 0;
	uint64_t heap_1_w = 0;

    if(Architecture != 0 && Architecture != 6 && Architecture != -1){
        heap_1_r = spm.get_bank_read_count(4);
        heap_1_w = spm.get_bank_write_count(4);
    }

    uint64_t total_access = code_r + code_w + data_r + data_w + stack_r + stack_w + heap_0_r + heap_0_w + heap_1_r + heap_1_w;

    DBG(std::cout << "[code accesses (r/w): " << code_r << " / " << code_w  << " ("<< (float)((code_r + code_w)*100)/(float)total_access  << "%)]" << std::endl);
    DBG(std::cout << "[data accesses (r/w): " << data_r << " / " << data_w  << " ("<< (float)((data_r + data_w)*100)/(float)total_access  << "%)]" << std::endl);
    DBG(std::cout << "[stack accesses (r/w): " << stack_r << " / " << stack_w  << " ("<< (float)((stack_r + stack_w)*100)/(float)total_access  << "%)]\n" << std::endl);

    DBG(std::cout<< "[heap read access count : " << heap_0_r + heap_1_r << " ("<< (float)((heap_0_r + heap_1_r)*100)/(float)total_access  << "%)]" << std::endl);
    DBG(std::cout<< "[heap write access count : " << heap_0_w + heap_1_w << " ("<< (float)((heap_0_w + heap_1_w)*100)/(float)total_access  << "%)]" << std::endl);
    DBG(std::cout<< "[heap read / write access ratio : " << (float)heap_0_r/(float)heap_0_w<< "]\n" << std::endl);

    DBG(std::cout << "[total size allocated: " << std::dec << total_size_allocated << "]" << std::endl);
    DBG(std::cout << "[heap memory footprint: " << std::dec << heap_footprint << "]" << std::endl);
    DBG(std::cout << "[heap memory reuse ratio: " << std::dec << (float)total_size_allocated/(float)heap_footprint << "]\n" << std::endl);
    DBG(std::cout << "[object count: " << std::dec << malloc_count << "]" << std::endl);
    DBG(std::cout << "[malloc fallback: " << std::dec << malloc_fallback << "(" << ((float)malloc_fallback*100)/(float)malloc_count << "%)]\n" << std::endl);

    DBG(std::cout << "[final cycle count: " << std::dec << total_cycle_count << "]" << std::endl);
    DBG(std::cout << "[malloc cycle count: " << std::dec << malloc_cycles << " ("<< (float)(malloc_cycles*100)/(float)total_cycle_count  << "%)]" << std::endl);
    DBG(std::cout << "[free cycle count: " << std::dec << free_cycles << " ("<< (float)(free_cycles*100)/(float)total_cycle_count  << "%)]" << std::endl);
    DBG(std::cout << "[total allocator cycle count: " << std::dec << (malloc_cycles + free_cycles) << " ("<< (float)(malloc_cycles*100 + free_cycles*100)/(float)total_cycle_count  << "%)]" << std::endl);
    timing_file << std::dec << cpu.get_cycles()<<std::endl;
    timing_file << std::dec << cpu.get_cycles()<<std::endl;
    timing_file.close();


    // outputs the resulting image/video/list/... in a file next to the simulator if needed
#ifdef h263
    retrieve_result_if_needed(dense_mem.storage, dataset_out_size[Dataset_index],  dataset_stream_out_size[Dataset_index]);
#elif defined(jpeg) || defined(jpg2000)
    retrieve_result_if_needed(dense_mem.storage, dataset_out_size[Dataset_index],  0);
#endif


    int carry = 0;
    carry = real_time_start.tv_usec / 1000000;
    real_time_start.tv_usec -= 1000000 * carry;
    real_time_end.tv_sec -= (real_time_start.tv_sec+carry);
    if (real_time_end.tv_usec < real_time_start.tv_usec){
        real_time_end.tv_usec +=1000000;
        real_time_end.tv_sec -=1;
    }
    real_time_end.tv_usec -= real_time_start.tv_usec;
    if (real_time_end.tv_usec > 1000000){
        carry = real_time_end.tv_usec/1000000;
        real_time_end.tv_usec -= 1000000 * carry;
    }

    int minutes = real_time_end.tv_sec/60;
    real_time_end.tv_sec %= 60;
    int hours = minutes/60;
    minutes %= 60;

    //display wall clock time elapsed :
    DBG(std::cout <<"[wall clock time elapsed : "
        << hours << "h " << minutes << "m " << real_time_end.tv_sec << "s " << real_time_end.tv_usec/1000 << "ms]" << std::endl);

    // deinit logtrace and delete memory architecture objects
    delete dy_mem_arch;
    dy_deinit_logtrace();
    delete platlog;
    return 0;
}


// print input options
void usage(void)
{
    std::string name;
    std::cout << "Command line usage:" << std::endl;
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
    std::cout << "\t -d dataset:\tdataset number (see below)" << std::endl;
    std::cout << "\t -a archi:\ttarget heap memory architecture (see below)" << std::endl;
    std::cout << "\t -s strategy:\tstrategy to use for multi heap dynamic memory allocation" << std::endl;
    std::cout << "\t -f filepath:\tif strategy needs an input file." << std::endl;
    std::cout << "\t -p length:\tprofile based strategy: select profile length." << std::endl;
    std::cout << "\t -c:\t\tdisable colors in the output." << std::endl;
    std::cout << "\t -h:\t\tinvoke help." << std::endl;
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
    std::cout << "Strategies:" << std::endl;
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
    for(int i=0; i<STRAT_COUNT; i++){
        name = Strat_string_array[i];
        std::cout << "\t - " << name << std::endl;
    }
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
    std::cout << "Architectures:" << std::endl;
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
    std::cout << "\t 0: 100\% of heap in fast memory" << std::endl;
    std::cout << "\t 1: 75\% of heap in fast memory" << std::endl;
    std::cout << "\t 2: 50\% of heap in fast memory" << std::endl;
    std::cout << "\t 3: 25\% of heap in fast memory" << std::endl;
    std::cout << "\t 4: 10\% of heap in fast memory" << std::endl;
    std::cout << "\t 5: 5\% of heap in fast memory" << std::endl;
    std::cout << "\t 6: 0\% of heap in fast memory" << std::endl;
    std::cout << "\t -2: 100\% of heap in fast and 100% in slow memory" << std::endl;
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
    std::cout << "Datasets for " << TARGET_APP << ":" << std::endl;
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
    for(int i=0; i<DATASET_COUNT; i++){

#ifdef ecdsa // workaround : ecdsa does not have filenames but just .h embedded messages
        name = std::string(*(dataset_array[i]));
        if(name.length() >= 60){
            name = name.substr(0, 60).append("...");
        }
        std::string sz_str = " (";
        sz_str.append(std::to_string(std::strlen(*dataset_array[i])));
        sz_str.append(" bytes)");
        name = name.append(sz_str);
#else
        name = dataset_array[i];
#endif
        std::cout << "\t - (" << std::dec << i << ") " << name << std::endl;
    }
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;

}


// Application specific platform memory initialisations

void mem_init_matmul(unsigned int * internal_pointer)
{
    DBG(std::cout <<"[MATMUL] pre-loading the matrices in memory" << std::endl);
    std::fstream mat_file;
    mat_file.open("../software/matmul/datasets/10kB_matrices.in", std::fstream::in);
    if(!mat_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();
    }
    unsigned int i = 0;
    int val = 0;
    while (mat_file >> val && i<(DENSE_MEM_SIZE/sizeof(uint32_t)))
    {
        internal_pointer[i] = uint32_be_to_machine(val);
        i++;
    }
    return;
}

void mem_init_mergesort(unsigned int * internal_pointer)
{    // load the linked list into memory (SPM->GP)
    DBG(std::cout <<"[MERGESORT] pre-loading the linked list in memory" << std::endl);
    std::fstream ll_file;
    ll_file.open("../software/mergesort/datasets/100kB_linked_list.in", std::fstream::in);
    if(!ll_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();
    }
    unsigned int i = 0;
    uint64_t val = 0;
    while (ll_file >> val && i<(DENSE_MEM_SIZE/sizeof(uint32_t)))
    {
        internal_pointer[i] = uint32_be_to_machine(val);
        i++;
    }

    return;
}

void mem_init_patricia(unsigned int * internal_pointer)
{
#ifdef patricia
    // load the linked list into memory (SPM->GP)
    DBG(std::cout <<"[PATRICIA] pre-loading the file in memory (" << DATASET_PATH_IN << ")." << std::endl);
    std::fstream patricia_file;
    patricia_file.open(DATASET_PATH_IN, std::fstream::in);
    if(!patricia_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();
    }
    unsigned int c = 0;
    uint32_t val = 0;
    for(uint32_t i = 0; i<(PATRICIA_IN_FILE_SIZE/sizeof(uint32_t)); i++)
    {
        patricia_file.read((char *)&val, 4);
        internal_pointer[i] = uint32_be_to_machine(val);
        if(i<50){
            std::cout << val << "("<<uint32_be_to_machine(val)<<") ";
            if((i+1)%4 == 0)
                std::cout << std::endl;
        }
        c++;
    }
    DBG(std::cout << "[PATRICIA] loaded " << c << " bytes in dense memory" << std::endl);
    for (uint32_t i = 0; i < 50; i++)
    {
        std::cout << internal_pointer[i] << "(" << uint32_be_to_machine(internal_pointer[i]) << ") ";
        if ((i + 1) % 4 == 0)
            std::cout << std::endl;
    }
#endif
    return;
}

void mem_init_dijkstra(unsigned int * internal_pointer, std::string dataset_path, unsigned int ds_sz)
{
    // load the linked list into memory (SPM->GP)
    DBG(std::cout <<"[DIJKSTRA] pre-loading the file in memory (" << dataset_path << ")." << std::endl);
    std::fstream dijkstra_file;
    dijkstra_file.open(dataset_path, std::fstream::in);
    if(!dijkstra_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();
    }
    unsigned int c = 0;
    uint32_t val = 0;
    for(uint32_t i = 0; i<(ds_sz/sizeof(uint32_t)); i++)
    {
        dijkstra_file.read((char *)&val, 4);
        internal_pointer[i] = uint32_be_to_machine(val);
        c++;
    }
    DBG(std::cout << "[DIJKSTRA] loaded " << c << " bytes in dense memory" << std::endl);
    return;
}

void mem_init_jpeg(unsigned int * internal_pointer, std::string dataset_path,  unsigned int ds_sz)
{
    // load the input image into dense memory
    DBG(std::cout <<"[JPEG] pre-loading the input image in memory (" << dataset_path << ")." << std::endl);
    std::fstream jpg_data_file_in(dataset_path, (std::fstream::in | std::ios::binary));
    if(!jpg_data_file_in)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();
    }
    unsigned int i = 0;
    uint32_t val = 0;
    for(uint32_t i = 0; i<(ds_sz/sizeof(uint32_t)); i++)
    {
        jpg_data_file_in.read((char *)&val, 4);
        internal_pointer[i] = uint32_be_to_machine(val);
        if(i < 50)
            printf("[%d] %d(%d) ", i, val, internal_pointer[i]);
    }
    jpg_data_file_in.close();
    std::cout << "\t___size loaded : " << i << std::endl;
    return;
}

void mem_init_h263(char * internal_pointer_char, std::string dataset_path, unsigned int ds_sz)
{
    // load the input image into dense memory
    DBG(std::cout <<"[h263]TEST pre-loading the input video in memory (" << dataset_path << ")." << std::endl);
    std::fstream h263_data_file_in(dataset_path, (std::fstream::in | std::ios::binary));
    if(!h263_data_file_in)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();
    }
    unsigned int i = 0;
    char buffer=0;
    for(i = 0; i<(ds_sz/sizeof(int)); i++){
        h263_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)+3] = buffer;
        h263_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)+2] = buffer;
        h263_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)+1] = buffer;
        h263_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)] = buffer;
    }
    h263_data_file_in.close();
    std::cout << "\t___size loaded : " << i << std::endl;
    return;
}

void mem_init_jpg2000(unsigned int * internal_pointer, std::string dataset_path,  unsigned int ds_sz)
{
    // load the input image into dense memory
    DBG(std::cout <<"[JPG2000] pre-loading the input image in memory (" << dataset_path << ")." << std::endl);
    std::fstream jpg2000_data_file_in(dataset_path, (std::fstream::in | std::ios::binary));
    if(!jpg2000_data_file_in){
        std::cerr << "unable to open dataset" << std::endl;
        abort();
    }
    unsigned int i = 0;
    uint32_t val = 0;
    for(uint32_t i = 0; i<(ds_sz/sizeof(uint32_t)); i++)
    {
        jpg2000_data_file_in.read((char *)&val, 4);
        internal_pointer[i] = uint32_be_to_machine(val);
        if(i < 50)
            printf("[%d] %d(%d) ", i, val, internal_pointer[i]);
    }
    jpg2000_data_file_in.close();
    std::cout << "\t___size loaded : " << i << std::endl;
    return;
}



void retrieve_result_if_needed(unsigned int * base_pointer, unsigned int ds_sz,  unsigned int ds_stream_sz)
{

#ifdef jpeg
    char * internal_pointer_char = 0;
    // retrieve the output image from the dense memory
    DBG(std::cout <<"[JPEG] retrieving the output image from memory (" << DATASET_PATH_OUT << ")" << std::endl);
    std::fstream jpeg_data_file_out(DATASET_PATH_OUT, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)base_pointer + (uint64_t)(DENSE_MEM_SIZE/2));
    for(uint32_t i = 0; i<(ds_sz); i+=4){
        jpeg_data_file_out.write(&(internal_pointer_char[i+3]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+2]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+1]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    jpeg_data_file_out.close();
#endif

#ifdef h263
    char * internal_pointer_char = 0;
    // retrieve the raw output from the dense memory
    DBG(std::cout <<"[h263] retrieving the raw output from memory (" << DATASET_PATH_OUT << ")" << std::endl);
    std::fstream raw_data_file_out(DATASET_PATH_OUT, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)base_pointer + (uint64_t)(DENSE_MEM_SIZE/3));
    for(uint32_t i = 0; i<(ds_sz); i+=4){
        raw_data_file_out.write(&(internal_pointer_char[i+3]),1);
        raw_data_file_out.write(&(internal_pointer_char[i+2]),1);
        raw_data_file_out.write(&(internal_pointer_char[i+1]),1);
        raw_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    raw_data_file_out.close();
    // retrieve the output video from the dense memory
    DBG(std::cout <<"[h263] retrieving the h263 output from memory (" << DATASET_STREAM_OUT << ")" << std::endl);
    std::fstream h263_data_file_out(DATASET_STREAM_OUT, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)base_pointer + (uint64_t)(DENSE_MEM_SIZE*2/3));
    for(uint32_t i = 0; i<(ds_stream_sz); i+=4){
        h263_data_file_out.write(&(internal_pointer_char[i+3]),1);
        h263_data_file_out.write(&(internal_pointer_char[i+2]),1);
        h263_data_file_out.write(&(internal_pointer_char[i+1]),1);
        h263_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    h263_data_file_out.close();
#endif

#ifdef jpg2000
    char * internal_pointer_char = 0;
    // retrieve the output image from the dense memory
    DBG(std::cout <<"[JPG2000] retrieving the output image from memory (" << JPG2000_OUT_FILE_PATH << ")" << std::endl);
    std::fstream jpeg_data_file_out(JPG2000_OUT_FILE_PATH, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)base_pointer + (uint64_t)(DENSE_MEM_SIZE/2));
    for(uint32_t i = 0; i<(ds_sz); i+=4){
        jpeg_data_file_out.write(&(internal_pointer_char[i+3]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+2]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+1]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    jpeg_data_file_out.close();
#endif
}
