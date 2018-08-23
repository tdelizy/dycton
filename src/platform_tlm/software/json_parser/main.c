/* example application for json parsing
 *
 * takes in input a json object read from a file by the
 * "general purpose buffer" simulation component.
 * the json array is then parsed, a cleanup phase is
 * then proceeded to keep only an array of episodes
 * a selection is then done upon criteria and the
 * resulting array is serialized and send out to
 * a writte buffer.
 *
 * used json file configuration in platform_tlm/iss/sc_main.cpp
 *
 * TDk @ CITI Lab 2017
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "address_map.h"
#include "hal.h"
#include "parson.h"

// #define RECIEVED_SIZE (1814) // walking_dead_shorter.json
#define RECIEVED_SIZE (13041) // walking_dead_short.json
// #define RECIEVED_SIZE (91343) // walking_dead.json
// #define RECIEVED_SIZE (62082) // games_of_thrones.json



int main() {
    uint32_t err = 0;
    char * string_to_parse = NULL;
    int * test;

    printf("\n\n\n================== JSON parsing application for DYCTON ==================\n");
    printf("(TDk @ CITI Lab)\n\n");

    // json library initialisation
    json_set_allocation_functions(malloc, free);


    // copy to memory the recieved file
    string_to_parse = (char*)malloc(RECIEVED_SIZE);
    printf("string_to_parse = 0x%x\n", (unsigned int)string_to_parse);
    char* addr = string_to_parse;
    for(int i = 0; i < RECIEVED_SIZE; i++){
        *addr = read_mem(IN_BUF_BASE);
        addr++;
    }

    printf("%d charaters recieved\n", RECIEVED_SIZE);
    printf("end of input stream, parsing resulting string.\n");


    // parse that string
    JSON_Value * json_value_tree_root = NULL;
    JSON_Object * json_object_tree_root = NULL;
    JSON_Value * json_value_ptr = NULL;
    JSON_Object * json_object_ptr = NULL;
    json_value_tree_root = json_parse_string(string_to_parse);
    printf("string parsed !\n");

    // free the parsed string
    free(string_to_parse);

    // getting an array of episodes from the json database
    json_object_tree_root = json_value_get_object(json_value_tree_root);
    json_value_ptr = json_object_get_value(json_object_tree_root, "_embedded");
    json_object_ptr = json_value_get_object(json_value_ptr);
    JSON_Array  * array = json_object_get_array(json_object_ptr, "episodes");

    // cleanup the serie description, keeping only the name, the genre and the summary
    json_object_dotremove(json_object_tree_root, "id");
    json_object_dotremove(json_object_tree_root, "url");
    json_object_dotremove(json_object_tree_root, "type");
    json_object_dotremove(json_object_tree_root, "langage");
    json_object_dotremove(json_object_tree_root, "status");
    json_object_dotremove(json_object_tree_root, "runtime");
    json_object_dotremove(json_object_tree_root, "premiered");
    json_object_dotremove(json_object_tree_root, "officialSite");
    json_object_dotremove(json_object_tree_root, "schedule");
    json_object_dotremove(json_object_tree_root, "rating");
    json_object_dotremove(json_object_tree_root, "weight");
    json_object_dotremove(json_object_tree_root, "network");
    json_object_dotremove(json_object_tree_root, "webChannel");
    json_object_dotremove(json_object_tree_root, "externals");
    json_object_dotremove(json_object_tree_root, "image");
    json_object_dotremove(json_object_tree_root, "updated");
    json_object_dotremove(json_object_tree_root, "_links");


    // for each episode remove anything but its name, number, season and summary
    // episode example :
    // {
    //   "id": 4095,
    //   "url": "http://www.tvmaze.com/episodes/4095/the-walking-dead-1x01-days-gone-bye",
    //   "name": "Days Gone Bye",
    //   "season": 1,
    //   "number": 1,
    //   "airdate": "2010-10-31",
    //   "airtime": "22:00",
    //   "airstamp": "2010-11-01T02:00:00+00:00",
    //   "runtime": 60,
    //   "image": {
    //     "medium": "http://static.tvmaze.com/uploads/images/medium_landscape/0/2104.jpg",
    //     "original": "http://static.tvmaze.com/uploads/images/original_untouched/0/2104.jpg"
    //   },
    //   "summary": "<p>Rick searches for his family after emerging from a coma into a world terrorized by the walking dead. Morgan and Duane, whom he meets along the way, help teach Rick the new rules for survival.</p>",
    //   "_links": {
    //     "self": {
    //       "href": "http://api.tvmaze.com/episodes/4095"
    //     }
    //   }
    // }
    JSON_Object * ep = NULL;
    if (array != NULL && json_array_get_count(array) > 1) {
        for (int i = 0; i < json_array_get_count(array); i++) {
            ep = json_array_get_object (array, i);
            printf("ep name : %s\n",json_object_get_string(ep, "name"));
            json_object_dotremove(ep, "id");
            json_object_dotremove(ep, "url");
            json_object_dotremove(ep, "airdate");
            json_object_dotremove(ep, "airtime");
            json_object_dotremove(ep, "airstamp");
            json_object_dotremove(ep, "runtime");
            json_object_dotremove(ep, "image");
            json_object_dotremove(ep, "_links");
        }
    }else{
        printf("json_array_get_count(array)=%d", json_array_get_count(array));
    }


    // serialize resulting json object array to a buffer
    printf("serialize to string...\n");
    char *serialized_string = json_serialize_to_string_pretty(json_value_tree_root);
    if (serialized_string == NULL) {
        err = 5;
        goto err_exit;
    }
    // printf("result string content:\n%s\n", serialized_string);


    printf("write to output buffer...\n");
    addr = serialized_string;
    for(int i = 0; i < RECIEVED_SIZE; i++){
        write_mem(OUT_BUF_BASE, addr);
        addr++;
    }



    // free memory
    free(json_object_tree_root);
    json_free_serialized_string(serialized_string);


    // exit with success
    printf("success.\n");
    printf("\n=========================== THE END ===========================\n");
    exit(err);
    return err;

err_exit:
    // exit due to error

    // creating a big black box on plot for timing of error
    test = (int*)malloc(0x1000);
    for(int c=0;c<0x8F;c++){
        free(test);
        test = (int*)malloc(0x1000);
        memset(test,c, 0x1000-1);
    }
    free(test);
    switch (err) {
    case 1:
        printf("ERROR_MALLOC_FAILED\n");
        break;
    case 2:
        printf("ERROR_REALLOC_FAILED\n");
        break;
    case 3:
        printf("ERROR_LINE_OVERFLOW\n");
        break;
    case 4:
        printf("ERROR_PARSING_FAILURE\n");
        break;
    case 5:
        printf("ERROR_SERIALIZE_FAILURE\n");
        break;
    default:
        printf("UNKNOWN_ERROR\n");
        break;
    }
    exit(err);
    return err;

}