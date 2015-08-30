#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"

#include "commands.h"

#include "driver/uart.h"

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static void user_procTask(os_event_t *events);

static volatile os_timer_t some_timer;

void user_rf_pre_init(void);

void user_rf_pre_init(void){}
void some_timerfunc(void *arg)
{
	
    //Do blinky stuff
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2)
    {
		//uart0_sendStr("Low!\r\n");
        //Set GPIO2 to LOW
        gpio_output_set(0, BIT2, BIT2, 0);
    }
    else
    {
		//uart0_sendStr("High!\r\n");
        //Set GPIO2 to HIGH
        gpio_output_set(BIT2, 0, BIT2, 0);
    }
}

uint8_t cmd_pos = 0;
uint8_t cmd_buffer[100];

//Do nothing function
static void ICACHE_FLASH_ATTR user_procTask(os_event_t *events)
{
    uint8_t temp;
    uint8_t len;
    uint8_t *cmd;
    while(READ_PERI_REG(UART_STATUS(UART0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S))
    {
        WRITE_PERI_REG(0X60000914, 0x73); //WTD
        
        temp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
		cmd_buffer[cmd_pos++] = temp;
        uart_tx_one_char(temp); //display back
        if (temp == '\r')
        {
            uart_tx_one_char('\n');
            cmd_buffer[cmd_pos-1] = ' ';
            cmd_buffer[cmd_pos] = 0x0d;
            cmd_buffer[cmd_pos+1] = 0;
            len = get_args(cmd_buffer);
            if (len > 0)
            {

                uint8_t *resp = exec_command(arguments[0], NULL);
                if (resp != NULL)
                    uart0_tx_buffer(resp,strlen(resp));
            }

            cmd_pos = 0;		
        }
    }

    if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST))
    {
        WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
    }
    else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_TOUT_INT_ST))
    {
        WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
    }
    ETS_UART_INTR_ENABLE();
}

//Init function 
void ICACHE_FLASH_ATTR user_init()
{
    char ssid[32] = "DIR-300NRU";
    char password[64] = "08862907";
    struct station_config stationConf;

    // Initialize the GPIO subsystem.
    gpio_init();

	uart_init(BIT_RATE_115200, BIT_RATE_115200);

    //Set GPIO2 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

    //Set GPIO2 low
    gpio_output_set(0, BIT2, BIT2, 0);

    
    wifi_set_opmode(1);

    os_memcpy(&stationConf.ssid, ssid, 32);
    os_memcpy(&stationConf.password, password, 64);
    wifi_station_set_config(&stationConf);

	
    exec_command("test", NULL);

    //Disarm timer
    os_timer_disarm(&some_timer);

    //Setup timer
    os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //1000 is the fire time in ms
    //0 for once and 1 for repeating
    os_timer_arm(&some_timer, 1000, 1);
    
    //Start os task
    system_os_task(user_procTask, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
}
