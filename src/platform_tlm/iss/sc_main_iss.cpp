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

namespace soclib {
namespace common {
extern bool elf_load(const std::string &filename,
                     soclib::common::Loader &loader);
}
};

// definition of debug global variables
__DY_DBG_DEF

const uint32_t heap_base = (HEAP_BASE);
const uint32_t heap_end = (HEAP_BASE + HEAP_SIZE);
uint32_t heap_footprint = 0;
uint64_t total_size_allocated = 0;
PlatformLogSystem * platlog;
uint64_t malloc_cycles = 0;
uint64_t free_cycles = 0;
Oracle * Tiresias;
uint32_t Strategy;
uint32_t malloc_count = 0;
uint32_t malloc_fallback = 0;
std::string oracle_filename;
std::string Strat_string_array[STRAT_COUNT] = {
    DEFAULT_STRAT_STRING,
    ORACLE_STRAT_STRING
};

#define DBG(x) DY_DBG(x, SCMAIN_ID, NULL_SUB_ID)



int sc_main(int argc, char *argv[]) {

    // command line parsing
    int option = 0;
    std::string strat_name;
    while ((option = getopt (argc, argv, "s:f:c")) != -1)
        switch (option)
        {
        case 's':
            strat_name = optarg;
            std::cout << "strategie : " << optarg << std::endl;
            break;
        case 'f':
            std::cout << "file path (for oracle strategy) : " << optarg << std::endl;
            oracle_filename = optarg;
            break;
        case 'c':
            std::cout << "colors in UART output disabled." << std::endl;
            colors_in_uart_output = 0;
            break;
        default:
            return 0;
        }

    for(int i=0; i<STRAT_COUNT; i++){
        if(strat_name == Strat_string_array[i])
            Strategy = i;
    }
    std::cout << "placement strategy : " << Strat_string_array[Strategy] << " (" << Strategy << ")" << std::endl;

    std::cout << "memory architecture : " << DY_ARCH <<std::endl;

    // wall clock time logging
    struct timeval real_time_start;
    struct timeval real_time_end;

    // first initialize log and trace system and configure here
    // what is going to be output on std::out and which module will generates log.
    // see platform_tlm/log_trace.h
    std::fstream access_log_file("../logs/mem_access.log", std::fstream::out);
    std::fstream timing_file("../logs/cycles", std::fstream::out);
    std::fstream object_log_file("../logs/heap_objects.log", std::fstream::out);
    std::fstream oracle_file(oracle_filename,  std::fstream::in);
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

    DBG(std::cout <<"[DYCTON] platform components instantiation ============================" << std::endl);
    DBG(std::cout <<"PLATFORM FREQUENCY : " << PROC_FREQ << "Hz" << std::endl);

#if (HEAP_COUNT == 2) // 2 heaps architectures

    // memory subsystem memory characteristics :
    unsigned int spm_sizes[] = {SPM_I_SIZE, SPM_D_SIZE, SPM_S_SIZE, HEAP_0_SIZE, HEAP_1_SIZE};
    unsigned int spm_r_lat[] = {1, 1, 1, HEAP_0_RLAT, HEAP_1_RLAT};
    unsigned int spm_w_lat[] = {1, 1, 1, HEAP_0_WLAT, HEAP_1_WLAT};
    unsigned int spm_ports = 3;
    unsigned int spm_banks = 5;
    unsigned int spm_prio[] = {4, 3, 2, 1, 0};

#elif (HEAP_COUNT == 1) // one heap architectures
    // memory subsystem memory characteristics :
    unsigned int spm_sizes[] = {SPM_I_SIZE, SPM_D_SIZE, SPM_S_SIZE, HEAP_SIZE};
    unsigned int spm_r_lat[] = {1, 1, 1, HEAP_0_RLAT};
    unsigned int spm_w_lat[] = {1, 1, 1, HEAP_0_WLAT};
    unsigned int spm_ports = 3;
    unsigned int spm_banks = 4;
    unsigned int spm_prio[] = {3, 2, 1, 0};
#else // error for now
    #error 'trying to select architecture with more than 2 heaps, not possible for now, see sc_main.cpp and address_map.h'
#endif


    // ISS and wrapper instanciation
    // workaround : for iss wrapper stack is at top of data. its the case in
    // our memory architecture but not in the same bank, but don't need to tell it to the iss
    MIPS32Wrapper cpu("MyWrap", 0, TEXT_BASE, TEXT_SIZE, DATA_BASE, SPM_D_SIZE + SPM_S_SIZE, SPM_GP_BASE, SPM_SIZE);

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
    Helper helper("helper", Tiresias);
    UART uart("uart");

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
    GPBuffer in_buf("Input_buffer", "../software/json_parser/datasets/walking_dead_short.json", "../logs/gib.log", GPBuffer::BUFF_IN);
    GPBuffer out_buf("Output_buffer", "../software/json_parser/gpb.output", "../logs/gob.log", GPBuffer::BUFF_OUT);
#endif


    DBG(std::cout <<"[DYCTON] signal declaration ===========================================" << std::endl);

    sc_core::sc_signal<bool> cpu_irq("cpu_irq"); // stub for now



    DBG(std::cout <<"[DYCTON] memory content loading =======================================" << std::endl);

    // Load the program in RAM
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
    } catch (soclib::exception::RunTimeError e) {
        std::cerr << "unable to load ELF file in memory:" << std::endl;
        std::cerr << e.what() << std::endl;
        abort();
    }

#ifdef matmul
    DBG(std::cout <<"[MATMUL] pre-loading the matrices in memory" << std::endl);
    std::fstream mat_file;
    mat_file.open("../software/matmul/datasets/10kB_matrices.in", std::fstream::in);
    if(!mat_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();  
    }
    internal_pointer = dense_mem.storage;
    unsigned int i = 0;
    int val = 0;
    while (mat_file >> val && i<(DENSE_MEM_SIZE/sizeof(uint32_t)))
    {
        internal_pointer[i] = uint32_be_to_machine(val);
        i++;
    }
#endif

#ifdef mergesort
    // load the linked list into memory (SPM->GP)
    DBG(std::cout <<"[MERGESORT] pre-loading the linked list in memory" << std::endl);
    std::fstream ll_file;
    ll_file.open("../software/mergesort/datasets/100kB_linked_list.in", std::fstream::in);
    if(!ll_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();  
    }
    internal_pointer = dense_mem.storage;
    unsigned int i = 0;
    uint64_t val = 0;
    while (ll_file >> val && i<(DENSE_MEM_SIZE/sizeof(uint32_t)))
    {
        internal_pointer[i] = uint32_be_to_machine(val);
        i++;
    }
#endif

#ifdef patricia
    #include "../software/patricia/dycton_patricia_app.h"
    // load the linked list into memory (SPM->GP)
    DBG(std::cout <<"[PATRICIA] pre-loading the file in memory (" << DATASET_PATH_IN << ")." << std::endl);
    std::fstream patricia_file;
    patricia_file.open(DATASET_PATH_IN, std::fstream::in);
    if(!patricia_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();  
    }
    internal_pointer = dense_mem.storage;
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

#ifdef dijkstra
    #include "../software/dijkstra/dycton_dijkstra_app.h"
    // load the linked list into memory (SPM->GP)
    DBG(std::cout <<"[DIJKSTRA] pre-loading the file in memory (" << DATASET_PATH_IN << ")." << std::endl);
    std::fstream dijkstra_file;
    dijkstra_file.open(DATASET_PATH_IN, std::fstream::in);
    if(!dijkstra_file)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();  
    }
    internal_pointer = dense_mem.storage;
    unsigned int c = 0;
    uint32_t val = 0;
    for(uint32_t i = 0; i<(DIJKSTRA_IN_FILE_SIZE/sizeof(uint32_t)); i++)
    {
        dijkstra_file.read((char *)&val, 4);
        internal_pointer[i] = uint32_be_to_machine(val);
        c++;
    }
    DBG(std::cout << "[DIJKSTRA] loaded " << c << " bytes in dense memory" << std::endl);
#endif

#ifdef jpeg
#include "../software/jpeg/jpeg_app.h"
    // load the input image into dense memory
    DBG(std::cout <<"[JPEG] pre-loading the input image in memory (" << DATASET_PATH_IN << ")." << std::endl);
    std::fstream jpeg_data_file_in(DATASET_PATH_IN, (std::fstream::in | std::ios::binary));
    if(!jpeg_data_file_in)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();  
    }
    char * internal_pointer_char = (char *)dense_mem.storage;
    unsigned int i = 0;
    char buffer=0;
    // had to handle endianness by hand as PPM is basically a text file
    for(i = 0; i<(DATASET_SZ_IN/sizeof(int)); i++){
        jpeg_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)+3] = buffer;
        jpeg_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)+2] = buffer;
        jpeg_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)+1] = buffer;
        jpeg_data_file_in.get(buffer);
        internal_pointer_char[i*sizeof(int)] = buffer;
    }
    jpeg_data_file_in.close();
    std::cout << "\t>>>size loaded : " << i << std::endl;
#endif

#ifdef h263
#include "../software/h263/h263_app.h"
    // load the input image into dense memory
    DBG(std::cout <<"[h263]TEST pre-loading the input video in memory (" << DATASET_PATH_IN << ")." << std::endl);
    std::fstream h263_data_file_in(DATASET_PATH_IN, (std::fstream::in | std::ios::binary));
    if(!h263_data_file_in)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();  
    }
    char * internal_pointer_char = (char *)dense_mem.storage;
    unsigned int i = 0;
    char buffer=0;
    for(i = 0; i<(DATASET_SZ_IN/sizeof(int)); i++){
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
    std::cout << "\t>>>size loaded : " << i << std::endl;
#endif

#ifdef jpg2000
#include "../software/jpg2000/jpg2000_app.h"
    // load the input image into dense memory
    DBG(std::cout <<"[JPG2000] pre-loading the input image in memory (" << JPG2000_IN_FILE_PATH << ")." << std::endl);
    std::fstream jpg2000_data_file_in(JPG2000_IN_FILE_PATH, (std::fstream::in | std::ios::binary));
    if(!jpg2000_data_file_in)
    {
        std::cerr << "unable to open dataset" << std::endl;
        abort();  
    }
    char * internal_pointer_char = (char *)dense_mem.storage;
    unsigned int i = 0;
    char buffer=0;
    unsigned int c = 0;
    uint32_t val = 0;
    internal_pointer = dense_mem.storage;
    for(uint32_t i = 0; i<(JPG2000_IN_FILE_SZ/sizeof(uint32_t)); i++)
    {
        jpg2000_data_file_in.read((char *)&val, 4);
        internal_pointer[i] = uint32_be_to_machine(val);
        if(i < 50)
        	printf("[%d] %d(%d) ", i, val, internal_pointer[i]);
        c++;
    }
    jpg2000_data_file_in.close();
    std::cout << "\t>>>size loaded : " << i << std::endl;
#endif

    DBG(std::cout <<"[DYCTON] mapping ======================================================" << std::endl);

    // construct memory architecture file while binding simulation components
    MemArchi* dy_mem_arch = new MemArchi(8);

    // initiators
    cpu.i_socket.bind(spm.sockets[0]);
    cpu.d_socket.bind(spm.sockets[1]);
    cpu.b_socket.bind(bus.target_sockets);

    // interrupts
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

    bus.bind_target(spm.sockets[2], SPM_BASE, SPM_SIZE);
    spm.set_bank_base_address(0, TEXT_BASE);
    spm.set_bank_base_address(1, DATA_BASE);
    spm.set_bank_base_address(2, DATA_BASE+DATA_SIZE);

    // heap(s) managment
    spm.set_bank_base_address(3, HEAP_0_BASE);
    dy_mem_arch->add("HEAP_0", HEAP_0_BASE, HEAP_0_SIZE, HEAP_0_RLAT, HEAP_0_WLAT);
#if (HEAP_COUNT > 1)
    spm.set_bank_base_address(4, HEAP_1_BASE);
    dy_mem_arch->add("HEAP_1", HEAP_1_BASE, HEAP_1_SIZE, HEAP_1_RLAT, HEAP_1_WLAT);
#if (HEAP_COUNT > 2)
    spm.set_bank_base_address(5, HEAP_2_BASE);
    dy_mem_arch->add("HEAP_2", HEAP_2_BASE, HEAP_2_SIZE, HEAP_2_RLAT, HEAP_2_WLAT);
#if (HEAP_COUNT > 3)
    spm.set_bank_base_address(6, HEAP_3_BASE);
    dy_mem_arch->add("HEAP_3", HEAP_3_BASE, HEAP_3_SIZE, HEAP_3_RLAT, HEAP_3_WLAT);
#if (HEAP_COUNT > 4)
    spm.set_bank_base_address(7, HEAP_4_BASE);
    dy_mem_arch->add("HEAP_4", HEAP_4_BASE, HEAP_4_SIZE, HEAP_4_RLAT, HEAP_4_WLAT);
#if (HEAP_COUNT > 5 && HEAP_COUNT < 7)
    spm.set_bank_base_address(8, HEAP_5_BASE);
    dy_mem_arch->add("HEAP_5", HEAP_5_BASE, HEAP_5_SIZE, HEAP_5_RLAT, HEAP_5_WLAT);
#else
#error "TOO MANY HEAPS FOR STATIC INIT, SEE SC_MAIN.CPP"
#endif
#endif
#endif
#endif
#endif

    bus.bind_target(dense_mem.target, DENSE_MEM_BASE, DENSE_MEM_SIZE);
    dense_mem.set_base_address(DENSE_MEM_BASE);

    bus.bind_target(uart.target, UART_BASEADDR, UART_SIZE);

    bus.bind_target(helper.target, HELPER_BASE, HELPER_SIZE);


    std::fstream dy_mem_arch_file("../logs/memory_architecture", std::fstream::out);
    dy_mem_arch->print(dy_mem_arch_file);

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

#if (HEAP_COUNT > 1)
    heap_1_r = spm.get_bank_read_count(4);
    heap_1_w = spm.get_bank_write_count(4);
#if (HEAP_COUNT > 2)
    #warning "no stats for more than 2 heaps for now."
#endif
#endif

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
    DBG(std::cout << "[malloc fallback: " << std::dec << malloc_fallback << "(" << ((float)malloc_fallback*100)/(float)malloc_count << "%]\n" << std::endl);

    DBG(std::cout << "[final cycle count: " << std::dec << total_cycle_count << "]" << std::endl);
    DBG(std::cout << "[malloc cycle count: " << std::dec << malloc_cycles << " ("<< (float)(malloc_cycles*100)/(float)total_cycle_count  << "%)]" << std::endl);
    DBG(std::cout << "[free cycle count: " << std::dec << free_cycles << " ("<< (float)(free_cycles*100)/(float)total_cycle_count  << "%)]" << std::endl);
    DBG(std::cout << "[total allocator cycle count: " << std::dec << (malloc_cycles + free_cycles) << " ("<< (float)(malloc_cycles*100 + free_cycles*100)/(float)total_cycle_count  << "%)]" << std::endl);
    timing_file << std::dec << cpu.get_cycles()<<std::endl;
    timing_file << std::dec << cpu.get_cycles()<<std::endl;
    timing_file.close();


#ifdef jpeg
    // retrieve the output image from the dense memory
    DBG(std::cout <<"[JPEG] retrieving the output image from memory (" << DATASET_PATH_OUT << ")" << std::endl);
    std::fstream jpeg_data_file_out(DATASET_PATH_OUT, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)dense_mem.storage + (uint64_t)(DENSE_MEM_SIZE/2));
    for(uint32_t i = 0; i<(DATASET_SZ_OUT+OUT_FILE_ADDITIONAL_BITS); i+=4){
        jpeg_data_file_out.write(&(internal_pointer_char[i+3]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+2]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+1]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    jpeg_data_file_out.close();
#endif

#ifdef h263
    // retrieve the raw output from the dense memory
    DBG(std::cout <<"[h263] retrieving the raw output from memory (" << DATASET_PATH_OUT << ")" << std::endl);
    std::fstream raw_data_file_out(DATASET_PATH_OUT, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)dense_mem.storage + (uint64_t)(DENSE_MEM_SIZE/3));
    for(uint32_t i = 0; i<(DATASET_RAW_SZ+OUT_FILE_ADDITIONAL_BITS); i+=4){
        raw_data_file_out.write(&(internal_pointer_char[i+3]),1);
        raw_data_file_out.write(&(internal_pointer_char[i+2]),1);
        raw_data_file_out.write(&(internal_pointer_char[i+1]),1);
        raw_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    raw_data_file_out.close();
    // retrieve the output video from the dense memory
    DBG(std::cout <<"[h263] retrieving the h263 output from memory (" << DATASET_STREAM_OUT << ")" << std::endl);
    std::fstream h263_data_file_out(DATASET_STREAM_OUT, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)dense_mem.storage + (uint64_t)(DENSE_MEM_SIZE*2/3));
    for(uint32_t i = 0; i<(DATASET_H263_SZ+OUT_FILE_ADDITIONAL_BITS); i+=4){
        h263_data_file_out.write(&(internal_pointer_char[i+3]),1);
        h263_data_file_out.write(&(internal_pointer_char[i+2]),1);
        h263_data_file_out.write(&(internal_pointer_char[i+1]),1);
        h263_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    h263_data_file_out.close();
#endif

#ifdef jpg2000
    // retrieve the output image from the dense memory
    DBG(std::cout <<"[JPG2000] retrieving the output image from memory (" << JPG2000_OUT_FILE_PATH << ")" << std::endl);
    std::fstream jpeg_data_file_out(JPG2000_OUT_FILE_PATH, (std::fstream::out | std::ios::binary));
    internal_pointer_char = (char *)((uint64_t)dense_mem.storage + (uint64_t)(DENSE_MEM_SIZE/2));
    for(uint32_t i = 0; i<(JPG2000_OUT_FILE_SZ); i+=4){
        jpeg_data_file_out.write(&(internal_pointer_char[i+3]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+2]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i+1]),1);
        jpeg_data_file_out.write(&(internal_pointer_char[i]),1);
    }
    jpeg_data_file_out.close();
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

    delete dy_mem_arch;
    dy_deinit_logtrace();
    delete platlog;
    return 0;
}