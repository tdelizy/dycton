#ifndef JPG2000_APP_H
#define JPG2000_APP_H

#include "address_map.h"



#define CMD_LINE_ENC_ARGV {"jasper","--verbose","-f",JPG2000_IN_FILE_PATH, "-t", "pnm", "-F",JPG2000_OUT_FILE_PATH,"-T","jp2","-O","rate=0.010416667"}
#define CMD_LINE_ENC_ARGC (12)

#define JPG2000_OUT_FILE_PATH	"../software/jpg2000/output.jp2"

#define JPG2000_IN_ADDR                (DENSE_MEM_BASE)
#define JPG2000_OUT_ADDR               (DENSE_MEM_BASE + (DENSE_MEM_SIZE/2))

#define DATASET_COUNT 		(8)

const char * dataset_array[DATASET_COUNT] = {	"../software/jpg2000/datasets/input_0.pnm", 
												"../software/jpg2000/datasets/input_1.pnm", 
												"../software/jpg2000/datasets/input_2.pnm", 
												"../software/jpg2000/datasets/input_3.pnm", 
												"../software/jpg2000/datasets/gradient.pnm", 
												"../software/jpg2000/datasets/lena.pnm", 
												"../software/jpg2000/datasets/maximum_the_color.pnm", 
												"../software/jpg2000/datasets/vitrail.pnm"};
											
uint32_t dataset_footprint[DATASET_COUNT] = {	1715760,
												3305536, 
												2791424, 
												2943040,
												2807480, 
												1638760, 
												1526528,
												1568616};

uint32_t dataset_in_size[DATASET_COUNT] = {	76086,
											206310,
											187560,
											196668,
											196668,
											67560,
											64860,
											70860};

uint32_t dataset_out_size[DATASET_COUNT] = {784,
											2128,
											21280,
											21280,
											21280,
											21280,
											21280,
											752};
#endif //JPG2000_APP_H