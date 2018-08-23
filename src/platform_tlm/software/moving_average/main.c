/********************************************************************
 * Copyright (C) 2017 CITI Lab - INSA de Lyon	                    *
 * Initial author: Tristan Delizy                                   *
 ********************************************************************/
/*
 * programming model :
 *
 * main()
 * {
 *  inputs i
 *  outputs o
 *  internal_state s
 *  time_to_next_boot t
 *  while(true) {
 *   i = get_inputs()
 *   {o,t,s} = compute(i,s)
 *   write_outputs(o)
 *   shut_down(t)
 *  }
 * }
 *
 *
 * moving average :
 *
 * based on static memory allocation, this application focus to develop
 * the normally of model of our simulation and test the simumlated platform
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "address_map.h"
#include "libdycton.h"
#include "hal.h"

#define TST_VAL 			(0XDEADBABA)
#define WINDOW_SIZE			(25)
#define MAX_ITE				(100)




unsigned int insert_and_compute_mov_avg(unsigned int input, unsigned int * internal_state, unsigned int count)
{
	int i = 0;
	unsigned int s = 0;
	unsigned int in_count = (count>WINDOW_SIZE)?WINDOW_SIZE:count;

	if(in_count == WINDOW_SIZE){
		// insersion of new element (DUMMY FOR NOW)
		for(i = 1; i< WINDOW_SIZE; i++){
			internal_state[i-1] = internal_state[i];
		}
		internal_state[WINDOW_SIZE-1] = input;
	}
	else{
		internal_state[in_count] = input;
		in_count++;
	}

	// moving average sum computation, still dummy
	for(i = 0; i< in_count; i++){
		s+=internal_state[i];
	}

	// this is really DUMMY
	return ((s+(in_count/2)) / in_count);

}


int main(void)
{
	unsigned int internal_state[WINDOW_SIZE] = {0};
    unsigned int input = 0;
    unsigned int output = 0;
	int ite = 0;

	printf("\n\n\n====================== MOVING AVERAGE ======================\n");
	printf("(TDk @ CITI Lab)\n\n");

    // memset(&internal_state, 2, sizeof(unsigned int)*WINDOW_SIZE);

    printf("entering loop \n");
	for(;;){

		input = read_mem(IN_BUF_BASE);
		if(input == EOF)
			break;
		output = insert_and_compute_mov_avg(input, internal_state, ite);
		write_mem(OUT_BUF_BASE, output);

		printf("iteration %d\n",ite);
		printf("\tinput = %d\n",input);
		printf("\tmoving average = %d\n",output);

		// if(ite%10==0){
		// 	for(int i=0; i<WINDOW_SIZE; i++){
		// 		print_dec(internal_state[i]);print(" ");
		// 	}
		// 	print("\n");
		// }


		// instrumentation
		if(++ite >= (int)MAX_ITE)
			break;
	}

    printf(" \n\n\n\n");
	printf("%d iterations done.\n",ite);
    printf("\n=============================== THE END ===============================\n");
    exit(0);

    return 0;

}
