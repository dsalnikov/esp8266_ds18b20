#include "commands.h"
#include "os_type.h"
#include "osapi.h"
#include <string.h>

Command_t commands[] = {
{"test", test_process},
{"stat", wifi_get_status}, 
};

uint16_t commands_len = sizeof(commands)/sizeof(Command_t);

uint8_t *test_process(uint8_t *params)
{
    return "This is a test!\r\n";
}

uint8_t buffer[20];

uint8_t *wifi_get_status(uint8_t *params)
{
    uint8_t c_status = wifi_station_get_connect_status();
    os_sprintf(buffer, "connection status:%d\r\n", c_status);
    return buffer;
}

uint8_t *exec_command(uint8_t *name,uint8_t *params)
{
    uint8_t i;
    uint8_t *res = NULL;
    for (i=0;i<commands_len;i++)
    {
        if (!strcmp(commands[i].name, name))
        {
            res = commands[i].process(params);            
            break;
        }
    }
    return res;
}

uint8_t *get_command(uint8_t *in)
{
    return strtok(in, " ");
}


uint8_t get_args(uint8_t *in)
{   
    uint8_t *arg;
    uint8_t len = 0;
    
    arg = strtok(in, " ");
    //arg = strtok(NULL, " ");
    while(arg != NULL)
    {
        strcpy(arguments[len++],arg);
        arg = strtok(NULL, " ");
    }
    return len;
}
