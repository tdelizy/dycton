/* Naive matrix multiplication algorithm
 *
 * the matrices are considered already loaded consecutively in memory with first node at TST_BASE address
 *
 * TDk @ CITI Lab 2017
*/

#include <stdio.h>
#include <string.h>

#include "address_map.h"
#include "libdycton.h"

#define MAT_DIM (25) // for data size of 5 kB
// #define MAT_DIM (36) // for data size of 10 kB
// #define MAT_DIM (80) // for data size of 50 kB
// #define MAT_DIM (113) // for data size of 100 kB
// #define MAT_DIM (253) // for data size of 500 kB


int (* A)[MAT_DIM][MAT_DIM] = (int (*)[MAT_DIM][MAT_DIM])TST_BASE;
int (* B)[MAT_DIM][MAT_DIM] = (int (*)[MAT_DIM][MAT_DIM])(TST_BASE + (MAT_DIM * MAT_DIM * sizeof(int)));


void matrix_multiplication(int (* mat_A)[MAT_DIM][MAT_DIM], int (* mat_B)[MAT_DIM][MAT_DIM], int (* mat_res)[MAT_DIM][MAT_DIM])
{
    int i, j, k;
    for(i=0; i<MAT_DIM; ++i){
        for(j=0; j<MAT_DIM; ++j){
            for(k=0; k<MAT_DIM; ++k){
                ((*mat_res)[i][j])+=((*mat_A)[i][k])*((*mat_B)[k][j]);
            }
            // print("j:");print_dec(i);print("\n");
        }
        // print("i:");print_dec(i);print("\n");
    }
}


void printmat(int (* mat)[MAT_DIM][MAT_DIM])
{
    int i, j;

    for (i = 0; i < MAT_DIM; ++i)
    {
        for (j = 0; j < MAT_DIM; ++j)
        {
            printf("%d",(*mat)[i][j]);
            printf(" \t");
            if (j == MAT_DIM - 1)
                printf("\n");
        }
    }
}



int main(void)
{
	printf("\n\n\n================== Matrix Multiplication for DYCTON ==================\n");
	printf("(TDk @ CITI Lab)\n\n");

    int (* R)[MAT_DIM][MAT_DIM];

    R = (int (*)[MAT_DIM][MAT_DIM])malloc(MAT_DIM * MAT_DIM * sizeof(int));
    memset(R, 0, MAT_DIM * MAT_DIM * sizeof(int));

    printf("TST_BASE: 0x%X\n",TST_BASE);
    printf("TST_BASE + (MAT_DIM * MAT_DIM * sizeof(int)): 0x%X\n",(TST_BASE + (MAT_DIM * MAT_DIM * sizeof(int))));

    printf("A: 0x%X\n",(unsigned int)A);
    printf("B: 0x%X\n",(unsigned int)B);
    printf("R: 0x%X\n",(unsigned int)R);

    // // display matrices to multiply
    // print("input matrix A:\n");
    // printmat(A);

    // print("input matrix B:\n");
    // printmat(B);

    // print("result matrix R (initial state):\n");
    // printmat(R);

    printf("starting computations...\n");

    matrix_multiplication(A, B, R);

    printf("success.\n");

    // display the result
    // print("Output Matrix R = AxB:\n");
    // printmat(R);

	printf("\n=========================== THE END ===========================\n");
	exit(0);

	return 0;

}






