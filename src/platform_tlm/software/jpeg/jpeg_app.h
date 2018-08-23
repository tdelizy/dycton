#ifndef JPEG_APP_H
#define JPEG_APP_H

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
#error "JPEG function not defined in software/jpeg/jpeg_app.h (code / decode)"
#endif

#endif //JPEG_APP_H