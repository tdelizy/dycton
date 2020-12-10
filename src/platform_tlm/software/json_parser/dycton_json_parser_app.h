#ifndef DYCTON_JSON_PARSER_APP_H
#define DYCTON_JSON_PARSER_APP_H
/*
 * caution ! 
 * 
 * this file is meant to be included only once in simulator (sc_main.cpp) and once in embedded target program !
 * (array init in header will fail in link otherwise anyway)
 */

#define DYCTON_RUN // for lib adaptation to simulator run

#define DATASET_COUNT 		(8)

const char * dataset_array[DATASET_COUNT] = {	"../software/json_parser/datasets/0_walking_dead_short.json", 
												"../software/json_parser/datasets/1_increased_depth.json",
												"../software/json_parser/datasets/2_lorem_ipsum.json",
												"../software/json_parser/datasets/3_minimal_episodes.json",
												"../software/json_parser/datasets/4_more_objects.json",
												"../software/json_parser/datasets/5_more_things_in_ep.json",
												"../software/json_parser/datasets/6_less_strings.json",
												"../software/json_parser/datasets/7_games_of_thrones_short.json"};
											
uint32_t dataset_footprint[DATASET_COUNT] = {	38964, 
												45988,
												46252,
												11844,
												41716,
												52180,
												120332,
												45644};

uint32_t dataset_received_size[DATASET_COUNT] = {	13041,
													18050,
													19356,
													2714,
													11183,
													22204,
													45171,
													15544};




#endif //DYCTON_DIJKSTRA_APP_H