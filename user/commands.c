#include "commands.h"
#include "os_type.h"
#include "osapi.h"
#include <string.h>
#include "ip_addr.h"
#include "espconn.h"

Command_t commands[] = {
{"test", test_process},
{"stat", wifi_get_status}, 
};

uint8_t buffer[20];

uint16_t commands_len = sizeof(commands)/sizeof(Command_t);

static struct espconn httpconfig_conn;
static esp_tcp httpconfig_tcp_conn;

static void ICACHE_FLASH_ATTR httpconfig_recv_cb(void *arg, char *data, unsigned short len) {
  struct espconn *conn=(struct espconn *)arg;
 
  espconn_disconnect(conn);
}
 
static void ICACHE_FLASH_ATTR httpconfig_recon_cb(void *arg, sint8 err) {
}
 
static void ICACHE_FLASH_ATTR httpconfig_discon_cb(void *arg) {
}
 
static void ICACHE_FLASH_ATTR httpconfig_sent_cb(void *arg) {
}

static void ICACHE_FLASH_ATTR httpconfig_connected_cb(void *arg) {
  struct espconn *conn=arg;
 
  espconn_regist_recvcb  (conn, httpconfig_recv_cb);
  espconn_regist_reconcb (conn, httpconfig_recon_cb);
  espconn_regist_disconcb(conn, httpconfig_discon_cb);
  espconn_regist_sentcb  (conn, httpconfig_sent_cb);

  uart0_sendStr("connected_cb()\r\n");
 
  //char *transmission = "OK\r\n\r\nOK!\n";
  therm_read_temperature(buffer);
  sint8 d = espconn_sent(conn,buffer,strlen(buffer));
}

uint8_t *test_process(uint8_t *params)
{
    httpconfig_conn.type=ESPCONN_TCP;
    httpconfig_conn.state=ESPCONN_NONE;
    httpconfig_tcp_conn.local_port=80;
    httpconfig_conn.proto.tcp=&httpconfig_tcp_conn;

    espconn_regist_connectcb(&httpconfig_conn, httpconfig_connected_cb);
    espconn_accept(&httpconfig_conn);

    return "This is a TCP SERVER test!\r\n";
}



uint8_t *wifi_get_status(uint8_t *params)
{
    //uint8_t c_status = wifi_station_get_connect_status();
    
    therm_read_temperature(buffer);
    //os_sprintf(buffer, "connection status:%d\r\n", c_status);
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
