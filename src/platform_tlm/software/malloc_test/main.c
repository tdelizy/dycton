#include <stdio.h>
#include <stdlib.h>

#include "hal.h"
#include "libdycton.h"
#include "address_map.h"


#define SIM_END            (*((int*)HELPER_BASE)=0)

#define ALLOC_SZ           (24)
#define NB_ALLOC            (120)


void test();

int global_test[1024];

void tempo_dbg(int * nop)
{
    for (int i = 0; i < 1024; i++) {
        global_test[i] = i;
        nop = &global_test[i % 5];
    }
}

int main(void)
{
    printf("\n\n\n================== Basic test for DYCTON ==================\n");
    printf("(TDk @ CITI Lab)\n\n");

	FILE * test_fd = NULL;
	char* test_mem = malloc(100 * sizeof(char));
	memset((void*)test_mem, (int)'P', 100);
	test_fd = fmemopen((void*)test_mem, 100, "rb");
	rewind(test_fd);
	char buffer = 0;
	for (int i = 0; i < 50; i++) {
		if (i % 4 == 0) {
			printf("\n");
			printf("[%d] ", i / 4);
		}
		fread(&buffer, sizeof(char), 1, test_fd);
		if (i < 50)
			printf("%d(%d) ", buffer, *(char*)((int)test_mem + i));
	}

    printf("\n\ndone");
    printf("\n=========================== THE END ===========================\n");
    exit(0);
    return 0;
error:
    printf("\nFAILURE\n");
    return 0;
}


 // size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);