#ifndef DYCTON_DIJKSTRA_APP_H
#define DYCTON_DIJKSTRA_APP_H


#define DYCTON_RUN


#define DIJKSTRA_IN_FILE_ADDR       (DENSE_MEM_BASE)

#define DATASET_COUNT 		(8)

const char * dataset_array[DATASET_COUNT] = {"../software/dijkstra/datasets/input_1.dat", 
											"../software/dijkstra/datasets/input_2.dat",
											"../software/dijkstra/datasets/input_3.dat",
											"../software/dijkstra/datasets/input_4.dat",
											"../software/dijkstra/datasets/input_5.dat",
											"../software/dijkstra/datasets/input_6.dat",
											"../software/dijkstra/datasets/input_7.dat",
											"../software/dijkstra/datasets/input_8.dat"};
											
uint32_t dataset_footprint[DATASET_COUNT] = {	10128, 
												13416,
												10848,
												10004,
												10542,
												10122,
												5128,
												8942};


uint32_t dataset_in_size[DATASET_COUNT] = {	29144,
											25914,
											28845,
											29217,
											29225,
											29214,
											40100,
											21039};






#endif //DYCTON_DIJKSTRA_APP_H