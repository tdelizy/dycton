#ifndef H263_APP_H
#define H263_APP_H

// compile with modification in code to run on our simulator
#define DYCTON_RUN

#define SMALL_DATASET_YUV_SZ              (1368576) // 1.4 MB
#define SMALL_DATASET_RAW_SZ              SMALL_DATASET_YUV_SZ
#define SMALL_DATASET_H263_SZ             (49961)  // 50.0 kB


#define H263_IN_ADDR                (DENSE_MEM_BASE)
#define H263_OUT_ADDR               (DENSE_MEM_BASE + (DENSE_MEM_SIZE/3))
#define H263_STREAM_ADDR            (DENSE_MEM_BASE + (DENSE_MEM_SIZE*2/3))


// other value definitions depending on application parameters
#define DATASET_PATH_OUT            "../software/h263/datasets/output.raw"
#define DATASET_STREAM_OUT          "../software/h263/datasets/output.263"

#define CMD_LINE_ENC_ARGC       (27)

#define DATASET_COUNT 		(8)

const char * dataset_array[DATASET_COUNT] = {	"../software/h263/datasets/input_0_i420_352x288.yuv",
												"../software/h263/datasets/input_1_i420_176x144.yuv",
												"../software/h263/datasets/input_2_i420_176x144.yuv",
												"../software/h263/datasets/input_3_i420_352x288.yuv",
												"../software/h263/datasets/input_4_i420_352x288.yuv",
												"../software/h263/datasets/input_5_i420_176x144.yuv",
												"../software/h263/datasets/input_6_i420_176x144.yuv",
												"../software/h263/datasets/input_7_i420_352x288.yuv"};

const char * command_lines_array[DATASET_COUNT][CMD_LINE_ENC_ARGC] = { 
			{"h263enc","-x","3","-a","0","-b","8","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
			{"h263enc","-x","2","-a","0","-b","5","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
			{"h263enc","-x","2","-a","0","-b","30","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
			{"h263enc","-x","3","-a","0","-b","11","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
			{"h263enc","-x","3","-a","0","-b","9","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
			{"h263enc","-x","2","-a","0","-b","49","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
			{"h263enc","-x","2","-a","0","-b","39","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
			{"h263enc","-x","3","-a","0","-b","11","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","stubbed","-o","output.raw","-B","output.263"},
};

											
uint32_t dataset_footprint[DATASET_COUNT] = {	1262320,
												368200,
												368224,
												1419224,
												1419208,
												370040,
												368224,
												1419224};

uint32_t dataset_in_size[DATASET_COUNT] = {	1368576,
											228096,
											1178496,
											1824768,
											1520640,
											1900800,
											1520640,
											1824768};


uint32_t dataset_out_size[DATASET_COUNT] = {1368576,
											228096,
											1178496,
											1824768,
											1520640,
											1900800,
											1520640,
											1824768};

uint32_t dataset_stream_out_size[DATASET_COUNT] = {	49961,
													34941,
													233312,
													159505,
													115080,
													358231,
													472695,
													167376};




#endif //H263_APP_H