#ifndef JPG2000_APP_H
#define JPG2000_APP_H

#include "address_map.h"


#if 0
// base dataset
#define JPG2000_IN_FILE_PATH	"../software/jpg2000/src/appl/input_base_4CIF.pnm"
#define JPG2000_IN_FILE_SZ		(1216527) // bytes
#define JPG2000_OUT_FILE_PATH	"../software/jpg2000/src/appl/output_base_4CIF_96bps.jp2"
#define JPG2000_OUT_FILE_SZ		(12649) // bytes
#endif

// small dataset
#define JPG2000_IN_FILE_PATH	"../software/jpg2000/src/appl/input_small.pnm"
#define JPG2000_IN_FILE_SZ		(76086) // bytes
#define JPG2000_OUT_FILE_PATH	"../software/jpg2000/src/appl/output_small.jp2"
#define JPG2000_OUT_FILE_SZ		(784) // bytes



#define CMD_LINE_ENC_ARGV {"jasper","-f",JPG2000_IN_FILE_PATH, "-t", "pnm", "-F",JPG2000_OUT_FILE_PATH,"-T","jp2","-O","rate=0.010416667"}
#define CMD_LINE_ENC_ARGC (11)


#define JPG2000_IN_ADDR                (DENSE_MEM_BASE)
#define JPG2000_OUT_ADDR               (DENSE_MEM_BASE + (DENSE_MEM_SIZE/2))



#endif //JPG2000_APP_H