#ifndef H263_APP_H
#define H263_APP_H

// compile with modification in code to run on our simulator
#define DYCTON_RUN

// value definitions
#define CODE                        (0)
#define DECODE                      (1)

#define DATASET_YUV_SZ              (5474304) // 5.22 MB
#define DATASET_RAW_SZ              DATASET_YUV_SZ
#define DATASET_H263_SZ             (114043)  // 111.4 kB
#define OUT_FILE_ADDITIONAL_BITS    (0xF)

#define SMALL_DATASET_YUV_SZ              (1368576) // 1.4 MB
#define SMALL_DATASET_RAW_SZ              DATASET_YUV_SZ
#define SMALL_DATASET_H263_SZ             (49961)  // 50.0 kB
#define SMALL_OUT_FILE_ADDITIONAL_BITS    (0xF)


#define CMD_LINE_ENCODING_BASE       "h263enc -x 4 -a 0 -b 8 -s 15 -G -R 30.00 -r 3508000 -S 3 -Z 30.0 -O 0 -i input_base_4CIF_0to8.yuv -o output_base_4CIF_96bps_15.raw -B output_base_4CIF_96bps_15.263"
#define CMD_LINE_ENCODING_SMALL       "h263enc -x 3 -a 0 -b 8 -s 15 -G -R 30.00 -r 3508000 -S 3 -Z 30.0 -O 0 -i input_small.yuv -o output_small.raw -B output_small.263"
#define CMD_LINE_ENC_ARGC       (27)
#define CMD_LINE_ENC_ARGV_BASE      {"h263enc","-x","4","-a","0","-b","8","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","input_base_4CIF_0to8.yuv","-o","output_base_4CIF_96bps_15.raw","-B","output_base_4CIF_96bps_15.263"}
#define CMD_LINE_ENC_ARGV_SMALL       {"h263enc","-x","3","-a","0","-b","8","-s","15","-G","-R","30.00","-r","3508000","-S","3","-Z","30.0","-O","0","-i","input_small.yuv","-o","output_small.raw","-B","output_small.263"}

#define CMD_LINE_DECODING       "h263dec -o3 input_base_4CIF_96bps.263 output_base_4CIF_96bps_dec_%03d"

#define CMD_LINE_ENCODING CMD_LINE_ENCODING_SMALL
#define CMD_LINE_ENC_ARGV CMD_LINE_ENC_ARGV_SMALL



// choice of the application parameters
#define H263_FUNC                   (CODE)

#define H263_IN_ADDR                (DENSE_MEM_BASE)
#define H263_OUT_ADDR               (DENSE_MEM_BASE + (DENSE_MEM_SIZE/3))
#define H263_STREAM_ADDR            (DENSE_MEM_BASE + (DENSE_MEM_SIZE*2/3))



// other value definitions depending on application parameters
#if(H263_FUNC == DECODE)
#define DATASET_SZ_IN               DATASET_H263_SZ // in bytes
#define DATASET_SZ_OUT              DATASET_YUV_SZ // in bytes
#define DATASET_PATH_IN             "../software/h263/input_base_4CIF_96bps.263"
#define DATASET_PATH_OUT            "../software/h263/TODO" // (seems that output correspond to n ppm image :/)
#elif(H263_FUNC == CODE)
#define DATASET_SZ_IN               SMALL_DATASET_YUV_SZ // in bytes
#define DATASET_SZ_OUT              SMALL_DATASET_RAW_SZ // in bytes
#define DATASET_SZ_STREAM_OUT       SMALL_DATASET_H263_SZ // in bytes
#define DATASET_PATH_IN             "../software/h263/input_small.yuv"
#define DATASET_PATH_OUT            "../software/h263/output_small.raw"
#define DATASET_STREAM_OUT          "../software/h263/output_small.263"
#else
#error "H263 function not defined in software/H263/H263_app.h (code / decode)"
#endif

#endif //H263_APP_H