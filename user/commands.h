#ifndef __COMMANDS__
#define __COMMANDS__

#include "os_type.h"

uint8_t arguments[10][10];  

typedef struct {
    //uint64_t id;
    uint8_t *name;
    uint8_t *(*process)(uint8_t *params);
} Command_t;

uint8_t *test_process(uint8_t *params);
uint8_t *wifi_get_status(uint8_t *params);


uint8_t *exec_command(uint8_t *name,uint8_t *params);
uint8_t *get_command(uint8_t *in);
uint8_t get_args(uint8_t *in);

#endif //__COMMANDS__
