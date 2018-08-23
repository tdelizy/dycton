
#include <stdio.h>

#include "../../address_map.h"
#include "lzw-lib.h"
#include "hal.h"
#include "libdycton.h"

#define DECOMPRESS


#define TST_VAL 	(0XDEADBABA)
#define MSG_LEN		(16)

int char_counter = 0;

// callbacks for lzw library communication
void print_out(int value)
{
	write_mem(OUT_BUF_BASE, value);
	return;
}

int read_in(void)
{
	unsigned int buff = read_mem(IN_BUF_BASE);
	return buff;
}


int main(void)
{
	int ret = 0;
	printf("\n\n\n================== LZW encoding test app for DYCTON ===================\n");
	printf("(TDk @ CITI Lab)\n\n");

#if defined(COMPRESS)
	printf("compressing stream from input buffer\n");
	ret = lzw_compress (print_out, read_in, 12);
#elif defined(DECOMPRESS)
	printf("decompressing stream from input buffer\n");
	ret = lzw_decompress (print_out, read_in);
#else
	#error lzw app needs environment variable "COMPRESS" or "DECOMPRESS" to be set.
#endif
    if(ret){
        goto kp;
    }

	print_out(EOF);

    printf("\nsuccess !\n");

	printf("\n=============================== THE END ===============================\n");
	exit(0);


kp:
	printf("\nerror during test");
    exit(1);

	return 0;

}
