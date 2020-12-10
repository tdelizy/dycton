#ifndef JPEG_APP_H
#define JPEG_APP_H

#include "address_map.h"

// value definitions
#define CODE                        (0)
#define DECODE                      (1)
#define SMALL                       (0)
#define LARGE                       (1)
#define SMALL_DATASET_PPM_SZ        (196666)
#define LARGE_DATASET_PPM_SZ        (786490)
#define SMALL_DATASET_JPEG_SZ       (9810)
#define LARGE_DATASET_JPEG_SZ       (19713)
#define OUT_FILE_ADDITIONAL_BITS    (0xF)




// choice of the application parameters
#define JPEG_FUNC                   (CODE)
#define JPEG_DATA                   (SMALL)

#define JPEG_IN_ADDR                (DENSE_MEM_BASE)
#define JPEG_OUT_ADDR               (DENSE_MEM_BASE + (DENSE_MEM_SIZE/2))



// other value definitions depending on application parameters
#if(JPEG_FUNC == DECODE)
#if(JPEG_DATA == SMALL)
#define DATASET_SZ_IN               SMALL_DATASET_JPEG_SZ // in bytes
#define DATASET_SZ_OUT              SMALL_DATASET_PPM_SZ // in bytes
#define DATASET_PATH_IN             "../software/jpeg/input_small.jpg"
#define DATASET_PATH_OUT            "../software/jpeg/output_small.ppm"
#else
#define DATASET_SZ_IN               LARGE_DATASET_JPEG_SZ // in bytes
#define DATASET_SZ_OUT              LARGE_DATASET_PPM_SZ // in bytes
#define DATASET_PATH_IN             "../software/jpeg/input_large.jpg"
#define DATASET_PATH_OUT            "../software/jpeg/output_large.ppm"
#endif
#elif(JPEG_FUNC == CODE)
#if(JPEG_DATA == SMALL)
#define DATASET_SZ_IN               SMALL_DATASET_PPM_SZ // in bytes
#define DATASET_SZ_OUT              SMALL_DATASET_JPEG_SZ // in bytes
#define DATASET_PATH_IN             "../software/jpeg/input_small.ppm"
#define DATASET_PATH_OUT            "../software/jpeg/output_small.jpg"
#else
#define DATASET_SZ_IN               LARGE_DATASET_PPM_SZ // in bytes
#define DATASET_SZ_OUT              LARGE_DATASET_JPEG_SZ // in bytes
#define DATASET_PATH_IN             "../software/jpeg/input_large.ppm"
#define DATASET_PATH_OUT            "../software/jpeg/output_large.jpg"
#endif
#else
#error "JPEG function not defined in software/jpeg/dycton_jpeg_app.h (code / decode)"
#endif

#define DATASET_COUNT 		(8)


/*
 * WARNING !
 * 
 * following datatests crash at exec:
 *	input_1.pnm
 *	input_3.pnm
 *	gradient.pnm
 *	vitrail.pnm
 *
 * the corresponding memory footprints are set to 0, allowing only execution on architecture -2
 */



const char * dataset_array[DATASET_COUNT] = {	"../software/jpeg/datasets/input_0.pnm", 
												"../software/jpeg/datasets/input_1.pnm", 
												"../software/jpeg/datasets/input_2.pnm", 
												"../software/jpeg/datasets/input_3.pnm", 
												"../software/jpeg/datasets/gradient.pnm", 
												"../software/jpeg/datasets/lena.pnm", 
												"../software/jpeg/datasets/maximum_the_color.pnm", 
												"../software/jpeg/datasets/vitrail.pnm"};
											
uint32_t dataset_footprint[DATASET_COUNT] = {	110784,
												500000, 
												233760, 
												500000,
												500000, 
												110928, 
												108816,
												115440};

uint32_t dataset_in_size[DATASET_COUNT] = {	76086,
											0,
											187560,
											0,
											0,
											67560,
											64860,
											0};

uint32_t dataset_out_size[DATASET_COUNT] = {4983,
											11572,
											7534,
											9810,
											2605,
											5013,
											6615,
											11831};



#endif //JPEG_APP_H